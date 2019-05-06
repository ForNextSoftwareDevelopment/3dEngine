#include "WinMain.h"

// Exit program flag
static bool exit_program = false;

// All render preferences in a structure
static struct struct_prefs prefs;

// Application folder
static char appDir[MAX_PATH];

// Mouse button down signal flag
static bool rightMouseButtonDown = false;
static bool leftMouseButtonDown = false;

// Coordinates for the mouse position and mouse wheel direction
static int mouseX = 0, mouseY = 0;
static int mouseOldX = 0, mouseOldY = 0;
static bool mouseUp = false, mouseDown = false;
static bool up = false, down = false;
static bool low = false, high = false;
static bool left = false, right = false;

// Stepsize for changing the position coordinates
static float delta = 0.05f;

// Move and/or rotate scene (or just move mouse pointer)
static bool move_rotate = true;

// Show status info in info window
static bool show_info = false;

// Start saving the objects
static bool save = false;

// Set test conditions in shader
static bool test = false;

// Counter for numbers of displays (monitors) in the system
static unsigned char numMonitor = 0;

// HDK2 looks backwards (angle around y-axis is more the 180 degrees)
bool backwards = false;
float oldPitch = 0;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // Main window
    WNDCLASS wndClass, wndClassExtended;
    HWND hWndMain, hWndExtendedLeft = NULL, hWndExtendedRight = NULL;
    MSG msg;

    // Settings
    Settings *pSettings = NULL;

    // Objects used in the shader (meshes, materials etc.)
    Objects *pObjects = NULL;

    // OpenGL context
    OpenGL *pOpenGL = NULL;
    OpenGL *pOpenGLExtendedLeft = NULL;
    OpenGL *pOpenGLExtendedRight = NULL;

    // Shader context
    Shader *pShader = NULL;

    // Set initial preferences
    prefs.show_textures = true;
    prefs.show_normalmap = true;
    prefs.repeat_textures = true;
    prefs.filter_textures = true;
	prefs.show_outlining = false;
	prefs.show_axis = true;
	prefs.render_mode = RM_FACES;
    prefs.vertex_size = 1;
	prefs.ambient_light = { 0xFF, 0xFF, 0xFF, 0x10 };
	prefs.diffuse_light = { 0xFF, 0xFF, 0xFF, 0x18 };
    prefs.diffuse_position = VecMat::Vec3(0.0f, 8.0f, 4.0f);
    prefs.pos_farplane = FARPLANEPOS;
    prefs.pos_nearplane = NEARPLANEPOS;
	prefs.hdk_offset_pos = DEFAULTOFFSETPOS;
	prefs.hdk_offset_angle = DEFAULTOFFSETANGLE;

    // Set application folder
    GetCurrentDirectoryA(sizeof(appDir), appDir);

    // Set Error logfile path
    std::string logFilePath;
    logFilePath.append(appDir);
    logFilePath.append("\\log.txt");
    Error::SetLogFilePath(logFilePath);

    // Reset log file
    FILE *logFile = NULL;
    bool result = fopen_s(&logFile, logFilePath.c_str(), "w");
    if (!result) fclose(logFile);

    // Clear windows class data
    ZeroMemory(&wndClass, sizeof(wndClass));

    // Set window class
    wndClass.hInstance = hInstance;
    wndClass.lpfnWndProc = (WNDPROC)WndProc;
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClass.lpszClassName = L"3dEngineOSVR";
    wndClass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wndClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

    // Register window class
    RegisterClass(&wndClass);

    // Create main window and show
    hWndMain = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, L"3dEngineOSVR", L"3dEngineOSVR", 
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        10, 10, SCREEN_WIDTH_MAIN, SCREEN_HEIGHT_MAIN, NULL, NULL, hInstance, NULL);

    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    // Create OpenGL context
    pOpenGL = new OpenGL(hWndMain);

	// Check OpenGL version
	if (!GLEW_VERSION_4_2)
	{
		// Exit
		Error::WriteLog("ERROR", "WinMain::WinMain", "OpenGL version 4.2 or higher needed");
		MessageBox(NULL, L"OpenGL version 4.2 or higher needed", L"ERROR", MB_OK | MB_ICONERROR);
		exit(0);
	}

	// Set wait cursor, could be a while reading all objects
	SetCursor(LoadCursor(hInstance, IDC_WAIT));

	// Setup elements to render / use as material etc.
	pObjects = new Objects(appDir);

	// Set normal cursor
	SetCursor(LoadCursor(hInstance, IDC_ARROW));

	// Create shader (renderer) for version 4.2 or later
	pShader = new Shader(pObjects);
	
	// Set angle so that it looks like a persons view
	pShader->SetViewAngle(0.0f, 0.0f, 0.0f);

    // Set initial view position
    pShader->SetViewPosition(0.0f, -3.0f, -20.0f);

	// Create USB class to get hdk2 device
	Hid *hid = new Hid();
	std::string info = hid->DeviceInfo();
	Error::WriteLog("INFO", "WinMain", info.c_str(), false);

	#ifdef DEBUGHID
		Error::ShowInfoWindow(info);
	#endif

	// Give warning messagebox if HDK2 is not found
	if (!hid->HDK2)
	{
		//MessageBox(NULL, L"HDK2 Device not found", L"WARNING", MB_OK | MB_ICONEXCLAMATION);
	}

	// Get display info (look for multiple displays, first two found will be marked as primary and secundary)
	numMonitor = 0;
	MONITOR monitor[MAX_MONITOR];
	memset(monitor, 0x00, MAX_MONITOR * sizeof(MONITOR));
	#ifdef DEBUGHDK
		Error::ShowInfoWindow("Display (Monitor) Info:\r\n");
	#endif
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)MonitorEnumProc, (LPARAM)&monitor);
	#ifdef DEBUGHDK
		Error::ShowInfoWindow("------------------------------------------------------------------------------------------------------------\r\n");
	#endif

	// Create extended display if multiple displays have been found
	if (numMonitor > 1)
	//if (false)
	{
		// Clear extended windows class data
		ZeroMemory(&wndClassExtended, sizeof(wndClassExtended));

		wndClassExtended.hInstance = hInstance;
		wndClassExtended.lpfnWndProc = (WNDPROC)WndProc;
		wndClassExtended.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wndClassExtended.lpszClassName = L"3dEngineHDK2_Extended";
		wndClassExtended.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		wndClassExtended.hCursor = LoadCursor(hInstance, IDC_ARROW);
		wndClassExtended.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));

		// Register window class
		RegisterClass(&wndClassExtended);

		// Get width of the primary screen
		int priMonitorWidth = GetSystemMetrics(SM_CXFULLSCREEN);

		// Get width of extended screen
		int secMonitorWidth = monitor[1].rect.right - monitor[1].rect.left;

		// Create borderless extended window and show (left)
		hWndExtendedLeft = CreateWindowEx(WS_EX_APPWINDOW, L"3dEngineHDK2_Extended", NULL, WS_POPUP, priMonitorWidth, 0, secMonitorWidth / 2, monitor[1].rect.bottom - monitor[1].rect.top, NULL, NULL, hInstance, NULL);
		if (hWndExtendedLeft != NULL)
		{
			ShowWindow(hWndExtendedLeft, SW_SHOWDEFAULT);
			UpdateWindow(hWndExtendedLeft);

			// Create OpenGL context with window connected to HDK2 device
			pOpenGLExtendedLeft = new OpenGL(hWndExtendedLeft);
		} else
		{
			Error::WriteLog("INFO", "WinMain::WinMain", "Could not create a (left-side) extended window");
		}

		// Create borderless extended window and show (right)
		hWndExtendedRight = CreateWindowEx(WS_EX_APPWINDOW, L"3dEngineHDK2_Extended", NULL, WS_POPUP, priMonitorWidth + secMonitorWidth / 2, 0, secMonitorWidth / 2, monitor[1].rect.bottom - monitor[1].rect.top, NULL, NULL, hInstance, NULL);
		if (hWndExtendedRight != NULL)
		{
			ShowWindow(hWndExtendedRight, SW_SHOW);
			UpdateWindow(hWndExtendedRight);

			// Create OpenGL context with window connected to HDK2 device
			pOpenGLExtendedRight = new OpenGL(hWndExtendedRight);
		} else
		{
			Error::WriteLog("INFO", "WinMain::WinMain", "Could not create a (right-side) extended window");
		}
	}

    // Set focus on main window
    SetFocus(hWndMain);

	// Hide cursor (if in move/rotate mode)
	if (move_rotate) while (ShowCursor(false) >= 0);

    // Program main loop
    while (!exit_program)
    {
        // Show message in info window
        if (show_info)
        {
            std::string info = pShader->GetInfo();
            Error::ShowInfoWindow(info.c_str());
            show_info = false;
        }

        // Delete (old) settings object (if needed)
        if (!prefs.show_settings && (pSettings != NULL))
        {
            delete (pSettings);
            pSettings = NULL;
        }

        // Show settings screen with preferences
        if (prefs.show_settings && (pSettings == NULL))
        {
            pSettings = new Settings(hInstance, hWndMain, prefs);
        }

        // Main message loop:
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			// Make main screen current
            pOpenGL->SetCurrentContext();

            // Get window rect (screen coordinates)
            RECT winRect;
            GetWindowRect(hWndMain, &winRect);

            // Get client window rect
            RECT clientRect;
            GetClientRect(hWndMain, &clientRect);

            // Set screen size in openGL so the full window will be used (no clipping)
            pOpenGL->ResizeScreen(clientRect.right, clientRect.bottom);

            if (pShader != NULL)
            {
                // Set shader settings
                pShader->SetTest(test);
				pShader->ShowOutlining(prefs.show_outlining);
				pShader->ShowAxis(prefs.show_axis);
				pShader->ShowTextures(prefs.show_textures);
                pShader->ShowNormalMap(prefs.show_normalmap);
                pShader->RepeatTextures(prefs.repeat_textures);
                pShader->FilterTextures(prefs.filter_textures);
                pShader->SetRenderMode(prefs.render_mode);
                pShader->SetVertexSize(prefs.vertex_size);
                pShader->SetScreenSize(clientRect.right, clientRect.bottom);
                pShader->SetAmbientLight((float)prefs.ambient_light.r / 255.0f, (float)prefs.ambient_light.g / 255.0f, (float)prefs.ambient_light.b / 255.0f, (float)prefs.ambient_light.a / 30.0f);
                pShader->SetDiffuseLight((float)prefs.diffuse_light.r / 255.0f, (float)prefs.diffuse_light.g / 255.0f, (float)prefs.diffuse_light.b / 255.0f, (float)prefs.diffuse_light.a / 10.0f);
                pShader->SetDiffusePosition(prefs.diffuse_position[0], prefs.diffuse_position[1], prefs.diffuse_position[2]);
                pShader->SetNearPlanePosition(prefs.pos_nearplane);
                pShader->SetFarPlanePosition(prefs.pos_farplane);

                // Adjust the light source to the color of the diffuse light
                for (unsigned int i = 0; i < pObjects->numMeshes; i++)
                {
                    if ((pObjects->pMeshArray[i]->name == "Bulb") && (pObjects->pMeshArray[i]->pMaterialEntryList[0] != NULL) && (pObjects->pMeshArray[i]->pMaterialEntryList[0]->pMaterial->pKd != NULL))
                    {
                        pObjects->pMeshArray[i]->pMaterialEntryList[0]->pMaterial->pKd->row[0] = (float)prefs.diffuse_light.r / 255.0f;
                        pObjects->pMeshArray[i]->pMaterialEntryList[0]->pMaterial->pKd->row[1] = (float)prefs.diffuse_light.g / 255.0f;
                        pObjects->pMeshArray[i]->pMaterialEntryList[0]->pMaterial->pKd->row[2] = (float)prefs.diffuse_light.b / 255.0f;
                    }
                }

				// Get (view)direction and accelleration data from HDK2 device
				if (hid->HDK2)
				{
					Hid::PACKET packet;
					result = hid->GetData(&packet);
					if (result)
					{
						#ifdef DEBUGHDK
						std::string strPacket;
						strPacket.append("Command  = " + Utils::ShortToStr(packet.command, 16) + "\r\n");
						strPacket.append("Sequence = " + Utils::ShortToStr(packet.sequence, 16) + "\r\n");
						strPacket.append("Quat     = " + Utils::ShortToStr(packet.quat[0], 16) + " " + Utils::ShortToStr(packet.quat[1], 16) + " " + Utils::ShortToStr(packet.quat[2], 16) + " " + Utils::ShortToStr(packet.quat[3], 16) + "\r\n");
						strPacket.append("Accel    = " + Utils::ShortToStr(packet.accel[0], 16) + " " + Utils::ShortToStr(packet.accel[1], 16) + " " + Utils::ShortToStr(packet.accel[2], 16) + "\r\n");
						#endif

						// Convert quat info to degrees
						float roll, pitch, yaw;

						double x =   ((double)packet.quat[0] / 16384.0);
						double y =   ((double)packet.quat[1] / 16384.0);
						double z = - ((double)packet.quat[2] / 16384.0);
						double w =   ((double)packet.quat[3] / 16384.0);

						#ifdef DEBUGHDK
						strPacket.append("x     = " + Utils::FloatToStr((float)x) + "\r\n");
						strPacket.append("y     = " + Utils::FloatToStr((float)y) + "\r\n");
						strPacket.append("z     = " + Utils::FloatToStr((float)z) + "\r\n");
						strPacket.append("w     = " + Utils::FloatToStr((float)w) + "\r\n");
						#endif

						// roll (x-axis rotation)
						double sinr_cosp = 2.0 * (w * x + y * z);
						double cosr_cosp = 1.0 - 2.0 * (x * x + y * y);
						roll = (float)(atan2(sinr_cosp, cosr_cosp) / PI * 360.0) - 180.0f;

						// pitch (y-axis rotation)
						double sinp = 2.0 * (w * y - z * x);
						if (fabs(sinp) >= 1)
							pitch = - (float) copysign(90.0, (float)sinp); // use 90 degrees if out of range
						else
							pitch = - (float) asin(sinp) / PI * 360.0f;

						// yaw (z-axis rotation)
						double siny_cosp = 2.0 * (w * z + x * y);
						double cosy_cosp = 1.0 - 2.0 * (y * y + z * z);
						yaw = (float) atan2(siny_cosp, cosy_cosp) / PI * 360.0f;

						// Accelleration
						float accelX = packet.accel[0] / 512.0f;
						float accelY = packet.accel[1] / 512.0f;
						float accelZ = packet.accel[2] / 512.0f;

						if (pitch > 0)
						{
							if (!backwards && (pitch < (oldPitch - 0.05f)) && (accelY > 0.05))
							{
								backwards = true;
							} else
							if (backwards && (pitch < (oldPitch - 0.05f)) && (accelY < -0.05))
							{
								backwards = false;
							}
						}
						if (pitch < 0)
						{
							if (!backwards && (pitch > (oldPitch + 0.05f)) && (accelY < -0.05))
							{
								backwards = true;
							} else
							if (backwards && (pitch > (oldPitch + 0.05f)) && (accelY > 0.05))
							{
								backwards = false;
							}
						}
						oldPitch = pitch;

						#ifdef DEBUGHDK
						strPacket.append("roll  = " + Utils::FloatToStr(roll) + "\r\n");
						strPacket.append("pitch = " + Utils::FloatToStr(pitch) + "\r\n");
						strPacket.append("yaw   = " + Utils::FloatToStr(yaw) + "\r\n");
						if (backwards) strPacket.append("Backward View\r\n"); else strPacket.append("Forward View\r\n");
						Error::ShowInfoWindow(strPacket.c_str());
						#endif

						if (!backwards)	pShader->SetViewAngle(roll, pitch, 0);
						else pShader->SetViewAngle(-roll, 360.0f - pitch, 0);
					}
				} else
				{
					// Move/rotate screen according to mouse info
					if (move_rotate)
					{
						// Change view angle left/right
						if (mouseX - mouseOldX < 0)
						{
							pShader->ChangeViewAngle(0.0f, -1.0f, 0.0f);
						}

						if (mouseX - mouseOldX > 0)
						{
							pShader->ChangeViewAngle(0.0f, 1.0f, 0.0f);
						}

						// Change view angle up/down
						if (mouseY - mouseOldY < 0)
						{
							pShader->ChangeViewAngle(-1.0f, 0.0f, 0.0f);
						}

						if (mouseY - mouseOldY > 0)
						{
							pShader->ChangeViewAngle(1.0f, 0.0f, 0.0f);
						}

						// Set cursor
						if ((mouseX < clientRect.left + 100) || (mouseX > clientRect.right - 100))
						{
							mouseX = clientRect.right / 2;
							SetCursorPos(winRect.left + mouseX + 8, winRect.top + mouseY + 8);
						}

						if ((mouseY < clientRect.top + 100) || (mouseY > clientRect.bottom - 100))
						{
							mouseY = clientRect.bottom / 2;
							SetCursorPos(winRect.left + mouseX + 8, winRect.top + mouseY + 8);
						}

						mouseOldX = mouseX;
						mouseOldY = mouseY;
					} 
                }

				// Increase or decrease moving speed according to the mousewheel
				if (mouseUp) delta += 0.01f;
				if (mouseDown) delta -= 0.01f;
				if (delta <= 0) delta = 0.01f;

				mouseUp = mouseDown = false;

				// Change movement 
				float xValue = 0.0f, yValue = 0.0f, zValue = 0.0f;
				if (left)  xValue += delta;
				if (right) xValue -= delta;
				if (up)    zValue += delta;
				if (down)  zValue -= delta;
				if (low)   yValue += delta;
				if (high)  yValue -= delta;

				pShader->ChangeViewPosition(xValue, yValue, zValue);
				
				// Check if saving is needed
                if (save)
                {
                    // Save the meshes
                    Error::ShowInfoWindow("Saving meshes");
                    pObjects->SaveMeshes();
                    save = false;
                }

                // Clear the main screen
                pOpenGL->ClearScreen();

                // Render in time not needed for message handling
                pShader->Render((GLint)clientRect.right, (GLint)clientRect.bottom);

                // Swap screens
                pOpenGL->SwapBuffers();

				// Make left extended screen current
				if (pOpenGLExtendedLeft != NULL)
				{
					result = pOpenGLExtendedLeft->SetCurrentContext();
					if (result)
					{
						// Get client window rect
						RECT clientRect;
						GetClientRect(hWndExtendedLeft, &clientRect);

						// Set screen size in openGL so the full window will be used (no clipping)
						pOpenGLExtendedLeft->ResizeScreen((GLint)clientRect.right, (GLint)clientRect.bottom);

						// Clear extended screen
						pOpenGLExtendedLeft->ClearScreen();

						// Render 
						pShader->Render((GLint)clientRect.right, (GLint)clientRect.bottom, prefs.hdk_offset_pos /100.0f, -prefs.hdk_offset_angle / 10.0f);

						// Swap screens
						pOpenGLExtendedLeft->SwapBuffers();
					} else
					{
						Error::WriteLog("ERROR", "WinMain::WinMain", "Can't switch to (left) extended window)");
					}
				}

				// Make right extended screen current
				if (pOpenGLExtendedRight != NULL)
				{
					result = pOpenGLExtendedRight->SetCurrentContext();
					if (result)
					{
						// Get client window rect
						RECT clientRect;
						GetClientRect(hWndExtendedRight, &clientRect);

						// Set screen size in openGL so the full window will be used (no clipping)
						pOpenGLExtendedRight->ResizeScreen((GLint)clientRect.right, (GLint)clientRect.bottom);

						// Clear extended screen
						pOpenGLExtendedRight->ClearScreen();

						// Render
						pShader->Render((GLint)clientRect.right, (GLint)clientRect.bottom, -prefs.hdk_offset_pos / 100.0f, prefs.hdk_offset_angle / 10.0f);

						// Swap screens
						pOpenGLExtendedRight->SwapBuffers();
					} else
					{
						Error::WriteLog("ERROR", "WinMain::WinMain", "Can't switch to (right) extended window)");
					}
				}
            }
        }
    }

    return (int) msg.wParam;
}

