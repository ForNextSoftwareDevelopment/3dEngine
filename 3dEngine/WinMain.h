#pragma once

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN             

// Windows Header Files
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// Project Header Files
#include "OpenGL.h"
#include "Shader.h"
#include "Settings.h"
#include "Hid.h"
#include "Resource.h"

// Screen defines
#define SCREEN_WIDTH_MAIN 800
#define SCREEN_HEIGHT_MAIN 600

// Multiple monitors 
#define MAX_MONITOR 2
typedef struct 
{
    unsigned char num;
    RECT          rect;
} MONITOR;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool    CALLBACK    MonitorEnumProc(HMONITOR hMonitor, HDC hDC, LPRECT lpRect, LPARAM lParam);

