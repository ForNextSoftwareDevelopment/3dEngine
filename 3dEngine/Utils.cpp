#include "utils.h"

namespace Utils
{
    /*********************************************************************
    * Window handle for displaying bmp's
    *********************************************************************/
    HWND hWndBump;

    /*********************************************************************
    * Convert short to a string
    *********************************************************************/
    std::string ShortToStr(short num, unsigned int base)
    {
        std::string str = "";

        short i = 0;
        bool isNegative = false;

        // A zero is same "0" string in all base
        if (num == 0)
        {
            str += "0";
            return str;
        }

        // negative numbers are only handled if base is 10, otherwise considered unsigned number
        if (num < 0 && base == 10)
        {
            isNegative = true;
            num = -num;
        }

        unsigned short number = (unsigned short)num;

        while (number != 0)
        {
            unsigned short rem = number % base;
            char ch = ((rem > 9) ? (rem - 10) + 'A' : rem + '0');
            str += ch;
            number = number / base;
        }

        /* Append negative sign for negative numbers */
        if (isNegative)
        {
            str += '-';
        }

        unsigned int strStart = 0;
        unsigned int strEnd;
        unsigned int strLen = (unsigned int)str.size();

        char temp;
        for (strStart = 0, strEnd = strLen - 1; strStart < strEnd; strStart++, strEnd--)
        {
            temp = str[strStart];
            str[strStart] = str[strEnd];
            str[strEnd] = temp;
        }

        return str;
    }

    /*********************************************************************
    * Convert int to a string
    *********************************************************************/
    std::string IntToStr(int num, unsigned int base)
    {
        std::string str = "";

        int i = 0;
        bool isNegative = false;

        // A zero is same "0" string in all base
        if (num == 0)
        {
            str += "0";
            return str;
        }

        // negative numbers are only handled if base is 10, otherwise considered unsigned number
        if (num < 0 && base == 10)
        {
            isNegative = true;
            num = -num;
        }

        unsigned int number = (unsigned int)num;

        while (number != 0)
        {
            unsigned int rem = number % base;
            char ch = ((rem > 9) ? (rem - 10) + 'A' : rem + '0');
            str += ch;
            number = number / base;
        }

        /* Append negative sign for negative numbers */
        if (isNegative)
        {
            str += '-';
        }

        unsigned int strStart = 0;
        unsigned int strEnd;
        unsigned int strLen = (unsigned int)str.size();

        char temp;
        for (strStart = 0, strEnd = strLen - 1; strStart < strEnd; strStart++, strEnd--)
        {
            temp = str[strStart];
            str[strStart] = str[strEnd];
            str[strEnd] = temp;
        }

        return str;
    }
    
    /*********************************************************************
    * Convert floating point to a string
    *********************************************************************/
    std::string FloatToStr (GLfloat f)
    {
        std::string strFloat;
        int iTemp;
        bool negative = false;
        bool minusset = false;
        bool smallerthenone = false;
        bool billions = false;
        bool hundredmillions = false;
        bool tenmillions = false;
        bool millions = false;
        bool hundredthousands = false;
        bool tenthousands = false;
        bool thousands = false;
        bool hundreds = false;
        bool tens = false;

        // If float is negative the add a minus sign and make it positive
        if (f < 0.0f)
        {
            negative = true;
            f = -f;
        } else strFloat.append(" ");

        // If float is larger then 10 bilion then forget it
        if (f >= 10000000000.0f) return ("Overflow");
        if (f < 1.0f) smallerthenone = true; else strFloat.append(" ");

        if (f >= 1000000000.0f)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // billions
            iTemp = (int)f /1000000000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no billions)
            f = f - (iTemp * 1000000000);

