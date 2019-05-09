#include "Triangulate.h"

#ifdef DEBUGTRIANGULATE
// Number of errors during triangulation of an object
unsigned int Triangulate::numErr = 0;
#endif

/************************************************************************************
* Triangulate a polygon, vertices are being copied (so we can change them in this
* routine), faces are transferred by reference and new ones added
*************************************************************************************/
bool Triangulate::Process(std::vector<int> &poly, std::vector<VecMat::Vertex> &vList, std::vector<VecMat::Face> &fList)
{
    try
    {
        #ifdef DEBUGTRIANGULATE
        std::string message = "";
        message = "original poly = \r\n";
        for (unsigned int i=0; i < poly.size(); i++)
        {
            if (poly[i]<10000) message.append(" ");
            if (poly[i]<1000)  message.append(" ");
            if (poly[i]<100)   message.append(" ");
            if (poly[i]<10)    message.append(" ");

            message.append(Utils::IntToStr(poly[i]));
            message.append(" (");
            message.append(Utils::FloatToStr(vList[poly[i]].x));
            message.append(", ");
            message.append(Utils::FloatToStr(vList[poly[i]].y));
            message.append(", ");
            message.append(Utils::FloatToStr(vList[poly[i]].z));
            message.append(")\r\n");
        }
        Error::ShowInfoWindow(message);
        #endif

        // Find the normal for this poly
        VecMat::Vec3 normal;
        normal = Triangulate::FindNormal(poly, vList);

        // If not found, return
        if ((normal[0] == 0.0f) && (normal[1] == 0.0f) && (normal[2] == 0.0f))
        {
            #ifdef DEBUGTRIANGULATE
            Error::ShowInfoWindow("Normal not found");
            Error::ShowInfoWindow("--------------------------------------------------------------------------------\r\n");
            #endif

            return false;
        }

        // Calculate the angle the normal makes with the y- and z-axis (in degrees) and return a rotation matrix for alignment
        VecMat::Mat4 rotationMatrix = Triangulate::FindRotation (normal);

        // Rotate all vertices from this poly and put them in a new list (all other vertices become dummies in the list to retain the proper indexing)
        std::vector<VecMat::Vertex> vListRotated;
        for (unsigned int i=0; i < vList.size(); i++)
        {
            VecMat::Vertex vertex;
            vertex.x = 0.0f;
            vertex.y = 0.0f;
            vertex.z = 0.0f;

            // Check if this vertex is part of the poly
            bool found = false;
            for (unsigned int j=0; j < poly.size(); j++)
            {
                if (i == (unsigned int) poly[j]) found = true;
            }

            if (found)
            {
                // Retreive the coordinates for this vertex and put them in a vector
                VecMat::Vec4 vec4;
                vec4[0] = vList[i].x;
                vec4[1] = vList[i].y;
                vec4[2] = vList[i].z;
                vec4[3] = 1;

                // Rotate this vector
                VecMat::Vec4 vec4r = rotationMatrix * vec4;

                // Put coordinates in a new vertex
                vertex.x = vec4r[0];
                vertex.y = vec4r[1];
                vertex.z = vec4r[2];
            }

            // Put the rotated vertex in a list
            vListRotated.push_back(vertex);
        }

        float temp = 0.0f;
        bool clockwise = false;

        // We want a counter-clockwise poly
        std::vector<int> ccPoly;
        int n = (int)poly.size();

        // Check if the poly is clockwise or counter-clockwise
        for(int i=n-1, j=0; j<n; i=j++)
        {
            temp += vListRotated[poly[i]].x * vListRotated[poly[j]].y - vListRotated[poly[j]].x * vListRotated[poly[i]].y;
        }

        if (temp >= 0.0f)
        {
            // Counter clockwise, so just copy indices
            for (int i=0; i < n; i++) ccPoly.push_back(poly[i]);
            clockwise = false;
        } else
        {
            // Clockwise , so copy in reverse order
            for (int i=n-1; i >= 0; i--) ccPoly.push_back(poly[i]);
            clockwise = true;
        }

        // Take three vertices to form a triangle
        unsigned int v1, v2, v3;

        // Triangulate: find a triangle in the poly (while it's still a poly and not becoming a quad) to add to the faces list
        bool found = true;
        while (found && ccPoly.size() > 3)
        {
            #ifdef DEBUGTRIANGULATE
            message = "poly working on = ";
            for (unsigned int i=0; i < ccPoly.size(); i++)
            {
                message.append(Utils::IntToStr(ccPoly[i]));
                message.append(" ");
            }
            message.append(":\r\n");
            Error::ShowInfoWindow(message);
            #endif

            found = false;
            for (unsigned int i=0; !found && i < ccPoly.size(); i++)
            {
                v1 = i;
                if (i == (ccPoly.size() - 1)) // If first vertex of the triangle is the last vertex of the poly
                {
                    v2 = 0;
                    v3 = 1;
                } else
                if (i == (ccPoly.size() - 2)) // If first vertex of the triangle is one before the last vertex of the poly
                {
                    v2 = i + 1;
                    v3 = 0;
                } else
                {
                    v2 = i + 1;
                    v3 = i + 2;
                }

                #ifdef DEBUGTRIANGULATE
                std::string message = "Triangle ";
                message.append(Utils::IntToStr(i));
                message.append(": ");
                message.append(Utils::IntToStr(ccPoly[v1]));
                message.append("-");
                message.append(Utils::IntToStr(ccPoly[v2]));
                message.append("-");
                message.append(Utils::IntToStr(ccPoly[v3]));

                message.append("\r\n");
                message.append("(");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v1]].x));
                message.append(", ");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v1]].y));
                message.append(", ");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v1]].z));
                message.append(")\r\n(");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v2]].x));
                message.append(", ");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v2]].y));
                message.append(", ");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v2]].z));
                message.append(")\r\n(");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v3]].x));
                message.append(", ");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v3]].y));
                message.append(", ");
                message.append(Utils::FloatToStr(vListRotated[ccPoly[v3]].z));
                message.append(")\r\n");
                #endif

                // Check if the middle vertex is convex
                if (Triangulate::IsConvex(vListRotated[ccPoly[v1]], vListRotated[ccPoly[v2]], vListRotated[ccPoly[v3]]))
                {
                    #ifdef DEBUGTRIANGULATE
                    message.append("ISCONVEX\r\n");
                    #endif

                    // Check for all remaining vertices if this one is in the triangle
                    bool inside = false;
                    for (unsigned int j=0; !inside && j < ccPoly.size(); j++)
                    {
                        if ((j != v1) && (j!=v2) && (j!=v3))
                        {
                            inside = Triangulate::InsideTriangle(vListRotated[ccPoly[v1]], vListRotated[ccPoly[v2]], vListRotated[ccPoly[v3]], vListRotated[ccPoly[j]]);
                        }
                    }

                    // Triangle found to cut (if no vertices of the poly are inside this triangle)
                    if (!inside)
                    {
                        found = true;

                        // Create face
                        VecMat::Face face;

                        // Set all coordinates (if clockwise then reverse again so the normal will be pointing the original way)
                        if (clockwise)
                        {
                            face.first  = ccPoly[v3];
                            face.second = ccPoly[v2];
                            face.third  = ccPoly[v1];
                        } else
                        {
                            face.first  = ccPoly[v1];
                            face.second = ccPoly[v2];
                            face.third  = ccPoly[v3];
                        }

                        // Put the new face in the faces list
                        fList.push_back (face);

                        // Remove middle vertex of the triangle from the poly
                        ccPoly.erase(ccPoly.begin() + v2);
                    } else
                    {
                        #ifdef DEBUGTRIANGULATE
                        message.append("ISINSIDE\r\n");
                        #endif
                    }
                } else
                {
                    #ifdef DEBUGTRIANGULATE
                    message.append("ISNOTCONVEX\r\n");
                    #endif
                }

                #ifdef DEBUGTRIANGULATE
                Error::ShowInfoWindow(message);
                #endif
            }
        }

        #ifdef DEBUGTRIANGULATE
        Error::ShowInfoWindow("--------------------------------------------------------------------------------\r\n");
        #endif

        // Last triangle
        if (ccPoly.size() == 3)
        {
            VecMat::Face face;

            // Create last face
            if (clockwise)
            {
                face.first  = ccPoly[2];
                face.second = ccPoly[1];
                face.third  = ccPoly[0];
            } else
            {
                face.first  = ccPoly[0];
                face.second = ccPoly[1];
                face.third  = ccPoly[2];
            }

            // Put the new face in the faces list
            fList.push_back (face);
        } else
        {
            #ifdef DEBUGTRIANGULATE
            Triangulate::numErr++;
            if (Triangulate::numErr < 100)
            {
                std::string s;
                s.append("number of vertices: ");
                s.append(Utils::IntToStr((int)ccPoly.size()));
                s.append("\r\n\r\n");

                for (unsigned int i=0; i<ccPoly.size(); i++)
                {
                    s.append("vertex ");
                    s.append(Utils::IntToStr(ccPoly[i]));
                    s.append(": x = " + Utils::FloatToStr(vList[ccPoly[i]].x));
                    s.append( " y = " + Utils::FloatToStr(vList[ccPoly[i]].y));
                    s.append( " z = " + Utils::FloatToStr(vList[ccPoly[i]].z) + "\r\n");
                }

                Error::WriteLog("WARNING", "Triangulate::Process", s.c_str());
            }

            if (Triangulate::numErr == 100)
            {
                Error::WriteLog("WARNING", "Triangulate::Process", "Too many triangulation errors: quit logging.");
            }
            #endif

            return (false);
        }

        return (true);
    }
    catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Triangulate::Process", message.c_str());
        return (false);
    }
}

