#include "Shader.h"

/*********************************************************************
* Vertex shader
*********************************************************************/
const char *Shader::vs_source[] =
{
    "#version 420 core                                                                                                                                                                                      \n"
    "                                                                                                                                                                                                       \n"
    "layout (location = 0) in vec3 vertex_position;                                                                                                                                                         \n"
    "layout (location = 1) in vec3 normal_position;                                                                                                                                                         \n"
    "layout (location = 2) in vec3 texture_position;                                                                                                                                                        \n"
    "layout (location = 3) in vec3 tangent_position;                                                                                                                                                        \n"
    "layout (location = 4) in vec3 bitangent_position;                                                                                                                                                      \n"
    "                                                                                                                                                                                                       \n"
    "out VS_OUT                                                                                                                                                                                             \n"
    "{                                                                                                                                                                                                      \n"
    "   vec3 N;                                                                                                                                                                                             \n"
    "   vec3 L;                                                                                                                                                                                             \n"
    "   vec3 V;                                                                                                                                                                                             \n"
    "   vec3 vertex_position;                                                                                                                                                                               \n"
    "   vec3 texture_position;                                                                                                                                                                              \n"
    "   vec3 normal_position;                                                                                                                                                                               \n"
    "   vec3 tangent_position;                                                                                                                                                                              \n"
    "   vec3 bitangent_position;                                                                                                                                                                            \n"
    "   vec3 diffuse_position;                                                                                                                                                                              \n"
    "   vec4 position_lightspace;                                                                                                                                                                           \n"
    "} vs_out;                                                                                                                                                                                              \n"
    "                                                                                                                                                                                                       \n"
    "uniform mat4 path_matrix;                                                                                                                                                                              \n"
    "uniform mat4 proj_matrix;                                                                                                                                                                              \n"
    "uniform mat4 lightspace_matrix;                                                                                                                                                                        \n"
    "uniform vec2 screen_scale = vec2(1.0, 1.0);                                                                                                                                                            \n"
    "uniform vec3 diffuse_position = vec3(0.0, 0.0, 0.0);                                                                                                                                                   \n"
    "                                                                                                                                                                                                       \n"
    "void main(void)                                                                                                                                                                                        \n"
    "{                                                                                                                                                                                                      \n"
    "   mat3 normal_matrix = transpose(inverse(mat3(path_matrix)));                                                                                                                                         \n"
    "   vec4 P =  path_matrix * vec4(vertex_position,  1.0);                                                                                                                                                \n"
    "   vec4 D =  path_matrix * vec4(diffuse_position, 1.0);                                                                                                                                                \n"
    "   vec4 Q =  lightspace_matrix * vec4(vertex_position,  1.0);                                                                                                                                                  \n"
    "                                                                                                                                                                                                       \n"
    "   vs_out.N = normal_matrix * normal_position;                                                                                                                                                         \n"
    "   vs_out.L = D.xyz - P.xyz;                                                                                                                                                                           \n"
    "   vs_out.V = -P.xyz;                                                                                                                                                                                  \n"
    "   vs_out.texture_position   = texture_position;                                                                                                                                                       \n"
    "   vs_out.normal_position    = (path_matrix * vec4(normal_position,    0.0)).xyz;                                                                                                                      \n"
    "   vs_out.tangent_position   = (path_matrix * vec4(tangent_position,   0.0)).xyz;                                                                                                                      \n"
    "   vs_out.bitangent_position = (path_matrix * vec4(bitangent_position, 0.0)).xyz;                                                                                                                      \n"
    "                                                                                                                                                                                                       \n"
    "   gl_Position = proj_matrix * P  * vec4(screen_scale, 1.0, 1.0);                                                                                                                                      \n"
    "                                                                                                                                                                                                       \n"
    "   vs_out.position_lightspace = proj_matrix * vec4(vec3(Q), 1.0);                                                                                                                                      \n"
    "   vs_out.vertex_position = vertex_position;                                                                                                                                                           \n"
    "   vs_out.diffuse_position = diffuse_position;                                                                                                                                                         \n"
    "}                                                                                                                                                                                                      \n"
};

