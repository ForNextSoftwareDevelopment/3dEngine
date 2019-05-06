#pragma once
#include <windows.h>
#include <commdlg.h>
#include <map>
#include "../error.h"

class CtrlCheckBox
{
    private:

        // Window handles
        HWND hWndParent;
        HWND hWndControl;

        // Control name
        std::wstring name;

        // (Initial) Status of the control
        bool checked;

        // Pointer to image file buffer
        static char *pBuffer;

        // Pointer to begin of bitmap
        static BITMAPFILEHEADER *pBmFh;

        // Collection of window handles and status of the control
        static std::map<HWND, bool> mapChecked;

        // Windows procedure
        static LRESULT CALLBACK WndProcControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    public:

        // Get the status of the control (checked = true or false)
        bool GetStatus (void);

        // Constructor
        CtrlCheckBox (std::string name, bool status, HWND hWndParent);

        // Destructor
        ~CtrlCheckBox (void);

        // Create the control
        HWND Create (int posX, int posY, int sizeX, int sizeY);

        // Get (control)window handle
        HWND GetHWnd (void);
};