/*********************************************************************
* Find the normal vector of this poly
*********************************************************************/
VecMat::Vec3 Triangulate::FindNormal(std::vector<int> &poly, std::vector<VecMat::Vertex> &vList)
{
    VecMat::Vec3 normal;
    normal[0] = 0.0f;
    normal[1] = 0.0f;
    normal[2] = 0.0f;

    bool result =false;
    unsigned int i = 1;

    try
    {
        // While not found try to find a normal of this poly
        while (!result && (i < (poly.size() - 1)))
        {
            // Calculate first vector
            VecMat::Vec3 first;
            first[0] = vList[poly[i + 1]].x - vList[poly[i]].x;
            first[1] = vList[poly[i + 1]].y - vList[poly[i]].y;
            first[2] = vList[poly[i + 1]].z - vList[poly[i]].z;

            // Calculate second vector
            VecMat::Vec3 second;
            second[0] = vList[poly[i - 1]].x - vList[poly[i]].x;
            second[1] = vList[poly[i - 1]].y - vList[poly[i]].y;
            second[2] = vList[poly[i - 1]].z - vList[poly[i]].z;

            // Calculate cross product of these two vectors (= normal)
            normal = first * second;

            // Normalize this vector
            float normFactor = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
            if (normFactor != 0.0f)
            {
                normal[0] = normal[0] / normFactor;
                normal[1] = normal[1] / normFactor;
                normal[2] = normal[2] / normFactor;

                // Check if normal has been found
                if ((normal[0] != 0.0f) || (normal[1] != 0.0f) || (normal[2] != 0.0f)) result = true;
            }

            i++;
        }

        // Not found, return 'NULL' vector
        if (!result)
        {
            normal[0] = 0.0f;
            normal[1] = 0.0f;
            normal[2] = 0.0f;
        }

        return (normal);
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Triangulate::FindNormal", message.c_str());
        return (normal);
    }
}

