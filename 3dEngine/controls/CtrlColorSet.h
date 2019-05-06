#pragma once
#include <windows.h>
#include <commdlg.h>
#include <map>
#include "../error.h"

struct Color
{
    int r;
    int g;
    int b;
    int a;
};

class CtrlColorSet
{
    private:

        // Window handles
        HWND hWndParent;
        HWND hWndControl;

        // Control name
        std::wstring name;

        // (Initial) Status of the control
        Color color;

        // Pointer to image file buffer
        static char *pBuffer;

        // Pointer to begin of bitmap
        static BITMAPFILEHEADER *pBmFh;

        // Collection of window handles and status of the control
        static std::map<HWND, Color> mapColor;

        // Windows procedure
        static LRESULT CALLBACK WndProcControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    public:

        // Get the status (colors) of the control
        Color GetStatus (void);

        // Constructor
        CtrlColorSet (std::string name, Color color, HWND hWndParent);

        // Destructor
        ~CtrlColorSet (void);

        // Create the control
        HWND Create (int posX, int posY, int sizeX, int sizeY);

        // Get (control)window handle
        HWND GetHWnd (void);
};
