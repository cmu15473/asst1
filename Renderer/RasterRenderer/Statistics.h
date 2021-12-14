#ifndef RASTER_RENDERER_STATISTICS
#define RASTER_RENDERER_STATISTICS

#include <atomic>
#include "CoreLib/PerformanceCounter.h"

namespace RasterRenderer
{
//#define ENABLE_INSTRUMENTATION

    class Statistics
    {
    public:
        static std::atomic<int> ShadingCount;
        static std::atomic<long long> Time_TriangleInput, Time_Render;
        static std::atomic<int> PackagesProcessed, PackagesCulled, PackagesOccluded;
        static std::atomic<int> CullingOverhead, TrianglesProcessed, TrianglesShaded;
        static std::atomic<int> TriangleAreaHistogram[10];
        static void Clear()
        {
            PackagesProcessed.store(0);
            PackagesCulled.store(0);
            CullingOverhead.store(0);
            TrianglesProcessed.store(0);
            TrianglesShaded.store(0);
            PackagesOccluded.store(0);
            ShadingCount.store(0);
            Time_TriangleInput.store(0);
            Time_Render.store(0);
            for (int i = 0; i<10; i++)
                TriangleAreaHistogram[i].store(0);
        }
    };
};

#endif