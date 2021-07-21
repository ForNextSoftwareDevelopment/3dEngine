#include "Objects.h"

/*********************************************************************
* Constructor
*********************************************************************/
Objects::Objects (char *pApplicationFolder)
{
    // Copy application folder
    applicationFolder.append(pApplicationFolder);

    // Reset current mesh / material
    pCurrentMesh = NULL;
    pMaterial = NULL;

    // Position of the current mesh
    posX = posY = posZ = 0.0f;
    std::string strPosX;
    std::string strPosY;
    std::string strPosZ;

    // Reset all meshes / materials
    numMeshes = 0;
    numMaterials = 0;
    for (int i = 0; i < MAX_NUM_MESHES; i++) pMeshArray[i] = nullptr;
    for (int i = 0; i < MAX_NUM_MATERIALS; i++) pMaterialArray[i] = nullptr;

    try
    {
        // Create a sky(cube) as a first object
        CreateSky();

        // Create axis as next objects
        CreateAxis();

        // Create terrain
        pTerrain = CreateTerrain();

        // Create a bulb (for directional lighting)
        CreateBulb();

        // Open file stream (objets file)
        std::string strObjectsFile;
        strObjectsFile.append(applicationFolder);
        strObjectsFile.append("\\objects\\objects.txt");
        std::ifstream objectsFile(strObjectsFile);
        if (!objectsFile)
        {
            std::string message = "Error reading file: " + strObjectsFile;
            Error::WriteLog("ERROR", "Objects::Objects", message.c_str());
            return;
        }

        // Read objects file line by line
        std::string strObjectsLine;
        std::string strObjectFile;
        unsigned int lineObjectsFile = 1;
        while (std::getline(objectsFile, strObjectsLine)) 
        {
            // Setup strings
            strObjectFile.clear();
            strPosX.clear();
            strPosY.clear();
            strPosZ.clear();
            strObjectFile.append(applicationFolder);
            strObjectFile.append("\\objects\\");
            
            // Set position into the string
            unsigned int posObjectsLine = 0;

            // Skip spaces, tabs, newlines and carriage returns
            while ((posObjectsLine < strObjectsLine.length()) && ((strObjectsLine[posObjectsLine] == ' ') || (strObjectsLine[posObjectsLine] == '\t') || (strObjectsLine[posObjectsLine] == '\n') || (strObjectsLine[posObjectsLine] == '\r'))) posObjectsLine++;

            // Process if not comment and not an empty line
            if ((strObjectsLine[posObjectsLine] != '#') && (posObjectsLine < strObjectsLine.length()))
            {
                // Get (file)name of object
                while ((posObjectsLine < strObjectsLine.length()) && (strObjectsLine[posObjectsLine] != ' ') && (strObjectsLine[posObjectsLine] != '\t') && (strObjectsLine[posObjectsLine] != '\n') && (strObjectsLine[posObjectsLine] != '\r')) strObjectFile.push_back(strObjectsLine[posObjectsLine++]);

                // Skip spaces and tabs
                while ((posObjectsLine < strObjectsLine.length()) && ((strObjectsLine[posObjectsLine] == ' ') || (strObjectsLine[posObjectsLine] == '\t'))) posObjectsLine++;

                // Get x pos of object
                while ((posObjectsLine < strObjectsLine.length()) && (strObjectsLine[posObjectsLine] != ' ') && (strObjectsLine[posObjectsLine] != '\t') && (strObjectsLine[posObjectsLine] != '\n') && (strObjectsLine[posObjectsLine] != '\r')) strPosX.push_back(strObjectsLine[posObjectsLine++]);
                posX = Utils::StrToFloat((char *)strPosX.c_str());

                // Skip spaces and tabs
                while ((posObjectsLine < strObjectsLine.length()) && ((strObjectsLine[posObjectsLine] == ' ') || (strObjectsLine[posObjectsLine] == '\t'))) posObjectsLine++;

                // Get y pos of object
                while ((posObjectsLine < strObjectsLine.length()) && (strObjectsLine[posObjectsLine] != ' ') && (strObjectsLine[posObjectsLine] != '\t') && (strObjectsLine[posObjectsLine] != '\n') && (strObjectsLine[posObjectsLine] != '\r')) strPosY.push_back(strObjectsLine[posObjectsLine++]);
                posY = Utils::StrToFloat((char *)strPosY.c_str());

                // Skip spaces and tabs
                while ((posObjectsLine < strObjectsLine.length()) && ((strObjectsLine[posObjectsLine] == ' ') || (strObjectsLine[posObjectsLine] == '\t'))) posObjectsLine++;

                // Get z pos of object
                while ((posObjectsLine < strObjectsLine.length()) && (strObjectsLine[posObjectsLine] != ' ') && (strObjectsLine[posObjectsLine] != '\t') && (strObjectsLine[posObjectsLine] != '\n') && (strObjectsLine[posObjectsLine] != '\r')) strPosZ.push_back(strObjectsLine[posObjectsLine++]);
                posZ = Utils::StrToFloat((char *)strPosZ.c_str());

                // Open file stream (object file)
                std::ifstream objectFile(strObjectFile);
                if (objectFile)
                {
                    // Reset the offset for the indices (number of vertices allready read from previous meshes in this file)
                    offsetV = 0;

                    // Reset the offset for the mapping (number of texture vertices allready read from previous meshes in this file)
                    offsetT = 0;

                    std::string strObjectLine;
                    unsigned int lineObjectFile = 1;
                    while (std::getline(objectFile, strObjectLine))
                    {
                        DecodeObjectLine(strObjectLine, strObjectFile, lineObjectFile);
                        lineObjectFile++;
                    }
                } else
                {
                    std::string message = "Error reading file: \r\n" + strObjectFile + "\r\nSkipping line " + Utils::IntToStr(lineObjectsFile) + " in objects file";
                    Error::WriteLog("ERROR", "Objects::Objects", message.c_str());
                }

                // Close object file
                objectFile.close();

                // If mesh present then close it
                if (pCurrentMesh != NULL) ProcessCurrentMesh();
            }

            // Next line
            lineObjectsFile++;
        }

        // Close objects file
        objectsFile.close();

        // Create some materials
        unsigned int startMaterial = numMaterials;
        for (unsigned int i = 0; i < 10; i++)
        {
            Material *pMaterial = new Material("Material" + Utils::IntToStr(i));

            // Read texture
            std::string fileTextureName = applicationFolder + "\\textures\\material" + Utils::IntToStr(i) + ".bmp";
            std::string fileNormalName = applicationFolder + "\\textures\\material" + Utils::IntToStr(i) + "_normal.bmp";
            bool result = pMaterial->ReadTexture(fileTextureName.c_str());
            pMaterial->ReadNormalTexture(fileNormalName.c_str());

            if (!result)
            {
                float r = Utils::Random();
                float g = Utils::Random();
                float b = Utils::Random();

                pMaterial->Ka[0] = r;
                pMaterial->Ka[1] = g;
                pMaterial->Ka[2] = b;

                pMaterial->Kd[0] = r;
                pMaterial->Kd[1] = g;
                pMaterial->Kd[2] = b;
            }

            // Add new Material to array of pointers (Material list)
            pMaterialArray[numMaterials++] = pMaterial;
        }
        
        // Create random objects
        for (unsigned int i=0; i<50; i++)
        {
            std::string texture;

            // Set random positions and sizes
            float randomX = (Utils::Random() - 0.5f) * 3;
            float randomY =  Utils::Random()         * 3;
            float randomZ = (Utils::Random() - 0.5f) * 3;
            float randomW = Utils::Random();
            float randomS = Utils::Random()  - 0.5f;

            // Pick a shape
            float shape = Utils::Random();
            if ((shape >= 0.0f) && (shape < 0.25f)) 
            {
                // Get a cube mesh
                Cube *pCube = new Cube(randomW * 2.0f);
                pCube->pos_x = randomX * 50;
                pCube->pos_y = randomY * 10;
                pCube->pos_z = randomZ * 50;
                pCube->speed_angle = randomS;
                pCube->speed_pos = randomS;
                pCube->name = "Cube" + Utils::IntToStr(i);
                pCube->numMaterials = 1;

                // Set material for this cube
                MaterialEntry *pMaterialEntry = new MaterialEntry();
                unsigned int index = (unsigned int) (startMaterial + Utils::Random() * (numMaterials - startMaterial - 1));
                pMaterialEntry->pMaterial = pMaterialArray[index];
                pCube->pMaterialEntryList[0] = pMaterialEntry;

                // Get all normals for this mesh
                CalculateNormals(pCube);

                // Get all (bi-)tangents for this mesh
                CalculateTangents(pCube);

                // Calculate sizes in x,y and z direction
                CalculateSizes(pCube);
            
                // Add new Mesh to array of pointers (Objects list)
                pMeshArray[numMeshes++] = pCube;
            }

            if ((shape >= 0.25f) && (shape < 0.5f)) 
            {
                // Get a sphere mesh
                Sphere *pSphere = new Sphere(randomW * 2.0f, 30, 30);
                pSphere->pos_x = randomX * 50;
                pSphere->pos_y = randomY * 10;
                pSphere->pos_z = randomZ * 50;
                pSphere->speed_angle = randomS;
                pSphere->speed_pos = randomS;
                pSphere->name = "Sphere" + Utils::IntToStr(i);
                pSphere->numMaterials = 1;

                // Set material for this cube
                MaterialEntry *pMaterialEntry = new MaterialEntry();
                unsigned int index = (unsigned int) (startMaterial + Utils::Random() * (numMaterials - startMaterial - 1));
                pMaterialEntry->pMaterial = pMaterialArray[index];
                pSphere->pMaterialEntryList[0] = pMaterialEntry;

                // Get all normals for this mesh
                CalculateNormals(pSphere);

                // Get all (bi-)tangents for this mesh
                CalculateTangents(pSphere);

                // Calculate sizes in x,y and z direction
                CalculateSizes(pSphere);
            
                // Add new Mesh to array of pointers (Objects list)
                pMeshArray[numMeshes++] = pSphere;
            }

            if ((shape >= 0.50f) && (shape < 0.75f)) 
            {
                // Get a cone mesh
                Cone *pCone = new Cone(randomW * 3.0f, 10, 10);
                pCone->pos_x = randomX * 50;
                pCone->pos_y = 0.0f;
                pCone->pos_z = randomZ * 50;
                pCone->speed_angle = randomS;
                pCone->speed_pos = randomS;
                pCone->name = "Cone" + Utils::IntToStr(i);
                pCone->numMaterials = 1;

                // Set material for this cube
                MaterialEntry *pMaterialEntry = new MaterialEntry();
                unsigned int index = (unsigned int) (startMaterial + Utils::Random() * (numMaterials - startMaterial - 1));
                pMaterialEntry->pMaterial = pMaterialArray[index];
                pCone->pMaterialEntryList[0] = pMaterialEntry;

                // Get all normals for this mesh
                CalculateNormals(pCone);

                // Get all (bi-)tangents for this mesh
                CalculateTangents(pCone);

                // Calculate sizes in x,y and z direction
                CalculateSizes(pCone);
            
                // Add new Mesh to array of pointers (Objects list)
                pMeshArray[numMeshes++] = pCone;
            }

            if ((shape >= 0.75f) && (shape < 1.0f)) 
            {
                // Get a dome mesh
                Dome *pDome = new Dome(randomW * 3.0f, 10, 10);
                pDome->pos_x = randomX * 50;
                pDome->pos_y = 0.0f;
                pDome->pos_z = randomZ * 50;
                pDome->speed_angle = randomS;
                pDome->speed_pos = randomS;
                pDome->name = "Dome" + Utils::IntToStr(i);
                pDome->numMaterials = 1;

                // Set material for this cube
                MaterialEntry *pMaterialEntry = new MaterialEntry();
                unsigned int index = (unsigned int) (startMaterial + Utils::Random() * (numMaterials - startMaterial - 1));
                pMaterialEntry->pMaterial = pMaterialArray[index];
                pDome->pMaterialEntryList[0] = pMaterialEntry;

                // Get all normals for this mesh
                CalculateNormals(pDome);

                // Get all (bi-)tangents for this mesh
                CalculateTangents(pDome);

                // Calculate sizes in x,y and z direction
                CalculateSizes(pDome);
            
                // Add new Mesh to array of pointers (Objects list)
                pMeshArray[numMeshes++] = pDome;
            }
        }
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::Objects", message.c_str());
    }
}

