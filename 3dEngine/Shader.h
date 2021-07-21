#pragma once
#include <windows.h>
#include "Mesh.h"
#include "Objects.h"
#include "VecMat.h"

#define SHADOWMAP_WIDTH  1024
#define SHADOWMAP_HEIGHT 1024

// Ambient / Diffuse light
struct Light
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};

// Drawing modes for mesh
typedef unsigned int RenderMode;
#define RM_VERTICES      0x0
#define RM_EDGES         0x1
#define RM_FACES         0x2

// Near- and farplane positions (default)
#define NEARPLANEPOS      2.0f
#define FARPLANEPOS     800.0f

// Left, right, top and bottom of nearplane
#define NPLEFT           -1.0f
#define NPRIGHT           1.0f
#define NPBOTTOM         -1.0f
#define NPTOP             1.0f

// Default angle offset for left and right window (eye) on extended display
#define DEFAULTOFFSETANGLE 20

class Shader
{
    private:

        // Objects for rendering / use as material etc.
        Objects *pObjects;

        // Render mode (vertices, edges or faces)
        RenderMode render_mode;

        // Show textures
        bool show_textures;

        // Show normalmap
        bool show_normalmap;

        // Test
        bool test;

        // Filter textures
        bool filter_textures;

        // Repeat textures
        bool repeat_textures;

        // Show outlining
        bool show_outlining;

        // Show axis
        bool show_axis;
        
        // Screen (window) scale factor
        GLint screen_width;
        GLint screen_height;

        // Size of the vertices drawn
        GLshort vertex_size;

        // Mesh angles
        GLfloat angle_x, angle_y, angle_z;

        // Mesh angles 
        VecMat::Mat4 angle_matrix;
        
        // Mesh position
        GLfloat pos_x, pos_y, pos_z;

        // Mesh position 
        VecMat::Mat4 position_matrix;
        
        // Mesh offset (angles and translation)
        VecMat::Mat4 offset_matrix;

        // Ambient color
        Light ambient_light;

        // Diffuse color
        Light diffuse_light;

        // Diffuse light position
        GLfloat diff_pos_x, diff_pos_y, diff_pos_z;

        // Reflectivity factor
        GLint reflectivity;

        // Position of Near Plane
        GLfloat pos_np;

        // Position of Far Plane
        GLfloat pos_fp;

        // Vertex shader name
        GLuint vertex_shader;

        // Fragment shader name
        GLuint fragment_shader;

        // Vertex shadow shader handler
        GLuint vertex_shadow_shader;

        // Fragment shadow shader handler
        GLuint fragment_shadow_shader;

        // Vertex array object
        GLuint vao[MAX_NUM_MESHES];

        // Names (int) of the vertex buffers
        GLuint vertex_buffer[MAX_NUM_MESHES];

        // Names (int) of the normal buffers
        GLuint normal_buffer[MAX_NUM_MESHES];

        // Names (int) of the index buffers
        GLuint index_buffer[MAX_NUM_MESHES];

        // Names (int) of the texture buffers
        GLuint texture_color_buffer[MAX_NUM_MATERIALS];

        // Names (int) of the texture normal buffers
        GLuint texture_normal_buffer[MAX_NUM_MATERIALS];

        // Name (int) of the framebuffer used for shadow mapping
        GLuint frame_buffer;

        // Name (int) of the texturebuffer used for shadow mapping
        GLuint texture_shadow_buffer;

        // Matrices for object path and projection
        GLuint path_matrix_location;
        GLuint path_shadow_matrix_location;
        GLuint proj_matrix_location;
        GLuint proj_shadow_matrix_location;
        GLuint path_lightspace_matrix_location;

        // Mesh texture
        GLuint texture_color_data_location;
        GLuint texture_normal_data_location;
        GLuint texture_position_location;
        GLuint texture_color_enable_location;
        GLuint texture_normal_enable_location;
        GLuint texture_scale_location;
        GLuint texture_offset_location;

