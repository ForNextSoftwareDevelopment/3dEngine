#pragma once
#include <windows.h>
#include <string.h>
#include <vector>
#include "Error.h"
#include "VecMat.h"

class Triangulate
{
    private:
        #ifdef DEBUGTRIANGULATE
        // Number of errors during triangulation of an object
        static unsigned int numErr;
        #endif

        // Find the normal vector of this polygon
        static VecMat::Vec3 FindNormal(std::vector<int> &polygon, std::vector<VecMat::Vertex> &vList);

        // Calculate the angle the normal makes with the y- and z-axis (in degrees) and return a rotation matrix for alignment
        static VecMat::Mat4 FindRotation(VecMat::Vec3 normal);

        // IsConvex decides if the middle vertex in a triangle defined by vertices v1, v2 and v3 is 'reflex' or 'convex'
        static bool IsConvex(VecMat::Vertex &v1, VecMat::Vertex &v2, VecMat::Vertex &v3);

        // InsideTriangle decides if a vertex p is inside of the triangle defined by v1, v2 and v3
        static bool InsideTriangle(VecMat::Vertex &v1, VecMat::Vertex &v2, VecMat::Vertex &v3, VecMat::Vertex &p);

    public:

        // Triangulate a polygon, result will be added to the faces list (fList)
        static bool Process(std::vector<int> &polygon, std::vector<VecMat::Vertex> &vList, std::vector<VecMat::Face> &fList);
};
