#pragma once
#include <windows.h>
#include "GL/glew.h"
#include "Error.h"

class OpenGL
{
    private:

		// Render context (only one for screen and HDK2, presuming using one graphics card)
		static HGLRC hRC;

		// Window handle
        HWND hWnd;

        // Device context
        HDC hDC;

        // Background color
        GLclampf red, green, blue;

    public:

        // Constructor
        OpenGL (HWND hWnd);

        // Destructor
        ~OpenGL (void);

        // Clear screen
        void ClearScreen (void);

        // Set background color
        void SetBackgroundColor(int red, int green, int blue);

        // Resize screen
        void ResizeScreen (int width, int height);

        // Swap (screen-) buffers
        void SwapBuffers (void);

		// Make this render context the current one
		bool SetCurrentContext(void);
};

