#include "Material.h"

/*********************************************************************
* Constructor
*********************************************************************/
Material::Material (std::string name)
{
    // Set default name of Material
    this->name.append(name);

    // Set pointer to texture
    pTextureData  = NULL;

    // Set pointer to texture Normal
    pTextureNormalData  = NULL;

    // Set default color albedo
    pKa = NULL;
    pKd = NULL;
    pKs = NULL;

    // Set specular reflectivity
    iKn = 64;

    // Set illumination model (no illumination)
    iIllum = 1;

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
    if (pTextureData  != NULL) delete (pTextureData);
    if (pKa != NULL) delete (pKa);
    if (pKd != NULL) delete (pKd);
    if (pKs != NULL) delete (pKs);
}

/*********************************************************************
* Read the texture for this material
*********************************************************************/
void Material::ReadTexture (const char *pFileName)
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
        // Set hourglass cursor
        SetCursor (LoadCursor (NULL, IDC_WAIT));

        // Read file
        unsigned int fileSize;
        pBuffer = Files::ReadBinary(pFileName, &fileSize);
        if (pBuffer == NULL)
        {
            message.clear();
            message.append("Could not read file: ");
            message.append(pFileName);
            Error::WriteLog("WARNING", "Material::ReadTexture", message.c_str());
        }

        // Set normal cursor
        SetCursor (LoadCursor (NULL, IDC_ARROW));

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
                return;
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
                return;
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
                return;
            }

            // Get the DIB width and height
            textWidth = abs (pBmi->bmiHeader.biWidth);
            textHeight = abs (pBmi->bmiHeader.biHeight);

            // Get the pointer to the image bits
            pBits = (GLubyte *) pBmFh + (GLubyte) pBmFh->bfOffBits;

            // Allocate memory for texture
            GLuint tSize  = textWidth * textHeight * 4 * sizeof (GLubyte);
            pTextureData = new GLubyte [tSize];

            // Copy data to new buffer (top down because the image is mirrored vertically)
            if (pBmi->bmiHeader.biBitCount == 32)
            {
                for (int i = textHeight - 1; i >= 0; i--)
                {
                    for (int j=0; j < textWidth; j++)
                    {
                        int index = (i * textWidth * 4) + (j * 4);

                        pTextureData[index + 2] = *pBits;
                        pBits++;
                        pTextureData[index + 1] = *pBits;
                        pBits++;
                        pTextureData[index    ] = *pBits;
                        pBits++;
                        pTextureData[index + 3] = *pBits;
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

                        pTextureData[index + 3] = 0xFF;
                        pTextureData[index + 2] = *pBits;
                        pBits++;
                        pTextureData[index + 1] = *pBits;
                        pBits++;
                        pTextureData[index    ] = *pBits; 
                        pBits++;
                    }
                }
            }
        }
            
        // Free buffer memory
        delete (pBuffer);
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Material::ReadTexture", message.c_str());
        if (pTextureData != NULL) delete (pTextureData);
        pTextureData = NULL;
    }
}

/*********************************************************************
* Read the normal texture for this material
*********************************************************************/
void Material::ReadNormalTexture (const char *pFileName)
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
        // Set hourglass cursor
        SetCursor (LoadCursor (NULL, IDC_WAIT));

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
        }

        // Set normal cursor
        SetCursor (LoadCursor (NULL, IDC_ARROW));

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
                return;
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
                return;
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
                return;
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
                return;
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
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Material::ReadNormalTexture", message.c_str());
        if (pTextureNormalData != NULL) delete (pTextureNormalData);
        pTextureNormalData = NULL;
    }
}
