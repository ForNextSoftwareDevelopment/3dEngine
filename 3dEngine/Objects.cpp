#include "Objects.h"

/*********************************************************************
* Constructor
*********************************************************************/
Objects::Objects (char *pApplicationFolder)
{
    // Copy application folder
    applicationFolder.append(pApplicationFolder);

    // Reset current mesh / material
    pMesh = NULL;
    pMaterial = NULL;

    // Position of the current mesh
    posX = posY = posZ = 0.0f;
    std::string strPosX;
    std::string strPosY;
    std::string strPosZ;

    // Reset all meshes / materials
    numMeshes = 0;
    numMaterials = 0;
    for (int i = 0; i < MAX_NUM_MESHES; i++) pMeshArray[i] = NULL;
    for (int i = 0; i < MAX_NUM_MATERIALS; i++) pMaterialArray[i] = NULL;
     
    try
    {
        // Create a sky(cube) as a first object
        CreateSky();

        // Create axis as next objects
        CreateAxis();

        // Create terrain
        CreateTerrain();

        /*
        // Get a cone mesh
        Cone *pCone = new Cone(2, 10, 10);
        pCone->name = "Cone";
        pCone->numMaterials = 1;

        // Assign material to the mesh
        pMaterial = new Material(("default-" + pCone->name).c_str());
        pCone->pMaterialEntryList[0] = new MaterialEntry();
        pCone->pMaterialEntryList[0]->pMaterial = pMaterial;

        // Add new Mesh to array of pointers (Objects list)
        pMeshArray[numMeshes++] = pCone;
        */

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
                        bool result = DecodeObjectLine(strObjectLine);
                        if (!result)
                        {
                            std::string message = "Error reading line in object file: \r\n" + strObjectFile + "\r\nSkipping line " + Utils::IntToStr(lineObjectFile) + " in object file";
                            Error::WriteLog("ERROR", "Objects::Objects", message.c_str());
                        }

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
                if (pMesh != NULL) CloseCurrentMesh();
            }

            // Next line
            lineObjectsFile++;
        }

        // Close objects file
        objectsFile.close();

        // Give a list of meshes read
        std::string meshNames;
        meshNames.clear();
        for (unsigned int i=0; i<numMeshes; i++)
        {
            meshNames.append("           " + pMeshArray[i]->name + "\r\n");
        }

        // Write the list to the log file
        Error::WriteLog("INFO", "Objects::Objects", ("Meshes read: \r\n" + meshNames).c_str());

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
bool Objects::DecodeObjectLine(std::string strObjectLine)
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
                if (pMesh != NULL) CloseCurrentMesh();

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
                if (pMesh != NULL)
                {
                    // Search material list for the correct material
                    bool found = false;
                    for (unsigned int i = 0; (i < numMaterials) && !found; i++)
                    {
                        if (!pMaterialArray[i]->name.compare(name))
                        {
                            found = true;

                            // Create new material entry in the current object
                            pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();

                            // Put found material in the new entry
                            pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = pMaterialArray[i];

                            // Insert the starting face for this material
                            pMesh->pMaterialEntryList[pMesh->numMaterials++]->start = (int)fList.size();
                        }
                    }

                    if (!found)
                    {
                        std::string message;
                        message.append("Material not found: ");
                        message.append(name);
                        Error::WriteLog("WARNING", "Objects::ReadObjectFile", message.c_str());
                    }
                }
            }

            // Vertex found
            if ((strObjectLine[pos] == 'v') && ((strObjectLine[pos + 1] == ' ') || (strObjectLine[pos + 1] == '\t')))
            {
                found = true;

                std::string strValue;

                // If no object available, then create new
                if (pMesh == NULL)
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
                vertex.x = Utils::StrToFloat((char *)strValue.c_str()) + posX;

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read y value
                vertex.y = Utils::StrToFloat((char *)strValue.c_str()) + posY;

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read z value
                vertex.z = Utils::StrToFloat((char *)strValue.c_str()) + posZ;

                // Put the new vertex in the vertices list
                vList.push_back(vertex);
            }

            // Vertex normal found
            if ((strObjectLine[pos] == 'v') && (strObjectLine[pos + 1] == 'n'))
            {
                found = true;

                std::string strValue;

                // If no object available, then create new
                if (pMesh == NULL)
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
                vertex.x = Utils::StrToFloat((char *)strValue.c_str()) + posX;

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read y value
                vertex.y = Utils::StrToFloat((char *)strValue.c_str()) + posY;

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read z value
                vertex.z = Utils::StrToFloat((char *)strValue.c_str()) + posZ;

                // Put the new vertex in the normals list
                vnList.push_back(vertex);
            }

            // Vertex texture found
            if ((strObjectLine[pos] == 'v') && (strObjectLine[pos + 1] == 't'))
            {
                found = true;

                std::string strValue;

                // If no object available, then create new
                if (pMesh == NULL)
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
                vertex.x = Utils::StrToFloat((char *)strValue.c_str()) + posX;

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read y value
                vertex.y = Utils::StrToFloat((char *)strValue.c_str()) + posY;

                // Skip to begin of next value
                while ((pos < strObjectLine.length()) && ((strObjectLine[pos] == ' ') || (strObjectLine[pos] == '\t'))) pos++;

                // Get value in string format
                strValue.clear();
                while ((pos < strObjectLine.length()) && (strObjectLine[pos] != ' ') && (strObjectLine[pos] != '\t')) strValue.push_back(strObjectLine[pos++]);

                // Read z value
                vertex.z = Utils::StrToFloat((char *)strValue.c_str()) + posZ;

                // Put the new vertex in the vertex texture list
                vtList.push_back(vertex);
            }

            // Face found
            if ((strObjectLine[pos] == 'f') && ((strObjectLine[pos + 1] == ' ') || (strObjectLine[pos + 1] == '\t')))
            {
                found = true;

                std::string strValue;

                // If no object available, then create new
                if (pMesh == NULL)
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
                        std::string message = "Vertex not in range in line: ";
                        message.append(strObjectLine);
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
                        pMesh->numTriangulationErrors++;
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
                        Error::WriteLog("WARNING", "Objects::DecodeObjectLine", "Face has less then 3 verices => ignored");
                    }
            }

            if (!found)
            {
                std::string message = "Unknown part in object file: ";
                message.append(strObjectLine);
                Error::WriteLog("WARNING", "Objects::DecodeObjectLine", message.c_str());
            }
        }
    } catch (std::exception& e)
    {
        std::string message = "Exception decoding object line: ";
        message.append(strObjectLine);
        message.append(": ");
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::DecodeObjectLine", message.c_str());

        delete (pMesh);
        pMesh = NULL;

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
        pMesh = new Mesh (name);

        if (pMesh != NULL) return (true);
    }

    Error::WriteLog("ERROR", "Objects::CreateCurrentMesh", "Could not add (all) meshes (too many meshes)");
    return (false);
}

