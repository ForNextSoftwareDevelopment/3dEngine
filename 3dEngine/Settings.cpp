#include "Settings.h"

// All preferences in a structure
struct struct_prefs *Settings::prefs;

// Controls
CtrlButton      *Settings::btnOK;
CtrlCheckBox    *Settings::cbTextures;
CtrlCheckBox    *Settings::cbNormalMap;
CtrlCheckBox    *Settings::cbOutlines;
CtrlCheckBox    *Settings::cbRepeat;
CtrlCheckBox    *Settings::cbFilter;
CtrlCheckBox    *Settings::cbAxis;
CtrlRadioButton *Settings::rbRenderModeVertex;
CtrlRadioButton *Settings::rbRenderModeEdge;
CtrlRadioButton *Settings::rbRenderModeFace;
CtrlValue       *Settings::cvVertexSize;
CtrlColorSet    *Settings::csColorAmbient;
CtrlColorSet    *Settings::csColorDiffuse;
CtrlPosSet      *Settings::psPositionDiffuse;
CtrlValue       *Settings::cvNearPlanePos;
CtrlValue       *Settings::cvFarPlanePos;
CtrlValue       *Settings::cvHdkOffsetPos;
CtrlValue       *Settings::cvHdkOffsetAngle;

// Control window handles
HWND Settings::hWndBtnOK;
HWND Settings::hWndCbTextures;
HWND Settings::hWndCbNormalMap;
HWND Settings::hWndCbOutlines;
HWND Settings::hWndCbRepeat;
HWND Settings::hWndCbFilter;
HWND Settings::hWndCbAxis;
HWND Settings::hWndRbRenderModeVertex;
HWND Settings::hWndRbRenderModeEdge;
HWND Settings::hWndRbRenderModeFace;
HWND Settings::hWndCvVertexSize;
HWND Settings::hWndCsColorAmbient;
HWND Settings::hWndCsColorDiffuse;
HWND Settings::hWndPsPositionDiffuse;
HWND Settings::hWndCvNearPlanePos;
HWND Settings::hWndCvFarPlanePos;
HWND Settings::hWndCvHdkOffsetPos;
HWND Settings::hWndCvHdkOffsetAngle;

/*********************************************************************
* Constructor
*********************************************************************/
Settings::Settings(HINSTANCE hInstance, HWND hWndMain, struct_prefs &preferences)
{
    // Set preferences from main screen
    Settings::prefs = &preferences;

    // Show cursor in this window
    ShowCursor(true);
    SetCursorPos (820, 40);

    // Create window class for this window
    WNDCLASS wndClass = { 0 };

    wndClass.style          = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc    = (WNDPROC) WndProcSettings;
    wndClass.cbClsExtra     = 0;
    wndClass.cbWndExtra     = 0;
    wndClass.hInstance      = hInstance;
    wndClass.hIcon          = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_ICON));
    wndClass.hCursor        = LoadCursor (hInstance, IDC_ARROW);
    wndClass.hbrBackground  = GetSysColorBrush(COLOR_INACTIVECAPTION);
    wndClass.lpszMenuName   = L"Settings";
    wndClass.lpszClassName  = L"Settings";
    RegisterClass (&wndClass);

    // Create and show settings window
    hWndSettings = CreateWindow (L"Settings", L"Settings", WS_DLGFRAME, 840, 40, 600, 500, NULL, NULL, hInstance, NULL) ;
    ShowWindow(hWndSettings, SW_SHOWNORMAL);
    UpdateWindow(hWndSettings);
}

/*********************************************************************
* Destructor
*********************************************************************/
Settings::~Settings(void)
{
    // Clean up and close the settings window
    delete (btnOK);
    delete (cbTextures);
    delete (cbFilter);
    delete (cbRepeat);
    delete (cbOutlines);
    delete (cbAxis);
    delete (rbRenderModeEdge);
    delete (rbRenderModeFace);
    delete (rbRenderModeVertex);
    delete (cvVertexSize);
    delete (csColorAmbient);
    delete (csColorDiffuse);
    delete (cvNearPlanePos);
    delete (cvFarPlanePos);
    delete (cvHdkOffsetPos);
    delete (cvHdkOffsetAngle);

    DestroyWindow(hWndSettings);

    // Set flag
    Settings::prefs->show_settings = false;
    ShowCursor(false);
}