            billions = true;
        } else strFloat.append(" ");

        if ((f >= 100000000.0f) || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // hundredmillions
            iTemp = (int)f /100000000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no hundredmillions)
            f = f - (iTemp * 100000000);

            hundredmillions = true;
        } else strFloat.append(" ");

        if ((f >= 10000000.0f) || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // tenmillions
            iTemp = (int)f /10000000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no tenmillions)
            f = f - (iTemp * 10000000);

            tenmillions = true;
        } else strFloat.append(" ");

        if ((f >= 1000000.0f) || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // millions
            iTemp = (int)f /1000000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no millions)
            f = f - (iTemp * 1000000);

            millions = true;
        } else strFloat.append(" ");

        if ((f >= 100000.0f) || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // hundredthousands
            iTemp = (int)f /100000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no hundredthousands)
            f = f - (iTemp * 100000);

            hundredthousands = true;
        } else strFloat.append(" ");

        if ((f >= 10000.0f) || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // tenthousands
            iTemp = (int)f /10000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no tenthousands)
            f = f - (iTemp * 10000);

            tenthousands = true;
        } else strFloat.append(" ");

        if ((f >= 1000.0f) || tenthousands || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // thousands
            iTemp = (int)f /1000;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no thousands)
            f = f - (iTemp * 1000);

            thousands = true;
        } else strFloat.append(" ");

        if ((f >= 100.0f) || thousands || tenthousands || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // hundreds
            iTemp = (int)f /100;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no hundreds)
            f = f - (iTemp * 100);

            hundreds = true;
        } else strFloat.append(" ");

        if ((f >= 10.0f) || hundreds || thousands || tenthousands || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // tens
            iTemp = (int)f /10;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (no tens)
            f = f - (iTemp * 10);

            tens = true;
        } else strFloat.append(" ");

        if ((f >= 1.0f) || tens || hundreds  || thousands || tenthousands || hundredthousands || millions || tenmillions || hundredmillions || billions)
        {
            // Set minus sign if negative number and not allready set
            if (negative && !minusset)
            {
                strFloat.append("-");
                minusset = true;
            }

            // ones
            iTemp = (int)f;
            strFloat.append(std::to_string(iTemp));

            // Calculate new f (everything after the decimal point)
            f = f - (iTemp);
        } else strFloat.append(" ");

        // Set minus sign if negative number and not allready set
        if (negative && !minusset)
        {
            strFloat.append("-");
            minusset = true;
        }

        // tend's
        if (smallerthenone) strFloat.append("0");
        iTemp = (int)(f * 10.0f);
        strFloat.append(".");
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.1f);

        // hundred's
        iTemp = (int)(f * 100.0f);
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.01f);

        // thousand's
        iTemp = (int)(f * 1000.0f);
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.001f);

        // tenth thousand's
        iTemp = (int)(f * 10000.0f);
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.0001f);

        // hundreds thousand's
        iTemp = (int)(f * 100000.0f);
        strFloat.append(std::to_string(iTemp));

        // Calculate new f
        f = f - (((GLfloat)iTemp) * 0.00001f);

        // millionth
        iTemp = (int)(f * 1000000.0f);
        strFloat.append(std::to_string(iTemp));

        return (strFloat);
    }

    /*********************************************************************
    * Convert a string to a floating point
    *********************************************************************/
    GLfloat StrToFloat (char *pStrFloat)
    {
        GLfloat f = 0.0;
        char *p = pStrFloat;
        bool decimalPointPresent = false;
        bool negativeNumber = false;
        int nBeforeDecimalPoint = 0;
        int nAfterDecimalPoint = 0;

        // Check for minus sign
        if (*p == '-')
        {
            negativeNumber = true;
            pStrFloat++;
            p++;
        }

        // First count the numbers before the decimal point
        while ((*p == '0') || (*p == '1') || (*p == '2') || (*p == '3') || (*p == '4') || (*p == '5') || (*p == '6') || (*p == '7') || (*p == '8') || (*p == '9'))
        {
            nBeforeDecimalPoint++;
            p++;
        }

        // Found dec point
        if (*p == '.')
        {
            decimalPointPresent = true;
            p++;
        }

        // Secondly count the numbers after the decimal point
        while ((*p == '0') || (*p == '1') || (*p == '2') || (*p == '3') || (*p == '4') || (*p == '5') || (*p == '6') || (*p == '7') || (*p == '8') || (*p == '9'))
        {
            nAfterDecimalPoint++;
            p++;
        }

        // Add number before decimal point
        for (int i=0; i<nBeforeDecimalPoint; i++)
        {
            float factor = 1.0f;
            for (int j=nBeforeDecimalPoint; j>(i+1); j--) factor = factor * 10.0f;

            f = f + factor * (*pStrFloat - '0');
            pStrFloat++;
        }

        // Add number after decimal point
        if (decimalPointPresent)
        {
            // Skip decimal point
            pStrFloat++;

            // Convert all numbers after the decimal point to float
            for (int i=0; i<nAfterDecimalPoint; i++)
            {
                float factor = 0.1f;
                for (int j=0; j<i; j++) factor = factor * 0.1f;

                f = f + factor * (*pStrFloat - '0');
                pStrFloat++;
            }
        }

        if (negativeNumber) f = -f;
        return (f);
    }

    /*********************************************************************
    * Create a random float between 0.0f and 1.0f
    *********************************************************************/
    float Random (void)
    {
        float res;
        unsigned int tmp;
        static unsigned int seed = 0xFFFF0C59;

        seed *= 16807;

        tmp = seed ^ (seed >> 4) ^ (seed << 15);

        *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

        return (float)(res - 1.0);
    }

    /*********************************************************************
    * Convert WCST to std::string
    *********************************************************************/
    std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen)
    {
        int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

        std::string dblstr(len, '\0');
        len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
            pstr, wslen /* not necessary NULL-terminated */,
            &dblstr[0], len,
            NULL, NULL /* no default char */);

        return dblstr;
    }

    /*********************************************************************
    * Show bmp file with provided raw data
    *********************************************************************/
    bool ShowBmp(std::wstring name, int sizeX, int sizeY, char* data)
    {
        // Error message
        std::string message;

        // Offset to view properly on screen
        int offset = 0;

        // Result writing bmp
        bool result = false;

        try
        {
            // Create window (if nescessary)
            if (hWndBump == NULL)
            {
                HINSTANCE hInstance = GetModuleHandle(0);

                // Create window class for this window
                WNDCLASS wndClass = { 0 };

                wndClass.style = CS_HREDRAW | CS_VREDRAW;
                wndClass.lpfnWndProc = (WNDPROC)WndProcInfo;
                wndClass.cbClsExtra = 0;
                wndClass.cbWndExtra = 0;
                wndClass.hInstance = hInstance;
                wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
                wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
                wndClass.hbrBackground = GetSysColorBrush(COLOR_INACTIVECAPTION);
                wndClass.lpszMenuName = L"ShowBump";
                wndClass.lpszClassName = L"ShowBump";
                RegisterClass(&wndClass);

                // Create and show info window
                hWndBump = CreateWindow(L"ShowBump", L"ShowBump", WS_DLGFRAME, 810, 10, sizeX + 8, sizeY + 10, NULL, NULL, hInstance, NULL);

                ShowWindow(hWndBump, SW_SHOWNORMAL);
                UpdateWindow(hWndBump);
            }

            // Create Bitmap info-header
            BITMAPINFOHEADER bmih;
            bmih.biSize = sizeof(BITMAPINFOHEADER);     // size of the structure = 40
            bmih.biWidth = sizeX;                       // width of the image in pixels
            bmih.biHeight = sizeY;                      // height of the image in pixels
            bmih.biPlanes = 1;                          // = 1
            bmih.biBitCount = 24;                       // bits per pixel (1, 4, 8, 16, 24, or 32)
            bmih.biCompression = BI_RGB;                // compression code
            bmih.biSizeImage = 0;                       // the size of the DIB pixel data in bytes
            bmih.biXPelsPerMeter = 72;                  // horizontal resolution
            bmih.biYPelsPerMeter = 72;                  // vertical resolution
            bmih.biClrUsed = 0;                         // number of colors used (0 for true-color)
            bmih.biClrImportant = 0;                    // number of important colors

            // Get device context
            HDC hdcWindow = GetWindowDC(hWndBump);

            // Create a Device Independent Bitmap
            void *pBits;
            HBITMAP hBitmap = CreateDIBSection(NULL, (BITMAPINFO *)&bmih, 0, &pBits, NULL, 0);

            // Copy data to DIB (mirrored in y-direction)
            char *ptr = (char*)pBits;
            for (int i = 0; i < sizeY; i++)
            {
                for (int j = 0; j < sizeX; j++)
                {
                    *ptr++ = data[(i * sizeX * 3) + (j * 3)];
                    *ptr++ = data[(i * sizeX * 3) + (j * 3) + 1];
                    *ptr++ = data[(i * sizeX * 3) + (j * 3) + 2];
                };
            };

            // Create memory device context
            HDC hdcMemory = CreateCompatibleDC(hdcWindow);
            SelectObject(hdcMemory, hBitmap);

            // Flush
            GdiFlush();

            // Copy data from the memory device to the screen device
            BitBlt(hdcWindow, offset, offset, sizeX, sizeY, hdcMemory, 0, 0, SRCCOPY);

            // Free bitmap memory / dc
            DeleteObject(hBitmap);
            ReleaseDC(hWndBump, hdcWindow);
            DeleteDC(hdcMemory);

            // File shown ok
            result = true;

        } catch (...)
        {
            std::string message;
            message.append("Could not show BMP file: ");
            message.append(Utils::ConvertWCSToMBS(name.c_str(), (long)wcslen(name.c_str())));
            Error::WriteLog("ERROR", "Utils::ShowBmp", message.c_str());
            return (false);
        };

        return (result);
    }

    /*********************************************************************
    * Create bmp file with provided raw data
    *********************************************************************/
    bool WriteBmp(std::wstring name, int sizeX, int sizeY, char* data)
    {
        // Error message
        std::string message;

        // Result writing bmp
        bool result = false;
        unsigned long bytesWritten;

        try
        {
            // Add file extension to name
            name.append(L".bmp");

            // Create (and open) file
            HANDLE hFile = CreateFile((L"textures\\" + name).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE) return false;

            // Create Bitmap file-header
            BITMAPFILEHEADER bmfh;
            bmfh.bfType = *(WORD *) "BM";                                         // signature word "BM" or 0x4D42
            bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeX * sizeY * 3;           // entire size of file
            bmfh.bfReserved1 = 0;                                                 // must be zero
            bmfh.bfReserved2 = 0;                                                 // must be zero
            bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // offset in file of DIB pixel bits

            // Write the BITMAPFILEHEADER
            result = WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &bytesWritten, NULL);
            if (!result || (bytesWritten != sizeof(BITMAPFILEHEADER)))
            {
                CloseHandle(hFile);
                DeleteFile(name.c_str());
                return false;
            };

            // Create Bitmap info-header
            BITMAPINFOHEADER bmih;
            bmih.biSize = sizeof(BITMAPINFOHEADER);     // size of the structure = 40
            bmih.biWidth = sizeX;                       // width of the image in pixels
            bmih.biHeight = sizeY;                      // height of the image in pixels
            bmih.biPlanes = 1;                          // = 1
            bmih.biBitCount = 24;                       // bits per pixel (1, 4, 8, 16, 24, or 32)
            bmih.biCompression = BI_RGB;                // compression code
            bmih.biSizeImage = 0;                       // the size of the DIB pixel data in bytes
            bmih.biXPelsPerMeter = 72;                  // horizontal resolution
            bmih.biYPelsPerMeter = 72;                  // vertical resolution
            bmih.biClrUsed = 0;                         // number of colors used (0 for true-color)
            bmih.biClrImportant = 0;                    // number of important colors

            // Write the BITMAPINFOHEADER
            result = WriteFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &bytesWritten, NULL);
            if (!result || (bytesWritten != sizeof(BITMAPINFOHEADER)))
            {
                CloseHandle(hFile);
                DeleteFile(name.c_str());
                return false;
            };

            // Write the image data
            result = WriteFile(hFile, data, sizeX * sizeY * 3, &bytesWritten, NULL);
            if (!result || (bytesWritten != ((unsigned int)sizeX * (unsigned int)sizeY * 3)))
            {
                CloseHandle(hFile);
                DeleteFile(name.c_str());
                return false;
            };

            // Close file
            CloseHandle(hFile);

            // File written ok
            result = true;

        } catch (...)
        {
            std::string message;
            message.append("Could not write BMP file: ");
            message.append(Utils::ConvertWCSToMBS(name.c_str(), (long)wcslen(name.c_str())));
            Error::WriteLog("ERROR", "Utils::WriteBmp", message.c_str());
            return (false);
        };

        return (result);
    }

    /*********************************************************************
    * Windows procedure (handle messages for the 'Bump View' window)
    *********************************************************************/
    LRESULT CALLBACK WndProcInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_CREATE:
                break;

            case WM_SIZE:
                break;

            case WM_MOUSEWHEEL:
                break;

            case WM_KEYDOWN:
                switch (wParam)
                {
                    case VK_ESCAPE:
                        DestroyWindow(hWnd);
                        hWnd = NULL;
                        break;

                    case 'Z':
                        DestroyWindow(hWnd);
                        hWnd = NULL;
                        break;
                }
                break;

            case WM_DESTROY:
                DestroyWindow(hWnd);
                hWnd = NULL;
                break;

            case WM_CLOSE:
                DestroyWindow(hWnd);
                hWnd = NULL;
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return (0);
    }
}

