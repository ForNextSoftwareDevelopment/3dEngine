#include "Material.h"

/*********************************************************************
* Constructor
*********************************************************************/
Material::Material (std::string name)
{
    // Set default name of Material
    this->name.append(name);

    // Set pointer to texture
    pTextureColorData  = NULL;

    // Set pointer to texture Normal
    pTextureNormalData  = NULL;

    // Set default color albedo
    Ka = VecMat::Vec3(1.0f, 1.0f, 1.0f);
    Kd = VecMat::Vec3(0.8f, 0.8f, 0.8f);
    Ks = VecMat::Vec3(0.1f, 0.1f, 0.1f);

    // Set specular reflectivity
    Kn = 64;

    // Set illumination model (no illumination)
    Illum = 1;

    // Set texture height and width
    textHeight = 0;
    textWidth  = 0;

    // Set scaling factors for the texture
    scaleX = 1.000000f;
    scaleY = 1.000000f;
    scaleZ = 1.000000f;

    // Set offset factors for the texture
    offsetX = 0.000000f;
    offsetY = 0.000000f;
    offsetZ = 0.000000f;

    // Set name of texture buffer
    texColorBuffer = 0;
}

/*********************************************************************
* Destructor
*********************************************************************/
Material::~Material (void)
{
    if (pTextureColorData  != NULL) delete (pTextureColorData);
    if (pTextureNormalData  != NULL) delete (pTextureNormalData);
}

/*********************************************************************
* Read the texture for this material
*********************************************************************/
bool Material::ReadTexture (const char *pFileName)
{
    // Pointer to file buffer (contents of file)
    char *pBuffer = NULL;

    // Variables for loading bitmap file
    BITMAPFILEHEADER *pBmFh;
    BITMAPINFO *pBmi;
    GLubyte *pBits;

    // Error message
    std::string message;

    try
    {
        // Read file
        unsigned int fileSize;
        pBuffer = Files::ReadBinary(pFileName, &fileSize);
        if (pBuffer == NULL)
        {
            message.clear();
            message.append("Could not read file: ");
            message.append(pFileName);
            Error::WriteLog("WARNING", "Material::ReadTexture", message.c_str());
            return(false);
        }

        // If file has been (correctly) read
        if (pBuffer != NULL)
        {
            pBmFh = (BITMAPFILEHEADER*)pBuffer;
            
            // Check if image is the correct format (bmp bitmap file)
            if (pBmFh->bfType != * (WORD *) "BM")
            {
                message.clear();
                message.append("File read of texture cancelled, is not a bitmap: ");
                message.append(pFileName);
                Error::WriteLog("WARNING", "Material::ReadTexture", message.c_str());

                // Free buffer memory
                delete pBuffer;
                return(false);
            }

            // Get the pointer to the info structure
            pBmi = (BITMAPINFO *) (pBmFh + 1);

            // Check if image is the correct format (24 or 32 bits)
            if ((pBmi->bmiHeader.biBitCount != 32) && (pBmi->bmiHeader.biBitCount != 24))
            {
                message.clear();
                message.append("File read of texture cancelled, is not a 24 or 32 bits bitmap: ");
                message.append(pFileName);
                Error::WriteLog("WARNING", "Material::ReadTexture", message.c_str());

                // Free buffer memory
                delete pBuffer;
                return(false);
            }

            // Check if image is the correct format (32 bits not compressed)
            if ((pBmi->bmiHeader.biCompression != BI_BITFIELDS) && (pBmi->bmiHeader.biCompression != BI_RGB))
            {
                message.clear();
                message.append("File read of texture cancelled, is a compressed bitmap: ");
                message.append(pFileName);
                Error::WriteLog("WARNING", "Material::ReadTexture", message.c_str());

                // Free buffer memory
                delete pBuffer;
                return(false);
            }

            // Get the DIB width and height
            textWidth = abs (pBmi->bmiHeader.biWidth);
            textHeight = abs (pBmi->bmiHeader.biHeight);

            // Get the pointer to the image bits
            pBits = (GLubyte *) pBmFh + (GLubyte) pBmFh->bfOffBits;

            // Allocate memory for texture
            GLuint tSize  = textWidth * textHeight * 4 * sizeof (GLubyte);
            pTextureColorData = new GLubyte [tSize];

            // Copy data to new buffer (top down because the image is mirrored vertically)
            if (pBmi->bmiHeader.biBitCount == 32)
            {
                for (int i = textHeight - 1; i >= 0; i--)
                {
                    for (int j=0; j < textWidth; j++)
                    {
                        int index = (i * textWidth * 4) + (j * 4);

                        pTextureColorData[index + 2] = *pBits;
                        pBits++;
                        pTextureColorData[index + 1] = *pBits;
                        pBits++;
                        pTextureColorData[index    ] = *pBits;
                        pBits++;
                        pTextureColorData[index + 3] = *pBits;
                        pBits++;
                    }
                }
            } else // Bitcount = 24
            {
                for (int i = textHeight - 1; i >= 0; i--)
                {
                    for (int j=0; j < textWidth; j++)
                    {
                        int index = (i * textWidth * 4) + (j * 4);

                        pTextureColorData[index + 3] = 0xFF;
                        pTextureColorData[index + 2] = *pBits;
                        pBits++;
                        pTextureColorData[index + 1] = *pBits;
                        pBits++;
                        pTextureColorData[index    ] = *pBits;
                        pBits++;
                    }
                }
            }
        }
            
        // Free buffer memory
        delete (pBuffer);

        return(true);
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Material::ReadTexture", message.c_str());
        if (pTextureColorData != NULL) delete (pTextureColorData);
        pTextureColorData = NULL;
    }

    return(false);
}

