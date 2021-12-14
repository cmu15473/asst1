#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "CoreLib/Basic.h"
#include "CoreLib/VectorMath.h"
#include "CoreLib/Threading.h"
#include "Parallel.h"
#include <memory.h>

namespace RasterRenderer
{
    using namespace CoreLib::Basic;
    using namespace VectorMath;

    class FrameBitMask
    {
    private:
        int Width, Height; // in 8x4 tiles
        List<unsigned int> mask;
    public:
        FrameBitMask()
        {
            Width = Height = 0;
        }
        void SetSize(int width, int height)
        {
            Width = ((width & 0x7) != 0) ? (width>>3) + 1: width>>3;
            Height = ((height & 0x3) != 0) ? (height>>2) + 1: height>>2;
            mask.SetSize(Width * Height);
        }
        inline void Clear()
        {
            for (int i = 0; i<mask.Count(); i++)
                mask[i] = 0;
        }
        inline void Set(int x, int y)
        {
            int bx = x>>3;
            int by = y>>2;
            int id = 1<<(((x&7)<<2) + (y&3));
            mask[by*Width+bx] |= id;
        }
        inline bool TestAnyZero(int x, int y, int x1, int y1)
        {
            int bx = x>>3;
            int by = y>>2;
            int bx1 = x1>>3;
            int by1 = y1>>2;
            for (int x = bx; x<=bx1; x++)
                for (int y = by; y<=by1; y++)
                    if (mask[y*Width+x] != 0xFFFFFFFFu)
                        return true;
            return false;
        }
    };

    class FrameBuffer
    {
    private:
        List<Vec4> pixels;
        List<Vec4> downSampledPixels;
        List<float> zBuffer;
        FrameBitMask mask;
        int width, height;
        int sampleCountLog2;
        int sampleCount;
    public:
        static const int MultiSampleOffsets[][64];
        FrameBuffer()
        {
            width = height = 0;
        }
        FrameBuffer(int width, int height, int sampleCountLog2=0)
        {
            SetSize(width, height, sampleCountLog2);
        }
        void SetSize(int width, int height, int sampleCountLog2)
        {
            this->sampleCountLog2 = sampleCountLog2;
            this->sampleCount = 1<<sampleCountLog2;
            pixels.SetSize(width*height*sampleCount);
            zBuffer.SetSize(width*height*sampleCount);
            mask.SetSize(width, height);
            if (sampleCount > 1)
                downSampledPixels.SetSize(width*height);
            this->width = width;
            this->height = height;
        }
        void Clear(Vec4 clearColor, bool color, bool depth)
        {
            if (color && depth)
            {
                for (int i = 0; i<pixels.Count(); i++)
                {
                    pixels[i] = clearColor;
                    zBuffer[i] = 1.0f;
                }
            }
            else if (color)
            {
                for (int i = 0; i<pixels.Count(); i++)
                {
                    pixels[i] = clearColor;
                }
            }
            else if (depth)
            {
                for (int i = 0; i<pixels.Count(); i++)
                {
                    zBuffer[i] = 1.0f;
                }
            }
        }
        inline void ClearMask()
        {
            mask.Clear();
        }
        inline float GetMaxZ()
        {
            float maxZ = -1.0f;
            for (int i = 0; i<zBuffer.Count(); i++)
                if (zBuffer[i] > maxZ)
                    maxZ = zBuffer[i];
            return maxZ;
        }
        inline float GetZ(int x, int y, int sampleId)
        {
            return zBuffer[((y*width+x)<<sampleCountLog2) + sampleId];
        }
        inline void SetZ(int x, int y, int sampleId, float z)
        {
            zBuffer[((y*width+x)<<sampleCountLog2) + sampleId] = z;
        }
        inline void SetPixel(int x, int y, int sampleId, const Vec4 & color)
        {
            pixels[((y*width+x)<<sampleCountLog2) + sampleId] = color;
        }
        inline void SetMask(int x, int y)
        {
            mask.Set(x, y);
        }
        inline bool TestAnyZero(int x, int y, int x1, int y1)
        {
            return mask.TestAnyZero(x, y, x1, y1);
        }

        inline void BlendPixel(int x, int y, int sampleId, const Vec4 & color)
        {
            // TODO:
            // Implement color blending here.
            // Samples are stored at:
            //    pixels[((y*width+x)<<sampleCountLog2) + sampleId]
        }
        inline Vec4 * GetDownsampledColorBuffer()
        {
            return downSampledPixels.Buffer();
        }
        inline Vec4 * GetColorBuffer()
        {
            if (sampleCount == 1)
                return pixels.Buffer();
            else
            {
                float div = 1.0f/(float)sampleCount;
                Parallel::For(0, height, [&](int y)
                {
                    for (int x = 0; x<width; x++)
                    {
                        int loc = y*width*sampleCount + x*sampleCount;
                        Vec4 rs = pixels[loc];
                        for (int i = 1; i<sampleCount; i++)
                        {
                            rs += pixels[loc+i];
                        }
                        rs *= div;
                        downSampledPixels[y*width+x] = rs;
                    }
                });
                return downSampledPixels.Buffer();
            }
        }
        inline Vec4 & GetPixel(int x, int y, int sampleId)
        {
            int loc = ((y*width+x)<<sampleCountLog2) + sampleId;
            return pixels[loc];
        }
        int GetWidth()
        {
            return width;
        }
        int GetHeight()
        {
            return height;
        }
        int GetSampleCount()
        {
            return sampleCount;
        }
        int GetSampleCountLog2()
        {
            return sampleCountLog2;
        }
        void SaveColorBuffer(String fileName);
    };


}

#endif
