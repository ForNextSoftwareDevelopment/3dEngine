#include "error.h"

namespace Error
{
    /*********************************************************************
    * Info Window handle
    *********************************************************************/
    HWND hWndInfoWindow;

    /*********************************************************************
    * Application folder + logfile name
    *********************************************************************/
    std::string logFilePath;

    /*********************************************************************
    * Messages to log / display
    *********************************************************************/
    std::vector<std::string> info;

    /*********************************************************************
    * Scrollbar position (vertical)
    *********************************************************************/
    int scrollPos;

    /*********************************************************************
    * Set logfile location
    *********************************************************************/
    void SetLogFilePath (std::string path)
    {
        logFilePath = path;
    }

	/*********************************************************************
	* Write in logfile (and in info window if available)
	*********************************************************************/
	void WriteLog(const char *type, const char *className, const char *message, bool showInfoWindow)
	{
		// Open debug log file
		FILE *logFile = NULL;
		time_t t;
		std::string strTime;
		std::string strMessage;

		char buf[26];
		time(&t);
		ctime_s(buf, 26, &t);
		strTime.append(buf);

		// Remove last '\n' from the string (returned from ctime)
		strTime[strTime.length() - 1] = ' ';

		strMessage.append("***");
		strMessage.append(type);
		strMessage.append("***\r\n");
		strMessage.append("TIMESTAMP: ");
		strMessage.append(strTime);
		strMessage.append("\r\n");
		strMessage.append("LOCATION : ");
		strMessage.append(className);
		strMessage.append("\r\n");
		strMessage.append("INFO     : ");
		strMessage.append(message);
		strMessage.append("\r\n--------------------------------------------------------------------------\r\n");

		// Show log file in info window
		#ifdef DEBUG
			if (showInfoWindow) Error::ShowInfoWindow(strMessage.c_str());
		#endif // DEBUG

		// Write info message to log file
		std::replace(strMessage.begin(), strMessage.end(), '\r', ' ');
		bool result = fopen_s(&logFile, logFilePath.c_str(), "a");
		if (!result)
		{
			fputs(strMessage.c_str(), logFile);

			// Close file
			fclose(logFile);
		}
	}

	/*********************************************************************
	* Clear info window
	*********************************************************************/
	void ClearInfoWindow(void)
	{
		info.clear();
	}

    /*********************************************************************
    * Reset OpenGL errors (just retrieve them until there are none left)
    *********************************************************************/
    void ResetOpenGLError (void)
    {
        int error = glGetError();
        while (error != GL_NO_ERROR)
        {
            // Check for next error (if any)
            error = glGetError();
        }
    }

    /*********************************************************************
    * Check for OpenGL error
    *********************************************************************/
    void CheckOpenGLError (const char *className)
    {
        int error = glGetError();
        while (error != GL_NO_ERROR)
        {
            std::string strMessage;

            switch (error)
            {
                case GL_INVALID_ENUM:
                    strMessage.append("GL_INVALID_ENUM");
                    break;

                case GL_INVALID_VALUE:
                    strMessage.append("GL_INVALID_VALUE");
                    break;

                case GL_INVALID_OPERATION:
                    strMessage.append("GL_INVALID_OPERATION");
                    break;

                case GL_STACK_OVERFLOW:
                    strMessage.append("GL_STACK_OVERFLOW");
                    break;

                case GL_STACK_UNDERFLOW:
                    strMessage.append("GL_STACK_UNDERFLOW");
                    break;

                case GL_OUT_OF_MEMORY:
                    strMessage.append("GL_OUT_OF_MEMORY");
                    break;

                default:
                    strMessage.append("Unknown OpenGL error");
                    break;
            }

            // Write to log file
            WriteLog ("OPENGL-ERROR", className, strMessage.c_str());

            // Check for next error (if any)
            error = glGetError();
        }
    }

    /*********************************************************************
    * Show message info window on screen (float)
    *********************************************************************/
    void ShowInfoWindow (float f)
    {
        std::string temp;
        temp.append(Utils::FloatToStr(f).c_str());
        Error::ShowInfoWindow(temp.c_str());
    }

    /*********************************************************************
    * Show message info window on screen (integer)
    *********************************************************************/
    void ShowInfoWindow (int i)
    {
        Error::ShowInfoWindow(std::to_string(i).c_str());
    }

    /*********************************************************************
    * Show message info window on screen (ascii)
    *********************************************************************/
    void ShowInfoWindow (std::string message)
    {
        ShowInfoWindow (message.c_str());
    }

    /*********************************************************************
    * Show message info window on screen (ascii)
    *********************************************************************/
    void ShowInfoWindow (const char *message)
    {
        // Create info dialog window (if needed)
        if (hWndInfoWindow == NULL)
        {
            HINSTANCE hInstance = GetModuleHandle(0);

            // Create window class for this window
			WNDCLASS wndClass =		{ 0 };

            wndClass.style          = CS_HREDRAW | CS_VREDRAW;
            wndClass.lpfnWndProc    = (WNDPROC) WndProcInfo;
            wndClass.cbClsExtra     = 0;
            wndClass.cbWndExtra     = 0;
            wndClass.hInstance      = hInstance;
            wndClass.hIcon          = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_ICON));
            wndClass.hCursor        = LoadCursor (NULL, IDC_ARROW);
            wndClass.hbrBackground  = GetSysColorBrush(COLOR_WINDOW);
            wndClass.lpszMenuName   = L"Info";
            wndClass.lpszClassName  = L"Info";
            RegisterClass (&wndClass);