/*********************************************************************
* Windows procedure (handle messages for the settings window)
*********************************************************************/
LRESULT CALLBACK Settings::WndProcSettings (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE :

            // Create a new radiobutton
            rbRenderModeVertex = new CtrlRadioButton("Render Mode", "Vertex", (Settings::prefs->render_mode == RM_VERTICES), hWnd);
            hWndRbRenderModeVertex = rbRenderModeVertex->Create(10, 10, 40, 40);

            // Create a new radiobutton
            rbRenderModeEdge = new CtrlRadioButton("Render Mode", "Edge", (Settings::prefs->render_mode == RM_EDGES), hWnd);
            hWndRbRenderModeEdge = rbRenderModeEdge->Create(100, 10, 40, 40);

            // Create a new radiobutton
            rbRenderModeFace = new CtrlRadioButton("Render Mode", "Face", (Settings::prefs->render_mode == RM_FACES), hWnd);
            hWndRbRenderModeFace = rbRenderModeFace->Create(190, 10, 40, 40);

            // Create a new checkbox
            cbAxis = new CtrlCheckBox("Show Axis (Key = X)", Settings::prefs->show_axis, hWnd);
            hWndCbAxis = cbAxis->Create(10, 60, 80, 40);
            
            // Create a new checkbox
            cbTextures = new CtrlCheckBox("Show Textures (Key = T)", Settings::prefs->show_textures, hWnd);
            hWndCbTextures = cbTextures->Create(10, 90, 80, 40);

            // Create a new checkbox
            cbNormalMap = new CtrlCheckBox("Show Normal Map (Key = B)", Settings::prefs->show_normalmap, hWnd);
            hWndCbNormalMap = cbNormalMap->Create(10, 120, 80, 40);

            // Create a new checkbox
            cbRepeat = new CtrlCheckBox("Repeat Textures  (Key = R)", Settings::prefs->repeat_textures, hWnd);
            hWndCbRepeat = cbRepeat->Create(10, 150, 80, 40);

            // Create a new checkbox
            cbFilter = new CtrlCheckBox("Enable Filtering (Key = L)", Settings::prefs->filter_textures, hWnd);
            hWndCbFilter = cbFilter->Create(10, 180, 80, 40);

            // Create a new checkbox
            cbOutlines = new CtrlCheckBox("Show Outlining (Key = O)", Settings::prefs->show_outlining, hWnd);
            hWndCbOutlines = cbOutlines->Create(10, 210, 80, 40);

            // Create a new colorbox for ambient light
            csColorAmbient = new CtrlColorSet("Ambient Light Color", Settings::prefs->ambient_light, hWnd);
            hWndCsColorAmbient = csColorAmbient->Create(10, 260, 382, 40);

            // Create a new colorbox for diffuse light
            csColorDiffuse = new CtrlColorSet("Diffuse Light Color", Settings::prefs->diffuse_light, hWnd);
            hWndCsColorDiffuse = csColorDiffuse->Create(10, 310, 382, 40);

            // Create a new positionbox for diffuse light position
            psPositionDiffuse = new CtrlPosSet("Diffuse Light (Offset) Position", Settings::prefs->diffuse_position, hWnd);
            hWndPsPositionDiffuse = psPositionDiffuse->Create(10, 360, 382, 40);

            // Create a new valuebox for HDK2 position offset between windows (eyes) 
            cvHdkOffsetPos = new CtrlValue("HDK2 eye position offset", Settings::prefs->hdk_offset_pos, hWnd);
            hWndCvHdkOffsetPos = cvHdkOffsetPos->Create(300, 10, 80, 40);

            // Create a new valuebox for HDK2 angle offset between windows (eyes) 
            cvHdkOffsetAngle = new CtrlValue("HDK2 eye angle offset", Settings::prefs->hdk_offset_angle, hWnd);
            hWndCvHdkOffsetAngle = cvHdkOffsetAngle->Create(300, 60, 80, 40);

            // Create a new valuebox for nearplane position
            cvNearPlanePos = new CtrlValue("NearPlane Position (x 10)", (int)Settings::prefs->pos_nearplane *  10, hWnd);
            hWndCvNearPlanePos = cvNearPlanePos->Create(300, 110, 80, 40);

            // Create a new valuebox for farplane position
            cvFarPlanePos = new CtrlValue("FarPlane Position", (int)Settings::prefs->pos_farplane, hWnd);
            hWndCvFarPlanePos = cvFarPlanePos->Create(300, 160, 80, 40);

            // Create a new valuebox for vertex size
            cvVertexSize = new CtrlValue("Vertex Size (Key = 1,2,3,4)", Settings::prefs->vertex_size, hWnd);
            hWndCvVertexSize = cvVertexSize->Create(300, 210, 80, 40);

            // Create a new button
            btnOK = new CtrlButton("OK", hWnd);
            hWndBtnOK = btnOK->Create(480, 410, 80, 30);

            break;

        case WM_CLOSE:
            DestroyWindow (hWnd);
            break;

        case WM_DESTROY:
            Settings::prefs->show_settings = false;
            PostQuitMessage (0);
            break;

        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case WM_LBUTTONUP:
                    if (lParam == (LPARAM) hWndBtnOK)
                    {
                        // Close settings window
                        Settings::prefs->show_settings = false;
                        PostQuitMessage (0);
                    }

                    if (lParam == (LPARAM) hWndCbTextures)
                    {
                        // Toggle 'show texture'
                        Settings::prefs->show_textures = !Settings::prefs->show_textures;
                    }

                    if (lParam == (LPARAM) hWndCbNormalMap)
                    {
                        // Toggle 'show bumpmap'
                        Settings::prefs->show_normalmap = !Settings::prefs->show_normalmap;
                    }

                    if (lParam == (LPARAM) hWndCbOutlines)
                    {
                        // Toggle 'show texture'
                        Settings::prefs->show_outlining = !Settings::prefs->show_outlining;
                    }

                    if (lParam == (LPARAM) hWndCbFilter)
                    {
                        // Toggle 'show texture'
                        Settings::prefs->filter_textures = !Settings::prefs->filter_textures;
                    }

                    if (lParam == (LPARAM)hWndCbAxis)
                    {
                        // Toggle 'show axis'
                        Settings::prefs->show_axis = !Settings::prefs->show_axis;
                    }
                    
                    if (lParam == (LPARAM) hWndCbRepeat)
                    {
                        // Toggle 'show texture'
                        Settings::prefs->repeat_textures = !Settings::prefs->repeat_textures;
                    }

                    if (lParam == (LPARAM) hWndRbRenderModeVertex)
                    {
                        // Change rendermode to vertex
                        Settings::prefs->render_mode = RM_VERTICES;
                        rbRenderModeEdge->SetStatus(false);
                        rbRenderModeFace->SetStatus(false);
                    }

                    if (lParam == (LPARAM) hWndRbRenderModeEdge)
                    {
                        // Change rendermode to edge
                        Settings::prefs->render_mode = RM_EDGES;
                        rbRenderModeVertex->SetStatus(false);
                        rbRenderModeFace->SetStatus(false);
                    }

                    if (lParam == (LPARAM) hWndRbRenderModeFace)
                    {
                        // Change rendermode to face
                        Settings::prefs->render_mode = RM_FACES;
                        rbRenderModeVertex->SetStatus(false);
                        rbRenderModeEdge->SetStatus(false);
                    }

                    if (lParam == (LPARAM) hWndCvVertexSize)
                    {
                        // Set new vertex size
                        if (cvVertexSize->GetStatus() != 0) Settings::prefs->vertex_size = cvVertexSize->GetStatus();
                    }

                    if (lParam == (LPARAM) hWndCsColorAmbient)
                    {
                        // Set new ambient color
                        Settings::prefs->ambient_light = csColorAmbient->GetStatus();
                    }

                    if (lParam == (LPARAM) hWndCsColorDiffuse)
                    {
                        // Set new diffuse color
                        Settings::prefs->diffuse_light = csColorDiffuse->GetStatus();
                    }

                    if (lParam == (LPARAM) hWndPsPositionDiffuse)
                    {
                        // Set new diffuse color position
                        Settings::prefs->diffuse_position = psPositionDiffuse->GetStatus();
                    }

                    if (lParam == (LPARAM) hWndCvNearPlanePos)
                    {
                        // Set new near plane position
                        if (cvNearPlanePos->GetStatus() > 0) Settings::prefs->pos_nearplane = (float)cvNearPlanePos->GetStatus() /10;
                    }

                    if (lParam == (LPARAM) hWndCvFarPlanePos)
                    {
                        // Set new near plane position
                        if (cvFarPlanePos->GetStatus() > 0) Settings::prefs->pos_farplane = (float)cvFarPlanePos->GetStatus();
                    }

                    if (lParam == (LPARAM) hWndCvHdkOffsetPos)
                    {
                        // Set new near plane position
                        if (cvHdkOffsetPos->GetStatus() > 0) Settings::prefs->hdk_offset_pos = cvHdkOffsetPos->GetStatus();
                    }

                    if (lParam == (LPARAM)hWndCvHdkOffsetAngle)
                    {
                        // Set new near plane position
                        if (cvHdkOffsetAngle->GetStatus() > 0) Settings::prefs->hdk_offset_angle = cvHdkOffsetAngle->GetStatus();
                    }
                    break;
            }
            break;

        default:
            return DefWindowProc (hWnd, message, wParam, lParam);
    }

    return (0);
}