/*********************************************************************
* Fragment shader
*********************************************************************/
const char *Shader::fs_source[] =
{
    "#version 420 core                                                                                                                                                                                      \n"
    "                                                                                                                                                                                                       \n"
    "in VS_OUT                                                                                                                                                                                              \n"
    "{                                                                                                                                                                                                      \n"
    "   vec3 N;                                                                                                                                                                                             \n"
    "   vec3 L;                                                                                                                                                                                             \n"
    "   vec3 V;                                                                                                                                                                                             \n"
    "   vec3 vertex_position;                                                                                                                                                                               \n"
    "   vec3 texture_position;                                                                                                                                                                              \n"
    "   vec3 normal_position;                                                                                                                                                                               \n"
    "   vec3 tangent_position;                                                                                                                                                                              \n"
    "   vec3 bitangent_position;                                                                                                                                                                            \n"
    "   vec3 diffuse_position;                                                                                                                                                                              \n"
    "   vec4 position_lightspace;                                                                                                                                                                           \n"
    "} fs_in;                                                                                                                                                                                               \n"
    "                                                                                                                                                                                                       \n"
    "layout (location = 0) out vec4 color;                                                                                                                                                                  \n"
    "                                                                                                                                                                                                       \n"
    "uniform vec4 ambient_light         = vec4(1.0, 1.0, 1.0, 1.0);                                                                                                                                         \n"
    "uniform vec4 diffuse_light         = vec4(1.0, 1.0, 1.0, 1.0);                                                                                                                                         \n"
    "uniform vec3 ambient_albedo        = vec3(1.0, 1.0, 1.0);                                                                                                                                              \n"
    "uniform vec3 diffuse_albedo        = vec3(1.0, 1.0, 1.0);                                                                                                                                              \n"
    "uniform vec3 specular_albedo       = vec3(1.0, 1.0, 1.0);                                                                                                                                              \n"
    "uniform int  reflectivity          = 0;                                                                                                                                                                \n"
    "uniform int  illumination          = 0;                                                                                                                                                                \n"
    "uniform bool texture_color_enable  = true;                                                                                                                                                             \n"
    "uniform bool texture_normal_enable = true;                                                                                                                                                             \n"
    "uniform bool test                  = false;                                                                                                                                                            \n"
    "uniform vec3 texture_scale         = vec3(1.0, 1.0, 1.0);                                                                                                                                              \n"
    "uniform vec3 texture_offset        = vec3(0.0, 0.0, 0.0);                                                                                                                                              \n"
    "uniform sampler2D texture_color_data;                                                                                                                                                                  \n"
    "uniform sampler2D texture_normal_data;                                                                                                                                                                 \n"
    "uniform sampler2D texture_shadow_data;                                                                                                                                                                 \n"
    "                                                                                                                                                                                                       \n"
    "void main(void)                                                                                                                                                                                        \n"
    "{                                                                                                                                                                                                      \n"
    "   vec3 N = normalize(fs_in.N);                                                                                                                                                                        \n"
    "   vec3 L = normalize(fs_in.L);                                                                                                                                                                        \n"
    "   vec3 V = normalize(fs_in.V);                                                                                                                                                                        \n"
    "   vec3 R = reflect(-L, N);                                                                                                                                                                            \n"
    "   vec3 vertex_position            = fs_in.vertex_position;                                                                                                                                            \n"
    "   vec3 texture_position           = fs_in.texture_position;                                                                                                                                           \n"
    "   vec3 normal_position            = fs_in.normal_position;                                                                                                                                            \n"
    "   vec3 tangent_position           = fs_in.tangent_position;                                                                                                                                           \n"
    "   vec3 bitangent_position         = fs_in.bitangent_position;                                                                                                                                         \n"
    "   vec3 diffuse_position           = fs_in.diffuse_position;                                                                                                                                           \n"
    "   vec4 position_lightspace        = fs_in.position_lightspace;                                                                                                                                        \n"
    "                                                                                                                                                                                                       \n"
    "   float x, y;                                                                                                                                                                                         \n"
    "   x =  -texture_position.x / texture_scale.x - texture_offset.x;                                                                                                                                      \n"
    "   y =  -texture_position.y / texture_scale.y - texture_offset.y;                                                                                                                                      \n"
    "   vec2 tex_pos = vec2(x, y);                                                                                                                                                                          \n"
    "                                                                                                                                                                                                       \n"
    "   vec3 projCoords = position_lightspace.xyz / position_lightspace.w;                                                                                                                                  \n"
    "   projCoords = 0.5 * projCoords + 0.5;                                                                                                                                                                \n"
    "                                                                                                                                                                                                       \n"
    "   float currentDepth = projCoords.z;                                                                                                                                                                  \n"
    "   float closestDepth = texture(texture_shadow_data, projCoords.xy).r;                                                                                                                                 \n"
    "                                                                                                                                                                                                       \n"
    "   float bias = 0.005;                                                                                                                                                                                 \n"
    "   float shadow =  0.0;                                                                                                                                                                                \n"
    "   if (closestDepth != 1.0)                                                                                                                                                                            \n"
    "   {                                                                                                                                                                                                   \n"
    "       shadow = (currentDepth - bias) > closestDepth  ? 1.0 : 0.0;                                                                                                                                     \n"
    "   }                                                                                                                                                                                                   \n"
    "                                                                                                                                                                                                       \n"
    "   if (texture_color_enable)                                                                                                                                                                           \n"
    "   {                                                                                                                                                                                                   \n"
    "       vec3 Normal    = normalize(normal_position);                                                                                                                                                    \n"
    "       vec3 Tangent   = normalize(tangent_position);                                                                                                                                                   \n"
    "       vec3 BiTangent = normalize(bitangent_position);                                                                                                                                                 \n"
    "                                                                                                                                                                                                       \n"
    "       vec3 TextureNormal = texture(texture_normal_data, tex_pos).rgb;                                                                                                                                 \n"
    "       TextureNormal = 2.0 * TextureNormal - 1.0;                                                                                                                                                      \n"
    "                                                                                                                                                                                                       \n"
    "       if (texture_normal_enable)                                                                                                                                                                      \n"
    "       {                                                                                                                                                                                               \n"
    "           mat3 TBN = mat3(Tangent, BiTangent, Normal);                                                                                                                                                \n"
    "           Normal = TBN * TextureNormal;                                                                                                                                                               \n"
    "       }                                                                                                                                                                                               \n"
    "                                                                                                                                                                                                       \n"
    "       float factor   = max(dot(Normal, L), 0.0) / sqrt(length(fs_in.L));                                                                                                                              \n"
    "       vec3  ambient  =          vec3(ambient_light) * ambient_light.a * ambient_albedo;                                                                                                               \n"
    "       vec3  diffuse  = factor * vec3(diffuse_light) * diffuse_light.a * diffuse_albedo;                                                                                                               \n"
    "       vec3  specular = factor * pow(max(dot(R, V), 0.0), reflectivity) * specular_albedo / 5.0;                                                                                                       \n"
    "                                                                                                                                                                                                       \n"
    "       color =                         texture(texture_color_data, tex_pos) * vec4(ambient,  1.0);                                                                                                     \n"
    "       color = color + (1.0- shadow) * texture(texture_color_data, tex_pos) * vec4(diffuse,  1.0);                                                                                                     \n"
    "       color = color + (1.0- shadow) * diffuse_light                        * vec4(specular, 1.0);                                                                                                     \n"
    "                                                                                                                                                                                                       \n"
    "       if (test)                                                                                                                                                                                       \n"
    "       {                                                                                                                                                                                               \n"
    "           //color = vec4(Normal, 1.0);                                                                                                                                                                  \n"
    "           if (currentDepth >  closestDepth) color = vec4(currentDepth - closestDepth, 0.0, 0.0, 1.0);                                                                                                 \n"
    "           if (currentDepth <  closestDepth) color = vec4(0.0, 0.0, closestDepth - currentDepth, 1.0);                                                                                                 \n"
    "           if ((currentDepth > closestDepth) && (currentDepth < closestDepth + 0.05)) color = vec4(1.0, 1.0, 0.0, 1.0);                                                                                \n"
    "           if ((currentDepth < closestDepth) && (currentDepth > closestDepth - 0.05)) color = vec4(0.0, 1.0, 0.0, 1.0);                                                                                \n"
    "       }                                                                                                                                                                                               \n"
    "   }                                                                                                                                                                                                   \n"
    "                                                                                                                                                                                                       \n"
    "   if (!texture_color_enable)                                                                                                                                                                          \n"
    "   {                                                                                                                                                                                                   \n"
    "       vec3 Normal = normalize(normal_position);                                                                                                                                                       \n"
    "                                                                                                                                                                                                       \n"
    "       float factor   = max(dot(Normal, L), 0.0) / sqrt(length(fs_in.L));                                                                                                                              \n"
    "       vec3  ambient  = vec3(ambient_light) * ambient_light.a * ambient_albedo;                                                                                                                        \n"
    "       vec3  diffuse  = factor * vec3(diffuse_light) * diffuse_light.a * diffuse_albedo;                                                                                                               \n"
    "       vec3  specular = factor * pow(max(dot(R, V), 0.0), reflectivity) * specular_albedo / 5.0;                                                                                                       \n"
    "                                                                                                                                                                                                       \n"
    "       color =                         vec4(ambient,  1.0);                                                                                                                                            \n"
    "       color = color + (1.0- shadow) * vec4(diffuse,  1.0);                                                                                                                                            \n"
    "       color = color + (1.0- shadow) * vec4(specular, 1.0);                                                                                                                                            \n"
    "                                                                                                                                                                                                       \n"
    "       if (test)                                                                                                                                                                                       \n"
    "       {                                                                                                                                                                                               \n"
    "           //color = vec4(Normal, 1.0);                                                                                                                                                                  \n"
    "           if (currentDepth >  closestDepth) color = vec4(currentDepth - closestDepth, 0.0, 0.0, 1.0);                                                                                                 \n"
    "           if (currentDepth <  closestDepth) color = vec4(0.0, 0.0, closestDepth - currentDepth, 1.0);                                                                                                 \n"
    "       }                                                                                                                                                                                               \n"
    "   }                                                                                                                                                                                                   \n"
    "}                                                                                                                                                                                                      \n"
};

/*********************************************************************
* Vertex shadow shader
*********************************************************************/
const char *Shader::vs_shadow_source[] =
{
    "#version 420 core                                                                                                                                                                                     \n"
    "                                                                                                                                                                                                      \n"
    "layout (location = 0) in vec3 vertex_position;                                                                                                                                                        \n"
    "                                                                                                                                                                                                      \n"
    "uniform mat4 path_shadow_matrix;                                                                                                                                                                      \n"
    "uniform mat4 proj_shadow_matrix;                                                                                                                                                                      \n"
    "                                                                                                                                                                                                      \n"
    "void main(void)                                                                                                                                                                                       \n"
    "{                                                                                                                                                                                                     \n"
    "   vec4 P =  path_shadow_matrix * vec4(vertex_position,  1.0);                                                                                                                                        \n"
    "   gl_Position = proj_shadow_matrix * P;                                                                                                                                                              \n"
    "}                                                                                                                                                                                                     \n"
};