/*********************************************************************
* Windows procedure (handle messages for this window)
*********************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			exit_program = true;
			PostQuitMessage(0);
			break;

		case WM_KEYUP:
			up = down = left = right = low = high = false;
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:
				exit_program = true;
				PostQuitMessage(0);
				break;

			case 'P':
				prefs.show_settings = true;
				break;

			case 'O':
				prefs.show_outlining = !prefs.show_outlining;
				break;

			case 'T':
				prefs.show_textures = !prefs.show_textures;
				break;

			case 'Z':
				test = !test;
				break;

			case 'N':
				prefs.show_normalmap = !prefs.show_normalmap;
				break;

			case 'R':
				prefs.repeat_textures = !prefs.repeat_textures;
				break;

			case 'L':
				prefs.filter_textures = !prefs.filter_textures;
				break;

			case 'I':
				show_info = true;
				break;

			case 'X':
				prefs.show_axis = !prefs.show_axis;
				break;
		
			case 'V':
				prefs.render_mode = RM_VERTICES;
				break;

			case 'E':
				prefs.render_mode = RM_EDGES;
				break;

			case 'F':
				prefs.render_mode = RM_FACES;
				break;

			case 'W':
				up = true;
				break;

			case 'A':
				left = true;
				break;

			case 'D':
				right = true;
				break;

			case 'S':
				down = true;
				break;

			case 'C':
				save = true;
				break;

			case '1':
				prefs.vertex_size = 1;
				break;

			case '2':
				prefs.vertex_size = 2;
				break;

			case '3':
				prefs.vertex_size = 3;
				break;

			case '4':
				prefs.vertex_size = 4;
				break;

			case 0x26:
				high = true;
				break;

			case 0x28:
				low = true;
				break;
			}
			break;

		case WM_RBUTTONDOWN:

			rightMouseButtonDown = true;

			// Begin or end rotate/move scene
			move_rotate = !move_rotate;
			
			if (move_rotate) while (ShowCursor(false) >= 0);
			if (!move_rotate) while (ShowCursor(true) < 0);

			break;

		case WM_RBUTTONUP:

			rightMouseButtonDown = false;
			break;

		case WM_LBUTTONDOWN:

			leftMouseButtonDown = true;
			break;

		case WM_LBUTTONUP:

			leftMouseButtonDown = false;
			break;

			// Set translation according to mouse wheel
		case WM_MOUSEWHEEL:

			// Get translation
			if ((short)HIWORD(wParam) > 0) mouseUp = true;
			if ((short)HIWORD(wParam) < 0) mouseDown = true;

			break;

		case WM_MOUSEMOVE:

			// Get mouse coordinates
			mouseX = (int)(short)LOWORD(lParam);
			mouseY = (int)(short)HIWORD(lParam);

			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

/*********************************************************************
* Monitor enumeration procedure
*********************************************************************/
bool CALLBACK MonitorEnumProc (HMONITOR hMonitor, HDC hDC, LPRECT lpRect, LPARAM lParam)
{
	MONITORINFOEX monitorInfoEx;
    monitorInfoEx.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &monitorInfoEx);

    if (monitorInfoEx.dwFlags == DISPLAY_DEVICE_MIRRORING_DRIVER)
    {
        return (true);
    }
    else
    {
		if (numMonitor < MAX_MONITOR)
		{
			MONITOR *pMonitor = (MONITOR*)lParam;

			// If allready monitor found, skip older entries
			MONITOR *ptr = pMonitor;
			for (unsigned int i = 0; i < numMonitor; i++) ptr++;

			// Set variables of this display
			ptr->num = numMonitor;
			ptr->rect = monitorInfoEx.rcWork;
			
			#ifdef DEBUGHDK
				Error::ShowInfoWindow("Monitor #:" + Utils::IntToStr(numMonitor));
				Error::ShowInfoWindow("Monitor Name:" + Utils::ConvertWCSToMBS(monitorInfoEx.szDevice, (long)wcslen(monitorInfoEx.szDevice)));
				Error::ShowInfoWindow("Monitor Top:" + Utils::IntToStr(ptr->rect.top));
				Error::ShowInfoWindow("Monitor Bottom:" + Utils::IntToStr(ptr->rect.bottom));
				Error::ShowInfoWindow("Monitor Left:" + Utils::IntToStr(ptr->rect.left));
				Error::ShowInfoWindow("Monitor Right:" + Utils::IntToStr(ptr->rect.right) + "\r\n");
			#endif
		}

		numMonitor++;
		if (numMonitor > MAX_MONITOR)
		{
			Error::WriteLog("WARNING", "WinMain::MonitorEnumProc", "More then 2 displays (monitors) attached => ignoring those");
		}
    }

	return (true);
}