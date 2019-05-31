#pragma once
#include <cmath>

namespace VecMat
{
    #define PI 3.14159265358979f

    /*********************************************************************
    * The structure for our vertex type
    *********************************************************************/
    typedef struct Vertex
    {
        // The untransformed, 3D position for the vertex
        float x, y, z;

        // Default constructor
        inline Vertex()
        {
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
        }

        // Constructor
        inline Vertex(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        // Add 2 vertices
        inline Vertex operator+ (Vertex arg)
        {
            Vertex result;

            result.x = this->x + arg.x;
            result.y = this->y + arg.y;
            result.z = this->z + arg.z;

            return (result);
        }
    } Vertex;

    /*********************************************************************
    * The structure for our face type (allways a triangle)
    *********************************************************************/
    typedef struct Face
    {
        // First vertex
        unsigned int first;

        // Second vertex
        unsigned int second;

        // Third vertex
        unsigned int third;

        // Default constructor
        inline Face()
        {
            this->first = 0;
            this->second = 0;
            this->third = 0;
        }

        // Constructor
        inline Face(int first, int second, int third)
        {
            this->first = first;
            this->second = second;
            this->third = third;
        }

    } Face;

    /*********************************************************************
    * The structure for our mapping of a texture vertex to a vertex
    *********************************************************************/
    typedef struct Map
    {
        // Vertex
        unsigned int ver;

        // Texture vertex
        unsigned int tex;

    } Map;

    /*********************************************************************
    * Degree to Radian
    *********************************************************************/
    inline float DtoR (float angleInDegrees)
    {
        return  ((2 * PI * angleInDegrees) / 360.0f);
    }

    /*********************************************************************
    * Radian to Degree
    *********************************************************************/
    inline float RtoD (float angleInRadians)
    {
        return  ((360.0f * angleInRadians) / (2 * PI));
    }

    /*********************************************************************
    * Vector with 2 coordinates
    *********************************************************************/
    class Vec2
    {
        private:

        public:

            // Vector row (1 column with 2 rows)
            float row[2];

            // Default constructor
            inline Vec2 ()
            {
                this->row[0] = 0.0f;
                this->row[1] = 0.0f;
            }

            // Constructor
            inline Vec2 (float x, float y)
            {
                this->row[0] = x;
                this->row[1] = y;
            }

            // Subtract 2 vectors
            inline Vec2 operator- (Vec2 arg)
            {
                Vec2 result;

                result.row[0] = this->row[0] - arg.row[0];
                result.row[1] = this->row[1] - arg.row[1];

                return (result);
            }

            // Add 2 vectors
            inline Vec2 operator+ (Vec2 arg)
            {
                Vec2 result;

                result.row[0] = this->row[0] + arg.row[0];
                result.row[1] = this->row[1] + arg.row[1];

                return (result);
            }

            // Return types
            inline operator const float*() const { return &row[0]; }
            inline float& operator[](int n) { return row[n]; }
            inline const float& operator[](int n) const { return row[n]; }
    };

    /*********************************************************************
    * Vector with 3 coordinates
    *********************************************************************/
    class Vec3
    {
        private:

        public:

            // Vector row (1 column with 3 rows)
            float row[3];

            // Default constructor
            inline Vec3 ()
            {
                this->row[0] = 0.0f;
                this->row[1] = 0.0f;
                this->row[2] = 0.0f;
            }

            // Constructor
            inline Vec3 (float x, float y, float z)
            {
                this->row[0] = x;
                this->row[1] = y;
                this->row[2] = z;
           }

            // Subtract 2 vectors
            inline Vec3 operator- (Vec3 arg)
            {
                Vec3 result;

                result.row[0] = this->row[0] - arg.row[0];
                result.row[1] = this->row[1] - arg.row[1];
                result.row[2] = this->row[2] - arg.row[2];

                return (result);
            }

            // Add 2 vectors
            inline Vec3 operator+ (Vec3 arg)
            {
                Vec3 result;

                result.row[0] = this->row[0] + arg.row[0];
                result.row[1] = this->row[1] + arg.row[1];
                result.row[2] = this->row[2] + arg.row[2];

                return (result);
            }

            // Multiply 2 vectors (cross product)
            inline Vec3 operator* (Vec3 arg)
            {
                Vec3 result;

                result.row[0] = (this->row[1] * arg.row[2]) - (this->row[2] * arg.row[1]);
                result.row[1] = (this->row[2] * arg.row[0]) - (this->row[0] * arg.row[2]);
                result.row[2] = (this->row[0] * arg.row[1]) - (this->row[1] * arg.row[0]);

                return (result);
            }

            // Return types
            inline operator const float*() const { return &row[0]; }
            inline float& operator[](int n) { return row[n]; }
            inline const float& operator[](int n) const { return row[n]; }
    };

    /*********************************************************************
    * Normalize a Vec3 vector
    *********************************************************************/
    inline Vec3 Normalize(Vec3 vec3)
    {
        // Normalize this vector
        float normFactor = sqrt(vec3[0] * vec3[0] + vec3[1] * vec3[1] + vec3[2] * vec3[2]);

        if (normFactor != 0.0f)
        {
            vec3[0] = vec3[0] / normFactor;
            vec3[1] = vec3[1] / normFactor;
            vec3[2] = vec3[2] / normFactor;
        }

        return (vec3);
    }

    /*********************************************************************
    * Calculate the dot product of 2 Vec3 vectors
    *********************************************************************/
    inline float Dot(Vec3 vec1, Vec3 vec2)
    {
        // Calculate
        float dot = vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];

        return (dot);
    }

