#pragma once
#include <windows.h>
#include <commdlg.h>
#include <map>
#include "../error.h"

class CtrlPosSet
{
    private:

        // Window handles
        HWND hWndParent;
        HWND hWndControl;

        // Control name
        std::wstring name;

        // (Initial) Status of the control
        VecMat::Vec3 pos;

        // Pointer to image file buffer
        static char *pBuffer;

        // Pointer to begin of bitmap
        static BITMAPFILEHEADER *pBmFh;

        // Collection of window handles and status of the control
        static std::map<HWND, VecMat::Vec3> mapPos;

        // Windows procedure
        static LRESULT CALLBACK WndProcControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    public:

        // Get the status (colors) of the control
        VecMat::Vec3 GetStatus (void);

        // Constructor
        CtrlPosSet (std::string name, VecMat::Vec3 pos, HWND hWndParent);

        // Destructor
        ~CtrlPosSet (void);

        // Create the control
        HWND Create (int posX, int posY, int sizeX, int sizeY);

        // Get (control)window handle
	    HWND GetHWnd(void);
};