        // Screen size scale
        GLuint screen_scale_location;

        // Ambulant light
        GLuint ambient_light_location;

        // Diffuse light
        GLuint diffuse_position_location;
        GLuint diffuse_light_location;

        // Material properties
        GLuint ambient_albedo_location;
        GLuint diffuse_albedo_location;
        GLuint specular_albedo_location;

        // Reflectivity factor
        GLuint reflectivity_location;

        // Illumination model
        GLuint illumination_location;

        // Vertices
        GLuint vertex_position_location;

        // Vertex normals
        GLuint normal_position_location;

        // (Bi-)Tangents
        GLuint tangent_position_location;
        GLuint bitangent_position_location;

        // Test in shader
        GLuint test_location;

        // Vertices in shadow shader
        GLuint vertex_shadow_position_location;

        // Texture shadow map data location
        GLuint texture_shadow_data_location;

        // Shader program name
        GLuint program;

        // Shadow shader program name
        GLuint program_shadow;

        // Vertex shader
        static const char *vs_source[];

        // Fragment shader
        static const char *fs_source[];

        // Vertex shadow shader
        static const char *vs_shadow_source[];

        // Fragment shadow shader
        static const char *fs_shadow_source[];

        // Create and compile the vertex and fragment shader
        void CreateShaders (void);

        // Create programs for OpenGL
        void CreatePrograms (void);

        // Create and set buffers (vertex, index)
        void CreateBuffers (void);

        // Create and set framebuffer (shadows)
        void CreateFrameBuffer(void);

        // Create and set textures
        void CreateTextures (void);

        // Draw all objects
        void DrawObjects(bool shadowMap);

    public:

        // Constructor
        Shader (Objects *pObjects);

        // Destructor
        ~Shader (void);

        // Render mesh object
        void Render (GLint screen_width, GLint screen_height);

        // Set color for ambient light
        void SetAmbientLight (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

        // Set color for diffuse light
        void SetDiffuseLight (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

        // Set position for diffuse light (and Bulb)
        void SetDiffusePosition (GLfloat x, GLfloat y, GLfloat z);

        // Set view angle
        void SetViewAngle (GLfloat angle_x, GLfloat angle_y, GLfloat angle_z);

        // Set view angle (quaternion)
        void SetViewAngle (VecMat::Mat4 rotate);

        // Change view angle
        void ChangeViewAngle (GLfloat delta_angle_x, GLfloat delta_angle_y, GLfloat delta_angle_z);

        // Set view position
        void SetViewPosition (GLfloat x, GLfloat y, GLfloat z);

        // Change view position
        void ChangeViewPosition (GLfloat delta_pos_x, GLfloat delta_pos_y, GLfloat delta_pos_z);

        // Get view position with provided delta
        void GetDeltaViewPosition(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat delta_pos_x, GLfloat delta_pos_y, GLfloat delta_pos_z);

        // Set offset
        void SetOffset (GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat angle_x, GLfloat angle_y, GLfloat angle_z);

        // Set near plane vision (perspective)
        void SetNearPlanePosition (GLfloat pos_np);

        // Set position of far plane
        void SetFarPlanePosition (GLfloat pos_fp);

        // Set rendermode (vertices, edges or faces)
        void SetRenderMode (RenderMode render_mode);

        // Set show textures
        void ShowTextures (bool show);

        // Set show normalmap
        void ShowNormalMap (bool show);

        // Set test conditions in shader
        void SetTest (bool test);

        // Set filter textures
        void FilterTextures (bool filter);

        // Set repeat textures
        void RepeatTextures (bool repeat);

        // Set show oulining
        void ShowOutlining (bool show);

        // Set show axis
        void ShowAxis(bool show);
            
        // Set vertex size
        void SetVertexSize (GLint vsize);

        // Set screen size
        void SetScreenSize (GLint width, GLint height);

        // Get info (position, angle etc.)
        std::string GetInfo (void);
};

