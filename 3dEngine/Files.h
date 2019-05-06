#pragma once
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <string>

class Files
{
    private:

        // File structure
        static OPENFILENAME ofn;

    public:

        // Forward declarations
        static char* ReadASCII  (const char* pFileName);
        static char* ReadBinary (const char* pFileName, unsigned int *pFileSize);
        static bool Write       (const char *pFileName, char *pBuffer, int iFileLength);
};