            // Create and show info window
            hWndInfoWindow = CreateWindow (L"Info", L"Info", WS_DLGFRAME, 810, 10, 1000, 600, NULL, NULL, hInstance, NULL);

            ShowWindow(hWndInfoWindow, SW_SHOWNORMAL);
            UpdateWindow(hWndInfoWindow);
        }

        // Add text
        if ((message != NULL) && (*message != 0))
        {
            std::string strMessage;
            strMessage.append(message);
            strMessage.append("\r\n");

            unsigned int beginPos = 0;
            unsigned int endPos = (int)strMessage.find("\r\n", beginPos);
            while (endPos < strMessage.size())
            {
                std::string strSubMessage = strMessage.substr(beginPos, endPos - beginPos);
                info.push_back(strSubMessage);

                beginPos = endPos + 2;
                endPos = (int)strMessage.find("\r\n", beginPos);
            }
        } else
        {
            info.push_back("EMPTY MESSAGE SEND");
        }

        scrollPos = (std::max) ((int)info.size() - 30, 0);
        SetScrollRange (hWndInfoWindow, SB_VERT, 0, scrollPos, FALSE);
        SetScrollPos (hWndInfoWindow, SB_VERT, scrollPos, TRUE);

        ShowWindow(hWndInfoWindow, SW_SHOWNORMAL);
        UpdateWindow(hWndInfoWindow);
        InvalidateRect (hWndInfoWindow, NULL, TRUE);
    }

    /*********************************************************************
    * Windows procedure (handle messages for the 'info' window)
    *********************************************************************/
    LRESULT CALLBACK WndProcInfo (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        static int cyChar, cyClient;

        unsigned int i, y;

        HDC hdc;
        RECT rect;
        HFONT hFont;
        PAINTSTRUCT ps;
        TEXTMETRIC tm;

        switch (message)
        {
            case WM_CREATE:
                hdc = GetDC (hWnd);

                GetTextMetrics (hdc, &tm);
                cyChar = tm.tmHeight + tm.tmExternalLeading;
                ReleaseDC (hWnd, hdc);

                scrollPos = 0;
                SetScrollRange (hWnd, SB_VERT, 0, 0, FALSE);
                SetScrollPos (hWnd, SB_VERT, 0, TRUE);
                break;

            case WM_SIZE:
                cyClient = HIWORD (lParam);
                break;

            case WM_MOUSEWHEEL:

                // Up
                if (SHORT(HIWORD (wParam)) == 120)
                {
                    scrollPos -= 1;
                }

                // Down
                if (SHORT(HIWORD (wParam)) == -120)
                {
                    scrollPos += 1;
                }

                // Get control rect size
                GetClientRect (hWnd, &rect);

                // Redraw rect
                InvalidateRect(hWnd, &rect, true);

                scrollPos = (std::max)  (0, (std::min) (scrollPos, (int)info.size() - 30));
                if (scrollPos != GetScrollPos (hWnd, SB_VERT))
                {
                    SetScrollPos (hWnd, SB_VERT, scrollPos, TRUE);
                    InvalidateRect (hWnd, NULL, TRUE);
                }
                break;

            case WM_VSCROLL:
                switch (LOWORD (wParam))
                {
                    case SB_LINEUP:
                        scrollPos -= 1;
                        break;

                    case SB_LINEDOWN:
                        scrollPos += 1;
                        break;

                    case SB_PAGEUP:
                        scrollPos -= cyClient / cyChar;
                        break;

                    case SB_PAGEDOWN:
                        scrollPos += cyClient / cyChar;
                        break;

                    case SB_THUMBPOSITION:
                        scrollPos = HIWORD (wParam);
                        break;

                    default:
                        break;
                }

                scrollPos = (std::max)  (0, (std::min) (scrollPos, (int)info.size() - 30));
                if (scrollPos != GetScrollPos (hWnd, SB_VERT))
                {
                    SetScrollPos (hWnd, SB_VERT, scrollPos, TRUE);
                    InvalidateRect (hWnd, NULL, TRUE);
                }
                break;

            case WM_PAINT:
                hdc = BeginPaint (hWnd, &ps);
                SetTextAlign (hdc, TA_LEFT | TA_TOP);
                SetBkMode (hdc, TRANSPARENT);
                SetTextColor (hdc, RGB(0x00, 0x00, 0x00));
                GetClientRect (hWnd, &rect);

                hFont = CreateFont (24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Fixedsys"));
                SendMessage(hWnd, WM_SETFONT, (WPARAM) hFont, TRUE);
                SelectObject (hdc, hFont);

                for (i = scrollPos; i < info.size() ; i++)
                {
                    y = cyChar * (i - scrollPos);
                    std::wstring widestr = std::wstring(info[i].begin(), info[i].end());
                    TextOut (hdc, 4, y, widestr.c_str(), (int)widestr.size());
                }

                DeleteObject(hFont);
                EndPaint (hWnd, &ps) ;

                break;

            case WM_DESTROY:
                PostQuitMessage (0) ;
                break;

            case WM_CLOSE:

                EndDialog (hWnd, FALSE) ;
                hWndInfoWindow = NULL;
                break;

            default:
                return DefWindowProc (hWnd, message, wParam, lParam);
        }

        return (0);
    }
}
