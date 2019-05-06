#pragma once
#include <windows.h>
#include <commdlg.h>
#include <map>
#include "../error.h"

class CtrlRadioButton
{
    private:

        // Window handles
        HWND hWndParent;
        HWND hWndControl;

        // Control name
        std::wstring name;

        // Control value
        std::wstring value;

        // Status (on or off)
        bool status;

        // Pointer to image file buffer
        static char *pBuffer;

        // Pointer to begin of bitmap
        static BITMAPFILEHEADER *pBmFh;

        // Collection of window handles and status of the control
        static std::map<HWND, bool> mapStatus;

        // Windows procedure
        static LRESULT CALLBACK WndProcControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    public:

        // Get the status of the control
        bool GetStatus (void);

        // set the status of the control
        void SetStatus (bool status);

        // Constructor
        CtrlRadioButton (std::string name, std::string value, bool status, HWND hWndParent);

        // Destructor
        ~CtrlRadioButton (void);

        // Create the control
        HWND Create (int posX, int posY, int sizeX, int sizeY);

        // Get (control)window handle
        HWND GetHWnd (void);
};
