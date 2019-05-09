#include "CtrlPosSet.h"

// Pointer to file buffer (contents of image file)
char *CtrlPosSet::pBuffer = NULL;

// Pointer to begin of bitmap
BITMAPFILEHEADER *CtrlPosSet::pBmFh;

// Collection of window handles and status of the control
std::map<HWND, VecMat::Vec3> CtrlPosSet::mapPos;

/*********************************************************************
* Constructor
*********************************************************************/
CtrlPosSet::CtrlPosSet(std::string name, VecMat::Vec3 pos, HWND hWndParent)
{
    this->name = std::wstring(name.begin(), name.end());
    this->hWndParent = hWndParent;
    this->pos = pos;
}

/*********************************************************************
* Destructor
*********************************************************************/
CtrlPosSet::~CtrlPosSet (void)
{
    CloseWindow(hWndControl);

    // Free buffer memory
    if (pBuffer != NULL) delete (pBuffer);

    pBuffer = NULL;
}

/*********************************************************************
* Create Control
*********************************************************************/
HWND CtrlPosSet::Create (int posX, int posY, int sizeX, int sizeY)
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
        std::string image = "images/posset.bmp";

        // Read file
        unsigned int fileSize;
        pBuffer = Files::ReadBinary(image.c_str(), &fileSize);
        pBmFh = (BITMAPFILEHEADER*)pBuffer;

        if (pBuffer == NULL)
        {
            message.clear();
            message.append("Could not read file: ");
            message.append(image);
            Error::WriteLog("ERROR", "CtrlPosSet::CtrlPosSet", message.c_str());
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
                Error::WriteLog("ERROR", "CtrlPosSet::CtrlPosSet", message.c_str());

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

        // Create PosSet window (control)
        hWndControl = CreateWindow((LPWSTR)name.c_str(), (LPWSTR)name.c_str(), WS_CHILD | WS_VISIBLE, posX, posY, imageWidth + 10 * (int)name.length(), imageHeight+2, hWndParent, (HMENU)1, NULL, NULL);

        ShowWindow(hWndControl, SW_SHOWNORMAL);
        UpdateWindow(hWndControl);

        // Put this window handle in a list connecting it to the checked member
        CtrlPosSet::mapPos.insert(std::pair<HWND, VecMat::Vec3>(hWndControl, this->pos));

    } catch (...)
    {
        Error::WriteLog("ERROR", "CtrlPosSet::Create", "Error creating PosSet control");
        hWndControl = NULL;
    }

    return hWndControl;
}

/*********************************************************************
* Get (PosSet)window handle
*********************************************************************/
HWND CtrlPosSet::GetHWnd (void)
{
    return hWndControl;
}

/*********************************************************************
* Get PosSet status
*********************************************************************/
VecMat::Vec3 CtrlPosSet::GetStatus (void)
{
    return CtrlPosSet::mapPos[hWndControl];
}

