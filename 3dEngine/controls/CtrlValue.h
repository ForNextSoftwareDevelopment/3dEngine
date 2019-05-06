#pragma once
#include <windows.h>
#include <commdlg.h>
#include <map>
#include "../error.h"

class CtrlValue
{
    private:

        // Window handles
        HWND hWndParent;
        HWND hWndControl;

        // Control name
        std::wstring name;

        // (Initial) Status of the control
        int value;

        // Pointer to image file buffer
        static char *pBuffer;

        // Pointer to begin of bitmap
        static BITMAPFILEHEADER *pBmFh;

        // Collection of window handles and status of the control
        static std::map<HWND, int> mapValue;

        // Windows procedure
        static LRESULT CALLBACK WndProcControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    public:

        // Get the status (colors) of the control
        int GetStatus (void);

        // Constructor
        CtrlValue (std::string name, int value, HWND hWndParent);

        // Destructor
        ~CtrlValue (void);

        // Create the control
        HWND Create (int posX, int posY, int sizeX, int sizeY);

        // Get (control)window handle
        HWND GetHWnd (void);
};
