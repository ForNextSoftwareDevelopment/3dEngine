#include "OpenGL.h"

// Render context (only one for screen and HDK2, presuming using one graphics card)
HGLRC OpenGL::hRC;

/*********************************************************************
* Constructor
*********************************************************************/
OpenGL::OpenGL (HWND hWnd)
{
    PIXELFORMATDESCRIPTOR pfd;

    // Set initial background color
    SetBackgroundColor(0x11, 0x11, 0xCC);

    // Copy window handle for which to set up OpenGL
    this->hWnd = hWnd;

    // get the device context (DC)
    hDC = GetDC(hWnd);

    // Fill pixel format descriptor
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize               = sizeof(pfd);
    pfd.nVersion            = 1;
    pfd.dwFlags             = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
    pfd.iPixelType          = PFD_TYPE_RGBA;
    pfd.cColorBits          = 24;
    pfd.cRedBits            = 8;
    pfd.cGreenBits          = 8;
    pfd.cBlueBits           = 8;
    pfd.cDepthBits          = 24;

    // Choose and set pixel format
    int iPixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, iPixelFormat, &pfd);

    // Create and enable the render context (RC)
    if (hRC == NULL) hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
    
    // Init glew (GL Extensions wrangler) so we know what openGL version to use
    GLenum retCode = glewInit();
    if (retCode != GLEW_OK)
    {
        // Check for OpenGL errors and report in info file
        Error::CheckOpenGLError("OpenGL::OpenGL");
        exit(-1);
    }

    // Check OpenGL version
    std::string s;
    s.append("OpenGL / Graphics Card:\r\nVersion: ");
    int major, minor, dot;
    s.append((const char*) glGetString(GL_VERSION));
    dot = (int)s.find_first_of('.');

    if (dot != 0)
    {
        major = s[dot-1]-'0';
        minor = s[dot+1]-'0';

        s.append("\r\nVendor: ");
        s.append((const char*) glGetString(GL_VENDOR));
        s.append("\r\nRenderer: ");
        s.append((const char*) glGetString(GL_RENDERER));

        if ((major >= 4) || ((major >= 3) && (minor >= 1)))
        {
            s.append("\r\nShading Language Version: ");
            s.append((const char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
        }

        // Write in log file
        Error::WriteLog("INFO", "OpenGL::OpenGL", s.c_str());

        if ((major >= 4) && (minor >= 1))
        {
            // Write in log file
            Error::WriteLog("INFO", "OpenGL::OpenGL", "OpenGL version OK");

            // Set point size
            glPointSize(2.0f);

            // Enable depth testing (check if the pixel to be drawn is in front of other pixels at the same location)
            glEnable (GL_DEPTH_TEST);
            glDepthFunc (GL_LEQUAL);
        } else
        {
            // Write in log file
            Error::WriteLog("INFO", "OpenGL version:", "OpenGL version too low (below 4.1) => Sorry, not OK => Program Exit");
        }
    } else
    {
        // Write in log file
        Error::WriteLog("INFO", "OpenGL version:", "No OpenGL found => Sorry, not OK => Program Exit");
    }
}

/*********************************************************************
* Destructor
*********************************************************************/
OpenGL::~OpenGL (void)
{
    // Shutdown OpenGL
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}

/*********************************************************************
* Resize window
*********************************************************************/
void OpenGL::ResizeScreen (int width, int height)
{
    glViewport(0, 0, width, height);
}

/*********************************************************************
* Swap (screen-) buffers
*********************************************************************/
void OpenGL::SwapBuffers (void)
{
    ::SwapBuffers (hDC);
}

/*********************************************************************
* Clear the screen
*********************************************************************/
void OpenGL::ClearScreen()
{
    // Blue background
    glClearColor(red, green, blue, 1.0f);

    // Set default depth buffer value
    glClearDepth(1.0f);

    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Check for OpenGL errors and report in info file
    Error::CheckOpenGLError("OpenGL::ClearScreen");
}

/*********************************************************************
* Set background color
*********************************************************************/
void OpenGL::SetBackgroundColor(int red, int green, int blue)
{
    this->red = (float)red / 255.0f;
    this->green = (float)green / 255.0f;
    this->blue = (float)blue / 255.0f;
}

/*********************************************************************
* Make this render context the current one
*********************************************************************/
bool OpenGL::SetCurrentContext()
{
    return (wglMakeCurrent(hDC, hRC));
}
