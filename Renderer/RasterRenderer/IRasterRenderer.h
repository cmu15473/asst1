#ifndef IRASTER_RENDERER_H
#define IRASTER_RENDERER_H

#include "FrameBuffer.h"
#include "CoreLib/VectorMath.h"
#include "VertexBuffer.h"
#include "Tracing.h"
#include "RenderState.h"

//#define ENABLE_TRACING

namespace RasterRenderer
{
    using namespace CoreLib::Basic;

    class IRasterRenderer : public Object
    {
    public:
        virtual void SetFrameBuffer(FrameBuffer * frameBuffer) = 0;
        virtual void Draw(RenderState & state, VertexBufferRef * vertBuffer, IndexBufferRef * indexBuffer, int * constantIndex) = 0;
        virtual void Finish() = 0;
        virtual TraceCollection * GetTraces() = 0;
        virtual void Clear(const VectorMath::Vec4 & clearColor, bool color = true, bool depth = true, bool mask = false) = 0;
    };
    IRasterRenderer * CreateForwardNonTiledRenderer();
    IRasterRenderer * CreateTiledRenderer();
    void DestroyRenderer(IRasterRenderer * renderer);
}

#endif