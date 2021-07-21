#include "Mesh.h"

/*********************************************************************
* Default Constructor
*********************************************************************/
Mesh::Mesh (void)
{
    // Set default name of object
    this->name = "Mesh";

    // Set number of triangulation errors (while importing this object) to 0
    numTriangulationErrors = 0;

    // Set number of vertices, texture vertices, faces and materials
    numVertices        = 0;
    numVertexNormals   = 0;
    numFaceNormals     = 0;
    numTangents        = 0;
    numTextureVertices = 0;
    numFaces           = 0;
    numMaterials       = 0;

    // Set pointers to the vertices and faces
    pVertices         = nullptr;
    pVertexNormals    = nullptr;
    pFaceNormals      = nullptr;
    pTangents         = nullptr;
    pBiTangents       = nullptr;
    pTextureVertices  = nullptr;
    pFaces            = nullptr;

    // Set pointers to the array of material entries for this object
    for (int i=0; i<MAX_NUM_MATERIALS; i++) pMaterialEntryList[i] = nullptr;
}

/*********************************************************************
* Constructor
*********************************************************************/
Mesh::Mesh(std::string name)
{
    // Set default name of object
    this->name = name;

    // Set initial position
    pos_x = 0.0f;
    pos_y = 0.0f;
    pos_z = 0.0f;

    // Set initial angle
    angle_x = 0.0f;
    angle_y = 0.0f;
    angle_z = 0.0f;

    // Set initial center
    center_x = 0.0f;
    center_y = 0.0f;
    center_z = 0.0f;

    // Set initial size
    size_x = 0.0f;
    size_y = 0.0f;
    size_z = 0.0f;

    // Set (angle)speed
    speed_pos = 0.0f;
    speed_angle = 0.0f;

    // Set number of triangulation errors (while importing this object) to 0
    numTriangulationErrors = 0;

    // Set number of vertices, texture vertices, faces and materials
    numVertices        = 0;
    numVertexNormals   = 0;
    numFaceNormals     = 0;
    numTangents        = 0;
    numTextureVertices = 0;
    numFaces           = 0;
    numMaterials       = 0;

    // Set pointers to the vertices and faces
    pVertices         = nullptr;
    pVertexNormals    = nullptr;
    pFaceNormals      = nullptr;
    pTangents         = nullptr;
    pBiTangents       = nullptr;
    pTextureVertices  = nullptr;
    pFaces            = nullptr;

    // Set pointers to the array of material entries for this object
    for (int i=0; i<MAX_NUM_MATERIALS; i++) pMaterialEntryList[i] = nullptr;
}

/*********************************************************************
* Destructor
*********************************************************************/
Mesh::~Mesh (void)
{
    if (pVertices        != nullptr) delete[] (pVertices);
    if (pVertexNormals   != nullptr) delete[] (pVertexNormals);
    if (pFaceNormals     != nullptr) delete[] (pFaceNormals);
    if (pTangents        != nullptr) delete[] (pTangents);
    if (pBiTangents      != nullptr) delete[] (pBiTangents);
    if (pTextureVertices != nullptr) delete[] (pTextureVertices);
    if (pFaces           != nullptr) delete[] (pFaces);

    for (int i=0; i<MAX_NUM_MATERIALS; i++)
    {
        if (pMaterialEntryList[i] != NULL) delete pMaterialEntryList[i];
    }
}

