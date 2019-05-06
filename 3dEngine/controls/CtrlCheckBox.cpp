#include "CtrlCheckBox.h"

// Pointer to file buffer (contents of file)
char *CtrlCheckBox::pBuffer = NULL;

// Pointer to begin of bitmap
BITMAPFILEHEADER *CtrlCheckBox::pBmFh;

// Collection of window handles and status of the control
std::map<HWND, bool> CtrlCheckBox::mapChecked;

/*********************************************************************
* Constructor
*********************************************************************/
CtrlCheckBox::CtrlCheckBox(std::string name, bool status, HWND hWndParent)
{
    this->name = std::wstring(name.begin(), name.end());
    this->hWndParent = hWndParent;
    checked = status;
}

/*********************************************************************
* Destructor
*********************************************************************/
CtrlCheckBox::~CtrlCheckBox (void)
{
    CloseWindow(hWndControl);

    // Free buffer memory
    if (pBuffer != NULL) delete (pBuffer);

    pBuffer = NULL;
}

/*********************************************************************
* Create Control
*********************************************************************/
HWND CtrlCheckBox::Create (int posX, int posY, int sizeX, int sizeY)
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

        // Image for use in control
        std::string image = "images/checkbox.bmp";

        // Read file
        unsigned int fileSize;
        pBuffer = Files::ReadBinary(image.c_str(), &fileSize);
        pBmFh = (BITMAPFILEHEADER*)pBuffer;

        if (pBuffer == NULL)
        {
            message.clear();
            message.append("Could not read file: ");
            message.append(image);
            Error::WriteLog("ERROR", "CtrlCheckBox::CtrlCheckBox", message.c_str());
        }

        // If file has been (correctly) read
        if (pBuffer != NULL)
        {
            // Check if image is the correct format (bmp bitmap file)
            if (pBmFh->bfType != * (WORD *) "BM")
            {
                message.clear();
                message.append("File read cancelled, is not a bitmap: ");
                message.append(image);
                Error::WriteLog("ERROR", "CtrlCheckBox::CtrlCheckBox", message.c_str());

                // Free buffer memory
                delete (pBuffer);
                return (NULL);
            }

            // Get the pointer to the info structure
            pBmi = (BITMAPINFO *) (pBmFh + 1);

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

        // Create CheckBox window (control)
        hWndControl = CreateWindow((LPWSTR)name.c_str(), (LPWSTR)name.c_str(), WS_CHILD | WS_VISIBLE, posX, posY, imageWidth + 10 * (int)name.length(), imageHeight+2, hWndParent, (HMENU)1, NULL, NULL);

        ShowWindow(hWndControl, SW_SHOWNORMAL);
        UpdateWindow(hWndControl);

        // Put this window handle in a list connecting it to the checked member
        CtrlCheckBox::mapChecked.insert(std::pair<HWND, bool>(hWndControl, this->checked));

    } catch (...)
    {
        Error::WriteLog("ERROR", "CtrlCheckBox::Create", "Error creating checkbox control");
        hWndControl = NULL;
    }

    return hWndControl;
}

/*********************************************************************
* Get (CheckBox)window handle
*********************************************************************/
HWND CtrlCheckBox::GetHWnd (void)
{
    return hWndControl;
}

/*********************************************************************
* Get checkbox status
*********************************************************************/
bool CtrlCheckBox::GetStatus (void)
{
    return CtrlCheckBox::mapChecked[hWndControl];
}

/*****************************************************************************
* Windows procedure (handle messages for the controls in the CtrlCheckBox window)
*****************************************************************************/
LRESULT CALLBACK CtrlCheckBox::WndProcControl (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR text[40];
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    HFONT hFont;
    HPEN hPen;

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

            // Get the pointer to the image bits
            pBits = (char *) pBmFh + pBmFh->bfOffBits;

            // Get the pointer to the info structure
            pBmi = (BITMAPINFO *) (pBmFh + 1);

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

            // Draw check according to state
            if (CtrlCheckBox::mapChecked[hWnd])
            {
                // Select graphic draw color
                hPen = CreatePen (PS_SOLID, 3, RGB (0, 0xDD, 0));
                SelectObject (hdc, hPen);

                // Draw green check
                MoveToEx (hdc, rect.left + 6, rect.top + 6, NULL);
                LineTo (hdc, imageWidth / 2, imageHeight - 6);
                LineTo (hdc, imageWidth - 2, rect.top - 6);
            } else
            {
                // Select graphic draw color
                hPen = CreatePen (PS_SOLID, 3, RGB (0xDD, 0, 0));
                SelectObject (hdc, hPen);

                // Draw red cros
                MoveToEx (hdc, rect.left + 6, rect.top + 6, NULL);
                LineTo (hdc, imageWidth -6, imageHeight -6);
                MoveToEx (hdc, rect.left + 6, rect.bottom - 6, NULL);
                LineTo (hdc, imageWidth -6, rect.top + 6);
            }

            // Create font
            hFont = CreateFontW(20, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, L"Tahoma");
            SelectObject (hdc, hFont);

            // Set colors
            SetBkMode (hdc, TRANSPARENT);
            SetTextColor (hdc, RGB(0x00, 0x00, 0x00));

            // Adjust rect for image in it
            rect.left = rect.left + imageWidth + 4;

            // Draw text
            DrawText (hdc, text, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

            // Close
            EndPaint (hWnd, &ps);
            break;

        case WM_LBUTTONUP:

            // Toggle checked value
            CtrlCheckBox::mapChecked[hWnd] = !CtrlCheckBox::mapChecked[hWnd];

            // Get control rect size
            GetClientRect (hWnd, &rect);

            // Redraw rect
            InvalidateRect(hWnd, &rect, true);

            // Send message to parent window
            SendMessage (GetParent (hWnd), WM_COMMAND, (WPARAM) WM_LBUTTONUP, (LPARAM) hWnd);
            break;

        default:
            return DefWindowProc (hWnd, message, wParam, lParam);
    }

    return (0);
}