/*********************************************************************
* Fragment shadow shader
*********************************************************************/
const char *Shader::fs_shadow_source[] =
{
    "#version 420 core                                                                                                                                                                                     \n"
    "                                                                                                                                                                                                      \n"
    "void main(void)                                                                                                                                                                                       \n"
    "{                                                                                                                                                                                                     \n"
    "   gl_FragDepth = gl_FragCoord.z;                                                                                                                                                                     \n"
    "}                                                                                                                                                                                                     \n"
};

/*********************************************************************
* Constructor
*********************************************************************/
Shader::Shader (Objects *pObjects)
{
    // Reset OpenGL error checking
    Error::ResetOpenGLError ();

    // Init viewer position, angle, pos_np, colors and rendermode
    angle_x         = 0.0f;
    angle_y         = 0.0f;
    angle_z         = 0.0f;
    pos_x           = 0.0f;
    pos_y           = 0.0f;
    pos_z           = -2.0f;
    pos_np          = NEARPLANEPOS;
    pos_fp          = FARPLANEPOS;
    render_mode     = RM_FACES;
    show_textures   = false;
    show_normalmap  = false;
    repeat_textures = false;
    filter_textures = false;
    show_outlining  = false;
    show_axis        = true;
    vertex_size     = 1;
    ambient_light    = { 1.0f, 1.0f, 1.0f, 0.3f };
    diffuse_light    = { 1.0f, 1.0f, 1.0f, 5.0f };

    // Set objects to render
    this->pObjects = pObjects;

    // Create vertex array objects
    glGenVertexArrays (pObjects->numMeshes, vao);

    // Create and compile the vertex and fragment shader
    CreateShaders();

    // Create programs for OpenGL
    CreatePrograms();

    // Create and set buffers (vertex, index)
    CreateBuffers();

    // Create and set textures
    CreateTextures();

    // Create and set framebuffer for shadow mapping
    CreateFrameBuffer();
}

/*********************************************************************
* Destructor
*********************************************************************/
Shader::~Shader (void)
{
    // Delete buffers for all objects
    for (unsigned int i = 0; i < pObjects->numMeshes; i++)
    {
        // Bind the vertex array object
        glBindVertexArray (vao[i]);

        glDeleteBuffers (1, &vertex_buffer[i]);
        glDeleteBuffers (1, &index_buffer[i]);
        glDeleteTextures(1, &texture_color_buffer[i]);
        glDeleteTextures(1, &texture_normal_buffer[i]);
    }

    glDeleteProgram (program);
    glDeleteProgram(program_shadow);
    glDeleteVertexArrays (MAX_NUM_MESHES, vao);

    // Check for OpenGL errors
    Error::CheckOpenGLError("Shader::~Shader");
}

/*********************************************************************
* Create and compile the vertex and fragment shader
*********************************************************************/
void Shader::CreateShaders(void)
{
    try
    {
        std::string info;
        GLint len;

        // Create and compile vertex shader
        vertex_shader = glCreateShader (GL_VERTEX_SHADER);
        glShaderSource (vertex_shader, 1, vs_source, NULL);
        glCompileShader (vertex_shader);

        // Write info about compilation of the vertex shader in log file
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1)
        {
            info.resize(len);
            glGetShaderInfoLog(vertex_shader, len, NULL, &info[0]);
            Error::WriteLog("INFO", "Shader::CreateShaders (Vertex)", info.c_str());
        }

        // Create and compile fragment shader
        fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);
        glShaderSource (fragment_shader, 1, fs_source, NULL);
        glCompileShader (fragment_shader);

        // Write info about compilation of the fragment shader in log file
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1)
        {
            info.resize(len);
            glGetShaderInfoLog(fragment_shader, len, NULL, &info[0]);
            Error::WriteLog("INFO", "Shader::CreateShaders (Fragment)", info.c_str());
        }

        // Create vertex shadow shader
        vertex_shadow_shader = glCreateShader(GL_VERTEX_SHADER);
        if (!vertex_shadow_shader)
        {
            std::string message = "Error creating vertex shadow shader object";
            Error::WriteLog("ERROR", "Shader::CreateShaders", message.c_str());
            exit(0);
        };

        // Set shadow shader source
        glShaderSource(vertex_shadow_shader, 1, vs_shadow_source, NULL);

        // Compile shadow shader
        glCompileShader(vertex_shadow_shader);

        // Create fragment shadow shader
        fragment_shadow_shader = glCreateShader(GL_FRAGMENT_SHADER);
        if (!fragment_shadow_shader)
        {
            std::string message = "Error creating fragment shadow shader object";
            Error::WriteLog("ERROR", "Shader::CreateShaders", message.c_str());
            exit(0);
        };

        // Set shader source
        glShaderSource(fragment_shadow_shader, 1, fs_shadow_source, NULL);

        // Compile shader
        glCompileShader(fragment_shadow_shader);

        // Write info about compilation of the vertex shader in log file
        glGetShaderiv(vertex_shadow_shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1)
        {
            info.resize(len);
            glGetShaderInfoLog(vertex_shadow_shader, len, NULL, &info[0]);
            Error::WriteLog("INFO", "Shader::CreateShaders (Shadow Vertex)", info.c_str());
        };

        // Write info about compilation of the fragment shader in log file
        glGetShaderiv(fragment_shadow_shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1)
        {
            info.resize(len);
            glGetShaderInfoLog(fragment_shadow_shader, len, NULL, &info[0]);
            Error::WriteLog("INFO", "Shader::CreateShaders (Shadow Fragment)", info.c_str());
        };

        // Check for OpenGL errors
        Error::CheckOpenGLError("Shader::CreateShaders - Create/Compile Shadow Shaders");

        // Check for OpenGL errors
        Error::CheckOpenGLError("Shader::CreateShaders - Create/Compile Shaders");
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Shader::CreateShaders", message.c_str());
    }
}