/*********************************************************************
* Destructor
*********************************************************************/
Objects::~Objects (void)
{
    for (unsigned int i=0; i<numMeshes; i++)
    {
        delete (pMeshArray[i]);
    }
}

/*********************************************************************
* Decode a line from the object file
*********************************************************************/
bool Objects::DecodeObjectLine(std::string strObjectLine, std::string strObjectsFile, unsigned int lineObjectFile)
{
    try
    {
        unsigned int pos = 0;
        bool found = false;

        // Skip spaces and tabs at the beginning of a new line
        while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

        // Process if not comment and not an empty line
        if ((strObjectLine[pos] != '#') && (pos < strObjectLine.length()))
        {
            // Mesh name found
            if ((strObjectLine[pos] == 'o') || (strObjectLine[pos] == 'O'))
            {
                found = true;

                // New object, so if old object present then close it
                if (pCurrentMesh != NULL) ProcessCurrentMesh();

                // Skip 'o', spaces and tabs
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == 'o') || (strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Read name
                std::string name;
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] != ' ') || (strObjectLine[pos] != '\t')))
                {
                    name.push_back(strObjectLine[pos++]);
                }

                // Create a new object
                if (!CreateCurrentMesh(name)) return (false);
            }

            // Material file name found
            if ((strObjectLine[pos] == 'm') && (strObjectLine[pos + 1] == 't') && (strObjectLine[pos + 2] == 'l') && (strObjectLine[pos + 3] == 'l') && (strObjectLine[pos + 4] == 'i') && (strObjectLine[pos + 5] == 'b'))
            {
                found = true;

                // Skip everything till space
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ')) pos++;

                // Skip spaces and tabs
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Read material file name
                std::string name;
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] != ' ') || (strObjectLine[pos] != '\t')))
                {
                    name.push_back(strObjectLine[pos++]);
                }

                // Get the materials from the file
                ReadMaterialFile(name);
            }

            // Use material found
            if ((strObjectLine[pos] == 'u') && (strObjectLine[pos + 1] == 's') && (strObjectLine[pos + 2] == 'e') && (strObjectLine[pos + 3] == 'm') && (strObjectLine[pos + 4] == 't') && (strObjectLine[pos + 5] == 'l'))
            {
                found = true;

                // Skip everything till space
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ')) pos++;

                // Skip spaces and tabs
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Read material file name
                std::string name;
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] != ' ') || (strObjectLine[pos] != '\t')))
                {
                    name.push_back(strObjectLine[pos++]);
                }

                // If object available connect the material to the object
                if (pCurrentMesh != NULL)
                {
                    // Search material list for the correct material
                    bool found = false;
                    for (unsigned int i = 0; (i < numMaterials) && !found; i++)
                    {
                        if (!pMaterialArray[i]->name.compare(name))
                        {
                            found = true;

                            // Create new material entry in the current object
                            pCurrentMesh->pMaterialEntryList[pCurrentMesh->numMaterials] = new MaterialEntry();

                            // Put found material in the new entry
                            pCurrentMesh->pMaterialEntryList[pCurrentMesh->numMaterials]->pMaterial = pMaterialArray[i];

                            // Insert the starting face for this material
                            pCurrentMesh->pMaterialEntryList[pCurrentMesh->numMaterials++]->start = (int)fList.size();
                        }
                    }

                    if (!found)
                    {
                        std::string message;
                        message.append("Material '");
                        message.append(name);
                        message.append("' not found in object file ");
                        message.append(strObjectsFile);
                        Error::WriteLog("WARNING", "Objects::ReadObjectFile", message.c_str());
                        return(false);
                    }
                }
            }

            // Vertex found
            if ((strObjectLine[pos] == 'v') && ((strObjectLine[pos + 1] == ' ') || (strObjectLine[pos + 1] == '\t')))
            {
                found = true;

                std::string strValue;

                // If no object available, then create new
                if (pCurrentMesh == NULL)
                {
                    if (!CreateCurrentMesh("unknown")) return (false);
                }

                // Create vertex
                VecMat::Vertex vertex;

                // Set all coordinates at 0 by default
                vertex.x = 0.0f;
                vertex.y = 0.0f;
                vertex.z = 0.0f;

                // Skip 'v', spaces and tabs
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == 'v') || (strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read x value
                vertex.x = Utils::StrToFloat((char *)strValue.c_str());

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read y value
                vertex.y = Utils::StrToFloat((char *)strValue.c_str());

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read z value
                vertex.z = Utils::StrToFloat((char *)strValue.c_str());

                // Put the new vertex in the vertices list
                vList.push_back(vertex);
            }

            // Vertex normal found
            if ((strObjectLine[pos] == 'v') && (strObjectLine[pos + 1] == 'n'))
            {
                found = true;

                std::string strValue;

                // If no object available, then create new
                if (pCurrentMesh == NULL)
                {
                    if (!CreateCurrentMesh("unknown")) return (false);
                }

                // Create vertex
                VecMat::Vertex vertex;

                // Set all coordinates at 0 by default
                vertex.x = 0.0f;
                vertex.y = 0.0f;
                vertex.z = 0.0f;

                // Skip 'v', 'n', spaces and tabs
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == 'v') || (strObjectLine[pos] == 'n') || (strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read x value
                vertex.x = Utils::StrToFloat((char *)strValue.c_str());

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read y value
                vertex.y = Utils::StrToFloat((char *)strValue.c_str());

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read z value
                vertex.z = Utils::StrToFloat((char *)strValue.c_str());

                // Put the new vertex in the normals list
                vnList.push_back(vertex);
            }

            // Vertex texture found
            if ((strObjectLine[pos] == 'v') && (strObjectLine[pos + 1] == 't'))
            {
                found = true;

                std::string strValue;

                // If no object available, then create new
                if (pCurrentMesh == NULL)
                {
                    if (!CreateCurrentMesh("unknown")) return (false);
                }

                // Create vertex
                VecMat::Vertex vertex;

                // Set all coordinates at 0 by default
                vertex.x = 0.0f;
                vertex.y = 0.0f;
                vertex.z = 0.0f;

                // Skip 'v', 't', spaces and tabs
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == 'v') || (strObjectLine[pos] == 't') || (strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read x value
                vertex.x = Utils::StrToFloat((char *)strValue.c_str());

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read y value
                vertex.y = Utils::StrToFloat((char *)strValue.c_str());

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read z value
                vertex.z = Utils::StrToFloat((char *)strValue.c_str());

                // Put the new vertex in the vertex texture list
                vtList.push_back(vertex);
            }

            // Face found
            if ((strObjectLine[pos] == 'f') && ((strObjectLine[pos + 1] == ' ') || (strObjectLine[pos + 1] == '\t')))
            {
                found = true;

                std::string strValue;

                // If no object available, then create new
                if (pCurrentMesh == NULL)
                {
                    if (!CreateCurrentMesh((char *)"unknown")) return (false);
                }

                // Read all indices (of vertices) from this face into a temporary polygon list
                std::vector<int> poly;

                while (pos < strObjectLine.length())
                {
                    // Skip 'f', spaces and tabs
                    while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == 'f') || (strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                    // Get value in string format
                    strValue.clear();
                    while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t') && (strObjectLine[pos] != '/')) strValue.push_back(strObjectLine[pos++]);

                    // Read value (-1 because the object file uses 1 as first vertex index for this face, we use 0)
                    // Also subtract the offset for the vertices
                    unsigned int ver = atoi(strValue.c_str()) - 1 - offsetV;

                    // Check if vertex number is within range
                    if ((ver >= 0) && (ver < vList.size()))
                    {
                        poly.push_back(ver);
                    } else
                    {
                        std::string message = "Vertex not in range in line nr. ";
                        message.append(Utils::IntToStr(lineObjectFile));
                        message.append(" in object file: ");
                        message.append(strObjectsFile);
                        Error::WriteLog("ERROR", "Objects::DecodeObjectLine", message.c_str());
                        return(false);
                    }

                    // Skip to begin of next value
                    while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                    // Check if a texture vertex is provided
                    if (strObjectLine[pos] == '/')
                    {
                        pos++;
                        if ((pos < strObjectLine.length()) && (strObjectLine[pos] != '/'))
                        {
                            // Get value in string format
                            strValue.clear();
                            while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                            // Read value (-1 because the object file uses 1 as first vertex index for this face, we use 0)
                            // Also subtract the offset for the texture vertices
                            unsigned int tex = atoi(strValue.c_str()) - 1 - offsetT;

                            // Create a new mapping
                            VecMat::Map mapping;
                            mapping.ver = ver;
                            mapping.tex = tex;

                            // Write the texture vertex
                            mList.push_back(mapping);
                        }

                        // Skip to end of value
                        while ((pos < strObjectLine.length()) && ((strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t'))) pos++;
                    }
                }

                // If the number of vertices in this face is more than 3 then triangulate (make triangles)
                if (poly.size() > 3)
                {
                    //  Invoke the triangulator to triangulate this polygon (put result in the fList).
                    if (!Triangulate::Process(poly, vList, fList))
                    {
                        // If error then add in list
                        pCurrentMesh->numTriangulationErrors++;
                    }
                } else
                    if (poly.size() == 3)
                    {
                        // Create face
                        VecMat::Face face;

                        // Set all coordinates
                        face.first = poly[0];
                        face.second = poly[1];
                        face.third = poly[2];

                        // Put the new face in the faces list
                        fList.push_back(face);
                    } else
                    {
                        std::string message = "Face has less then 3 verices (=> ignored) in line nr. ";
                        message.append(Utils::IntToStr(lineObjectFile));
                        message.append(" in object file: ");
                        message.append(strObjectsFile);
                        Error::WriteLog("WARNING", "Objects::DecodeObjectLine", message.c_str());
                        return(false);
                    }
            }

            if (!found)
            {
                std::string message = "Unknown part in line nr. ";
                message.append(Utils::IntToStr(lineObjectFile));
                message.append(" in object file: ");
                message.append(strObjectsFile);
                Error::WriteLog("WARNING", "Objects::DecodeObjectLine", message.c_str());
                return(false);
            }
        }
    } catch (std::exception& e)
    {
        std::string message = "Exception decoding object line in line nr. ";
        message.append(Utils::IntToStr(lineObjectFile));
        message.append(" in object file: ");
        message.append(strObjectsFile);
        message.append(": ");
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::DecodeObjectLine", message.c_str());

        delete (pCurrentMesh);
        pCurrentMesh = NULL;

        return (false);
    }

    return (true);
}