    /*********************************************************************
    * Vector with 3 coordinates and a scalar
    *********************************************************************/
    class Vec4
    {
        private:

        public:

            // Vector row (1 column with 4 rows)
            float row[4];

            // Default constructor
            inline Vec4 ()
            {
                this->row[0] = 0.0f;
                this->row[1] = 0.0f;
                this->row[2] = 0.0f;
                this->row[3] = 0.0f;
            }

            // Constructor
            inline Vec4 (float x, float y, float z, float w)
            {
                this->row[0] = x;
                this->row[1] = y;
                this->row[2] = z;
                this->row[3] = w;
            }

            // Return types
            inline operator const float*() const { return &row[0]; }
            inline float& operator[](int n) { return row[n]; }
            inline const float& operator[](int n) const { return row[n]; }
    };

    /*********************************************************************
    * 3 by 3 matrix
    *********************************************************************/
    class Mat3
    {
        private:

        public:

            Vec3 column[3];

            inline Mat3 ()
            {
                column[0] = Vec3 (0.0f, 0.0f, 0.0f);
                column[1] = Vec3 (0.0f, 0.0f, 0.0f);
                column[2] = Vec3 (0.0f, 0.0f, 0.0f);
            }

            // Multiply this matrix with a vector
            inline Vec3 operator* (Vec3 arg)
            {
                Vec3 result;

                for (int i=0; i<3; i++)
                {
                    result.row[i] += this->column[0].row[i] * arg.row[0];
                    result.row[i] += this->column[1].row[i] * arg.row[1];
                    result.row[i] += this->column[2].row[i] * arg.row[2];
                }

                return (result);
            }

            // Multiply 2 matrices
            inline Mat3 operator* (Mat3 arg)
            {
                Mat3 result;

                for (int i=0; i<3; i++)
                {
                    for (int j=0; j<3; j++)
                    {
                        result.column[j].row[i] += this->column[0].row[i] * arg.column[j].row[0];
                        result.column[j].row[i] += this->column[1].row[i] * arg.column[j].row[1];
                        result.column[j].row[i] += this->column[2].row[i] * arg.column[j].row[2];
                    }
                }

                return (result);
            }

            // Return types
            inline Vec3& operator[](int n) { return column[n]; }
            inline const Vec3& operator[](int n) const { return column[n]; }
            inline operator float*() { return &column[0].row[0]; }
            inline operator const float*() const { return &column[0].row[0]; }
    };

    /*********************************************************************
    * 4 by 4 matrix
    *********************************************************************/
    class Mat4
    {
        private:

        public:

            Vec4 column[4];

            inline Mat4 ()
            {
                column[0] = Vec4 (0.0f, 0.0f, 0.0f, 0.0f);
                column[1] = Vec4 (0.0f, 0.0f, 0.0f, 0.0f);
                column[2] = Vec4 (0.0f, 0.0f, 0.0f, 0.0f);
                column[3] = Vec4 (0.0f, 0.0f, 0.0f, 0.0f);
            }

            // Multiply this matrix with a vector
            inline Vec4 operator* (Vec4 arg)
            {
                Vec4 result;

                for (int i=0; i<4; i++)
                {
                    result.row[i] += this->column[0].row[i] * arg.row[0];
                    result.row[i] += this->column[1].row[i] * arg.row[1];
                    result.row[i] += this->column[2].row[i] * arg.row[2];
                    result.row[i] += this->column[3].row[i] * arg.row[3];
                }

                return (result);
            }

            // Multiply 2 matrices
            inline Mat4 operator* (Mat4 arg)
            {
                Mat4 result;

                for (int i=0; i<4; i++)
                {
                    for (int j=0; j<4; j++)
                    {
                        result.column[j].row[i] += this->column[0].row[i] * arg.column[j].row[0];
                        result.column[j].row[i] += this->column[1].row[i] * arg.column[j].row[1];
                        result.column[j].row[i] += this->column[2].row[i] * arg.column[j].row[2];
                        result.column[j].row[i] += this->column[3].row[i] * arg.column[j].row[3];
                    }
                }

                return (result);
            }

            // Return types
            inline Vec4& operator[](int n) { return column[n]; }
            inline const Vec4& operator[](int n) const { return column[n]; }
            inline operator float*() { return &column[0].row[0]; }
            inline operator const float*() const { return &column[0].row[0]; }
    };

    /*********************************************************************
    * 4 by 4 orthogonal matrix
    *********************************************************************/
    class Orthogonal : public Mat4
    {
        private:

        public:

            inline Orthogonal (float left, float right, float bottom, float top, float n, float f)
            {
                if ((right == left) || (top == bottom) || (n == f) || (n < 0.0) || (f < 0.0)) return;

                column[0].row[0] = 2.0f / (right - left);

                column[1].row[1] = 2.0f / (top - bottom);

                column[2].row[2] = -2.0f / (f - n);

                column[3].row[0] = -((right + left) / (right - left));
                column[3].row[1] = -((top + bottom) / (top - bottom));
                column[3].row[2] = -((f + n) / (f - n));
                column[3].row[3] =  1.0f;
            }
    };

    /*********************************************************************
    * 4 by 4 frustum matrix
    *********************************************************************/
    class Frustum : public Mat4
    {
        private:

        public:

            inline Frustum (float left, float right, float bottom, float top, float n, float f)
            {
                if ((right == left) || (top == bottom) || (n == f) || (n < 0.0) || (f < 0.0)) return;

                column[0].row[0] = (2.0f * n) / (right - left);
                column[1].row[1] = (2.0f * n) / (top - bottom);

                column[2].row[0] = (right + left) / (right - left);
                column[2].row[1] = (top + bottom) / (top - bottom);
                column[2].row[2] = -((f + n) / (f - n));
                column[2].row[3] = -1.0f;

                column[3].row[2] = -((2.0f * f * n) / (f - n));
            }
    };

    /*********************************************************************
    * 4 by 4 perspective matrix
    *********************************************************************/
    class Perspective : public Mat4
    {
        private:

        public:

            inline Perspective (float fov, float aspect, float n, float f)
            {
                float q = 1.0f / tan (DtoR(fov));
                float A = q / aspect;
                float B = (n + f) / (n - f);
                float C = (2.0f * n * f) / (n - f);

                column[0].row[0] = A;
                column[1].row[1] = q;
                column[2].row[2] = B;
                column[2].row[3] = -1.0f;
                column[3].row[2] = C;
            }
    };

    /*********************************************************************
    * Calculate a 'Look At' matrix (camera)
    *********************************************************************/
    class LookAt : public Mat4
    {
        private:

        public:

        inline LookAt (Vec3 eye, Vec3 center, Vec3 up)
        {
            Vec3  f = Normalize(center - eye);
            Vec3  u = Normalize(up);
            Vec3  s = Normalize(f * u);

            // f and u are parallel
            if ((s[0] == 0.0f) && (s[1] == 0.0f) && (s[2] == 0.0f))
            {
                s[0] = 0.0f;
                s[1] = 0.0f;
                s[2] = 1.0f;
            }
            u = s * f;

            column[0].row[0] = s.row[0];
            column[1].row[0] = s.row[1];
            column[2].row[0] = s.row[2];
            column[3].row[0] =-Dot(s, eye);

            column[0].row[1] = u.row[0];
            column[1].row[1] = u.row[1];
            column[2].row[1] = u.row[2];
            column[3].row[1] =-Dot(u, eye);

            column[0].row[2] =-f.row[0];
            column[1].row[2] =-f.row[1];
            column[2].row[2] =-f.row[2];
            column[3].row[2] = Dot(f, eye);

            column[0].row[3] = 0.0f;
            column[1].row[3] = 0.0f;
            column[2].row[3] = 0.0f;
            column[3].row[3] = 1.0f;
        }
    };

    /*********************************************************************
    * 4 by 4 translation matrix
    *********************************************************************/
    class Translate : public Mat4
    {
        private:

        public:

            inline Translate (float x, float y, float z)
            {
                column[0].row[0] = 1.0f;
                column[1].row[1] = 1.0f;
                column[2].row[2] = 1.0f;
                column[3].row[0] = x;
                column[3].row[1] = y;
                column[3].row[2] = z;
                column[3].row[3] = 1.0f;
            }
    };

    /*********************************************************************
    * 4 by 4 rotation matrix
    *********************************************************************/
    class Rotate : public Mat4
    {
        private:

            float rads_x, rads_y, rads_z;
            float A, B, C, D, E, F;

        public:
            inline Rotate (float angle_x, float angle_y, float angle_z)
            {
                rads_x = float(angle_x) * 0.0174532925f;
                rads_y = float(angle_y) * 0.0174532925f;
                rads_z = float(angle_z) * 0.0174532925f;

                A = cos(rads_x);
                B = sin(rads_x);
                C = cos(rads_y);
                D = sin(rads_y);
                E = cos(rads_z);
                F = sin(rads_z);

                column[0].row[0] =  C * E;
                column[0].row[1] =  B * D * E + A * F;
                column[0].row[2] = -A * D * E + B * F;

                column[1].row[0] = -C * F;
                column[1].row[1] = -B * D * F + A * E;
                column[1].row[2] =  A * D * F + B * E;

                column[2].row[0] =  D;
                column[2].row[1] = -B * C;
                column[2].row[2] =  A * C;

                column[3].row[3] = 1.0f;
            }
    };
};