/*********************************************************************
* Create programs for OpenGL
*********************************************************************/
void Shader::CreatePrograms(void)
{
    try
    {
        std::string info;
        GLint len;

        // Create program
        program = glCreateProgram();

        // Attach shaders to the program
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        // Link the objects for an executable program
        glLinkProgram(program);

        // Check for OpenGL errors
        Error::CheckOpenGLError("Shader::CreatePrograms -Link Programs (Regular)");

        // Make sure link works too
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        if (len > 1)
        {
            info.resize(len);
            glGetProgramInfoLog(program, len, NULL, &info[0]);
            Error::WriteLog("INFO", "Shader::CreatePrograms (regular)", info.c_str());
        };

        // Use this program
        glUseProgram(program);

        // Delete the shaders
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        // Get the location of the projection and object-path matrix
        path_matrix_location = glGetUniformLocation(program, "path_matrix");
        proj_matrix_location = glGetUniformLocation(program, "proj_matrix");

        // Get the location of the lightspace matrix (for shadows)
        lightspace_matrix_location = glGetUniformLocation(program, "lightspace_matrix");

        // Get the location of the object vertices, normal vertices, texture vertices and (bi-)tangent vectors
        vertex_position_location = glGetAttribLocation(program, "vertex_position");
        normal_position_location = glGetAttribLocation(program, "normal_position");
        texture_position_location = glGetAttribLocation(program, "texture_position");
        tangent_position_location = glGetAttribLocation(program, "tangent_position");
        bitangent_position_location = glGetAttribLocation(program, "bitangent_position");

        // Get the location of the light, color, texture enable, texture data etc.
        ambient_light_location = glGetUniformLocation(program, "ambient_light");
        diffuse_position_location = glGetUniformLocation(program, "diffuse_position");
        diffuse_light_location = glGetUniformLocation(program, "diffuse_light");
        ambient_albedo_location = glGetUniformLocation(program, "ambient_albedo");
        diffuse_albedo_location = glGetUniformLocation(program, "diffuse_albedo");
        specular_albedo_location = glGetUniformLocation(program, "specular_albedo");
        texture_color_enable_location = glGetUniformLocation(program, "texture_color_enable");
        texture_normal_enable_location = glGetUniformLocation(program, "texture_normal_enable");
        texture_color_data_location = glGetUniformLocation(program, "texture_color_data");
        texture_normal_data_location = glGetUniformLocation(program, "texture_normal_data");
        texture_shadow_data_location = glGetUniformLocation(program, "texture_shadow_data");
        reflectivity_location = glGetUniformLocation(program, "reflectivity");
        illumination_location = glGetUniformLocation(program, "illumination");
        test_location = glGetUniformLocation(program, "test");

        // Get the location of the texture scale factors
        texture_scale_location = glGetUniformLocation(program, "texture_scale");

        // Get the location of the texture offset factors
        texture_offset_location = glGetUniformLocation(program, "texture_offset");

        // Get the location of the screen scale factor
        screen_scale_location = glGetUniformLocation(program, "screen_scale");

        // Check for OpenGL errors
        Error::CheckOpenGLError("Shader::CreatePrograms - Locations");

        // Create shadow program
        program_shadow = glCreateProgram();

        // Attach shaders to the program
        glAttachShader(program_shadow, vertex_shadow_shader);
        glAttachShader(program_shadow, fragment_shadow_shader);

        // Link the objects for an executable program
        glLinkProgram(program_shadow);

        // Check for OpenGL errors
        Error::CheckOpenGLError("Shader::CreatePrograms - Link Programs (Shadow)");

        // Make sure link works too
        glGetProgramiv(program_shadow, GL_INFO_LOG_LENGTH, &len);
        if (len > 1)
        {
            info.resize(len);
            glGetProgramInfoLog(program_shadow, len, NULL, &info[0]);
            Error::WriteLog("INFO", "Shader::CreatePrograms (Shadow)", info.c_str());
        };

        // Use this program
        glUseProgram(program_shadow);

        // Delete the shaders
        glDeleteShader(vertex_shadow_shader);
        glDeleteShader(fragment_shadow_shader);

        // Get the location in the shadow shader
        vertex_shadow_position_location = glGetAttribLocation(program_shadow, "vertex_position");

        // Should be the same, otherwise bad shadows
        if (vertex_shadow_position_location != vertex_position_location)
        {
            Error::WriteLog("ERROR", "Shader::CreatePrograms", "vertex_shadow_position_location != vertex_position_location => Bad Shadows");
        }

        // Get shadow matrix locations
        path_shadow_matrix_location = glGetUniformLocation(program_shadow, "path_shadow_matrix");
        proj_shadow_matrix_location = glGetUniformLocation(program_shadow, "proj_shadow_matrix");

        // Check for OpenGL errors
        Error::CheckOpenGLError("Shader::CreatePrograms - Locations (Shadow)");
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Shader::CreateProgram", message.c_str());
    }
}

/*********************************************************************
* Create and set buffers (vertex, index, normal)
*********************************************************************/
void Shader::CreateBuffers(void)
{
    try
    {
        std::string message;

        // Create buffers for all objects
        for (unsigned int i = 0; i < pObjects->numMeshes; i++)
        {
            // Bind the vertex array object
            glBindVertexArray (vao[i]);

            // Generate a name for the vertex buffer
            glGenBuffers (1, &vertex_buffer[i]);

            // Bind it to the context using the GL_ARRAY_BUFFER binding point
            glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer[i]);

            // Create the vertex buffer (vertices, normals, texture vertices and (bi-)tangent vectors)
            int bufSize = (pObjects->pMeshArray[i]->numVertices + pObjects->pMeshArray[i]->numVertexNormals + pObjects->pMeshArray[i]->numTextureVertices + pObjects->pMeshArray[i]->numTangents + pObjects->pMeshArray[i]->numBiTangents) * sizeof(VecMat::Vertex);
            glBufferData (GL_ARRAY_BUFFER, bufSize, NULL, GL_STATIC_DRAW);

            // Put vertices in the buffer.
            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            pObjects->pMeshArray[i]->numVertices * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->pVertices);

            // Put vertex normals in the buffer.
            glBufferSubData(GL_ARRAY_BUFFER, pObjects->pMeshArray[i]->numVertices * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->numVertexNormals * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->pVertexNormals);

            // Put texture vertices in the buffer.
            glBufferSubData(GL_ARRAY_BUFFER, (pObjects->pMeshArray[i]->numVertices + pObjects->pMeshArray[i]->numVertexNormals) * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->numTextureVertices * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->pTextureVertices);

            // Put tangent vectors in the buffer.
            glBufferSubData(GL_ARRAY_BUFFER, (pObjects->pMeshArray[i]->numVertices + pObjects->pMeshArray[i]->numVertexNormals + pObjects->pMeshArray[i]->numTextureVertices) * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->numTangents * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->pTangents);

            // Put bitangent vectors in the buffer.
            glBufferSubData(GL_ARRAY_BUFFER, (pObjects->pMeshArray[i]->numVertices + pObjects->pMeshArray[i]->numVertexNormals + pObjects->pMeshArray[i]->numTextureVertices + pObjects->pMeshArray[i]->numTangents) * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->numBiTangents * sizeof(VecMat::Vertex),
                            pObjects->pMeshArray[i]->pBiTangents);

            // Describe the data in the vertex buffer to OpenGL
            glVertexAttribPointer (vertex_position_location,    3, GL_FLOAT, GL_FALSE, 0, NULL);
            glVertexAttribPointer (normal_position_location,    3, GL_FLOAT, GL_FALSE, 0, (void *) ( pObjects->pMeshArray[i]->numVertices * sizeof(VecMat::Vertex)));
            glVertexAttribPointer (texture_position_location,   3, GL_FLOAT, GL_FALSE, 0, (void *) ((pObjects->pMeshArray[i]->numVertices + pObjects->pMeshArray[i]->numVertexNormals) * sizeof(VecMat::Vertex)));
            glVertexAttribPointer (tangent_position_location,   3, GL_FLOAT, GL_FALSE, 0, (void *) ((pObjects->pMeshArray[i]->numVertices + pObjects->pMeshArray[i]->numVertexNormals + pObjects->pMeshArray[i]->numTextureVertices) * sizeof(VecMat::Vertex)));
            glVertexAttribPointer (bitangent_position_location, 3, GL_FLOAT, GL_FALSE, 0, (void *) ((pObjects->pMeshArray[i]->numVertices + pObjects->pMeshArray[i]->numVertexNormals + pObjects->pMeshArray[i]->numTextureVertices + pObjects->pMeshArray[i]->numTangents) * sizeof(VecMat::Vertex)));

            // Turn it on
            glEnableVertexAttribArray (vertex_position_location);
            glEnableVertexAttribArray (normal_position_location);
            glEnableVertexAttribArray (texture_position_location);
            glEnableVertexAttribArray (tangent_position_location);
            glEnableVertexAttribArray (bitangent_position_location);

            // Generate a name for the index buffer
            glGenBuffers (1, &index_buffer[i]);

            // Bind it to the context using the GL_ELEMENT_ARRAY_BUFFER binding point
            glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer[i]);

            // Create the index buffer
            glBufferData (GL_ELEMENT_ARRAY_BUFFER, pObjects->pMeshArray[i]->numFaces * sizeof(VecMat::Face), pObjects->pMeshArray[i]->pFaces, GL_STATIC_DRAW);

            // Unbind the vertex array object
            glBindVertexArray (0);

            // Check for OpenGL errors
            message = "Shader::CreateBuffers - Create/Bind Buffers for object: " + pObjects->pMeshArray[i]->name;
            Error::CheckOpenGLError(message.c_str());
        }
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Shader::CreateBuffers", message.c_str());
    }
}