/*********************************************************************
* Create a new (current) mesh
*********************************************************************/
bool Objects::CreateCurrentMesh (std::string name)
{
    if (numMeshes < MAX_NUM_MESHES)
    {
        // Create a new mesh
        pCurrentMesh = new Mesh (name);

        if (pCurrentMesh != NULL) return (true);
    }

    Error::WriteLog("ERROR", "Objects::CreateCurrentMesh", "Could not add (all) meshes (too many meshes)");
    return (false);
}

/*********************************************************************
* Move the current mesh to the list and close it
*********************************************************************/
bool Objects::ProcessCurrentMesh (void)
{
    std::string error;

    #ifdef DEBUG
    // If errors have occured during the triangulation of the last object then show
    if (pCurrentMesh->numTriangulationErrors > 0)
    {
        error.clear();
        error.append("Not all triangles found in '" + pCurrentMesh->name + "'\r\n");
        error.append("Number of errors = ");
        error.append(std::to_string(pCurrentMesh->numTriangulationErrors));
        error.append(" (probably some polygons are illegal or have double vertices)\r\n");
        Error::WriteLog("WARNING", "Objects::ProcessCurrentMesh", error.c_str());
    }
    #endif

    try
    {
        // Set position of the mesh
        pCurrentMesh->pos_x = posX;
        pCurrentMesh->pos_y = posY;
        pCurrentMesh->pos_z = posZ;

        // Get the number of vertices in the Mesh
        pCurrentMesh->numVertices = (int)vList.size();

        // Add the number of vertices to the offset
        offsetV += pCurrentMesh->numVertices;

        // Add the number of texture vertices to the offset
        offsetT += pCurrentMesh->numTextureVertices;

        // Allocate memory for vertices and clear this memory
        pCurrentMesh->pVertices = new VecMat::Vertex[pCurrentMesh->numVertices];
		if (pCurrentMesh->pVertices == nullptr) return false;
        ZeroMemory (pCurrentMesh->pVertices, pCurrentMesh->numVertices * sizeof(VecMat::Vertex));

        // Copy vertices from the list to the vertices memory block
        VecMat::Vertex *pv = pCurrentMesh->pVertices;
        for (unsigned int i=0; i<pCurrentMesh->numVertices; i++) *pv++ = vList[i];

        // Get the number of faces (triangles) in the Mesh
        pCurrentMesh->numFaces = (int)fList.size();

        // Allocate memory for faces and clear this memory
        pCurrentMesh->pFaces = new VecMat::Face[pCurrentMesh->numFaces];
		if (pCurrentMesh->pFaces == nullptr) return false;
		ZeroMemory (pCurrentMesh->pFaces, pCurrentMesh->numFaces * sizeof(VecMat::Face));

        // Copy faces from the list to the faces memory block
        VecMat::Face *pf = pCurrentMesh->pFaces;
        for (unsigned int i=0; i<pCurrentMesh->numFaces; i++) *pf++ = fList[i];

        // Check if vertex normals are provided
        bool result = true;
        if (vnList.size() != vList.size())
        {
            // Calculate all normals (face and vertex) for the current mesh
            CalculateNormals(pCurrentMesh);
        } else
        {
            // Set the number of normals in the Mesh
            pCurrentMesh->numVertexNormals = (int)vList.size();

            // Allocate memory
            pCurrentMesh->pVertexNormals = new VecMat::Vertex[pCurrentMesh->numVertexNormals];
			if (pCurrentMesh->pVertexNormals == nullptr) return false;
			ZeroMemory (pCurrentMesh->pVertexNormals, pCurrentMesh->numVertexNormals * sizeof(VecMat::Vertex));

            VecMat::Vertex *pvn = pCurrentMesh->pVertexNormals;

            // Just copy the provided vertex normals
            for (unsigned int i=0; i<pCurrentMesh->numVertexNormals; i++)
            {
                pvn[i] = vnList[i];
            }
        }

        // Check if texture mapping is provided (mappings and texture vertices present)
        if ((mList.size() != 0) && (vtList.size() != 0))
        {
            // Set the number of texture vertices in the Mesh
            pCurrentMesh->numTextureVertices = (int)vtList.size();

            // Allocate memory
            pCurrentMesh->pTextureVertices = new VecMat::Vertex[pCurrentMesh->numTextureVertices];
			if (pCurrentMesh->pTextureVertices == nullptr) return false;
			ZeroMemory (pCurrentMesh->pTextureVertices, pCurrentMesh->numTextureVertices * sizeof(VecMat::Vertex));

            VecMat::Vertex *pvt = pCurrentMesh->pTextureVertices;

            // Copy vertices from the mapping list to the texture vertices memory block
            for (unsigned int i=0; i<mList.size(); i++)
            {
                unsigned int indexVertex  = mList[i].ver;
                unsigned int indexTexture = mList[i].tex;
                if ((indexVertex < pCurrentMesh->numTextureVertices) && (indexTexture < pCurrentMesh->numTextureVertices))
                {
                    pvt[indexVertex] = vtList[indexTexture];
                }
            }
        } else
        {
            // Calculate the scale (max and min x,y,z)
            float minX = vList[0].x;
            float minY = vList[0].y;
            float minZ = vList[0].z;
            float maxX = vList[0].x;
            float maxY = vList[0].y;
            float maxZ = vList[0].z;

            for (unsigned int i=0; i<pCurrentMesh->numVertices; i++)
            {
                if (vList[i].x < minX) minX = vList[i].x;
                if (vList[i].y < minY) minY = vList[i].y;
                if (vList[i].z < minZ) minZ = vList[i].z;
                if (vList[i].x > maxX) maxX = vList[i].x;
                if (vList[i].y > maxY) maxY = vList[i].y;
                if (vList[i].z > maxZ) maxZ = vList[i].z;
            }

            float scaleX = std::abs(maxX - minX);
            float scaleY = std::abs(maxY - minY);
            float scaleZ = std::abs(maxZ - minZ);

            // Set the number of texture vertices in the Mesh (allways number of vertices, every vertex will be mapped)
            pCurrentMesh->numTextureVertices = pCurrentMesh->numVertices;

            // Allocate memory
            pCurrentMesh->pTextureVertices = new VecMat::Vertex[pCurrentMesh->numTextureVertices];
			if (pCurrentMesh->pTextureVertices == nullptr) return false;
			ZeroMemory (pCurrentMesh->pTextureVertices, pCurrentMesh->numTextureVertices * sizeof(VecMat::Vertex));

            // Copy vertices from the list (now used as texture vertices) to the texture vertices memory block
            VecMat::Vertex *pvt = pCurrentMesh->pTextureVertices;
            for (unsigned int i=0; i<pCurrentMesh->numTextureVertices; i++)
            {
                // Scale and assign
                if (scaleX > 0.0f) vList[i].x = (vList[i].x - minX) / scaleX;
                if (scaleY > 0.0f) vList[i].y = (vList[i].y - minY) / scaleY;
                if (scaleZ > 0.0f) vList[i].z = (vList[i].z - minZ) / scaleZ;

                //VecMat::Vec3 vec = VecMat::Normalize(VecMat::Vec3(pvn[i].x, pvn[i].y, pvn[i].z));
                //pvt->x = vList[i].x +      std::abs(VecMat::Dot(vec, VecMat::Vec3(0, 1, 0)))  * std::abs(vList[i].z);
                //pvt->y = vList[i].y + (1 - std::abs(VecMat::Dot(vec, VecMat::Vec3(1, 0, 0)))) * std::abs(vList[i].z);
                pvt->x = vList[i].x + vList[i].z;
                pvt->y = vList[i].y - vList[i].z;
                pvt->z = 0;

                pvt++;
            }
        }

        // Check if material is provided, if not then supply default material
        if (pCurrentMesh->numMaterials == 0)
        {
            pCurrentMesh->numMaterials = 1;
            pMaterial = new Material(("default-" + pCurrentMesh->name).c_str());
            pCurrentMesh ->pMaterialEntryList[0] = new MaterialEntry();
            pCurrentMesh ->pMaterialEntryList[0]->pMaterial = pMaterial;
        }

        // Calculate all (bi-)tangents for the current mesh
        CalculateTangents(pCurrentMesh);

        // Calculate sizes for the current mesh (x, y and z)
        CalculateSizes(pCurrentMesh);

        // Add new Mesh to array of pointers (Objects list)
        pMeshArray[numMeshes++] = pCurrentMesh;

        // Clear vertices, faces and mapping lists
        vList.clear();
        vtList.clear();
        vnList.clear();
        fList.clear();
        mList.clear();
    } catch (std::exception& e)
    {
        std::string message = "Exception processing mesh: ";
        message.append(pCurrentMesh->name);
        message.append(": ");
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::ProcessCurrentMesh", message.c_str());

        // Free memory
        delete[] (pCurrentMesh->pVertices);
		delete[] (pCurrentMesh->pFaceNormals);
		delete[] (pCurrentMesh->pVertexNormals);
		delete[] (pCurrentMesh->pFaces);
		delete[] (pCurrentMesh->pTangents);
		delete[] (pCurrentMesh->pBiTangents);
    }

    // Set current object to NULL
    pCurrentMesh = NULL;

	return (true);
}

