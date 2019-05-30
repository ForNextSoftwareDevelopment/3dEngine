#pragma once
#include <windows.h>
#include <cstdio>
#include <ctime>
#include <string>
#include "Error.h"
#include "VecMat.h"
#include "GL/glew.h"

namespace Utils
{
    // Window handle for displaying bmp's
    extern HWND hWndBump;

    // Convert short to a string
    std::string ShortToStr(short num, unsigned int base = 10);

    // Convert integer to a string
    std::string IntToStr(int i, unsigned int base = 10);

    // Convert floating point to a string
    std::string FloatToStr (GLfloat f);

    // Convert a string to a floating point
    GLfloat StrToFloat (char *pStrFloat);

    // Convert WCST to std::string
    std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen);
    
    // Show bmp file with provided raw data
    bool ShowBmp(std::wstring name, int sizeX, int sizeY, char* data);

    // Write bmp file with provided raw data
    bool WriteBmp(std::wstring name, int sizeX, int sizeY, char* data);

    // Windows procedure (handle messages for the 'Bump View' window)
    LRESULT CALLBACK WndProcInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // Create a random float
    float Random(void);
};