/*********************************************************************
* Create and set textures
*********************************************************************/
void Shader::CreateTextures(void)
{
    try
    {
        // Activate texture unit 0
        glActiveTexture(GL_TEXTURE0);

        // Set texture selector 0 in fragment shader program
        glProgramUniform1i(program, texture_color_data_location, 0);

        // Create textures for all materials (if present)
        for (unsigned int i = 0; i < pObjects->numMaterials; i++)
        {
            // Generate a name for the texture buffer
            glGenTextures(1, &texture_color_buffer[i]);

            // Bind it to the context using the GL_TEXTURE_2D binding point
            glBindTexture(GL_TEXTURE_2D, texture_color_buffer[i]);

            // Set texture for this Mesh (if present)
            if ((pObjects->pMaterialArray[i] != NULL) && (pObjects->pMaterialArray[i]->pTextureData != NULL))
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                // Set texture
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, pObjects->pMaterialArray[i]->textWidth, pObjects->pMaterialArray[i]->textHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pObjects->pMaterialArray[i]->pTextureData);

                // Set texture buffer name in the material
                pObjects->pMaterialArray[i]->texColorBuffer = texture_color_buffer[i];

                // Check for OpenGL errors
                Error::CheckOpenGLError("Shader::CreateTextures - Create/Bind Texture Buffers - GL_TEXTURE0");
            };
        };

        // Unbind
        glBindTexture(GL_TEXTURE_2D, 0);

        // Activate texture unit 1
        glActiveTexture(GL_TEXTURE1);

        // Set texture selector 1 in fragment shader program
        glProgramUniform1i(program, texture_normal_data_location, 1);

        // Create texture normals for all materials (if present)
        for (unsigned int i = 0; i < pObjects->numMaterials; i++)
        {
            // Generate a name for the texture normal buffer
            glGenTextures(1, &texture_normal_buffer[i]);

            // Bind it to the context using the GL_TEXTURE_2D binding point
            glBindTexture(GL_TEXTURE_2D, texture_normal_buffer[i]);

            // Set texture for this Mesh (if present)
            if ((pObjects->pMaterialArray[i] != NULL) && (pObjects->pMaterialArray[i]->pTextureNormalData != NULL))
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

                // Set texture
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, pObjects->pMaterialArray[i]->textWidth, pObjects->pMaterialArray[i]->textHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pObjects->pMaterialArray[i]->pTextureNormalData);

                // Set texture buffer name in the material
                pObjects->pMaterialArray[i]->texNormalBuffer = texture_normal_buffer[i];

                // Check for OpenGL errors
                Error::CheckOpenGLError("Shader::CreateTextures - Create/Bind Texture Buffers - GL_TEXTURE1");
            };
        };

        // Unbind
        glBindTexture(GL_TEXTURE_2D, 0);

        // Activate texture unit 2
        glActiveTexture(GL_TEXTURE2);

        // Set texture selector 2 in fragment shader program
        glProgramUniform1i(program, texture_shadow_data_location, 2);

        // Generate a name for the texture depth buffer
        glGenTextures(1, &texture_shadow_buffer);

        // Bind it to the context using the GL_TEXTURE_2D binding point
        glBindTexture(GL_TEXTURE_2D, texture_shadow_buffer);

        // Set texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        // Set texture mode to luminance
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

        // Disable compare mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        
        // Set texture border color to white
        float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        // Set the wrapping and filtering modes
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       
        // Check for OpenGL errors
        Error::CheckOpenGLError("Shader::CreateTextures - Create/Bind Texture Buffers - GL_TEXTURE2");

        // Unbind
        glBindTexture(GL_TEXTURE_2D, 0);
    } catch (...)
    {
        std::string message = "Error creating textures";
        Error::WriteLog("ERROR", "Shader::CreateTextures", message.c_str());
    }
}

/*********************************************************************
* Create and set framebuffer for shadow mapping
*********************************************************************/
void Shader::CreateFrameBuffer(void)
{
    try
    {
        std::string message;

        // Create a framebuffer (for shadow mapping)
        glGenFramebuffers(1, &frame_buffer);

        // Bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

        // Attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_shadow_buffer, 0);

        // No color buffer is drawn to
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // Always check that our framebuffer is ok
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::string message = "Error creating framebuffer, status not ok";
            Error::WriteLog("ERROR", "Shader::CreateFrameBuffer", message.c_str());
        };

        // Unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Check for OpenGL errors
        Error::CheckOpenGLError("Shader::CreateFrameBuffer");
    } catch (...)
    {
        std::string message = "Error creating framebuffer";
        Error::WriteLog("ERROR", "Shader::CreateFrameBuffer", message.c_str());
    }
}

/*********************************************************************
* Render shader object
*********************************************************************/
void Shader::Render(GLint screen_width, GLint screen_height, GLfloat offsetPos, GLfloat offsetAngle)
{
    try
    {
        // Render to depth map
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

        // Using the shadow program to render to the depthbuffer
        glUseProgram(program_shadow);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
           Error::CheckOpenGLError("Shader::Render - Set Shadow Shader");
        #endif

        // In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly
        glViewport(0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);

        // Calculate the projection matrix
        //VecMat::Mat4 proj_shadow_matrix = VecMat::Orthogonal(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
        VecMat::Mat4 proj_shadow_matrix = VecMat::Frustum(NPLEFT, NPRIGHT, NPBOTTOM, NPTOP, pos_np, pos_fp);
        glProgramUniformMatrix4fv(program_shadow, proj_shadow_matrix_location, 1, GL_FALSE, proj_shadow_matrix);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
            Error::CheckOpenGLError("Shader::Render - ProjMatrix (Shadows)");
        #endif

        // Calculate the object-path matrix (for shadow shader)
        VecMat::Vec3 eye = VecMat::Vec3(diff_pos_x, diff_pos_y, diff_pos_z);
        VecMat::Vec3 center = VecMat::Vec3(0.0f, 0.0f, 0.0f);
        VecMat::Vec3 up = VecMat::Vec3(0.0f, 1.0f, 0.0f);
        VecMat::Mat4 path_shadow_matrix = VecMat::LookAt(eye, center, up);
        glProgramUniformMatrix4fv(program_shadow, path_shadow_matrix_location, 1, GL_FALSE, path_shadow_matrix);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
            Error::CheckOpenGLError("Shader::Render - PathMatrix (Shadows)");
        #endif

        // Draw all objects (shadow mode)
        DrawObjects(true, offsetPos, offsetAngle);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
            Error::CheckOpenGLError("Shader::Render - Draw Objects (Shadows)");
        #endif

        // Now the regular rendering (disable framebuffer)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Set regular sizes
        glViewport(0, 0, screen_width, screen_height);

        // Use regular program
        glUseProgram(program);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
            Error::CheckOpenGLError("Shader::Render - Set Main Shader");
        #endif

        // Set test conditions in shader
        if (test) glUniform1i(test_location, GL_TRUE);
        else glUniform1i(test_location, GL_FALSE);

        // Set screen width and height
        glUniform2fv(screen_scale_location, 1, VecMat::Vec2(1000.0f / (float)screen_width, 1000.0f / (float)screen_height));

        // Set ambient light (color and power)
        glUniform4fv(ambient_light_location, 1, VecMat::Vec4 (ambient_light.r, ambient_light.g, ambient_light.b, ambient_light.a));

        // Set diffuse light (color and power)
        glUniform4fv(diffuse_light_location, 1, VecMat::Vec4 (diffuse_light.r, diffuse_light.g, diffuse_light.b, diffuse_light.a));

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
            Error::CheckOpenGLError("Shader::Render - Color/Ambient Light");
        #endif

        // Set diffuse light position
        VecMat::Vec3 diffuse_position(diff_pos_x, diff_pos_y, diff_pos_z);
        glUniform3fv(diffuse_position_location, 1, diffuse_position);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
            Error::CheckOpenGLError("Shader::Render - Diffuse Light Position");
        #endif

        // Calculate the projection matrix
        VecMat::Mat4 proj_matrix = VecMat::Frustum(NPLEFT, NPRIGHT, NPBOTTOM, NPTOP, pos_np, pos_fp);
        glUniformMatrix4fv(proj_matrix_location, 1, GL_FALSE, proj_matrix);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
            Error::CheckOpenGLError("Shader::Render - ProjMatrix");
        #endif

        glUniformMatrix4fv(lightspace_matrix_location, 1, GL_FALSE, path_shadow_matrix);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
        Error::CheckOpenGLError("Shader::Render - PathMatrix (Shadows)");
        #endif

        // Draw all objects
        DrawObjects(false, offsetPos, offsetAngle);

        // Check for OpenGL errors and report in info file
        #ifdef DEBUG
        Error::CheckOpenGLError("Shader::Render - DrawObjects");
        #endif

    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Shader::Render", message.c_str());
    }
}

