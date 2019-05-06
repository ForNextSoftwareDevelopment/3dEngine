#pragma once
#include <windows.h>
#include <commdlg.h>
#include "../error.h"

class CtrlButton
{
    private:

        // Window handles
        HWND hWndParent;
        HWND hWndControl;

        // Control name
        std::wstring name;

        // Pointer to image file buffer
        static char *pBuffer1;
        static char *pBuffer2;

        // Pointers to begin of bitmaps
        static BITMAPFILEHEADER *pBmFh1;
        static BITMAPFILEHEADER *pBmFh2;

        // Mouse hoovering over button
        static bool hoover;

        // Windows procedure
        static LRESULT CALLBACK WndProcControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    public:

        // Constructor
        CtrlButton (std::string name, HWND hWndParent);

        // Destructor
        ~CtrlButton (void);

        // Create the control
        HWND Create (int posX, int posY, int sizeX, int sizeY);

        // Get (control)window handle
        HWND GetHWnd (void);
};
