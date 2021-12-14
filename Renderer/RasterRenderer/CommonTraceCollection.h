#ifndef COMMON_TRACE_COLLECTION_H
#define COMMON_TRACE_COLLECTION_H

#include "Tracing.h"
#include "Fragment.h"
#include "Parallel.h"

namespace RasterRenderer
{
    class CommonTraceCollection : public TraceCollection
    {
    public:
        static const int TileSizeLog2 = 4;
        static const int TileSize = 1<<TileSizeLog2;
        static const int TileMask = ~(TileSize-1);
        bool traceActivated[4];

        void UpdatePerFragment(int triangleId, int x, int y, FragmentCoverageMask & mask, int sampleCount)
        {

        }
        void Update(int triangleId, int sampleId, int x, int y, FragmentCoverageMask & mask, int sampleCount)
        {

        }

        virtual void Init(int width, int height)
        {

        }

        virtual void ActivateTrace(int id)
        {

        }

        virtual void DeactivateTrace(int id)
        {

        }

        virtual void Clear()
        {

        }

        virtual int GetTraceCount()
        {
            return 0;
        }

        virtual const wchar_t * GetTraceName(int id)
        {
            switch (id)
            {
            case 0:
                return L"Fragments per Pixel";
            case 1:
                return L"Triangles per Pixel";
            case 2:
                return L"Peak Triangles per Pixel";
            case 3:
                return L"Peak Triangles per Tile";
            default:
                return L"";
            }
        }

        virtual Vec4 * GetFrameTraceImage(int id, int valMin, Vec4 colorMin, int valMid, Vec4 colorMid, int valMax, Vec4 colorMax)
        {
                return 0;
        }

        virtual void GetFrameTraceHistogram(int id, Histogram & result, int & valMin, int & valMax, int totalGroups)
        {

        }

        // fill tiles with the first pixel in each tile.
        List<Vec4> fullImage;
        Vec4 * UpsampleTileImage(Vec4 * input, int w, int h, int tileSizeLog2)
        {
            fullImage.SetSize(w*h);
            int tileSize = 1<<tileSizeLog2;
            int maxTileX = w>>tileSizeLog2;
            int maxTileY = h>>tileSizeLog2;
            if ((maxTileX<<tileSizeLog2) < w)
                maxTileX++;
            if ((maxTileY<<tileSizeLog2) < h)
                maxTileY++;
            int stride = (w>>tileSizeLog2) + 1;
            Parallel::For (0, maxTileY, [&](int i)
            {
                for (int j = 0; j<=maxTileX; j++)
                {
                    int tileX = j << tileSizeLog2;
                    int tileY = i << tileSizeLog2;
                    Vec4 color = input[i*stride + j];
                    for (int y = tileY; y<Math::Min(tileY+tileSize, h); y++)
                        for (int x = tileX; x<Math::Min(tileX+tileSize, w); x++)
                            fullImage[y*w+x] = color;
                }
            });
            return fullImage.Buffer();
        }
    };
}

#endif