/*********************************************************************
* Calculate the angle the normal makes with the y- and
* z-axis (in degrees) and return a rotation matrix for alignment
*********************************************************************/
VecMat::Mat4 Triangulate::FindRotation(VecMat::Vec3 normal)
{
    float r1     = 0.0f;
    float r2     = 0.0f;
    float arg    = 0.0f;
    float angleZ = 0.0f;
    float angleY = 0.0f;
    VecMat::Mat4 rotationMatrix;

    try
    {
        // Find the angle of the projected normal on the xz-plane with the z-axis
        r1 = sqrt((normal[0] * normal[0]) + (normal[2] * normal[2]));
        if (r1 != 0.0f)
        {
            arg = normal[0] / r1;
            if (arg >  1.0f) arg =  1.0f;
            if (arg < -1.0f) arg = -1.0f;
            angleZ = VecMat::RtoD(asin(arg));
        }

        // Find the angle of the normal with the y-axis
        r2 = sqrt((r1 * r1) + (normal[1] * normal[1]));
        if (r2 != 0.0f)
        {
            arg = normal[1] / r2;
            if (arg >  1.0f) arg =  1.0f;
            if (arg < -1.0f) arg = -1.0f;
            angleY = VecMat::RtoD(asin(arg));
        }

        // Calculate the rotation matrices to be executed on the poly to align it
        // Rotate around the x-axis and y-axis so all z-coordinates line up (become the same)
        rotationMatrix = VecMat::Rotate(angleY, -angleZ , 0.0f);

        return (rotationMatrix);
    } catch (std::exception& e)
    {
        std::string message = "Exception: ";
        message.append(e.what());
        Error::WriteLog("EXCEPTION", "Triangulate::FindRotation", message.c_str());
        return (rotationMatrix);
    }
}

/*********************************************************************
* IsConvex decides if the middle vertex in a triangle defined by
* vertices v1, v2 and v3 is 'reflex' or 'convex'
*********************************************************************/
bool Triangulate::IsConvex(VecMat::Vertex &v1, VecMat::Vertex &v2, VecMat::Vertex &v3)
{
    float temp = ((v2.x - v1.x) * (v3.y - v1.y)) - ((v2.y - v1.y) * (v3.x - v1.x));
    if(temp > 0.0f) return true;
    else return false;
}

/*********************************************************************
* InsideTriangle decides if a vertex p is inside of the triangle
* defined by vertices v1, v2 and v3
*********************************************************************/
bool Triangulate::InsideTriangle(VecMat::Vertex &v1, VecMat::Vertex &v2, VecMat::Vertex &v3, VecMat::Vertex &p)
{
    if(Triangulate::IsConvex(v1, p, v2)) return false;
    if(Triangulate::IsConvex(v2, p, v3)) return false;
    if(Triangulate::IsConvex(v3, p, v1)) return false;

    return true;
}
