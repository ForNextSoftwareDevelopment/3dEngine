#include "CtrlButton.h"

// Pointers to begin of bitmaps
BITMAPFILEHEADER *CtrlButton::pBmFh1;
BITMAPFILEHEADER *CtrlButton::pBmFh2;

// Pointer to image file buffer
char *CtrlButton::pBuffer1 = NULL;
char *CtrlButton::pBuffer2 = NULL;

// Mouse hoovering over button
bool CtrlButton::hoover;

/*********************************************************************
* Constructor
*********************************************************************/
CtrlButton::CtrlButton(std::string name, HWND hWndParent)
{
    this->name = std::wstring(name.begin(), name.end());
    this->hWndParent = hWndParent;
}

/*********************************************************************
* Destructor
*********************************************************************/
CtrlButton::~CtrlButton (void)
{
    CloseWindow(hWndControl);

    // Free buffer memory
    if (pBuffer1 != NULL) delete (pBuffer1);
    if (pBuffer2 != NULL) delete (pBuffer2);

    pBuffer1 = NULL;
    pBuffer2 = NULL;
}

/*********************************************************************
* Create Control
*********************************************************************/
HWND CtrlButton::Create (int posX, int posY, int sizeX, int sizeY)
{
    WNDCLASS wndClass = { 0 };

    wndClass.style          = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc    = (WNDPROC) WndProcControl;
    wndClass.cbClsExtra     = 0;
    wndClass.cbWndExtra     = 0;
    wndClass.hIcon          = NULL;
    wndClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
    wndClass.hbrBackground  = GetSysColorBrush(COLOR_INACTIVECAPTION);
    wndClass.lpszMenuName   = NULL;
    wndClass.lpszClassName  = (LPWSTR)name.c_str();
    RegisterClass (&wndClass);

    try
    {
        // Variables for loading bitmap file
        BITMAPINFO *pBmi = NULL;
        int imageWidth = 0;
        int imageHeight = 0;

        // Error message
        std::string message;

        // Images for use in control
        std::string image1 = "images/button.bmp";
        std::string image2 = "images/buttonhoover.bmp";

        // Read first image
        unsigned int fileSize;
        pBuffer1 = Files::ReadBinary(image1.c_str(), &fileSize);
        pBmFh1 = (BITMAPFILEHEADER*)pBuffer1;

        if (pBuffer1 == NULL)
        {
            message.clear();
            message.append("Could not read file: ");
            message.append(image1);
            Error::WriteLog("ERROR", "CtrlButton::CtrlButton", message.c_str());
        }

        // If file has been (correctly) read
        if (pBuffer1 != NULL)
        {
            // Check if image is the correct format (bmp bitmap file)
            if (pBmFh1->bfType != * (WORD *) "BM")
            {
                message.clear();
                message.append("File read cancelled, is not a bitmap: ");
                message.append(image1);
                Error::WriteLog("ERROR", "CtrlButton::CtrlButton", message.c_str());

                // Free buffer memory
                delete (pBuffer1);
                return (NULL);
            }

            // Get the pointer to the info structure
            pBmi = (BITMAPINFO *) (pBmFh1 + 1);

            // Get the DIB width and height
            if (pBmi->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
            {
                imageWidth = ((BITMAPCOREHEADER *) pBmi)->bcWidth;
                imageHeight = ((BITMAPCOREHEADER *) pBmi)->bcHeight;
            }
            else
            {
                imageWidth = pBmi->bmiHeader.biWidth;
                imageHeight = abs (pBmi->bmiHeader.biHeight);
            }
        }

        // Read second image
        pBuffer2 = Files::ReadBinary(image2.c_str(), &fileSize);
        pBmFh2 = (BITMAPFILEHEADER*)pBuffer2;

        if (pBuffer2 == NULL)
        {
            message.clear();
            message.append("Could not read file: ");
            message.append(image2);
            Error::WriteLog("ERROR", "CtrlButton::CtrlButton", message.c_str());
        }

        // If file has been (correctly) read
        if (pBuffer2 != NULL)
        {
            // Check if image is the correct format (bmp bitmap file)
            if (pBmFh2->bfType != * (WORD *) "BM")
            {
                message.clear();
                message.append("File read cancelled, is not a bitmap: ");
                message.append(image2);
                Error::WriteLog("ERROR", "CtrlButton::CtrlButton", message.c_str());

                // Free buffer memory
                delete (pBuffer2);
                return (NULL);
            }
        }

        // Create button window (control)
        hWndControl = CreateWindow((LPWSTR)name.c_str(), (LPWSTR)name.c_str(), WS_CHILD | WS_VISIBLE, posX, posY, imageWidth+2, imageHeight+2, hWndParent, (HMENU)1, NULL, NULL);

        ShowWindow(hWndControl, SW_SHOWNORMAL);
        UpdateWindow(hWndControl);
    } catch (...)
    {
        Error::WriteLog("ERROR", "CtrlButton::Create", "Error creating button control");
        hWndControl = NULL;
    }

    return hWndControl;
}

/*********************************************************************
* Get (button)window handle
*********************************************************************/
HWND CtrlButton::GetHWnd (void)
{
    return hWndControl;
}

/*****************************************************************************
* Windows procedure (handle messages for the controls in the CtrlButton window)
*****************************************************************************/
LRESULT CALLBACK CtrlButton::WndProcControl (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR text[40];
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    HFONT hFont;

    BITMAPINFO *pBmi;
    char *pBits;
    int imageWidth;
    int imageHeight;

    switch (message)
    {
        case WM_CREATE:
            break;

        case WM_CLOSE:
            DestroyWindow (hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        case WM_PAINT:

            // Get control rect size
            GetClientRect (hWnd, &rect);

            // Get text (from window caption)
            GetWindowText (hWnd, text, sizeof (text));

            // Start painting
            hdc = BeginPaint (hWnd, &ps);

            // Get the pointer to the image bits and to the info structure
            if (!CtrlButton::hoover)
            {
                pBits = (char *) pBmFh1 + pBmFh1->bfOffBits;
                pBmi = (BITMAPINFO *) (pBmFh1 + 1);
            } else
            {
                pBits = (char *) pBmFh2 + pBmFh2->bfOffBits;
                pBmi = (BITMAPINFO *) (pBmFh2 + 1);
            }

            // Get the DIB width and height
            if (pBmi->bmiHeader.biSize == sizeof (BITMAPCOREHEADER))
            {
                imageWidth = ((BITMAPCOREHEADER *) pBmi)->bcWidth;
                imageHeight = ((BITMAPCOREHEADER *) pBmi)->bcHeight;
            }
            else
            {
                imageWidth = pBmi->bmiHeader.biWidth;
                imageHeight = abs (pBmi->bmiHeader.biHeight);
            }

            // Copy image data to screen
            SetDIBitsToDevice (hdc, 0, 0, imageWidth, imageHeight, 0, 0, 0, imageHeight, pBits, pBmi, DIB_RGB_COLORS);

            // Create font
            hFont = CreateFontW(22, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, L"Tahoma");
            SelectObject (hdc, hFont);

            // Set colors
            SetBkMode (hdc, TRANSPARENT);
            SetTextColor (hdc, RGB(0x00, 0x00, 0x00));

            // Draw text
            DrawText (hdc, text, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            // Close
            EndPaint (hWnd, &ps);
            break;

        case WM_MOUSEFIRST:

            // Mouse hoovering over button
            CtrlButton::hoover = true;

            // Get control rect size
            GetClientRect (hWnd, &rect);

            // Redraw rect
            InvalidateRect(hWnd, &rect, true);
            break;

        case WM_MOUSEHOVER:
            break;

        case WM_MOUSELEAVE:

            // Mouse hoovering over button
            CtrlButton::hoover = false;

            // Get control rect size
            GetClientRect (hWnd, &rect);

            // Redraw rect
            InvalidateRect(hWnd, &rect, true);
            break;

        case WM_LBUTTONUP:

            SendMessage (GetParent (hWnd), WM_COMMAND, (WPARAM) WM_LBUTTONUP, (LPARAM) hWnd);
            break;

        default:
            return DefWindowProc (hWnd, message, wParam, lParam);
    }

    return (0);
}