/*********************************************************************
* Move the current mesh to the list and close it
*********************************************************************/
void Objects::CloseCurrentMesh (void)
{
    std::string error;

    #ifdef DEBUG
    // If errors have occured during the triangulation of the last object then show
    if (pMesh->numTriangulationErrors > 0)
    {
        error.clear();
        error.append("Not all triangles found in '" + pMesh->name + "'\r\n");
        error.append("Number of errors = ");
        error.append(std::to_string(pMesh->numTriangulationErrors));
        error.append(" (probably some polygons are illegal or have double vertices)\r\n");
        Error::WriteLog("WARNING", "Objects::CloseCurrentMesh", error.c_str());
    }
    #endif

    try
    {
        // Get the number of vertices in the Mesh
        pMesh->numVertices = (int)vList.size();

        // Add the number of vertices to the offset
        offsetV += pMesh->numVertices;

        // Add the number of texture vertices to the offset
        offsetT += pMesh->numTextureVertices;

        // Allocate memory for vertices and clear this memory
        pMesh->pVertices = (VecMat::Vertex *) malloc (pMesh->numVertices * sizeof(VecMat::Vertex));
        ZeroMemory (pMesh->pVertices, pMesh->numVertices * sizeof(VecMat::Vertex));

        // Copy vertices from the list to the vertices memory block
        VecMat::Vertex *pv = pMesh->pVertices;
        for (unsigned int i=0; i<pMesh->numVertices; i++) *pv++ = vList[i];

        // Get the number of faces (triangles) in the Mesh
        pMesh->numFaces = (int)fList.size();

        // Allocate memory for faces and clear this memory
        pMesh->pFaces = (VecMat::Face *) malloc (pMesh->numFaces * sizeof(VecMat::Face));
        ZeroMemory (pMesh->pFaces, pMesh->numFaces * sizeof(VecMat::Face));

        // Copy faces from the list to the faces memory block
        VecMat::Face *pf = pMesh->pFaces;
        for (unsigned int i=0; i<pMesh->numFaces; i++) *pf++ = fList[i];

        // Calculate all face normals and put them in a list
        std::vector<VecMat::Vertex> fnList;
        for (unsigned int i=0; i<pMesh->numFaces; i++)
        {
            VecMat::Vec3 normal;

            // Calculate first vector
            VecMat::Vec3 first;
            first[0] = vList[fList[i].second].x - vList[fList[i].first].x;
            first[1] = vList[fList[i].second].y - vList[fList[i].first].y;
            first[2] = vList[fList[i].second].z - vList[fList[i].first].z;

            // Calculate second vector
            VecMat::Vec3 second;
            second[0] = vList[fList[i].third].x - vList[fList[i].first].x;
            second[1] = vList[fList[i].third].y - vList[fList[i].first].y;
            second[2] = vList[fList[i].third].z - vList[fList[i].first].z;

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
        pMesh->pFaceNormals = (VecMat::Vertex *) malloc (pMesh->numFaceNormals * sizeof(VecMat::Vertex));
        ZeroMemory (pMesh->pFaceNormals, pMesh->numFaceNormals * sizeof(VecMat::Vertex));

        // Copy face normals from the list to the face normals memory block
        VecMat::Vertex *pfn = pMesh->pFaceNormals;
        for (unsigned int i=0; i<pMesh->numFaceNormals; i++) *pfn++ = fnList[i];

        // Set number of vertex normals
        pMesh->numVertexNormals = pMesh->numVertices;

        // Allocate memory for vertex normals
        pMesh->pVertexNormals = (VecMat::Vertex *) malloc (pMesh->numVertexNormals * sizeof(VecMat::Vertex));
        ZeroMemory (pMesh->pVertexNormals, pMesh->numVertexNormals * sizeof(VecMat::Vertex));
        VecMat::Vertex *pvn = pMesh->pVertexNormals;

        // Check if vertex normals are provided
        bool result = true;
        if (vnList.size() != vList.size())
        {
            // Not valid, so clear
            vnList.clear();

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
            for (unsigned int i=0; i<pMesh->numVertexNormals; i++)
            {
                float normFactor = sqrt(pvn[i].x * pvn[i].x + pvn[i].y * pvn[i].y + pvn[i].z * pvn[i].z);
                if (normFactor == 0.0f) result = false; else
                {
                    pvn[i].x = pvn[i].x / normFactor;
                    pvn[i].y = pvn[i].y / normFactor;
                    pvn[i].z = pvn[i].z / normFactor;
                }
            }
        } else
        {
            // Just copy the provided vertex normals
            for (unsigned int i=0; i<pMesh->numVertexNormals; i++)
            {
                pvn[i] = vnList[i];
            }
        }

        // Error in calculation
        if (!result)
        {
            std::string message = "Could not calculate all normals for: ";
            message.append(pMesh->name);
            Error::WriteLog("WARNING", "Objects::CloseCurrentMesh", message.c_str());
        }

        // Check if texture mapping is provided (mappings and texture vertices present)
        if ((mList.size() != 0) && (vtList.size() != 0))
        {
            // Set marker that mapping has been provided in this mesh
            pMesh->mapped = true;

            // Set the number of texture vertices in the Mesh
            pMesh->numTextureVertices = (int)vtList.size();

            // Allocate memory
            pMesh->pTextureVertices = (VecMat::Vertex *) malloc (pMesh->numTextureVertices * sizeof(VecMat::Vertex));
            ZeroMemory (pMesh->pTextureVertices, pMesh->numTextureVertices * sizeof(VecMat::Vertex));

            VecMat::Vertex *pvt = pMesh->pTextureVertices;

            // Copy vertices from the mapping list to the texture vertices memory block
            for (unsigned int i=0; i<mList.size(); i++)
            {
                unsigned int indexVertex  = mList[i].ver;
                unsigned int indexTexture = mList[i].tex;
                if ((indexVertex < pMesh->numTextureVertices) && (indexTexture < pMesh->numTextureVertices))
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

            for (unsigned int i=0; i<pMesh->numVertices; i++)
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
            pMesh->numTextureVertices = pMesh->numVertices;

            // Allocate memory
            pMesh->pTextureVertices = (VecMat::Vertex *) malloc (pMesh->numTextureVertices * sizeof(VecMat::Vertex));
            ZeroMemory (pMesh->pTextureVertices, pMesh->numTextureVertices * sizeof(VecMat::Vertex));

            // Copy vertices from the list (now used as texture vertices) to the texture vertices memory block
            VecMat::Vertex *pvt = pMesh->pTextureVertices;
            for (unsigned int i=0; i<pMesh->numTextureVertices; i++)
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
        if (pMesh->numMaterials == 0)
        {
            pMesh->numMaterials = 1;
            pMaterial = new Material(("default-" + pMesh->name).c_str());
            pMesh ->pMaterialEntryList[0] = new MaterialEntry();
            pMesh ->pMaterialEntryList[0]->pMaterial = pMaterial;
        }

        // Set number of tangents
        pMesh->numTangents = pMesh->numVertices;

        // Allocate memory for tangents
        pMesh->pTangents = (VecMat::Vertex *) malloc (pMesh->numTangents * sizeof(VecMat::Vertex));
        ZeroMemory (pMesh->pTangents, pMesh->numTangents * sizeof(VecMat::Vertex));
        VecMat::Vertex *pt = pMesh->pTangents;

        // Set number of bitangents
        pMesh->numBiTangents = pMesh->numVertices;

        // Allocate memory for bitangents
        pMesh->pBiTangents = (VecMat::Vertex *) malloc (pMesh->numBiTangents * sizeof(VecMat::Vertex));
        ZeroMemory (pMesh->pBiTangents, pMesh->numBiTangents * sizeof(VecMat::Vertex));
        VecMat::Vertex *pbt = pMesh->pBiTangents;

        // Check all faces and calculate the (bi-)tangent for that face
        std::vector<VecMat::Vertex> ftList;
        std::vector<VecMat::Vertex> fbtList;
        result = true;
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
/*
            bitangent[0] = f * (deltaUV1[0] * deltaPos2[0] - deltaUV2[0] * deltaPos1[0]);
            bitangent[1] = f * (deltaUV1[0] * deltaPos2[1] - deltaUV2[0] * deltaPos1[1]);
            bitangent[2] = f * (deltaUV1[0] * deltaPos2[2] - deltaUV2[0] * deltaPos1[2]);
*/
            VecMat::Vertex fBiTangent;
            fBiTangent.x = bitangent[0];
            fBiTangent.y = bitangent[1];
            fBiTangent.z = bitangent[2];

            // Add bitangent to list
            fbtList.push_back(fBiTangent);
        }

        // Error in calculation
        if (!result)
        {
            std::string message = "Could not calculate all (bi)tangent for: ";
            message.append(pMesh->name);
            Error::WriteLog("WARNING", "Objects::CloseCurrentMesh", message.c_str());
        }

        #ifdef DEBUGBUMP
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

        // Add new Mesh to array of pointers (Objects list)
        pMeshArray[numMeshes++] = pMesh;

        #ifdef DEBUGBUMP
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

        // Clear vertices, faces and mapping lists
        vList.clear();
        vtList.clear();
        vnList.clear();
        fList.clear();
        mList.clear();
    } catch (std::exception& e)
    {
        std::string message = "Exception closing mesh: ";
        message.append(pMesh->name);
        message.append(": ");
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::CloseCurrentMesh", message.c_str());

        // Free memory
        free (pMesh->pVertices);
        free (pMesh->pFaceNormals);
        free (pMesh->pVertexNormals);
        free (pMesh->pFaces);
        free (pMesh->pTangents);
        free (pMesh->pBiTangents);
    }

    // Set current object to NULL
    pMesh = NULL;
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
            bool result = DecodeMaterialLine(strMaterialLine);
            if (!result)
            {
                std::string message = "Error reading line in material file: \r\n" + strMaterialFile + "\r\nSkipping line " + Utils::IntToStr(lineMaterialFile) + " in material file";
                Error::WriteLog("ERROR", "Objects::ReadMaterialFile", message.c_str());
            }

            lineMaterialFile++;
        }

        // Close last material
        if (pMaterial != NULL) CloseCurrentMaterial();

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
            // New material, so if old material present then close it
            if (pMaterial != NULL) CloseCurrentMaterial();

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
            if (pMaterial != NULL) pMaterial->pKa = new VecMat::Vec3(Utils::StrToFloat((char *)red.c_str()), Utils::StrToFloat((char *)green.c_str()), Utils::StrToFloat((char *)blue.c_str()));
        }

        // Diffuse color albedo found
        if ((strMaterialLine[pos] == 'K') && (strMaterialLine[pos + 1] == 'd'))
        {
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
            if (pMaterial != NULL) pMaterial->pKd = new VecMat::Vec3(Utils::StrToFloat((char *)red.c_str()), Utils::StrToFloat((char *)green.c_str()), Utils::StrToFloat((char *)blue.c_str()));
        }

        // Specular color albedo found
        if ((strMaterialLine[pos] == 'K') && (strMaterialLine[pos + 1] == 's'))
        {
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
            if (pMaterial != NULL) pMaterial->pKs = new VecMat::Vec3(Utils::StrToFloat((char *)red.c_str()), Utils::StrToFloat((char *)green.c_str()), Utils::StrToFloat((char *)blue.c_str()));
        }

        // Specular reflectivity found
        if ((strMaterialLine[pos] == 'N') && (strMaterialLine[pos + 1] == 's'))
        {
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
            if (pMaterial != NULL) pMaterial->iKn = atoi((char *)specular.c_str());
        }

        // Illumination model found
        if ((strMaterialLine[pos] == 'i') && (strMaterialLine[pos + 1] == 'l') && (strMaterialLine[pos + 2] == 'l') && (strMaterialLine[pos + 3] == 'u') && (strMaterialLine[pos + 4] == 'm'))
        {
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
            if (pMaterial != NULL) pMaterial->iIllum = atoi((char *)illum.c_str());
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

    return (true);
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
void Objects::CloseCurrentMaterial (void)
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
        pMesh->mapped = true;
        pMeshArray[numMeshes] = pMesh;
        numMeshes++;

        // Number of vertices in the sky (cube)
        pMesh->numVertices = 24;

        // Allocate memory for the Sky vertices
        pMesh->pVertices = (VecMat::Vertex*) malloc (pMesh->numVertices * sizeof(VecMat::Vertex));

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
        pMesh->pFaces = (VecMat::Face*) malloc (pMesh->numFaces * sizeof(VecMat::Face));

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
        pMesh->pTextureVertices = (VecMat::Vertex*) malloc (pMesh->numTextureVertices * sizeof(VecMat::Vertex));

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

        // Number of vertex normals in the sky (cube)
        pMesh->numVertexNormals = 24;

        // Allocate memory for the Sky vertex normals
        pMesh->pVertexNormals = (VecMat::Vertex*) malloc (pMesh->numVertexNormals * sizeof(VecMat::Vertex));

        // Fill this memory with 0's
        ZeroMemory(pMesh->pVertexNormals, pMesh->numVertexNormals * sizeof(VecMat::Vertex));

        // Fill vertex normals list
        VecMat::Vertex *ptrVN = pMesh->pVertexNormals;

        // back
        *ptrVN++ = VecMat::Vertex( 0.0f,  0.0f, 1.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  0.0f, 1.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  0.0f, 1.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  0.0f, 1.0f);

        // left
        *ptrVN++ = VecMat::Vertex( 1.0f,  0.0f, 0.0f);
        *ptrVN++ = VecMat::Vertex( 1.0f,  0.0f, 0.0f);
        *ptrVN++ = VecMat::Vertex( 1.0f,  0.0f, 0.0f);
        *ptrVN++ = VecMat::Vertex( 1.0f,  0.0f, 0.0f);

        // right
        *ptrVN++ = VecMat::Vertex( -1.0f,  0.0f, 0.0f);
        *ptrVN++ = VecMat::Vertex( -1.0f,  0.0f, 0.0f);
        *ptrVN++ = VecMat::Vertex( -1.0f,  0.0f, 0.0f);
        *ptrVN++ = VecMat::Vertex( -1.0f,  0.0f, 0.0f);

        // front
        *ptrVN++ = VecMat::Vertex( 0.0f,  0.0f, -1.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  0.0f, -1.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  0.0f, -1.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  0.0f, -1.0f);

        // top
        *ptrVN++ = VecMat::Vertex( 0.0f, -1.0f,  0.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f, -1.0f,  0.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f, -1.0f,  0.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f, -1.0f,  0.0f);

        // bottom
        *ptrVN++ = VecMat::Vertex( 0.0f,  1.0f,  0.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  1.0f,  0.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  1.0f,  0.0f);
        *ptrVN++ = VecMat::Vertex( 0.0f,  1.0f,  0.0f);

        // Create a new material for the backplane
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("SkyBack");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->iIllum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKa = new VecMat::Vec3(2.0f, 2.0f, 2.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKd = new VecMat::Vec3(0.0f, 0.0f, 0.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKs = new VecMat::Vec3(0.0f, 0.0f, 0.0f);

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
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->iIllum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKa = new VecMat::Vec3(2.0f, 2.0f, 2.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKd = new VecMat::Vec3(0.0f, 0.0f, 0.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKs = new VecMat::Vec3(0.0f, 0.0f, 0.0f);

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
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->iIllum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKa = new VecMat::Vec3(2.0f, 2.0f, 2.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKd = new VecMat::Vec3(0.0f, 0.0f, 0.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKs = new VecMat::Vec3(0.0f, 0.0f, 0.0f);

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
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->iIllum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKa = new VecMat::Vec3(2.0f, 2.0f, 2.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKd = new VecMat::Vec3(0.0f, 0.0f, 0.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKs = new VecMat::Vec3(0.0f, 0.0f, 0.0f);

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
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->iIllum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKa = new VecMat::Vec3(2.0f, 2.0f, 2.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKd = new VecMat::Vec3(0.0f, 0.0f, 0.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKs = new VecMat::Vec3(0.0f, 0.0f, 0.0f);

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
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->iIllum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKa = new VecMat::Vec3(2.0f, 2.0f, 2.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKd = new VecMat::Vec3(0.0f, 0.0f, 0.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKs = new VecMat::Vec3(0.0f, 0.0f, 0.0f);

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
* Create Axis
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
* Create a Terrain as a second object
*********************************************************************/
void Objects::CreateTerrain (void)
{
    try
    {
        // Create terrain object and add to the list
        Mesh *pMesh = new Mesh();
        pMesh->name = "Terrain";
        pMesh->mapped = true;
        pMeshArray[numMeshes] = pMesh;
        numMeshes++;

        // Create a new material for the terrain
        pMesh->pMaterialEntryList[pMesh->numMaterials] = new MaterialEntry();
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial = new Material("Terrain");
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->iIllum = 1;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKa = new VecMat::Vec3(1.0f, 1.0f, 1.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKd = new VecMat::Vec3(1.0f, 1.0f, 1.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->pKs = new VecMat::Vec3(1.0f, 1.0f, 1.0f);
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->scaleX = 0.005f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->scaleY = 0.005f;
        pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->scaleZ = 0.005f;

		// Read texture
		std::string textureFileName = applicationFolder + "\\textures\\terrain.bmp";
		pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadTexture(textureFileName.c_str());

		// Read normal texture
		std::string normalFileName = applicationFolder + "\\textures\\terrain_normal.bmp";
		pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial->ReadNormalTexture(normalFileName.c_str());
		
		// Add new Material to array of pointers (Material list)
        pMaterialArray[numMaterials++] = pMesh->pMaterialEntryList[pMesh->numMaterials]->pMaterial;

        // Insert the starting face for this material
        pMesh->pMaterialEntryList[pMesh->numMaterials]->start = 0;

        // Add material to counter
        pMesh->numMaterials++;

        // Number of vertices in the terrain
        pMesh->numVertices = 4;

        // Allocate memory for the Terrain vertices
        pMesh->pVertices = (VecMat::Vertex*) malloc (pMesh->numVertices * sizeof(VecMat::Vertex));

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
        pMesh->pFaces = (VecMat::Face*) malloc (pMesh->numFaces * sizeof(VecMat::Face));

        // Fill this memory with 0's
        ZeroMemory(pMesh->pFaces, pMesh->numFaces * sizeof(VecMat::Face));

        // Fill faces list
        VecMat::Face *ptrF = pMesh->pFaces;

        *ptrF++ = VecMat::Face(0, 1, 2);
        *ptrF++ = VecMat::Face(2, 3, 0);

        // Number of texture vertices in the Terrain
        pMesh->numTextureVertices = 4;

        // Allocate memory for the Terrain texture vertices
        pMesh->pTextureVertices = (VecMat::Vertex*) malloc (pMesh->numTextureVertices * sizeof(VecMat::Vertex));

        // Fill this memory with 0's
        ZeroMemory(pMesh->pTextureVertices, pMesh->numTextureVertices * sizeof(VecMat::Vertex));

        // Fill texture vertices list
        VecMat::Vertex *ptrTV = pMesh->pTextureVertices;
        *ptrTV++ = VecMat::Vertex(0.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 0.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(1.0f, 1.0f, 0.0f);
        *ptrTV++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);

        // Number of vertex normals in the Terrain
        pMesh->numVertexNormals = 4;

        // Allocate memory for the Terrain vertex normals
        pMesh->pVertexNormals = (VecMat::Vertex*) malloc (pMesh->numVertexNormals * sizeof(VecMat::Vertex));

        // Fill this memory with 0's
        ZeroMemory(pMesh->pVertexNormals, pMesh->numVertexNormals * sizeof(VecMat::Vertex));

        // Fill vertex normals list
        VecMat::Vertex *ptrVN = pMesh->pVertexNormals;
        for (unsigned int i=0; i < pMesh->numVertexNormals; i++)
        {
            *ptrVN++ = VecMat::Vertex( 0.0f, 1.0f, 0.0f);
        }
	
		// Number of face normals in the Terrain
		pMesh->numFaceNormals = 2;

		// Allocate memory for the Terrain face normals
		pMesh->pFaceNormals = (VecMat::Vertex*) malloc(pMesh->numFaceNormals * sizeof(VecMat::Vertex));

		// Fill this memory with 0's
		ZeroMemory(pMesh->pFaceNormals, pMesh->numFaceNormals * sizeof(VecMat::Vertex));

		// Fill face normals list
		VecMat::Vertex *ptrFN = pMesh->pFaceNormals;
		for (unsigned int i = 0; i < pMesh->numFaceNormals; i++)
		{
			*ptrFN++ = VecMat::Vertex(0.0f, 1.0f, 0.0f);
		}
	} catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Objects::CreateTerrain", message.c_str());
        return;
    }
}