/*********************************************************************
* Draw all objects
*********************************************************************/
void Shader::DrawObjects(bool shadowMap, GLfloat offsetPos, GLfloat offsetAngle)
{
    try
    {
        // Calculate all (x,y and z) position offsets for the hdk2 device (if needed)
        GLfloat offsetPosX = 0.0f;
        GLfloat offsetPosY = 0.0f;
        GLfloat offsetPosZ = 0.0f;
        offsetPosX = offsetPos * cos(VecMat::DtoR(angle_y));
        offsetPosZ = offsetPos * sin(VecMat::DtoR(angle_y));

        // Calculate all (x,y and z) angle offsets for the hdk2 device (if needed)
        GLfloat offsetAngleX = 0.0f;
        GLfloat offsetAngleY = 0.0f;
        GLfloat offsetAngleZ = 0.0f;
        offsetAngleY = offsetAngle * cos(VecMat::DtoR(angle_x));

        // Clear previous frame values
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render all objects
        for (unsigned int i = 0; i < pObjects->numMeshes; i++)
        {
            // If Axis check if show is needed
            if ((pObjects->pMeshArray[i]->name.substr(0, 4) != "Axis") || (show_axis))
            {
                // Bind the vertex array object
                glBindVertexArray(vao[i]);

                // Check for OpenGL errors and report in info file
                #ifdef DEBUG
                Error::CheckOpenGLError("Shader::DrawObjects - Bind Vertex Array");
                #endif

                // Standard depth settings
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glDepthFunc(GL_LEQUAL);
                glDepthRange(0.0, 1.0);

                // Enable front face culling
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);

                if (shadowMap)
                {
                    //Disable color rendering, we only want to write to the Z-Buffer
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

                    // Don't draw the bulb and sky in shadowmap
                    if ((pObjects->pMeshArray[i]->name != "Bulb") && (pObjects->pMeshArray[i]->name != "Sky"))
                    {
                        // Set mode
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                        // For all materials in this object, draw faces connected to it
                        for (unsigned int j = 0; j < pObjects->pMeshArray[i]->numMaterials; j++)
                        {
                            // Calculate number of triangles to draw with this material
                            unsigned int num_triangles;
                            if (j < (pObjects->pMeshArray[i]->numMaterials - 1))
                            {
                                num_triangles = pObjects->pMeshArray[i]->pMaterialEntryList[j + 1]->start - pObjects->pMeshArray[i]->pMaterialEntryList[j]->start;
                            } else
                            {
                                num_triangles = pObjects->pMeshArray[i]->numFaces - pObjects->pMeshArray[i]->pMaterialEntryList[j]->start;
                            };

                            // Draw triangles
                            glDrawElements(GL_TRIANGLES, 3 * num_triangles, GL_UNSIGNED_INT, (void*)(3 * pObjects->pMeshArray[i]->pMaterialEntryList[j]->start * sizeof(GL_UNSIGNED_INT)));
                        };

                        // Check for OpenGL errors and report in info file
                        #ifdef DEBUG
                        Error::CheckOpenGLError("Shader::DrawObjects - Draw shadow objects");
                        #endif
                    }
                } else
                {
                    //Enabling color write
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

                    // Calculate the object-path matrix
                    VecMat::Mat4 path_matrix = VecMat::Rotate(angle_x + offsetAngleX, angle_y + offsetAngleY, angle_z + offsetAngleZ) * VecMat::Translate(pos_x + offsetPosX, pos_y + offsetPosY, pos_z + offsetPosZ);
                    glUniformMatrix4fv(path_matrix_location, 1, GL_FALSE, path_matrix);

                    // Check for OpenGL errors and report in info file
                    #ifdef DEBUG
                    Error::CheckOpenGLError("Shader::DrawObjects - ObjPathMatrix");
                    #endif

                    // If Bulb (Light) then adjust the object-path matrix
                    if (pObjects->pMeshArray[i]->name == "Bulb")
                    {
                        path_matrix = path_matrix * VecMat::Translate(this->diff_pos_x, this->diff_pos_y, this->diff_pos_z);
                        glUniformMatrix4fv(path_matrix_location, 1, GL_FALSE, path_matrix);
                    }

                    // If Sky then disable depth test and front face culling and adjust the object-path matrix (only rotation)
                    if (pObjects->pMeshArray[i]->name == "Sky")
                    {
                        // Disable depth settings
                        glDisable(GL_DEPTH_TEST);
                        glDepthMask(GL_FALSE);

                        // Disable front face culling
                        glDisable(GL_CULL_FACE);

                        path_matrix = VecMat::Rotate(angle_x, angle_y, angle_z);
                        glUniformMatrix4fv(path_matrix_location, 1, GL_FALSE, path_matrix);
                    }

                    // If Axis then disable front face culling
                    if (pObjects->pMeshArray[i]->name == "Axis")
                    {
                        // Disable front face culling
                        glDisable(GL_CULL_FACE);
                    }

                    // Check for OpenGL errors and report in info file
                    #ifdef DEBUG
                    Error::CheckOpenGLError("Shader::DrawObjects - Bulb/Sky ObjPathMatrix");
                    #endif

                    // Draw faces
                    if (render_mode == RM_FACES)
                    {
                        // Set mode
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                        // For all materials in this object, draw faces connected to it
                        for (unsigned int j = 0; j < pObjects->pMeshArray[i]->numMaterials; j++)
                        {
                            // Set default texture mode (enabled / disabled)
                            if (show_textures) glProgramUniform1i(program, texture_color_enable_location, GL_TRUE);
                            else glProgramUniform1i(program, texture_color_enable_location, GL_FALSE);

                            // Set default texture normal map mode (enabled / disabled)
                            if (show_normalmap && pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pTextureNormalData != NULL) glProgramUniform1i(program, texture_normal_enable_location, GL_TRUE);
                            else glProgramUniform1i(program, texture_normal_enable_location, GL_FALSE);

                            // Set the material properties (if available)
                            if (pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial != NULL)
                            {
                                // Set ambient material properties
                                if (pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pKa != NULL)
                                {
                                    glProgramUniform3fv(program, ambient_albedo_location, 1, *pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pKa);
                                } else
                                {
                                    VecMat::Vec3 ambient_albedo(1.0f, 1.0f, 1.0f);
                                    glProgramUniform3fv(program, ambient_albedo_location, 1, ambient_albedo);
                                }

                                // Set diffuse material properties
                                if (pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pKd != NULL)
                                {
                                    glProgramUniform3fv(program, diffuse_albedo_location, 1, *pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pKd);
                                } else
                                {
                                    VecMat::Vec3 diffuse_albedo(0.64f, 0.64f, 0.64f);
                                    glProgramUniform3fv(program, diffuse_albedo_location, 1, diffuse_albedo);
                                }

                                // Set specular material properties
                                if (pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pKs != NULL)
                                {
                                    glProgramUniform3fv(program, specular_albedo_location, 1, *pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pKs);
                                } else
                                {
                                    VecMat::Vec3 specular_albedo(0.5f, 0.5f, 0.5f);
                                    glProgramUniform3fv(program, specular_albedo_location, 1, specular_albedo);
                                }

                                // Set reflective material properties
                                glProgramUniform1i(program, reflectivity_location, pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->iKn);

                                // Set illumination model
                                glProgramUniform1i(program, illumination_location, pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->iIllum);

                                // Bind the texture to render (if available)
                                if (pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->texColorBuffer != 0)
                                {
                                    // Activate texture selector 0
                                    glActiveTexture(GL_TEXTURE0);

                                    // Bind texture
                                    glBindTexture(GL_TEXTURE_2D, pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->texColorBuffer);

                                    // Check for OpenGL errors and report in info file
                                    #ifdef DEBUG
                                    Error::CheckOpenGLError("Shader::DrawObjects - Bind Color Texture");
                                    #endif

                                    // Set environment mode (replace object color with that of texture)
                                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

                                    // Set texture scale
                                    VecMat::Vec3 scale_factor(pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->scaleX, pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->scaleY, pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->scaleZ);
                                    glProgramUniform3fv(program, texture_scale_location, 1, scale_factor);

                                    // Set texture offset
                                    VecMat::Vec3 offset_factor(pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->offsetX, pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->offsetY, pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->offsetZ);
                                    glProgramUniform3fv(program, texture_offset_location, 1, offset_factor);

                                    // Set texture wrap mode
                                    if (repeat_textures)
                                    {
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
                                    } else
                                    {
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
                                    }

                                    // Set texture filtering
                                    if (filter_textures && (pObjects->pMeshArray[i]->name != "Sky"))
                                    {
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                    } else
                                    {
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                                    }

                                    // Check for OpenGL errors and report in info file
                                    #ifdef DEBUG
                                    Error::CheckOpenGLError("Shader::DrawObjects - Texture Environment");
                                    #endif

                                    // Bind the normal texture map to render (if available)
                                    if (pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->texNormalBuffer != 0)
                                    {
                                        // Activate texture selector 1
                                        glActiveTexture(GL_TEXTURE1);

                                        // Bind texture
                                        glBindTexture(GL_TEXTURE_2D, pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->texNormalBuffer);

                                        // Check for OpenGL errors and report in info file
                                        #ifdef DEBUG
                                        Error::CheckOpenGLError("Shader::DrawObjects - Bind Normal Texture");
                                        #endif

                                        // Set texture wrap mode
                                        if (repeat_textures)
                                        {
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
                                        } else
                                        {
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
                                        };

                                        // Set texture filtering
                                        if (filter_textures)
                                        {
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                        } else
                                        {
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                                        }
                                    }

                                    // Bind the depth texture map to render (for shadows)
                                    if (texture_shadow_buffer != 0)
                                    {
                                        // Activate texture selector 2
                                        glActiveTexture(GL_TEXTURE2);

                                        // Bind texture
                                        glBindTexture(GL_TEXTURE_2D, texture_shadow_buffer);

                                        // Check for OpenGL errors and report in info file
                                        #ifdef DEBUG
                                        Error::CheckOpenGLError("Shader::DrawObjects - Bind Depth Texture");
                                        #endif
                                    }
                                } else
                                {
                                    // Disable textures (not available for this material)
                                    glProgramUniform1i(program, texture_color_enable_location, GL_FALSE);
                                }
                            }

                            // Calculate number of triangles to draw with this material
                            unsigned int num_triangles;
                            if (j < (pObjects->pMeshArray[i]->numMaterials - 1))
                            {
                                num_triangles = pObjects->pMeshArray[i]->pMaterialEntryList[j + 1]->start - pObjects->pMeshArray[i]->pMaterialEntryList[j]->start;
                            } else
                            {
                                num_triangles = pObjects->pMeshArray[i]->numFaces - pObjects->pMeshArray[i]->pMaterialEntryList[j]->start;
                            };

                            // Draw triangles
                            glDrawElements(GL_TRIANGLES, 3 * num_triangles, GL_UNSIGNED_INT, (void*)(3 * pObjects->pMeshArray[i]->pMaterialEntryList[j]->start * sizeof(GL_UNSIGNED_INT)));
                        }

                        if (show_outlining)
                        {
                            // Disable texture mode
                            glProgramUniform1i(program, texture_color_enable_location, GL_FALSE);

                            // Set ambient to black for outlining the object
                            VecMat::Vec3 ambient_albedo(0.0f, 0.0f, 0.0f);
                            glProgramUniform3fv(program, ambient_albedo_location, 1, ambient_albedo);

                            // Set mode
                            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                            // Draw triangle lines (outlining the edges)
                            glDrawElements(GL_TRIANGLES, 3 * pObjects->pMeshArray[i]->numFaces, GL_UNSIGNED_INT, 0);
                        }
                    }

                    // Draw edges
                    if (render_mode == RM_EDGES)
                    {
                        // Disable texture mode
                        glProgramUniform1i(program, texture_color_enable_location, GL_FALSE);

                        // Set mode
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                        // For all materials in this object, draw edges connected to it
                        for (unsigned int j = 0; j < pObjects->pMeshArray[i]->numMaterials; j++)
                        {
                            // Set ambient material properties
                            if (pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pKa != NULL)
                            {
                                glProgramUniform3fv(program, ambient_albedo_location, 1, *pObjects->pMeshArray[i]->pMaterialEntryList[j]->pMaterial->pKa);
                            } else
                            {
                                VecMat::Vec3 ambient_albedo(1.0f, 1.0f, 1.0f);
                                glProgramUniform3fv(program, ambient_albedo_location, 1, ambient_albedo);
                            }

                            // Calculate number of triangles to draw with this material
                            unsigned int num_triangles;
                            if (j < (pObjects->pMeshArray[i]->numMaterials - 1))
                            {
                                num_triangles = pObjects->pMeshArray[i]->pMaterialEntryList[j + 1]->start - pObjects->pMeshArray[i]->pMaterialEntryList[j]->start;
                            } else
                            {
                                num_triangles = pObjects->pMeshArray[i]->numFaces - pObjects->pMeshArray[i]->pMaterialEntryList[j]->start;
                            }

                            // Draw triangles
                            glDrawElements(GL_TRIANGLES, 3 * num_triangles, GL_UNSIGNED_INT, (void*)(3 * pObjects->pMeshArray[i]->pMaterialEntryList[j]->start * sizeof(GL_UNSIGNED_INT)));
                        }
                    }

                    // Draw vertices
                    if (render_mode == RM_VERTICES)
                    {
                        // Disable texture mode
                        glProgramUniform1i(program, texture_color_enable_location, GL_FALSE);

                        // Set vertex size
                        glPointSize(vertex_size);

                        // Set ambient material properties
                        VecMat::Vec3 ambient_albedo(1.0f, 1.0f, 1.0f);
                        glProgramUniform3fv(program, ambient_albedo_location, 1, ambient_albedo);

                        // Draw points
                        glDrawArrays(GL_POINTS, 0, pObjects->pMeshArray[i]->numVertices);
                    }

                    // Check for OpenGL errors and report in info file
                    #ifdef DEBUG
                    Error::CheckOpenGLError("Shader::DrawObjects - Draw Objects/Arrays");
                    #endif
                }
            }
        }

        // Unbind the vertex array object
        glBindVertexArray(0);

    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Shader::DrawObjects", message.c_str());
    }
}

