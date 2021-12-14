#ifndef RASTER_RENDERER_TRACING_H
#define RASTER_RENDERER_TRACING_H

#include "CoreLib/Basic.h"
#include "CoreLib/VectorMath.h"
#include "Parallel.h"

using namespace CoreLib::Basic;
using namespace VectorMath;

namespace RasterRenderer
{
    template <typename T>
    class FrameStorage
    {
    protected:
        List<T> buffer;
        int width, height;
    public:
        FrameStorage()
        {
            width = 0;
            height = 0;
        }

        inline int GetWidth()
        {
            return width;
        }
        
        inline int GetHeight()
        {
            return height;
        }

        void SetSize(int width, int height)
        {
            this->width = width;
            this->height = height;
            buffer.SetSize(width*height);
        }

        void Set(int x, int y, const T & val)
        {
            buffer[y*width + x] = val;
        }

        T & Get(int x, int y)
        {
            return buffer[y*width + x];
        }
    };

    class Histogram
    {
    public:
        Array<volatile long, 64> Values;
        Array<int, 64> Groups;
    };

    class FixedIntSet
    {
    public:
        static const int Size = 32;
        int Values[Size];
        inline int Count()
        {
            int count = 0;
            for (int i = 0; i<Size; i++)
            {
                if (Values[i] == -1)
                    continue;
                bool found = false;
                for (int j = 0; j<i; j++)
                {
                    if (Values[j]==Values[i])
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    count++;
            }
            return count;
        }
    };

    class TraceCollection : public Object
    {
    public:
        virtual void Init(int width, int height) = 0;
        virtual void Clear() = 0;
        virtual int GetTraceCount() = 0;
        virtual void ActivateTrace(int id) = 0;
        virtual void DeactivateTrace(int id) = 0;
        virtual const wchar_t * GetTraceName(int id) = 0;
        virtual Vec4 * GetFrameTraceImage(int id, int valMin, Vec4 colorMin, int valMid, Vec4 colorMid, int valMax, Vec4 colorMax) = 0;
        virtual void GetFrameTraceHistogram(int id, Histogram & result, int & valMin, int & valMax, int totalGroups) = 0;
    };
}

#endif
