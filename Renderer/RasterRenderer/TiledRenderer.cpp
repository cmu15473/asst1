#include "RendererImplBase.h"
#include "CommonTraceCollection.h"
#include <algorithm>
#include <immintrin.h>

using namespace std;

namespace RasterRenderer
{
    class TiledRendererAlgorithm
    {
    private:

        // starter code uses 32x32 tiles, feel free to change
        static const int Log2TileSize = 5;
        static const int TileSize = 1 << Log2TileSize;

        // HINT: 
        // a compact representation of a frame buffer tile
        // Use if you wish (optional).  
        // (What other data-structures might be necessary?)
        vector<FrameBuffer> frameBufferTiles;

        // render target is grid of tiles: see SetFrameBuffer
        int gridWidth, gridHeight;
        FrameBuffer * frameBuffer;
    public:
        inline void Init()
        {

        }

        inline void Clear(const Vec4 & clearColor, bool color, bool depth)
        {
            for (auto & fb : frameBufferTiles)
                fb.Clear(clearColor, color, depth);
        }

        inline void SetFrameBuffer(FrameBuffer * frameBuffer)
        {
            this->frameBuffer = frameBuffer;

            // compute number of necessary bins
            gridWidth = frameBuffer->GetWidth() >> Log2TileSize;
            gridHeight = frameBuffer->GetHeight() >> Log2TileSize;
            if (frameBuffer->GetWidth() & (TileSize - 1))
                gridWidth++;
            if (frameBuffer->GetHeight() & (TileSize - 1))
                gridHeight++;
            
            for(int y = 0; y < frameBuffer->GetHeight(); y++) {
                for(int x = 0; x < frameBuffer->GetWidth(); x++) {
                    frameBuffer->SetZ(x,y,0,FLT_MAX);
                }
            }

            // TODO: 
            // Implementations may want to allocation/initialize
            // any necessary data-structures here. You may wish to
            // consult the HINT at the top of the file.
        }

        inline void Finish()
        {
            // TODO: 
            // Finish() is called at the end of the frame. If it hasn't done so, your
            // implementation should flush local per-tile framebuffer contents to the 
            // global frame buffer (this->frameBuffer) here.
        }

        inline void BinTriangles(RenderState & state, ProjectedTriangleInput & input, int vertexOutputSize, int threadId)
        {

            auto & triangles = input.triangleBuffer[threadId];

            for (int i = 0; i < triangles.Count(); i++)
            {
                auto & tri = triangles[i];
                
                // TODO: 
                // Process triangles into bins here.
                // Keep in mind BinTriangles is called once per worker thread.
                // Each thread grabs the list of triangles is it responsible
                // for, and should bin them here.
            }
        }

        inline void ProcessBin(RenderState & state, ProjectedTriangleInput & input, int vertexOutputSize, int tileId)
        {
            // TODO: 
            // This thread should process the bin of triangles corresponding to
            // render target tile 'tileId'. You should take heavy inspiration from
            // 'RenderProjectedBatch' in 'NontiledForwardRenderer.cpp'

            // Keep in mind that in a tiled renderer, rasterization and fragment processing
            // is parallelized across tiles, not fragments within a tile.  Processing within a tile
            // is carried out sequentially. Thus your implementation will likely call ShadeFragment()
            // to shade a single quad fragment at a time, rather than call ShadeFragments()
            // as was done on the full fragment buffer in the non-tiled reference
            // implementation. (both functions are defined in RendererImplBase.h).
            //
            // Some example code is given below:
            //
            // let 'tri' be a triangle
            // let 'triangleId' be its position in the original input list
            // let 'triSIMD' be the triangle info loaded into SIMD registers (see reference impl)

            // get triangle barycentric coordinates:
            //     __m128 gamma, beta, alpha;
            //     triSIMD.GetCoordinates(gamma, alpha, beta, coordX_center, coordY_center);
            //     auto z = triSIMD.GetZ(coordX_center, coordY_center);
            //
            // call ShadeFragment to shade a single quad fragment, storing the output colors (4 float4's) in shadeResult:
            //     CORE_LIB_ALIGN_16(float shadeResult[16]);
            //     ShadeFragment(state, shadeResult, beta, gamma, alpha, triangleId, tri.ConstantId, VERTEX_BUFFER_FOR_TRIANGLE, vertexOutputSize, INDEX_BUFFER_FOR_TRIANGLE);
        
        
        }


        inline void RenderProjectedBatch(RenderState & state, ProjectedTriangleInput & input, int vertexOutputSize)
        {
            // Pass 1:
            //
            // The renderer is structured so that input a set of triangle lists
            // (exactly one list of triangles for each core to process).
            // As shown in BinTriangles() above, each thread bins the triangles in
            // input.triangleBuffer[threadId]
            //
            // Below we create one task per core (i.e., one thread per
            // core).  That task should bin all the triangles in the
            // list it is provided into bins: via a call to BinTriangles
            Parallel::For(0, Cores, 1, [&](int threadId)
            {
                BinTriangles(state, input, vertexOutputSize, threadId);
            });

            // Pass 2:
            //
            // process all the tiles created in pass 1. Create one task per
            // tile (not one per core), and distribute all the tasks among the cores.  The
            // third parameter to the Parallel::For call is the work
            // distribution granularity.  Increasing its value might reduce
            // scheduling overhead (consecutive tiles go to the same core),
            // but could increase load imbalance.  (You can probably leave it
            // at one.)
            Parallel::For(0, gridWidth*gridHeight, 1, [&](int tileId)
            {
                ProcessBin(state, input, vertexOutputSize, tileId);
            });

        }
    };

    IRasterRenderer * CreateTiledRenderer()
    {
        return new RendererImplBase<TiledRendererAlgorithm>();
    }
}
