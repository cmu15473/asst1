#include "IRasterRenderer.h"

namespace RasterRenderer
{
    void DestroyRenderer(IRasterRenderer * renderer)
    {
        delete renderer;
    }
}