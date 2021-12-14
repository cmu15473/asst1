#ifndef PROJECTED_TRIANGLE_H
#define PROJECTED_TRIANGLE_H

namespace RasterRenderer
{
    class ProjectedTriangle
    {
    public:

        // fixed-point vertex positions (N.4 format)
        unsigned short X0, Y0;
        unsigned short X1, Y1;
        unsigned short X2, Y2;

        // edge equations
        short A0, B0, A1, B1, A2, B2;

        // depth plane equation
        float fZ0, fDZDX, fDZDY;

        // triangle id and constant buffer id associated with triangle
        unsigned short ConstantId, Id;

        // useful precomputed values
        float InvArea, MinZ;
    };
}

#endif