/*********************************************************************
* Calculate all normals (face and vertex) for a mesh
*********************************************************************/
bool Objects::CalculateNormals(Mesh *pMesh)
{
    try
    {
        // Calculate all face normals and put them in a list
        std::vector<VecMat::Vertex> fnList;
        for (unsigned int i=0; i<pMesh->numFaces; i++)
        {
            VecMat::Vec3 normal;

            // Calculate first vector
            VecMat::Vec3 first;

            first[0] = pMesh->pVertices[pMesh->pFaces[i].second].x - pMesh->pVertices[pMesh->pFaces[i].first].x;
            first[1] = pMesh->pVertices[pMesh->pFaces[i].second].y - pMesh->pVertices[pMesh->pFaces[i].first].y;
            first[2] = pMesh->pVertices[pMesh->pFaces[i].second].z - pMesh->pVertices[pMesh->pFaces[i].first].z;

            // Calculate second vector
            VecMat::Vec3 second;
            second[0] = pMesh->pVertices[pMesh->pFaces[i].third].x - pMesh->pVertices[pMesh->pFaces[i].first].x;
            second[1] = pMesh->pVertices[pMesh->pFaces[i].third].y - pMesh->pVertices[pMesh->pFaces[i].first].y;
            second[2] = pMesh->pVertices[pMesh->pFaces[i].third].z - pMesh->pVertices[pMesh->pFaces[i].first].z;

            // Calculate cross product of these two vectors (= normal)
            normal = first * second;

            // Put the new normal in the list
            VecMat::Vertex fn;
            fn.x = normal[0];
            fn.y = normal[1];
            fn.z = normal[2];

            fnList.push_back(fn);
        }

        // Get the number of facenormals in the Mesh
        pMesh->numFaceNormals = (int)fnList.size();

        // Allocate memory for face normals and clear this memory
        pMesh->pFaceNormals = new VecMat::Vertex[pMesh->numFaceNormals];
		if (pMesh->pFaceNormals == nullptr) return false;
        ZeroMemory (pMesh->pFaceNormals, pMesh->numFaceNormals * sizeof(VecMat::Vertex));

        // Copy face normals from the list to the face normals memory block
        VecMat::Vertex *pfn = pMesh->pFaceNormals;
        for (unsigned int i=0; i<pMesh->numFaceNormals; i++) *pfn++ = fnList[i];

        // Set number of vertex normals
        pMesh->numVertexNormals = pMesh->numVertices;

        // Allocate memory for vertex normals
        pMesh->pVertexNormals = new VecMat::Vertex[pMesh->numVertexNormals];
		if (pMesh->pVertexNormals == nullptr) return false;
		ZeroMemory (pMesh->pVertexNormals, pMesh->numVertexNormals * sizeof(VecMat::Vertex));
        VecMat::Vertex *pvn = pMesh->pVertexNormals;

        // Check all faces for vertices in them and add the face normal to that vertex normal
        for (unsigned int i=0; i<pMesh->numFaces; i++)
        {
            unsigned int first  = pMesh->pFaces[i].first;
            unsigned int second = pMesh->pFaces[i].second;
            unsigned int third  = pMesh->pFaces[i].third;

            pvn[first]  = pvn[first]  + fnList[i];
            pvn[second] = pvn[second] + fnList[i];
            pvn[third]  = pvn[third]  + fnList[i];
        }

        // Normalize all vertex normals
        bool error = false;
        for (unsigned int i=0; i<pMesh->numVertexNormals; i++)
        {
            float normFactor = sqrt(pvn[i].x * pvn[i].x + pvn[i].y * pvn[i].y + pvn[i].z * pvn[i].z);
            if (normFactor == 0.0f) error = true; else
            {
                pvn[i].x = pvn[i].x / normFactor;
                pvn[i].y = pvn[i].y / normFactor;
                pvn[i].z = pvn[i].z / normFactor;
            }
        }

        // Error in calculation
        if (error)
        {
            std::string message = "Could not calculate all normals for: ";
            message.append(pMesh->name);
            Error::WriteLog("WARNING", "Objects::CalculateNormals", message.c_str());
        }

        return (true);
    } catch (std::exception& e)
    {
        std::string message = "Exception calculating normals: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::CalculateNormals", message.c_str());
        return (false);
    }
}

