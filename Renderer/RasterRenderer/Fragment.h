#ifndef RASTER_RENDERER_FRAGMENT_H
#define RASTER_RENDERER_FRAGMENT_H

#include "CoreLib/Allocator.h"

namespace RasterRenderer
{
    class FragmentCoverageMask
    {
    public:
        int Bits[4]; // For up to 32x MSAA coverage mask
        inline FragmentCoverageMask()
        {
            Bits[0] = 0; Bits[1] = 0; Bits[2] = 0; Bits[3] = 0;
        }
        inline void SetBit(int i)
        {
            int id = i>>5;
            int shift = i&31;
            Bits[id] |= (1<<shift);
        }
        inline int GetBit(int i)
        {
            int id = i>>5;
            int shift = i&31;
            return Bits[id] & (1<<shift);
        }
        inline int Merge()
        {
            return Bits[0]|Bits[1]|Bits[2]|Bits[3];
        }
        inline bool Any()
        {
            return Bits[0]||Bits[1]||Bits[2]||Bits[3];
        }
    };

    // represents a 2x2 quad-pixel fragment
    class Fragment
    {
    public:
        __m128 gamma, beta;
        float ShadeResult[16];
        unsigned short x, y;
        int ConstId;
        // index of first vertex in indexOutputBuffer,
        // the triangle is defined by 
        // vertexOutputBuffer[indexOutputBuffer[TriangleId*3/+1/+2]*vertexOutputSize] 
        int CoreId:8;
        int TriangleId:24;
        FragmentCoverageMask BitMask; 
        Fragment()
        {}
        Fragment(int x, int y, __m128 gamma, __m128 beta, int coreId, int triId, int constId, FragmentCoverageMask & mask)
        {
            Set(x, y, gamma, beta, coreId, triId, constId, mask);
        }
        inline void Set(int x, int y, __m128 gamma, __m128 beta, int coreId, int triId, int constId, FragmentCoverageMask & mask)
        {
            this->x = x; 
            this->y = y;
            this->gamma = gamma;
            this->beta = beta;
            this->CoreId = coreId;
            this->TriangleId = triId;
            this->ConstId = constId;
            this->BitMask = mask;
        }
    };
}

#endif
