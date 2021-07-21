#pragma once
#include <windows.h>
#include "error.h"
#include "controls/CtrlButton.h"
#include "controls/CtrlCheckBox.h"
#include "controls/CtrlRadioButton.h"
#include "controls/CtrlColorSet.h"
#include "controls/CtrlValue.h"
#include "controls/CtrlPosSet.h"

// Drawing modes for mesh (used in preferences)
typedef unsigned int RenderMode;

#define RM_VERTICES      0x0
#define RM_EDGES         0x1
#define RM_FACES         0x2

struct struct_prefs
{
    // Show settings flag
    bool         show_settings      = false;

    bool         show_axis          = false;
    int          hdk_offset_angle   = 0;
    VecMat::Vec3 diffuse_position   = { 0, 0, 0 };
    unsigned int terrain            = 0;

    // Preferences for shader
    bool         show_textures      = false;
    bool         show_normalmap     = false;
    bool         repeat_textures    = false;
    bool         filter_textures    = false;
    bool         show_outlining     = false;
    int          vertex_size        = 0;
    RenderMode   render_mode        = RM_FACES;
    Color        ambient_light      = { 0xFF, 0xFF, 0xFF, 0xFF };
    Color        diffuse_light      = { 0xFF, 0xFF, 0xFF, 0xFF };
    float        pos_nearplane      = 0.0f;
    float        pos_farplane       = 0.0f;
};

class Settings
{
    private:

        // All preferences in a structure
        static struct struct_prefs *prefs;

        // Window handle
        HWND hWndSettings;

        // Controls
        static CtrlButton      *btnOK;
        static CtrlCheckBox    *cbTextures;
        static CtrlCheckBox    *cbNormalMap;
        static CtrlCheckBox    *cbOutlines;
        static CtrlCheckBox    *cbRepeat;
        static CtrlCheckBox    *cbFilter;
        static CtrlCheckBox    *cbAxis;
        static CtrlRadioButton *rbRenderModeVertex;
        static CtrlRadioButton *rbRenderModeEdge;
        static CtrlRadioButton *rbRenderModeFace;
        static CtrlValue       *cvVertexSize;
        static CtrlColorSet    *csColorAmbient;
        static CtrlColorSet    *csColorDiffuse;
        static CtrlPosSet      *psPositionDiffuse;
        static CtrlValue       *cvNearPlanePos;
        static CtrlValue       *cvFarPlanePos;
        static CtrlValue       *cvHdkOffsetPos;
        static CtrlValue       *cvHdkOffsetAngle;

        // Control window handles
        static HWND hWndBtnOK;
        static HWND hWndCbTextures;
        static HWND hWndCbNormalMap;
        static HWND hWndCbOutlines;
        static HWND hWndCbRepeat;
        static HWND hWndCbFilter;
        static HWND hWndCbAxis;
        static HWND hWndRbRenderModeVertex;
        static HWND hWndRbRenderModeEdge;
        static HWND hWndRbRenderModeFace;
        static HWND hWndCvVertexSize;
        static HWND hWndCsColorAmbient;
        static HWND hWndCsColorDiffuse;
        static HWND hWndPsPositionDiffuse;
        static HWND hWndCvNearPlanePos;
        static HWND hWndCvFarPlanePos;
        static HWND hWndCvHdkOffsetPos;
        static HWND hWndCvHdkOffsetAngle;

        // Windows procedure (handle messages for the (controls in the) settings window)
        static LRESULT CALLBACK WndProcSettings (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    public:

        // Constructor
        Settings (HINSTANCE hInstance, HWND hWndMain, struct_prefs &preferences);

        // Destructor
        ~Settings (void);
};