/*********************************************************************
* Calculate all (bi-)tangents for a mesh
*********************************************************************/
bool Objects::CalculateTangents(Mesh *pMesh)
{
    if ((pMesh->numTextureVertices == 0) || (pMesh->pTextureVertices == 0))
    {
        std::string message = "No texture vertices for: ";
        message.append(pMesh->name);
        Error::WriteLog("WARNING", "Objects::CalculateTangents", message.c_str());
        return(false);
    }

    try
    {
        // Set number of tangents
        pMesh->numTangents = pMesh->numVertices;

        // Allocate memory for tangents
        pMesh->pTangents = new VecMat::Vertex[pMesh->numTangents];
		if (pMesh->pTangents == nullptr) return false;
		ZeroMemory (pMesh->pTangents, pMesh->numTangents * sizeof(VecMat::Vertex));
        VecMat::Vertex *pt = pMesh->pTangents;

        // Set number of bitangents
        pMesh->numBiTangents = pMesh->numVertices;

        // Allocate memory for bitangents
        pMesh->pBiTangents = new VecMat::Vertex[pMesh->numBiTangents];
		if (pMesh->pBiTangents == nullptr) return false;
		ZeroMemory (pMesh->pBiTangents, pMesh->numBiTangents * sizeof(VecMat::Vertex));
        VecMat::Vertex *pbt = pMesh->pBiTangents;

        // Check all faces and calculate the (bi-)tangent for that face
        std::vector<VecMat::Vertex> ftList;
        std::vector<VecMat::Vertex> fbtList;
        bool result = true;
        for (unsigned int i=0; i<pMesh->numFaces; i++)
        {
            VecMat::Vec3 tangent, bitangent;

            // Indices
            unsigned int first  = pMesh->pFaces[i].first;
            unsigned int second = pMesh->pFaces[i].second;
            unsigned int third  = pMesh->pFaces[i].third;

            // Positions
            VecMat::Vec3 pos1(pMesh->pVertices[first].x,  pMesh->pVertices[first].y,  pMesh->pVertices[first].z);
            VecMat::Vec3 pos2(pMesh->pVertices[second].x, pMesh->pVertices[second].y, pMesh->pVertices[second].z);
            VecMat::Vec3 pos3(pMesh->pVertices[third].x,  pMesh->pVertices[third].y,  pMesh->pVertices[third].z);

            // Texture coordinates
            VecMat::Vec2 uv1(pMesh->pTextureVertices[first].x,  pMesh->pTextureVertices[first].y);
            VecMat::Vec2 uv2(pMesh->pTextureVertices[second].x, pMesh->pTextureVertices[second].y);
            VecMat::Vec2 uv3(pMesh->pTextureVertices[third].x,  pMesh->pTextureVertices[third].y);

            // Normal vector
            VecMat::Vec3 normal(pMesh->pFaceNormals[i].x, pMesh->pFaceNormals[i].y, pMesh->pFaceNormals[i].z);

            // Calculate the first triangle's edges and delta UV coordinates:
            VecMat::Vec3 deltaPos1 = pos2 - pos1;
            VecMat::Vec3 deltaPos2 = pos3 - pos1;
            VecMat::Vec2 deltaUV1  = uv2 - uv1;
            VecMat::Vec2 deltaUV2  = uv3 - uv1;

            // Calculate tangent
            GLfloat f;
            GLfloat divider = deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1];
            if ((divider < 0.0001f) && (divider > -0.0001f))
            {
                result = false;
                f = 0.0f;
            } else
            {
                f = 1.0f / divider;
            }

            tangent[0] = f * (deltaUV2[1] * deltaPos1[0] - deltaUV1[1] * deltaPos2[0]);
            tangent[1] = f * (deltaUV2[1] * deltaPos1[1] - deltaUV1[1] * deltaPos2[1]);
            tangent[2] = f * (deltaUV2[1] * deltaPos1[2] - deltaUV1[1] * deltaPos2[2]);

            VecMat::Vertex fTangent;
            fTangent.x = tangent[0];
            fTangent.y = tangent[1];
            fTangent.z = tangent[2];

            // Add tangent to list
            ftList.push_back(fTangent);

            // Calculate bitangent
            bitangent = normal * tangent;
            
            //bitangent[0] = f * (deltaUV1[0] * deltaPos2[0] - deltaUV2[0] * deltaPos1[0]);
            //bitangent[1] = f * (deltaUV1[0] * deltaPos2[1] - deltaUV2[0] * deltaPos1[1]);
            //bitangent[2] = f * (deltaUV1[0] * deltaPos2[2] - deltaUV2[0] * deltaPos1[2]);

            VecMat::Vertex fBiTangent;
            fBiTangent.x = bitangent[0];
            fBiTangent.y = bitangent[1];
            fBiTangent.z = bitangent[2];

            // Add bitangent to list
            fbtList.push_back(fBiTangent);
        }

        #ifdef DEBUGNORMALMAP
        // Error in calculation
        if (!result)
        {
            std::string message = "Could not calculate all (bi)tangent for: ";
            message.append(pMesh->name);
            Error::WriteLog("WARNING", "Objects::CalculateTangents", message.c_str());
        }

        // Give info for all normals and (bi-)tangents
        if (pMesh->name != "Bulb")
        {
            std::string message = "### ";
            message.append(pMesh->name);
            message.append(" Faces ###\r\n");
            Error::ShowInfoWindow(message);

            for (unsigned int i=0; i<pMesh->numFaces; i++)
            {
                message = "FaceNum   : ";
                message.append(Utils::IntToStr(i));
                message.append("\r\n");

                message.append("Face      : ");
                message.append(Utils::IntToStr(pMesh->pFaces[i].first));
                message.append(", ");
                message.append(Utils::IntToStr(pMesh->pFaces[i].second));
                message.append(", ");
                message.append(Utils::IntToStr(pMesh->pFaces[i].third));
                message.append("\r\n");

                message.append("Tangent   : ");
                message.append(Utils::FloatToStr(ftList[i].x));
                message.append(", ");
                message.append(Utils::FloatToStr(ftList[i].y));
                message.append(", ");
                message.append(Utils::FloatToStr(ftList[i].z));
                message.append("\r\n");

                message.append("BiTangent : ");
                message.append(Utils::FloatToStr(fbtList[i].x));
                message.append(", ");
                message.append(Utils::FloatToStr(fbtList[i].y));
                message.append(", ");
                message.append(Utils::FloatToStr(fbtList[i].z));
                message.append("\r\n");

                message.append("Normal    : ");
                message.append(Utils::FloatToStr(pMesh->pFaceNormals[i].x));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pFaceNormals[i].y));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pFaceNormals[i].z));
                message.append("\r\n");

                Error::ShowInfoWindow(message);
            }

            Error::ShowInfoWindow("\r\n--------------------------------------------------------------------------\r\n");
        }
        #endif

        // Check all faces for vertices in them and add the face (bi)tangent to that vertex (bi)tangent
        for (unsigned int i=0; i<pMesh->numFaces; i++)
        {
            unsigned int first  = pMesh->pFaces[i].first;
            unsigned int second = pMesh->pFaces[i].second;
            unsigned int third  = pMesh->pFaces[i].third;

            pt[first]  = pt[first]  + ftList[i];
            pt[second] = pt[second] + ftList[i];
            pt[third]  = pt[third]  + ftList[i];

            pbt[first]  = pbt[first]  + fbtList[i];
            pbt[second] = pbt[second] + fbtList[i];
            pbt[third]  = pbt[third]  + fbtList[i];
        }

        // Normalize all vertex tangents
        for (unsigned int i=0; i<pMesh->numTangents; i++)
        {
            float normFactor = sqrt(pt[i].x * pt[i].x + pt[i].y * pt[i].y + pt[i].z * pt[i].z);
            pt[i].x = pt[i].x / normFactor;
            pt[i].y = pt[i].y / normFactor;
            pt[i].z = pt[i].z / normFactor;
        }

        // Normalize all vertex bitangents
        for (unsigned int i=0; i<pMesh->numBiTangents; i++)
        {
            float normFactor = sqrt(pbt[i].x * pbt[i].x + pbt[i].y * pbt[i].y + pbt[i].z * pbt[i].z);
            pbt[i].x = pbt[i].x / normFactor;
            pbt[i].y = pbt[i].y / normFactor;
            pbt[i].z = pbt[i].z / normFactor;
        }

        #ifdef DEBUGNORMALMAP
        // Give info for all normals and (bi-)tangents
        if (pMesh->name != "Bulb")
        {
            std::string message = "### ";
            message.append(pMesh->name);
            message.append(" Vertices ###\r\n");
            Error::ShowInfoWindow(message);

            for (unsigned int i=0; i<pMesh->numVertexNormals; i++)
            {
                message = "VertexNum : ";
                message.append(Utils::IntToStr(i));
                message.append("\r\n");

                message.append("Vertex    : ");
                message.append(Utils::FloatToStr(pMesh->pVertices[i].x));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pVertices[i].y));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pVertices[i].z));
                message.append("\r\n");

                message.append("Texture   : ");
                message.append(Utils::FloatToStr(pMesh->pTextureVertices[i].x));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pTextureVertices[i].y));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pTextureVertices[i].z));
                message.append("\r\n");

                message.append("Tangent   : ");
                message.append(Utils::FloatToStr(pMesh->pTangents[i].x));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pTangents[i].y));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pTangents[i].z));
                message.append("\r\n");

                message.append("BiTangent : ");
                message.append(Utils::FloatToStr(pMesh->pBiTangents[i].x));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pBiTangents[i].y));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pBiTangents[i].z));
                message.append("\r\n");

                message.append("Normal    : ");
                message.append(Utils::FloatToStr(pMesh->pVertexNormals[i].x));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pVertexNormals[i].y));
                message.append(", ");
                message.append(Utils::FloatToStr(pMesh->pVertexNormals[i].z));
                message.append("\r\n");

                Error::ShowInfoWindow(message);
            }

            Error::ShowInfoWindow("\r\n--------------------------------------------------------------------------\r\n");
        }
        #endif

        return (true);
    } catch (std::exception& e)
    {
        std::string message = "Exception calculating tangents: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::CalculateTangents", message.c_str());
        return (false);
    }
}

/*********************************************************************
* Calculate sizes for the current mesh (x, y and z)
*********************************************************************/
void Objects::CalculateSizes(Mesh* pMesh)
{
    if (pMesh->numVertices > 0)
    {
        float maxX = pMesh->pVertices[0].x; 
        float maxY = pMesh->pVertices[0].y; 
        float maxZ = pMesh->pVertices[0].z; 

        float minX = pMesh->pVertices[0].x; 
        float minY = pMesh->pVertices[0].y; 
        float minZ = pMesh->pVertices[0].z; 

        for (unsigned int i = 0; i < pMesh->numVertices; i++)
        {
            if (pMesh->pVertices[i].x > maxX) maxX = pMesh->pVertices[i].x;
            if (pMesh->pVertices[i].y > maxY) maxY = pMesh->pVertices[i].y;
            if (pMesh->pVertices[i].z > maxZ) maxZ = pMesh->pVertices[i].z;
 
            if (pMesh->pVertices[i].x < minX) minX = pMesh->pVertices[i].x;
            if (pMesh->pVertices[i].y < minY) minY = pMesh->pVertices[i].y;
            if (pMesh->pVertices[i].z < minZ) minZ = pMesh->pVertices[i].z;

            pMesh->center_x += pMesh->pVertices[i].x;
            pMesh->center_y += pMesh->pVertices[i].y;
            pMesh->center_z += pMesh->pVertices[i].z;
        }

        pMesh->size_x = abs(maxX - minX);
        pMesh->size_y = abs(maxY - minY);
        pMesh->size_z = abs(maxZ - minZ);

        pMesh->center_x = pMesh->center_x / (float)pMesh->numVertices / 2.0f;
        pMesh->center_y = pMesh->center_y / (float)pMesh->numVertices / 2.0f;
        pMesh->center_z = pMesh->center_z / (float)pMesh->numVertices / 2.0f;
    }
}

/*********************************************************************
* Read material file
*********************************************************************/
bool Objects::ReadMaterialFile(std::string name)
{
    std::string strMaterialFile;
    strMaterialFile.append(applicationFolder + "\\objects\\");
    strMaterialFile.append(name);

    // Open file stream (object file)
    std::ifstream materialFile(strMaterialFile);
    if (materialFile)
    {
        std::string strMaterialLine;
        unsigned int lineMaterialFile = 1;
        while (std::getline(materialFile, strMaterialLine))
        {
            if (strMaterialLine != "")
            {
                bool result = DecodeMaterialLine(strMaterialLine);
                if (!result)
                {
                    std::string message = "Error reading line in material file: \r\n" + strMaterialFile + "\r\nSkipping line " + Utils::IntToStr(lineMaterialFile) + " in material file";
                    Error::WriteLog("ERROR", "Objects::ReadMaterialFile", message.c_str());
                }
            }

            lineMaterialFile++;
        }

        // Process last material
        if (pMaterial != NULL) ProcessCurrentMaterial();

    } else
    {
        std::string message = "Error reading file: \r\n" + strMaterialFile;
        Error::WriteLog("ERROR", "Objects::ReadMaterialFile", message.c_str());

        return (false);
    }

    // Close material file
    materialFile.close();

    return (true);
}

