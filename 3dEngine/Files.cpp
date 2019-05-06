#include "Files.h"

/*********************************************************************
* Read ASCII file
*********************************************************************/
char* Files::ReadASCII(const char* pFileName)
{
    // Open the file:
    std::streampos fileSize;
    std::ifstream file(pFileName, std::ifstream::binary);

    // Bad file(name)
    if (file.fail())
    {
        return (NULL);
    }

    // Get its size:
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the data
    char* fileData = new char[(int)fileSize + 1];
    file.read(fileData, fileSize);

    // Finish with 0
    fileData[(int)fileSize] = 0x00;

    // Close stream
    file.close();

    // Return pointer to binary data
    return fileData;
}

/*********************************************************************
* Read binary file
*********************************************************************/
char* Files::ReadBinary (const char* pFileName, unsigned int *pFileSize)
{
    // Open the file:
    std::streampos fileSize;
    std::ifstream file(pFileName, std::ios::binary);

    // Bad file(name)
    if (file.fail())
    {
        *pFileSize = 0;
        return (NULL);
    }

    // Get its size:
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the data
    char* fileData = new char[fileSize];
    file.read(fileData, fileSize);

    // Close stream
    file.close();

    // Set filesize
    *pFileSize = (unsigned int) fileSize;

    // Return pointer to binary data
    return fileData;
}

/*********************************************************************
* Write file
*********************************************************************/
bool Files::Write (const char *pFileName, char *pBuffer, int iFileLength)
{
    unsigned long dwBytesWritten;
    HANDLE hFile;

    // Open the file, creating it if necessary
    hFile = CreateFile ((LPWSTR)pFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;

    // Write the buffer out to the file.
    WriteFile (hFile, pBuffer, iFileLength * sizeof (char), &dwBytesWritten, NULL) ;
    if (((unsigned int) iFileLength * sizeof (char)) != (unsigned int) dwBytesWritten)
    {
        CloseHandle (hFile) ;
        return false;
    }

    CloseHandle (hFile) ;
    return true;
}