/*********************************************************************
* Set color for ambient light
*********************************************************************/
void Shader::SetAmbientLight (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    this->ambient_light.r = red;
    this->ambient_light.g = green;
    this->ambient_light.b = blue;
    this->ambient_light.a = alpha;
}

/*********************************************************************
* Set color for diffuse light
*********************************************************************/
void Shader::SetDiffuseLight (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    this->diffuse_light.r = red;
    this->diffuse_light.g = green;
    this->diffuse_light.b = blue;
    this->diffuse_light.a = alpha;
}

/*********************************************************************
* Set position for diffuse light
*********************************************************************/
void Shader::SetDiffusePosition (GLfloat x, GLfloat y, GLfloat z)
{
    this->diff_pos_x = x;
    this->diff_pos_y = y;
    this->diff_pos_z = z;

    if (test)
    {
        try
        {
            // Activate texture selector 2
            glActiveTexture(GL_TEXTURE2);

            // Bind texture
            glBindTexture(GL_TEXTURE_2D, texture_shadow_buffer);

            // Allocate memory
            float *data = (float *)malloc(SHADOWMAP_WIDTH * SHADOWMAP_HEIGHT * sizeof(float));

            // Get data into memory
            glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);

            // Allocate memory
            char *bmpData = (char *)malloc(SHADOWMAP_WIDTH * SHADOWMAP_HEIGHT * 3);

            for (int i = 0; i < (SHADOWMAP_WIDTH * SHADOWMAP_HEIGHT); i++)
            {
                bmpData[3 * i] = (char)(data[i] * 255.0f);
                bmpData[3 * i + 1] = (char)(data[i] * 255.0f);
                bmpData[3 * i + 2] = (char)(data[i] * 255.0f);
            }

            // Save bump from data
            //Utils::WriteBmp(L"ShadowMap", SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, bmpData);
            Utils::ShowBmp(L"ShadowMap", SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, bmpData);

            // De-Allocate memory
            free(data);
            free(bmpData);
        } catch (...)
        {
            std::string message = "Error showing shadowmap bump";
            Error::WriteLog("ERROR", "Shader::SetTest", message.c_str());
        }
    }
}