/*********************************************************************
* Decode a line from the material file
*********************************************************************/
bool Objects::DecodeMaterialLine(std::string strMaterialLine)
{
    bool found = false;

    try
    {
        unsigned int pos = 0;

        // Skip spaces and tabs at the beginning of a new line
        while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;
    
        // Comment found
        if (strMaterialLine[pos] == '#')
        {
            // Just return
            return (true);
        }

        // New material found
        if ((strMaterialLine[pos] == 'n') && (strMaterialLine[pos+1] == 'e') && (strMaterialLine[pos+2] == 'w') && (strMaterialLine[pos+3] == 'm') && (strMaterialLine[pos+4] == 't') && (strMaterialLine[pos+5] == 'l'))
        {
            found = true;

            // New material, so if old material present then close it
            if (pMaterial != NULL) ProcessCurrentMaterial();

            // Skip to end of command
            pos += 6;

              // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read name
            std::string name;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                name.push_back(strMaterialLine[pos++]);
            }

            // Check if this material is allready in use
            bool found = false;
            for (unsigned int i = 0; i < numMaterials; i++)
            {
                if (!pMaterialArray[i]->name.compare(name)) found = true;
            }

            // If duplicate found, then show warning and ignore
            if (found)
            {
                std::string message = "Duplicate material found: ";
                message.append(name);
                Error::WriteLog("WARNING", "Objects::DecodeMatrialLine", message.c_str());
            } else
            {
                // Create new material
                CreateCurrentMaterial(name);
            }
        }

        // Texture found
        if ((strMaterialLine[pos] == 'm') && (strMaterialLine[pos + 1] == 'a') && (strMaterialLine[pos + 2] == 'p') && (strMaterialLine[pos + 3] == '_') && (strMaterialLine[pos + 4] == 'K') && (strMaterialLine[pos + 5] == 'd'))
        {
            found = true;

            std::string strValue;

            // Skip to end of command
            pos += 6;

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Check for options
            while (strMaterialLine[pos] == '-')
            {
                pos++;

                // Scaling found
                if (strMaterialLine[pos] == 's')
                {
                    pos++;

                    // Skip spaces and tabs
                    while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

                    // Get value in string format
                    strValue.clear();
                    while ((pos < strMaterialLine.length()) && (strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')) strValue.push_back(strMaterialLine[pos++]);

                    // Read x value
                    if (pMaterial != NULL) pMaterial->scaleX = Utils::StrToFloat((char *)strValue.c_str());

                    // Skip spaces and tabs
                    while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

                    // Get value in string format
                    strValue.clear();
                    while ((pos < strMaterialLine.length()) && (strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')) strValue.push_back(strMaterialLine[pos++]);

                    // Read y value
                    if (pMaterial != NULL)  pMaterial->scaleY = Utils::StrToFloat((char *)strValue.c_str());

                    // Skip spaces and tabs
                    while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

                    // Get value in string format
                    strValue.clear();
                    while ((pos < strMaterialLine.length()) && (strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')) strValue.push_back(strMaterialLine[pos++]);

                    // Read z value
                    if (pMaterial != NULL)  pMaterial->scaleZ = Utils::StrToFloat((char *)strValue.c_str());
                }

                // Offset found
                if (strMaterialLine[pos] == 'o')
                {
                    pos++;

                    // Skip spaces and tabs
                    while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

                    // Get value in string format
                    strValue.clear();
                    while ((pos < strMaterialLine.length()) && (strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')) strValue.push_back(strMaterialLine[pos++]);

                    // Read x value
                    if (pMaterial != NULL) pMaterial->offsetX = Utils::StrToFloat((char *)strValue.c_str());

                    // Skip spaces and tabs
                    while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

                    // Get value in string format
                    strValue.clear();
                    while ((pos < strMaterialLine.length()) && (strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')) strValue.push_back(strMaterialLine[pos++]);

                    // Read y value
                    if (pMaterial != NULL) pMaterial->offsetY = Utils::StrToFloat((char *)strValue.c_str());

                    // Skip spaces and tabs
                    while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

                    // Get value in string format
                    strValue.clear();
                    while ((pos < strMaterialLine.length()) && (strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')) strValue.push_back(strMaterialLine[pos++]);

                    // Read z value
                    if (pMaterial != NULL) pMaterial->offsetZ = Utils::StrToFloat((char *)strValue.c_str());
                }
            }

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;
            
            // Read (normal) texture name
            std::string nameTexture = "";
            std::string nameNormal = "";
            while ((pos < strMaterialLine.length()) && (strMaterialLine[pos] != '.')) 
            {
                nameTexture.push_back(strMaterialLine[pos]);
                nameNormal.push_back(strMaterialLine[pos]);
                pos++;
            }

            // Complete normal texture name
            nameNormal.append("_normal");

            // Add extension
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                nameTexture.push_back(strMaterialLine[pos]);
                nameNormal.push_back(strMaterialLine[pos]);
                pos++;
            }

            // Read texture
            if (pMaterial != NULL) pMaterial->ReadTexture((char *)nameTexture.c_str());

            // Read normal texture
            if (pMaterial != NULL) pMaterial->ReadNormalTexture((char *)nameNormal.c_str());
        }

        // Ambient color albedo found
        if ((strMaterialLine[pos] == 'K') && (strMaterialLine[pos + 1] == 'a'))
        {
            found = true;

            // Skip command
            pos += 2;

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read red component
            std::string red;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                red.push_back(strMaterialLine[pos++]);
            }

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read green component
            std::string green;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                green.push_back(strMaterialLine[pos++]);
            }

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read blue component
            std::string blue;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                blue.push_back(strMaterialLine[pos++]);
            }

            // Put in the color
            if (pMaterial != NULL) 
            {
                pMaterial->Ka[0] = Utils::StrToFloat((char*)red.c_str());
                pMaterial->Ka[1] = Utils::StrToFloat((char*)green.c_str());
                pMaterial->Ka[2] = Utils::StrToFloat((char*)blue.c_str());
            }
        }

        // Diffuse color albedo found
        if ((strMaterialLine[pos] == 'K') && (strMaterialLine[pos + 1] == 'd'))
        {
            found = true;

            // Skip command
            pos += 2;

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read red component
            std::string red;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                red.push_back(strMaterialLine[pos++]);
            }

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read green component
            std::string green;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                green.push_back(strMaterialLine[pos++]);
            }

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read blue component
            std::string blue;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                blue.push_back(strMaterialLine[pos++]);
            }

            // Put in the color
            if (pMaterial != NULL) 
            {
                pMaterial->Kd[0] = Utils::StrToFloat((char*)red.c_str());
                pMaterial->Kd[1] = Utils::StrToFloat((char*)green.c_str());
                pMaterial->Kd[2] = Utils::StrToFloat((char*)blue.c_str());
            }
        }

        // Specular color albedo found
        if ((strMaterialLine[pos] == 'K') && (strMaterialLine[pos + 1] == 's'))
        {
            found = true;

            // Skip command
            pos += 2;

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read red component
            std::string red;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                red.push_back(strMaterialLine[pos++]);
            }

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read green component
            std::string green;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                green.push_back(strMaterialLine[pos++]);
            }

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read blue component
            std::string blue;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                blue.push_back(strMaterialLine[pos++]);
            }

            // Put in the color
            if (pMaterial != NULL)
            {
                pMaterial->Ks[0] = Utils::StrToFloat((char*)red.c_str());
                pMaterial->Ks[1] = Utils::StrToFloat((char*)green.c_str());
                pMaterial->Ks[2] = Utils::StrToFloat((char*)blue.c_str());
            }
        }

        // Specular reflectivity found
        if ((strMaterialLine[pos] == 'N') && (strMaterialLine[pos + 1] == 's'))
        {
            found = true;

            // Skip command
            pos += 2;

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read specular factor
            std::string specular;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                specular.push_back(strMaterialLine[pos++]);
            }

            // Put in the color
            if (pMaterial != NULL) pMaterial->Kn = atoi((char *)specular.c_str());
        }

        // Illumination model found
        if ((strMaterialLine[pos] == 'i') && (strMaterialLine[pos + 1] == 'l') && (strMaterialLine[pos + 2] == 'l') && (strMaterialLine[pos + 3] == 'u') && (strMaterialLine[pos + 4] == 'm'))
        {
            found = true;

            // Skip command
            pos += 5;

            // Skip spaces and tabs
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] == ' ') || (strMaterialLine[pos] == '\t'))) pos++;

            // Read illumination model
            std::string illum;
            while ((pos < strMaterialLine.length()) && ((strMaterialLine[pos] != ' ') && (strMaterialLine[pos] != '\t')))
            {
                illum.push_back(strMaterialLine[pos++]);
            }

            // Put in the color
            if (pMaterial != NULL) pMaterial->Illum = atoi((char *)illum.c_str());
        }
    } catch (std::exception& e)
    {
        std::string message = "Exception decoding material line: ";
        message.append(strMaterialLine);
        message.append(": ");
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::DecodeMaterialLine", message.c_str());
        return (false);
    }

   if (found)
   {
       return (true);
   } else 
   {
       return (false);
   } 
}