/*********************************************************************
* Read the normal texture for this material
*********************************************************************/
bool Material::ReadNormalTexture (const char *pFileName)
{
    // Pointer to file buffer (contents of file)
    char *pBuffer = NULL;

    // Variables for loading bitmap file
    BITMAPFILEHEADER *pBmFh;
    BITMAPINFO *pBmi;
    GLubyte *pBits;

    // Error message
    std::string message;

    try
    {
        // Read file
        unsigned int fileSize;
        pBuffer = Files::ReadBinary(pFileName, &fileSize);
        pBmFh = (BITMAPFILEHEADER*) pBuffer;
        if (pBuffer == NULL)
        {
            message.clear();
            message.append("Could not read file: ");
            message.append(pFileName);
            Error::WriteLog("WARNING", "Material::ReadNormalTexture", message.c_str());
            return(false);
        }

        // If file has been (correctly) read
        if (pBuffer != NULL)
        {
            // Check if image is the correct format (bmp bitmap file)
            if (pBmFh->bfType != * (WORD *) "BM")
            {
                message.clear();
                message.append("File read of normal texture cancelled, is not a bitmap: ");
                message.append(pFileName);
                Error::WriteLog("WARNING", "Material::ReadNormalTexture", message.c_str());

                // Free buffer memory
                delete (pBuffer);
                return(false);
            }

            // Get the pointer to the info structure
            pBmi = (BITMAPINFO *) (pBmFh + 1);

            // Check if image is the correct format (24 or 32 bits)
            if ((pBmi->bmiHeader.biBitCount != 32) && (pBmi->bmiHeader.biBitCount != 24))
            {
                message.clear();
                message.append("File read of normal texture cancelled, is not a 24 or 32 bits bitmap: ");
                message.append(pFileName);
                Error::WriteLog("ERROR", "Material::ReadNormalTexture", message.c_str());

                // Free buffer memory
                delete (pBuffer);
                return(false);
            }

            // Check if image is the correct format (32 bits not compressed)
            if ((pBmi->bmiHeader.biCompression != BI_BITFIELDS) && (pBmi->bmiHeader.biCompression != BI_RGB))
            {
                message.clear();
                message.append("File read of normal texture cancelled, is a compressed bitmap: ");
                message.append(pFileName);
                Error::WriteLog("ERROR", "Material::ReadNormalTexture", message.c_str());

                // Free buffer memory
                delete (pBuffer);
                return(false);
            }

            // Check if image is the same size as texture image
            unsigned int width = abs (pBmi->bmiHeader.biWidth);
            unsigned int height = abs (pBmi->bmiHeader.biHeight);
            if ((textWidth != width) || (textHeight != height))
            {
                message.clear();
                message.append("File read of normal texture cancelled (not the same size as texture): ");
                message.append(pFileName);
                Error::WriteLog("ERROR", "Material::ReadNormalTexture", message.c_str());

                // Free buffer memory
                delete (pBuffer);
                return(false);
            }

            // Get the pointer to the image bits
            pBits = (GLubyte *) pBmFh + (GLubyte) pBmFh->bfOffBits;

            // Allocate memory for texture
            GLint tSize  = textWidth * textHeight * 4 * sizeof (GLubyte);
            pTextureNormalData = new GLubyte[tSize];

            // Copy data to new buffer (top down because the image is mirrored vertically)
            if (pBmi->bmiHeader.biBitCount == 32)
            {
                for (int i = textHeight - 1; i >= 0; i--)
                {
                    for (int j = 0; j < textWidth; j++)
                    {
                        int index = (i * textWidth * 4) + (j * 4);

                        pTextureNormalData[index + 2] = *pBits;
                        pBits++;
                        pTextureNormalData[index + 1] = *pBits;
                        pBits++;
                        pTextureNormalData[index    ] = *pBits;
                        pBits++;
                        pTextureNormalData[index + 3] = *pBits;
                        pBits++;
                    }
                }
            }
            else // Bitcount = 24
            {
                for (int i = textHeight - 1; i >= 0; i--)
                {
                    for (int j = 0; j < textWidth; j++)
                    {
                        int index = (i * textWidth * 4) + (j * 4);

                        pTextureNormalData[index + 3] = 0xFF;
                        pTextureNormalData[index + 2] = *pBits;
                        pBits++;
                        pTextureNormalData[index + 1] = *pBits;
                        pBits++;
                        pTextureNormalData[index    ] = *pBits;
                        pBits++;
                    }
                }
            }
        }

        // Free buffer memory
        delete (pBuffer);

        return(true);
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Material::ReadNormalTexture", message.c_str());
        if (pTextureNormalData != NULL) delete (pTextureNormalData);
        pTextureNormalData = NULL;
    }

    return(false);
}
