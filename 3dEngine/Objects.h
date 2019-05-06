#pragma once
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Mesh.h"
#include "Error.h"
#include "Triangulate.h"
#include "Meshes/Axis.h"
#include "Meshes/Cone.h"
#include "Meshes/Cube.h"
#include "Meshes/Dome.h"
#include "Meshes/Sphere.h"
#include "Meshes/Tube.h"

class Objects
{
    private:

        // Application folder
        std::string applicationFolder;

        // Current mesh read
        Mesh *pMesh;

        // Current material read
        Material *pMaterial;

        // Position of the current mesh in scene
        float posX, posY, posZ;

        // Offset for the vertex indices (in the file the vertices will be counted as one mesh)
        unsigned int offsetV;

        // Offset for the texture vertex indices (in the file the texture vertices will be counted as one mesh)
        unsigned int offsetT;

        // List for vertices
        std::vector<VecMat::Vertex> vList;

        // List for faces
        std::vector<VecMat::Face> fList;

        // List for texture vertices
        std::vector<VecMat::Vertex> vtList;

        // List for vertices mapped to texture vertices
        std::vector<VecMat::Map> mList;

        // List for vertex normals
        std::vector<VecMat::Vertex> vnList;

        // Create a sky as a first object
        void CreateSky (void);

		// Create axis 
		void CreateAxis(void);
			
		// Create terrain
        void CreateTerrain (void);

		// Decode a line from the object file
		bool DecodeObjectLine(std::string strObjectLine);

		// Decode a line from the material file
		bool DecodeMaterialLine(std::string strMaterialLine);
			
		// Create a new (current) mesh
        bool CreateCurrentMesh (std::string name);

        // Close current mesh
        void CloseCurrentMesh (void);

        // Read materials from a material file
        bool ReadMaterialFile (std::string name);

        // Create a new (current) material
        bool CreateCurrentMaterial (std::string name);

        // Close current material
        void CloseCurrentMaterial (void);

    public:

        // Number of meshes in use
        unsigned int numMeshes;

        // Number of materials in use
        unsigned int numMaterials;

        // Array of pointers to meshes
        Mesh *pMeshArray[MAX_NUM_MESHES];

        // Array of pointers to materials
        Material *pMaterialArray[MAX_NUM_MATERIALS];

        // Constructor
        Objects (char *pApplicationFolder);

        // Destructor
        ~Objects (void);

        // Save meshes (triangulated)
        void SaveMeshes (void);
};