/*********************************************************************
* Create a new (current) material
*********************************************************************/
bool Objects::CreateCurrentMaterial (std::string name)
{
    if (numMaterials < MAX_NUM_MATERIALS)
    {
        // Create new material
        pMaterial = new Material(name);

        if (pMaterial != NULL) return (true);
    }

    Error::WriteLog("ERROR", "Objects::CreateCurrentMaterial", "Could not add (all) materials (too many materials)");
    return (false);
}

/*********************************************************************
* Move the current material to the list and close it
*********************************************************************/
void Objects::ProcessCurrentMaterial (void)
{
    // Add new Material to array of pointers (Material list)
    pMaterialArray[numMaterials++] = pMaterial;

    // Set current material to NULL
    pMaterial = NULL;
}

/*********************************************************************
* Save meshes (triangulated)
*********************************************************************/
void Objects::SaveMeshes (void)
{
    int indexFaces = 1;
    bool result;
    Error::WriteLog("INFO", "Objects::SaveMeshes", "Creating new (triangulated) meshes");

    // Add material file entry to meshes file
    std::string content = "# Mesh file for 3dEngine\r\n\r\n";
    content.append ("mtllib meshes.mtl");
    content.append ("\r\n\r\n");

    // Add header to material file
    std::string contentMaterial;
    contentMaterial.append ("# Material file for 3dEngine\r\n\r\n");

    for (unsigned int i=0; i < numMeshes; i++)
    {
        try
        {
            if ((pMeshArray[i]->name != "Sky") && (pMeshArray[i]->name != "Terrain") && (pMeshArray[i]->name != "Bulb"))
            {
                // Add object name
                content.append ("o ");
                content.append (pMeshArray[i]->name);
                content.append ("\r\n\r\n");

                // Add all Vertices
                for (unsigned int j=0; j < pMeshArray[i]->numVertices; j++)
                {
                    // Write vertex label
                    content.append ("v ");

                    // Write vertex x-coordinate
                    content.append (Utils::FloatToStr(pMeshArray[i]->pVertices[j].x));
                    content.append (" ");

                    // Write vertex y-coordinate
                    content.append (Utils::FloatToStr(pMeshArray[i]->pVertices[j].y));
                    content.append (" ");

                    // Write vertex z-coordinate
                    content.append (Utils::FloatToStr(pMeshArray[i]->pVertices[j].z));
                    content.append ("\r\n");
                }
                content.append ("\r\n");

                // Add material
                content.append ("usemtl material_");
                content.append (pMeshArray[i]->name);
                content.append("\r\n\r\n");

                // Add all Faces (+1 because the first in the file is '1', while internal it is '0')
                for (unsigned int j=0; j < pMeshArray[i]->numFaces; j++)
                {
                    // Write face label
                    content.append ("f ");

                    // Write first face vertex index
                    content.append (std::to_string(pMeshArray[i]->pFaces[j].first + indexFaces));
                    content.append (" ");

                    // Write second face vertex index
                    content.append (std::to_string(pMeshArray[i]->pFaces[j].second + indexFaces));
                    content.append (" ");

                    // Write third face vertex index
                    content.append (std::to_string(pMeshArray[i]->pFaces[j].third + indexFaces));
                    content.append ("\r\n");
                }
                content.append ("\r\n");

                // Vertices are counted through all meshes
                indexFaces += pMeshArray[i]->numVertices;

                // Write material file for this mesh
                contentMaterial.append("newmtl material_");
                contentMaterial.append (pMeshArray[i]->name);
                contentMaterial.append("\r\n");
                contentMaterial.append ("Ns 128\r\n");
                contentMaterial.append ("Ka 1.0 1.0 1.0\r\n");
                contentMaterial.append ("Kd 0.64 0.64 0.64\r\n");
                contentMaterial.append ("Ks 0.5 0.5 0.5\r\n");
                contentMaterial.append ("map_Kd -s 1.0 1.0 1.0 ");
                contentMaterial.append ("textures/");
                contentMaterial.append (pMeshArray[i]->name);
                contentMaterial.append (".bmp\r\n\r\n");
            }
        } catch (std::exception& e)
        {
            std::string message = "Could not save mesh: ";
            message.append (pMeshArray[i]->name);
            message.append(": ");
            message.append(e.what());
            Error::WriteLog("EXCEPTION", "Objects::SaveMeshes", message.c_str());
            return;
        }
    }

    // Save mesh file
    result = Files::Write ("objects//meshes.obj", (char *) content.c_str(), (int)content.size());
    if (!result)
    {
        Error::WriteLog("ERROR", "Objects::SaveMeshes", "Could not save objects/meshes.obj");
        return;
    }

    // Save material file
    result = Files::Write ("objects//meshes.mtl", (char *)contentMaterial.c_str(), (int)contentMaterial.size());
    if (!result)
    {
        Error::WriteLog("ERROR", "Objects::SaveMeshes", "Could not save objects/meshes.mtl");
        return;
    }
}