/*****************************************************************************
* Windows procedure (handle messages for the controls in the CtrlPosSet window)
*****************************************************************************/
LRESULT CALLBACK CtrlPosSet::WndProcControl (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR text[40];
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    RECT rectTemp;
    HFONT hFont;
    HPEN hPen;

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
                imageHeight = std::abs (pBmi->bmiHeader.biHeight);
            }

            // Copy image data to screen
            SetDIBitsToDevice (hdc, 0, 0, imageWidth, imageHeight, 0, 0, 0, imageHeight, pBits, pBmi, DIB_RGB_COLORS);

            // Create font
            hFont = CreateFontW(22, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0, 0, 0, 0, 0, L"Tahoma");
            SelectObject (hdc, hFont);

            // Set PosSets
            SetBkMode (hdc, TRANSPARENT);
            SetTextColor (hdc, RGB(0x00, 0x00, 0x00));

            // Adjust rect (for x)
            rectTemp = rect;
            rectTemp.left = rectTemp.left + 22;
            if (std::abs(CtrlPosSet::mapPos[hWnd][0]) < 100) rectTemp.left += 6;
            if (std::abs(CtrlPosSet::mapPos[hWnd][0]) < 10)  rectTemp.left += 6;

            // Draw x pos
            DrawText (hdc, std::to_wstring((int)CtrlPosSet::mapPos[hWnd][0]).c_str(), -1, &rectTemp, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

            // Adjust rect (for y)
            rectTemp = rect;
            rectTemp.left = rectTemp.left + 114;
            if (std::abs(CtrlPosSet::mapPos[hWnd][1]) < 100) rectTemp.left += 6;
            if (std::abs(CtrlPosSet::mapPos[hWnd][1]) < 10)  rectTemp.left += 6;

            // Draw y pos
            DrawText (hdc, std::to_wstring((int)CtrlPosSet::mapPos[hWnd][1]).c_str(), -1, &rectTemp, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

            // Adjust rect (for z)
            rectTemp = rect;
            rectTemp.left = rectTemp.left + 206;
            if (std::abs(CtrlPosSet::mapPos[hWnd][2]) < 100) rectTemp.left += 6;
            if (std::abs(CtrlPosSet::mapPos[hWnd][2]) < 10)  rectTemp.left += 6;

            // Draw z pos
            DrawText (hdc, std::to_wstring((int)CtrlPosSet::mapPos[hWnd][2]).c_str(), -1, &rectTemp, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

            // Select graphic draw PosSet
            hPen = CreatePen (PS_SOLID, 4, RGB (CtrlPosSet::mapPos[hWnd][0], CtrlPosSet::mapPos[hWnd][1], CtrlPosSet::mapPos[hWnd][2]));
            SelectObject (hdc, hPen);

            // Adjust rect
            rectTemp = rect;
            rectTemp.left = rectTemp.left + imageWidth + 8;

            // Draw text
            DrawText (hdc, text, -1, &rectTemp, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

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

                // Check where clicked
                if ((curPos.x > (rect.left      )) && (curPos.x < (rect.left +  76))) CtrlPosSet::mapPos[hWnd][0] += 1;
                if ((curPos.x > (rect.left +  76)) && (curPos.x < (rect.left + 170))) CtrlPosSet::mapPos[hWnd][1] += 1;
                if ((curPos.x > (rect.left + 170)) && (curPos.x < (rect.left + 264))) CtrlPosSet::mapPos[hWnd][2] += 1;
            }

            // Down
            if (SHORT(HIWORD (wParam)) == -120)
            {
                // Change value according to mouse position
                GetCursorPos(&curPos);

                // Get position of the window
                GetWindowRect(hWnd, &rect);

                // Check where clicked
                if ((curPos.x > (rect.left      )) && (curPos.x < (rect.left +  76))) CtrlPosSet::mapPos[hWnd][0] -= 1;
                if ((curPos.x > (rect.left +  76)) && (curPos.x < (rect.left + 170))) CtrlPosSet::mapPos[hWnd][1] -= 1;
                if ((curPos.x > (rect.left + 170)) && (curPos.x < (rect.left + 264))) CtrlPosSet::mapPos[hWnd][2] -= 1;
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
            if ((curPos.x > (rect.left + 64))  && (curPos.x < (rect.left + 76))  && (curPos.y <  (rect.top + 16))) CtrlPosSet::mapPos[hWnd][0] += 1;
            if ((curPos.x > (rect.left + 64))  && (curPos.x < (rect.left + 76))  && (curPos.y >= (rect.top + 16))) CtrlPosSet::mapPos[hWnd][0] -= 1;
            if ((curPos.x > (rect.left + 158)) && (curPos.x < (rect.left + 170)) && (curPos.y <  (rect.top + 16))) CtrlPosSet::mapPos[hWnd][1] += 1;
            if ((curPos.x > (rect.left + 158)) && (curPos.x < (rect.left + 170)) && (curPos.y >= (rect.top + 16))) CtrlPosSet::mapPos[hWnd][1] -= 1;
            if ((curPos.x > (rect.left + 252)) && (curPos.x < (rect.left + 264)) && (curPos.y <  (rect.top + 16))) CtrlPosSet::mapPos[hWnd][2] += 1;
            if ((curPos.x > (rect.left + 252)) && (curPos.x < (rect.left + 264)) && (curPos.y >= (rect.top + 16))) CtrlPosSet::mapPos[hWnd][2] -= 1;

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

