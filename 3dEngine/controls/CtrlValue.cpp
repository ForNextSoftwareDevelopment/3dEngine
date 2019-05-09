#include "CtrlValue.h"

// Pointer to file buffer (contents of image file)
char *CtrlValue::pBuffer = NULL;

// Pointer to begin of bitmap
BITMAPFILEHEADER *CtrlValue::pBmFh;

// Collection of window handles and status of the control
std::map<HWND, int> CtrlValue::mapValue;

/*********************************************************************
* Constructor
*********************************************************************/
CtrlValue::CtrlValue(std::string name, int value, HWND hWndParent)
{
    this->name = std::wstring(name.begin(), name.end());
    this->hWndParent = hWndParent;
    this->value = value;
}

/*********************************************************************
* Destructor
*********************************************************************/
CtrlValue::~CtrlValue (void)
{
    CloseWindow(hWndControl);

    // Free buffer memory
    if (pBuffer != NULL) delete (pBuffer);

    pBuffer = NULL;
}

/*********************************************************************
* Create Control
*********************************************************************/
HWND CtrlValue::Create (int posX, int posY, int sizeX, int sizeY)
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
    wndClass.lpszClassName  = (LPCWSTR) name.c_str();
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
        std::string image = "images/value.bmp";

        // Read file
        unsigned int fileSize;
        pBuffer = Files::ReadBinary (image.c_str(), &fileSize);
        pBmFh = (BITMAPFILEHEADER*) pBuffer;

        if (pBuffer == NULL)
        {
            message.clear();
            message.append("Could not read file: ");
            message.append(image);
            Error::WriteLog("ERROR", "CtrlValue::CtrlValue", message.c_str());
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
                Error::WriteLog("ERROR", "CtrlValue::CtrlValue", message.c_str());

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

        // Create Value window (control)
        hWndControl = CreateWindow((LPWSTR)name.c_str(), (LPWSTR)name.c_str(), WS_CHILD | WS_VISIBLE, posX, posY, imageWidth + 10 * (int)name.length(), imageHeight+2, hWndParent, (HMENU)1, NULL, NULL);

        ShowWindow(hWndControl, SW_SHOWNORMAL);
        UpdateWindow(hWndControl);

        // Put this window handle in a list connecting it to the checked member
        CtrlValue::mapValue.insert(std::pair<HWND, int>(hWndControl, this->value));

    } catch (...)
    {
        Error::WriteLog("ERROR", "CtrlValue::Create", "Error creating value control");
        hWndControl = NULL;
    }

    return hWndControl;
}

/*********************************************************************
* Get (Value)window handle
*********************************************************************/
HWND CtrlValue::GetHWnd (void)
{
    return hWndControl;
}

/*********************************************************************
* Get Value status
*********************************************************************/
int CtrlValue::GetStatus (void)
{
    return CtrlValue::mapValue[hWndControl];
}

/*****************************************************************************
* Windows procedure (handle messages for the controls in the CtrlValue window)
*****************************************************************************/
LRESULT CALLBACK CtrlValue::WndProcControl (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR text[40];
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect, tempRect;
    HFONT hFont;

    BITMAPINFO *pBmi;
    char *pBits;
    int imageWidth;
    int imageHeight;

    POINT curPos;

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

            // Create font
            hFont = CreateFontW(22, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, L"Tahoma");
            SelectObject (hdc, hFont);

            // Set Values
            SetBkMode (hdc, TRANSPARENT);
            SetTextColor (hdc, RGB(0x00, 0x00, 0x00));

            // Adjust rect
            tempRect = rect;
            tempRect.left = tempRect.left + 10;
            if (CtrlValue::mapValue[hWnd] < 100) tempRect.left += 6;
            if (CtrlValue::mapValue[hWnd] < 10)  tempRect.left += 6;

            // Draw text
            DrawText (hdc, std::to_wstring(CtrlValue::mapValue[hWnd]).c_str(), -1, &tempRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

            // Adjust rect
            rect.left = rect.left + imageWidth + 4;

            // Draw text (name of control)
            DrawText (hdc, text, -1, &rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

            // Close
            EndPaint (hWnd, &ps);
            break;

        case WM_MOUSEWHEEL:

            // Up
            if (SHORT(HIWORD (wParam)) == 120)
            {
                // Change value according to mouse position
                GetCursorPos(&curPos);

                // Get position of the window
                GetWindowRect(hWnd, &rect);

                // Adjust value
                CtrlValue::mapValue[hWnd] += 1;

                // Check boundaries
                if (CtrlValue::mapValue[hWnd] > 999) CtrlValue::mapValue[hWnd] = 0;
            }

            // Down
            if (SHORT(HIWORD (wParam)) == -120)
            {
                // Change value according to mouse position
                GetCursorPos(&curPos);

                // Get position of the window
                GetWindowRect(hWnd, &rect);

                // Adjust value
                CtrlValue::mapValue[hWnd] -= 1;

                // Check boundaries
                if (CtrlValue::mapValue[hWnd] < 0) CtrlValue::mapValue[hWnd] = 999;
            }

            // Get control rect size
            GetClientRect (hWnd, &rect);

            // Redraw rect
            InvalidateRect(hWnd, &rect, true);

            // Send message to parent window
            SendMessage (GetParent (hWnd), WM_COMMAND, (WPARAM) WM_LBUTTONUP, (LPARAM) hWnd);
            break;

        case WM_LBUTTONUP:

            // Change value according to mouse position
            GetCursorPos(&curPos);

            // Get position of the window
            GetWindowRect(hWnd, &rect);

            // Check if clicked in up or down sections
            if ((curPos.x > (rect.left + 44))  && (curPos.x < (rect.left + 56))  && (curPos.y <  (rect.top + 16))) CtrlValue::mapValue[hWnd] += 1;
            if ((curPos.x > (rect.left + 44))  && (curPos.x < (rect.left + 56))  && (curPos.y >= (rect.top + 16))) CtrlValue::mapValue[hWnd] -= 1;

            if (CtrlValue::mapValue[hWnd] > 999) CtrlValue::mapValue[hWnd] = 0;
            if (CtrlValue::mapValue[hWnd] < 0) CtrlValue::mapValue[hWnd] = 999;

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