/*********************************************************************
* Create a sky as a first object
*********************************************************************/
void Objects::CreateSky (void)
{
    try
    {
        // Create sky (cube) object and add to the list
        Mesh *pMesh = new Mesh();
        pMesh->name = "Sky";
        pMeshArray[numMeshes] = pMesh;
        numMeshes++;

        // Number of vertices in the sky (cube)
        pMesh->numVertices = 24;

        // Allocate memory for the Sky vertices
        pMesh->pVertices = new VecMat::Vertex[pMesh->numVertices];
		if (pMesh->pVertices == nullptr) return;

        // Fill this memory with 0's
        ZeroMemory(pMesh->pVertices, pMesh->numVertices * sizeof(VecMat::Vertex));

        // Fill vertices list
        VecMat::Vertex *ptrV = pMesh->pVertices;

        // back
        *ptrV++ = VecMat::Vertex(-10.0f, -5.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(-10.0f, 15.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, 15.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, -5.0f, -10.0f);

        // left
        *ptrV++ = VecMat::Vertex(-10.0f, -5.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(-10.0f, 15.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(-10.0f, 15.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(-10.0f, -5.0f, 10.0f);

        // right
        *ptrV++ = VecMat::Vertex(10.0f, -5.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, 15.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, 15.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, -5.0f, 10.0f);

        // front
        *ptrV++ = VecMat::Vertex(-10.0f, -5.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(-10.0f, 15.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, 15.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, -5.0f, 10.0f);

        // top
        *ptrV++ = VecMat::Vertex(-10.0f, 15.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(-10.0f, 15.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, 15.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, 15.0f, -10.0f);

        // bottom
        *ptrV++ = VecMat::Vertex(-10.0f, -5.0f, -10.0f);
        *ptrV++ = VecMat::Vertex(-10.0f, -5.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, -5.0f, 10.0f);
        *ptrV++ = VecMat::Vertex(10.0f, -5.0f, -10.0f);

        // Number of faces in the Sky (Cube) (triangles, not squares)
        pMesh->numFaces = 12;

        // Allocate memory for the Sky faces
        pMesh->pFaces = new VecMat::Face[pMesh->numFaces];
		if (pMesh->pFaces == nullptr) return;

        // Fill this memory with 0's
        ZeroMemory(pMesh->pFaces, pMesh->numFaces * sizeof(VecMat::Face));

        // Fill faces list
        VecMat::Face *ptrF = pMesh->pFaces;

        // back
        *ptrF++ = VecMat::Face( 0,  1,  2);
        *ptrF++ = VecMat::Face( 2,  3,  0);

        // left
        *ptrF++ = VecMat::Face( 4,  5,  6);
        *ptrF++ = VecMat::Face( 6,  7,  4);

        // right
        *ptrF++ = VecMat::Face( 8,  9, 10);
        *ptrF++ = VecMat::Face(10, 11,  8);

        // front
        *ptrF++ = VecMat::Face(12, 13, 14);
        *ptrF++ = VecMat::Face(14, 15, 12);

        // top
        *ptrF++ = VecMat::Face(16, 17, 18);
        *ptrF++ = VecMat::Face(18, 19, 16);

        // bottom
        *ptrF++ = VecMat::Face(20, 21, 22);
        *ptrF++ = VecMat::Face(22, 23, 20);

        // Number of texture vertices in the sky (cube)
        pMesh->numTextureVertices = 24;

        // Allocate memory for the Sky texture vertices
        pMesh->pTextureVertices = new VecMat::Vertex[pMesh->numTextureVertices];
		if (pMesh->pTextureVertices == nullptr) return;

        // Fill this memory with 0's
        ZeroMemory(pMesh->pTextureVertices, pMesh->numTextureVertices * sizeof(VecMat::Vertex));

        // Fill texture vertices list
        VecMat::Vertex *ptrTV = pMesh->pTextureVertices;

        // back
        *ptrTV++ = VecMat::Vertex(0.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 0.0f, 0.0f);

        // left
        *ptrTV++ = VecMat::Vertex(1.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 0.0f, 0.0f);

        // right
        *ptrTV++ = VecMat::Vertex(0.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 0.0f, 0.0f);

        // front
        *ptrTV++ = VecMat::Vertex(1.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 0.0f, 0.0f);

        // top
        *ptrTV++ = VecMat::Vertex(1.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);

        // bottom
        *ptrTV++ = VecMat::Vertex(0.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 0.0f, 0.0f);

        // Create a new material for the backplane
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("SkyBack");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Illum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[0] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[1] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[2] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[2] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[2] = 0.0f;

        // Read texture
        std::string fileName = applicationFolder + "\\textures\\sky\\skyback.bmp";
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadTexture(fileName.c_str());

        // Add new Material to array of pointers (Material list)
        pMaterialArray[numMaterials++] = pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial;

        // Insert the starting face for this material
        pMesh->pMaterialEntryList[pMesh->numMaterials]->start = 0;

        // Add material to counter
        pMesh->numMaterials++;

        // Create a new material for the leftplane
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("SkyLeft");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Illum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[0] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[1] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[2] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[2] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[2] = 0.0f;

        // Read texture
        fileName = applicationFolder + "\\textures\\sky\\skyleft.bmp";
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadTexture(fileName.c_str());

        // Add new Material to array of pointers (Material list)
        pMaterialArray[numMaterials++] = pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial;

        // Insert the starting face for this material
        pMesh->pMaterialEntryList[pMesh->numMaterials]->start = 2;

        // Add material to counter
        pMesh->numMaterials++;

        // Create a new material for the rightplane
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("SkyRight");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Illum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[0] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[1] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[2] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[2] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[2] = 0.0f;

        // Read texture
        fileName = applicationFolder + "\\textures\\sky\\skyright.bmp";
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadTexture(fileName.c_str());

        // Add new Material to array of pointers (Material list)
        pMaterialArray[numMaterials++] = pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial;

        // Insert the starting face for this material
        pMesh->pMaterialEntryList[pMesh->numMaterials]->start = 4;

        // Add material to counter
        pMesh->numMaterials++;

        // Create a new material for the frontplane
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("SkyFront");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Illum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[0] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[1] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[2] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[2] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[2] = 0.0f;

        // Read texture
        fileName = applicationFolder + "\\textures\\sky\\skyfront.bmp";
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadTexture(fileName.c_str());

        // Add new Material to array of pointers (Material list)
        pMaterialArray[numMaterials++] = pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial;

        // Insert the starting face for this material
        pMesh->pMaterialEntryList[pMesh->numMaterials]->start = 6;

        // Add material to counter
        pMesh->numMaterials++;

        // Create a new material for the topplane
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("SkyTop");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Illum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[0] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[1] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[2] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[2] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[2] = 0.0f;

        // Read texture
        fileName = applicationFolder + "\\textures\\sky\\skytop.bmp";
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadTexture(fileName.c_str());

        // Add new Material to array of pointers (Material list)
        pMaterialArray[numMaterials++] = pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial;

        // Insert the starting face for this material
        pMesh->pMaterialEntryList[pMesh->numMaterials]->start = 8;

        // Add material to counter
        pMesh->numMaterials++;

        // Create a new material for the bottomplane
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("SkyBottom");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Illum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[0] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[1] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[2] = 2.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[2] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[0] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[1] = 0.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[2] = 0.0f;

        // Read texture
        fileName = applicationFolder + "\\textures\\sky\\skybottom.bmp";
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadTexture(fileName.c_str());

        // Add new Material to array of pointers (Material list)
        pMaterialArray[numMaterials++] = pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial;

        // Insert the starting face for this material
        pMesh->pMaterialEntryList[pMesh->numMaterials]->start = 10;

        // Add material to counter
        pMesh->numMaterials++;
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(": ");
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::CreateSky", message.c_str());
        return;
    }
}

/*********************************************************************
* Create a Terrain as a second object
*********************************************************************/
Mesh* Objects::CreateTerrain (void)
{
    try
    {
        // Create terrain object and add to the list
        Mesh *pMesh = new Mesh();
        pMesh->name = "Terrain";
        pMeshArray[numMeshes] = pMesh;
        numMeshes++;

        // Create a new material for the terrain
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("Terrain");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Illum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[0] = 1.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[1] = 1.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ka[2] = 1.0f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[0] = 0.5f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[1] = 0.5f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Kd[2] = 0.5f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[0] = 0.1f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[1] = 0.1f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->Ks[2] = 0.1f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->scaleX = 0.005f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->scaleY = 0.005f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->scaleZ = 0.005f;

        // Read texture
        std::string textureFileName = applicationFolder + "\\textures\\terrain0.bmp";
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadTexture(textureFileName.c_str());

        // Read normal texture
        std::string normalFileName = applicationFolder + "\\textures\\terrain0_normal.bmp";
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadNormalTexture((char *)normalFileName.c_str());

        // Add new Material to array of pointers (Material list)
        pMaterialArray[numMaterials++] = pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial;

        // Insert the starting face for this material
        pMesh->pMaterialEntryList[pMesh->numMaterials]->start = 0;

        // Add material to counter
        pMesh->numMaterials++;

        // Create some more materials for terrain
        unsigned int startMaterial = numMaterials;
        for (unsigned int i = 1; i < 7; i++)
        {
            Material *pMaterial = new Material("Terrain" + Utils::IntToStr(i));
            pMaterial->Illum = 1;
            pMaterial->scaleX = 0.01f;
            pMaterial->scaleY = 0.01f;
            pMaterial->scaleZ = 0.01f;

            // Read texture
            std::string fileTextureName = applicationFolder + "\\textures\\terrain" + Utils::IntToStr(i) + ".bmp";
            std::string fileNormalName = applicationFolder + "\\textures\\terrain" + Utils::IntToStr(i) + "_normal.bmp";
            bool result = pMaterial->ReadTexture(fileTextureName.c_str());
            pMaterial->ReadNormalTexture(fileNormalName.c_str());

            if (!result)
            {
                float r = Utils::Random();
                float g = Utils::Random();
                float b = Utils::Random();

                pMaterial->Ka[0] = r;
                pMaterial->Ka[1] = g;
                pMaterial->Ka[2] = b;

                pMaterial->Kd[0] = r;
                pMaterial->Kd[1] = g;
                pMaterial->Kd[2] = b;
            }

            // Add new Material to array of pointers (Material list)
            pMaterialArray[numMaterials++] = pMaterial;
        }

        // Number of vertices in the terrain
        pMesh->numVertices = 4;

        // Allocate memory for the Terrain vertices
        pMesh->pVertices = new VecMat::Vertex[pMesh->numVertices];
        if (pMesh->pVertices == nullptr) return NULL;

        // Fill this memory with 0's
        ZeroMemory(pMesh->pVertices, pMesh->numVertices * sizeof(VecMat::Vertex));

        // Fill vertices list
        VecMat::Vertex *ptrV = pMesh->pVertices;
        *ptrV++ = VecMat::Vertex(-1000.0f, 0.0f, -1000.0f);
        *ptrV++ = VecMat::Vertex(-1000.0f, 0.0f,  1000.0f);
        *ptrV++ = VecMat::Vertex( 1000.0f, 0.0f,  1000.0f);
        *ptrV++ = VecMat::Vertex( 1000.0f, 0.0f, -1000.0f);

        // Number of faces in the Terrain (triangles, not squares)
        pMesh->numFaces = 2;

        // Allocate memory for the Terrain faces
        pMesh->pFaces = new VecMat::Face[pMesh->numFaces];
        if (pMesh->pFaces == nullptr) return NULL;

        // Fill this memory with 0's
        ZeroMemory(pMesh->pFaces, pMesh->numFaces * sizeof(VecMat::Face));

        // Fill faces list
        VecMat::Face *ptrF = pMesh->pFaces;

        *ptrF++ = VecMat::Face(0, 1, 2);
        *ptrF++ = VecMat::Face(2, 3, 0);

        // Number of texture vertices in the Terrain
        pMesh->numTextureVertices = 4;

        // Allocate memory for the Terrain texture vertices
        pMesh->pTextureVertices = new VecMat::Vertex[pMesh->numTextureVertices];
        if (pMesh->pTextureVertices == nullptr) return NULL;

        // Fill this memory with 0's
        ZeroMemory(pMesh->pTextureVertices, pMesh->numTextureVertices * sizeof(VecMat::Vertex));

        // Fill texture vertices list
        VecMat::Vertex *ptrTV = pMesh->pTextureVertices;
        *ptrTV++ = VecMat::Vertex(0.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);

        // Calculate normals for this mesh
        CalculateNormals(pMesh);

        // Calculate (bi-)tangents for this mesh
        CalculateTangents(pMesh);

        return (pMesh);
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::CreateTerrain", message.c_str());
        return NULL;
    }
}

/*********************************************************************
* Create Axis as a third object
*********************************************************************/
void Objects::CreateAxis (void)
{
    try
    {
        // Get an x-axis 
        Axis *pAxisX = new Axis(0.2f, 'x');

        // Add new Mesh to array of pointers (Objects list)
        pMeshArray[numMeshes++] = pAxisX;

        // Get an y-axis 
        Axis *pAxisY = new Axis(0.2f, 'y');

        // Add new Mesh to array of pointers (Objects list)
        pMeshArray[numMeshes++] = pAxisY;

        // Get an z-axis 
        Axis *pAxisZ = new Axis(0.2f, 'z');

        // Add new Mesh to array of pointers (Objects list)
        pMeshArray[numMeshes++] = pAxisZ;

    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::CreateAxis", message.c_str());
        return;
    }
}


/*********************************************************************
* Create Bulb as a fouth object
*********************************************************************/
void Objects::CreateBulb (void)
{
    try
    {
        // Get a sphere mesh
        Sphere *pSphere = new Sphere(0.2f, 10, 10);
        pSphere->pos_x = 0.0f;
        pSphere->pos_y = 0.0f;
        pSphere->pos_z = 0.0f;
        pSphere->speed_angle = 0.0f;
        pSphere->speed_pos = 0.0f;
        pSphere->name = "Bulb";
        pSphere->numMaterials = 1;

        // Set material for this sphere
        Material* pMaterial = new Material("MaterialBulb");
        pMaterial->Ka[0] = 1.0f;
        pMaterial->Ka[1] = 1.0f;
        pMaterial->Ka[2] = 1.0f;

        MaterialEntry *pMaterialEntry = new MaterialEntry();
        pMaterialEntry->pMaterial = pMaterial;
        pSphere->pMaterialEntryList[0] = pMaterialEntry;

        // Get all normals for this mesh
        CalculateNormals(pSphere);

        // Get all (bi-)tangents for this mesh
        CalculateTangents(pSphere);

        // Calculate sizes in x,y and z direction
        CalculateSizes(pSphere);

        // Add new Mesh to array of pointers (Objects list)
        pMeshArray[numMeshes++] = pSphere;
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::CreateBulb", message.c_str());
        return;
    }
}
