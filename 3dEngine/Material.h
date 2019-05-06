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
        VecMat::Vec3 *pKa;

        // Diffuse color albedo
        VecMat::Vec3 *pKd;

        // Specular color albedo
        VecMat::Vec3 *pKs;

        // Specular reflectivity factor
        GLint iKn;

        // Illumination model
        GLint iIllum;

        // Texture (raw) data
        GLubyte *pTextureData;

        // Texture Normal (raw) data
        GLubyte *pTextureNormalData;

        // Width and Height of the texture
        GLint textHeight, textWidth;

        // Scaling factors of the texture
        GLfloat scaleX, scaleY, scaleZ;

        // Offset factors of the texture
        GLfloat offsetX, offsetY, offsetZ;

        // Name of buffer for this texture
        GLuint texBuffer;

        // Name of normal buffer for this texture
        GLuint texNormalBuffer;

        // Constructor
        Material (std::string name);

        // Destructor
        ~Material (void);

        // Read texture from a bmp file
        void ReadTexture (const char *fileName);

        // Read normal texture from a bmp file
        void ReadNormalTexture (const char *fileName);
};