/*********************************************************************
* Set view angle
*********************************************************************/
void Shader::SetViewAngle (GLfloat angle_x, GLfloat angle_y, GLfloat angle_z)
{
    this->angle_x = angle_x;
    this->angle_y = angle_y;
    this->angle_z = angle_z;
}

/*********************************************************************
* Get view angle (viewing from 'ground level' to left and right)
*********************************************************************/
GLfloat Shader::GetViewAngle (void)
{
    return (VecMat::DtoR(angle_y));
}

/*********************************************************************
* Change view angle
*********************************************************************/
void Shader::ChangeViewAngle (GLfloat delta_angle_x, GLfloat delta_angle_y, GLfloat delta_angle_z)
{
    this->angle_x += delta_angle_x;
    this->angle_y += delta_angle_y;
    this->angle_z += delta_angle_z;

    if (this->angle_x > 360.0f) this->angle_x = 0.0f;
    if (this->angle_x < 0.0f)   this->angle_x = 360.0f;

    if (this->angle_y > 360.0f) this->angle_y = 0.0f;
    if (this->angle_y < 0.0f)   this->angle_y = 360.0f;

    if (this->angle_z > 360.0f) this->angle_z = 0.0f;
    if (this->angle_z < 0.0f)   this->angle_z = 360.0f;
}

/*********************************************************************
* Set view position
*********************************************************************/
void Shader::SetViewPosition (GLfloat pos_x, GLfloat pos_y, GLfloat pos_z)
{
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->pos_z = pos_z;
}

/*********************************************************************
* Change view position
*********************************************************************/
void Shader::ChangeViewPosition (GLfloat delta_pos_x, GLfloat delta_pos_y, GLfloat delta_pos_z)
{
    float viewAngle = GetViewAngle();
    this->pos_x += (delta_pos_x * cos(viewAngle) - delta_pos_z * sin(viewAngle));
    this->pos_y += delta_pos_y;
    this->pos_z += (delta_pos_z * cos(viewAngle) + delta_pos_x * sin(viewAngle));
}

/*********************************************************************
* Set position of near plane
*********************************************************************/
void Shader::SetNearPlanePosition (GLfloat pos_np)
{
    this->pos_np = pos_np;
}

/*********************************************************************
* Set position of far plane
*********************************************************************/
void Shader::SetFarPlanePosition (GLfloat pos_fp)
{
    this->pos_fp = pos_fp;
}

/*********************************************************************
* Set show textures
*********************************************************************/
void Shader::ShowTextures (bool show)
{
    this->show_textures = show;
}

/*********************************************************************
* Set show normalmap
*********************************************************************/
void Shader::ShowNormalMap (bool show)
{
    this->show_normalmap = show;
}

/*********************************************************************
* Set test conditions in shader
*********************************************************************/
void Shader::SetTest (bool test)
{
    // If end of test then close bump view window
    if (!test && this->test)
    {
        if (Utils::hWndBump != NULL)
        {
            DestroyWindow(Utils::hWndBump);
            Utils::hWndBump = NULL;
        }
    }

    this->test = test;
}

/*********************************************************************
* Set filter textures
*********************************************************************/
void Shader::FilterTextures (bool filter)
{
    this->filter_textures = filter;
}

/*********************************************************************
* Set repeat textures
*********************************************************************/
void Shader::RepeatTextures (bool repeat)
{
    this->repeat_textures = repeat;
}

/*********************************************************************
* Set show outlining
*********************************************************************/
void Shader::ShowOutlining (bool show)
{
    this->show_outlining = show;
}

/*********************************************************************
* Set show axis
*********************************************************************/
void Shader::ShowAxis(bool show)
{
    this->show_axis = show;
}

/*********************************************************************
* Set rendermode (vertices, edges or faces)
*********************************************************************/
void Shader::SetRenderMode (RenderMode render_mode)
{
    this->render_mode = render_mode;
}

/*********************************************************************
* Set vertex size
*********************************************************************/
void Shader::SetVertexSize (GLint vsize)
{
    this->vertex_size = vsize;
}

/*********************************************************************
* Set screen size
*********************************************************************/
void Shader::SetScreenSize (GLint width, GLint height)
{
    this->screen_width = width;
    this->screen_height = width;
}

/*********************************************************************
* Get info (position, angle etc.)
*********************************************************************/
std::string Shader::GetInfo (void)
{
    std::string info;

    info.append(" X-position = " + Utils::FloatToStr(this->pos_x) + "\r\n");
    info.append(" Y-position = " + Utils::FloatToStr(this->pos_y) + "\r\n");
    info.append(" Z-position = " + Utils::FloatToStr(this->pos_z) + "\r\n\r\n");

    info.append(" X-angle    = " + Utils::FloatToStr(this->angle_x) + "\r\n");
    info.append(" Y-angle    = " + Utils::FloatToStr(this->angle_y) + "\r\n");
    info.append(" Z-angle    = " + Utils::FloatToStr(this->angle_z) + "\r\n\r\n");

    info.append(" Near Plane = -" + Utils::FloatToStr(this->pos_np) + "\r\n");
    info.append(" Far  Plane = -" + Utils::FloatToStr(FARPLANEPOS) + "\r\n\r\n");

    info.append(" Bulb X-position = " + Utils::FloatToStr(this->diff_pos_x) + "\r\n");
    info.append(" Bulb Y-position = " + Utils::FloatToStr(this->diff_pos_y) + "\r\n");
    info.append(" Bulb Z-position = " + Utils::FloatToStr(this->diff_pos_z) + "\r\n\r\n");

    info.append("--------------------------------------------------------------------------------\r\n");

    return (info);
}

