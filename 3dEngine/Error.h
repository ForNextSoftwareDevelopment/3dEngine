#pragma once
#include <windows.h>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>
#include <exception>
#include "GL/glew.h"
#include <GL/gl.h>
#include "resource.h"
#include "Files.h"
#include "Utils.h"
#include "VecMat.h"

// Switch for debugging (shows info window)
#define DEBUG
#undef  DEBUGNORMALMAP
#undef  DEBUGHID
#undef  DEBUGHDK
#undef  DEBUGTRIANGULATE

// Max number of lines before clearing
#define MAXLINES 200

namespace Error
{
    // Info window handle
    extern HWND hWndInfoWindow;

    // Application folder + logfile name
    extern std::string logFilePath;

    // Messages to log / display
    extern std::vector<std::string> info;

    // Scrollbar position (vertical)
    extern int scrollPos;

    // Set logfile location
    void SetLogFilePath(std::string path);

    // Write in logfile
    void WriteLog(const char *type, const char *className, const char *message, bool showInfoWindow = true);

    // Reset OpenGL errors (just retrieve them until there are none left)
    void ResetOpenGLError(void);

    // Check OpenGL error
    void CheckOpenGLError (const char *className);

    // Show message info window on screen (float)
    void ShowInfoWindow (float f);

    // Show message info window on screen (integer)
    void ShowInfoWindow (int i);

     // Show message info window on screen (ascii)
    void ShowInfoWindow (std::string message);

     // Show message info window on screen (ascii)
    void ShowInfoWindow (const char *message);

    // Show message info window on screen (vector)
    void ShowInfoWindow(VecMat::Vec4 vec4);

    // Show message info window on screen (matrix)
    void ShowInfoWindow(VecMat::Mat4 mat4);

    // Clear all messages in info window
    void ClearInfoWindow (void);

    // Windows procedure (handle messages for the 'info' window)
    LRESULT CALLBACK WndProcInfo (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

