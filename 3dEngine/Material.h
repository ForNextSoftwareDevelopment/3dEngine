#pragma once
#include <windows.h>
#include <string>
#include <cctype>
#include <vector>
#include "GL/glew.h"
#include "Error.h"
#include "Utils.h"
#include "VecMat.h"

class Material
{
    private:

    public:

        // Name of Material
        std::string name;

        // Ambient color albedo
        VecMat::Vec3 Ka;

        // Diffuse color albedo
        VecMat::Vec3 Kd;

        // Specular color albedo
        VecMat::Vec3 Ks;

        // Specular reflectivity factor
        GLint Kn;

        // Illumination model
        GLint Illum;

        // Texture (raw) data
        GLubyte *pTextureColorData;

        // Texture Normal (raw) data
        GLubyte *pTextureNormalData;

        // Width and Height of the texture
        GLint textHeight, textWidth;

        // Scaling factors of the texture
        GLfloat scaleX, scaleY, scaleZ;

        // Offset factors of the texture
        GLfloat offsetX, offsetY, offsetZ;

        // Name of buffer for this texture
        GLuint texColorBuffer;

        // Name of normal buffer for this texture
        GLuint texNormalBuffer;

        // Constructor
        Material (std::string name);

        // Destructor
        ~Material (void);

        // Read texture from a bmp file
        bool ReadTexture (const char *fileName);

        // Read normal texture from a bmp file
        bool ReadNormalTexture (const char *fileName);
};

