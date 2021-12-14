#include "Statistics.h"

namespace RasterRenderer
{
    std::atomic<int> Statistics::PackagesProcessed;
    std::atomic<int> Statistics::PackagesOccluded;
    std::atomic<int> Statistics::PackagesCulled;
    std::atomic<int> Statistics::CullingOverhead;
    std::atomic<int> Statistics::TrianglesProcessed;
    std::atomic<int> Statistics::TrianglesShaded;
    std::atomic<int> Statistics::ShadingCount;
    std::atomic<long long> Statistics::Time_TriangleInput;
    std::atomic<long long> Statistics::Time_Render;
    std::atomic<int> Statistics::TriangleAreaHistogram[10];
}