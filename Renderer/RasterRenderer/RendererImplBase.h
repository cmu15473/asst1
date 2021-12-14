#ifndef RASTER_RENDERER_IMPL_BASE
#define RASTER_RENDERER_IMPL_BASE

#include "IRasterRenderer.h"
#include "ProjectedTriangle.h"
#include "Fragment.h"
#include "PolygonClipper.h"
#include "Parallel.h"
#include "Rasterizer.h"
#include "Statistics.h"
#include "CoreLib/LibIO.h"
#include "CoreLib/PerformanceCounter.h"
#include <atomic>

namespace RasterRenderer
{
    using namespace CoreLib::Diagnostics;

    // interpret this as max number of worker threads
    static const int Cores = 12;

    // sets up the triangle based on three post-projection clip space coordinates.
    // it computes the edges equation and performs back-face culling.

    // parameters:
    //    [out] tri: returns the computed edge equations
    //     [in] constId: offset to constant buffer
    //     [in] id: triangle Id
    //     [in] s0, s1, s2: screen space coordinates of three vertices, range in [-1.0f, 1.0f]
    //     [in] state: renderer state bits

    inline bool SetupTriangle(ProjectedTriangle & tri, int constId, int id, Vec3 s0, Vec3 s1, Vec3 s2, RenderState & state)
    {
        // convert s0,s1,s2 from [-1.0f, 1.0f] (clip space) into fixed point (4 fraction bits) screen grid
        tri.X0 = (unsigned short)((s0.x + 1.0f)*state.HalfWidth * 16);
        tri.Y0 = (unsigned short)((s0.y + 1.0f)*state.HalfHeight * 16);
        tri.X1 = (unsigned short)((s1.x + 1.0f)*state.HalfWidth * 16);
        tri.Y1 = (unsigned short)((s1.y + 1.0f)*state.HalfHeight * 16);
        tri.X2 = (unsigned short)((s2.x + 1.0f)*state.HalfWidth * 16);
        tri.Y2 = (unsigned short)((s2.y + 1.0f)*state.HalfHeight * 16);

        // compute edge equations
        tri.A0 = tri.Y0 - tri.Y1;
        tri.B0 = tri.X1 - tri.X0;
        tri.A1 = tri.Y1 - tri.Y2;
        tri.B1 = tri.X2 - tri.X1;
        tri.A2 = tri.Y2 - tri.Y0;
        tri.B2 = tri.X0 - tri.X2;

        // the divisor is used to normalize barycentric coordinates,
        // its sign indicates whether the triangle is front-facing or back-facing
        int divisor = tri.B2*tri.A0 - tri.A2*tri.B0;
        if (state.BackfaceCulling)
        {
            if (divisor >= 0)
                return false;
        }
        else if (divisor > 0)
        {
            tri.A0 = -tri.A0;
            tri.B0 = -tri.B0;
            tri.A1 = -tri.A1;
            tri.B1 = -tri.B1;
            tri.A2 = -tri.A2;
            tri.B2 = -tri.B2;
        }
        tri.InvArea = 1.0f / (float)abs(divisor); // pre-compute the reciprocal

        // compute z-plane that is used to get z-value of each visibility sample point
        // fragment(x,y)'s z-value is: tri.fZ0 + (x-tri.X0)*tri.fDZDX + (y-tri.Y0)*tri.fDZDY
        float Nx = (s1.z - s0.z)*(tri.Y2 - tri.Y0) - (tri.Y1 - tri.Y0)*(s2.z - s0.z);
        float Ny = (s2.z - s0.z)*(tri.X1 - tri.X0) - (tri.X2 - tri.X0)*(s1.z - s0.z);
        float Nz = (float)((tri.Y2 - tri.Y0)*(tri.X1 - tri.X0) - (tri.X2 - tri.X0)*(tri.Y1 - tri.Y0));
        float invNZ = 1.0f / Nz;

        tri.fZ0 = s0.z;
        tri.fDZDX = Nx*invNZ;
        tri.fDZDY = Ny*invNZ;

        // pass along other parameters in the returning structure
        tri.Id = id;
        tri.ConstantId = constId;
        tri.MinZ = Math::Min(s0.z, Math::Min(s1.z, s2.z));
        return true;
    }

    // implements the geometry processing stages of the pipeline. This includes fetching
    // vertex attributes, running the vertex shader, triangle set up, and clipping
    class ProjectedTriangleInput
    {
    public:
        static const int SegmentSize = 512;
        static const int DefaultSegmentMask = ~(SegmentSize - 1);
        static const int MaxBatchSize = 32767;
        static const int IdealVertexDataBufferSize = 1 << 18;
        static const int IdealTriangleBufferSize = 1 << 15;
        static const int MaxVertexDataBufferSize = IdealVertexDataBufferSize + IdealVertexDataBufferSize / 2;
        static const int MaxTriangleBufferSize = IdealTriangleBufferSize + IdealTriangleBufferSize / 2;
    public:
        //Tessellator tessellators[Cores];
        List<float> tessVertBuffer[Cores];
        List<int> vertexMap[Cores];
        List<ProjectedTriangle> triangleBuffer[Cores];
        List<float> vertexOutputBuffer[Cores];
        List<int> indexOutputBuffer[Cores];
        int SegmentMask;
    public:
        // an iterator that records the consume progress of setup triangles, for all cores.
        // because the triangle setup is processed by many threads, each thread will write setup
        // triangles to its own stream. This iterator will merge all these streams and allow
        // us to iterate though setup triangles in the right order.
        class Iterator
        {
        private:
            ProjectedTriangleInput * input;
            int ptr[Cores];
            int curCore;
            int curSegId;
            int segMask;
        public:
            Iterator(ProjectedTriangleInput & input)
            {
                this->input = &input;
                curCore = -1;
                int minId = INT_MAX;
                segMask = input.SegmentMask;
                for (int i = 0; i < Cores; i++)
                {
                    ptr[i] = 0;
                    if (input.triangleBuffer[i].Count() > 0 && input.triangleBuffer[i][0].Id < minId)
                    {
                        minId = input.triangleBuffer[i][0].Id;
                        curSegId = minId & segMask;
                        curCore = i;
                    }
                }
            }
            void MoveNext()
            {

                if (curCore != -1)
                {
                    int p = ++ptr[curCore];
                    if (p < input->triangleBuffer[curCore].Count())
                    {
                        if ((input->triangleBuffer[curCore][p].Id & segMask) == curSegId)
                            return;
                    }

                    curCore = -1;
                    int minId = INT_MAX;
                    for (int i = 0; i < Cores; i++)
                    {
                        if (ptr[i] < input->triangleBuffer[i].Count())
                        {
                            int curId = input->triangleBuffer[i][ptr[i]].Id;
                            if (curId < minId)
                            {
                                minId = curId;
                                curSegId = minId & segMask;
                                curCore = i;
                            }
                        }
                    }
                }
            }
            int GetCoreId()
            {
                return curCore;
            }
            int GetPtr()
            {
                return ptr[curCore];
            }
            bool Valid()
            {
                return curCore != -1;
            }
            ProjectedTriangle & GetProjectedTriangle()
            {
                return input->triangleBuffer[curCore][ptr[curCore]];
            }
            float * GetVertexOutput(int i, int vertexOutputSize)
            {
                return input->vertexOutputBuffer[curCore].Buffer() + (ptr[curCore] * 3 + i)*vertexOutputSize;
            }
        };
    public:
        ProjectedTriangleInput()
        {
            for (int i = 0; i < Cores; i++)
            {
                //indexInputBuffer[i].Reserve(1<<17);
                vertexMap[i].Reserve(1 << 18);
                triangleBuffer[i].Reserve(MaxTriangleBufferSize);
                vertexOutputBuffer[i].Reserve(MaxVertexDataBufferSize);
                tessVertBuffer[i].Reserve(MaxVertexDataBufferSize);
                indexOutputBuffer[i].Reserve(1 << 17);
            }
        }
        // returns number of triangles consumed
        static const int PackageSizeBits = 12;
        static const int MaxPackageSize = 1 << PackageSizeBits;
        static const int PackageMask = MaxPackageSize - 1;


        inline int ClipTriangle(
            List<ProjectedTriangle> & triangleBuffer, // stores the clipped triangles
            List<float> & vertexOutputBuffer,  // the vertex attribute buffer
            int & vertCount,  // gets and updates the number of vertexes stored in vertex attribute buffer (clipping will add new vertexes to this buffer)
            List<int> & indexOutputBuffer, // returns the vertex indices of output triangles (index into vertexOutputBuffer)
            Array<Vec3, MaxClipPlanes> & clipDistances, // input clip distances
            RenderState &state,
            int triId, int constantId, int vertId[3], int vertexOutputSize)
        {
            /*
              Side effects:
              1. write new clip vertex into vertexOutputBuffer
              2. append vertex indices to indexOutputBuffer, which points
              to vertices in vertexOutputBuffer
              3. append one or more projected triangles into triangleBuffer
              */
            Polygon poly0, poly1;
            Polygon * currentPolygon = &poly0, *bufferPolygon = &poly1;
            int id = triangleBuffer.Count();
            int clipVertIds[12];

            float * vertBuffer = vertexOutputBuffer.Buffer();
            int id0 = vertId[0];
            int id1 = vertId[1];
            int id2 = vertId[2];
            currentPolygon->FromTriangle(*(Vec4*)(vertBuffer + id0*vertexOutputSize),
                *(Vec4*)(vertBuffer + id1*vertexOutputSize),
                *(Vec4*)(vertBuffer + id2*vertexOutputSize));
            if (PolygonClipper::Clip(currentPolygon, bufferPolygon, clipDistances))
            {
                if (currentPolygon->Vertices.Count() < 3)
                    return 0;
                for (int j = 0; j < currentPolygon->ClipWeights.Count(); j++)
                {
                    Vec3 weight = *(Vec3*)(void*)(currentPolygon->ClipWeights.Buffer() + j);
                    float invW = 1.0f / currentPolygon->Vertices[j].w;
                    if (weight.x == 1.0f)
                    {
                        clipVertIds[j] = id0;
                    }
                    else if (weight.y == 1.0f)
                    {
                        clipVertIds[j] = id1;
                    }
                    else if (weight.z == 1.0f)
                    {
                        clipVertIds[j] = id2;
                    }
                    else
                    {
                        clipVertIds[j] = vertCount;
                        vertexOutputBuffer.GrowToSize((vertCount + 1) * vertexOutputSize);
                        vertexOutputBuffer[vertCount*vertexOutputSize] = currentPolygon->Vertices[j].x;
                        vertexOutputBuffer[vertCount*vertexOutputSize + 1] = currentPolygon->Vertices[j].y;
                        vertexOutputBuffer[vertCount*vertexOutputSize + 2] = currentPolygon->Vertices[j].z;
                        vertexOutputBuffer[vertCount*vertexOutputSize + 3] = currentPolygon->Vertices[j].w;
                        for (int k = 4; k < vertexOutputSize; k++)
                        {
                            float val1 = vertexOutputBuffer[id0*vertexOutputSize + k];
                            float val2 = vertexOutputBuffer[id1*vertexOutputSize + k];
                            float val3 = vertexOutputBuffer[id2*vertexOutputSize + k];
                            vertexOutputBuffer[vertCount*vertexOutputSize + k] =
                                val1*weight.x + val2*weight.y + val3*weight.z;
                        }
                        vertCount++;
                    }

                    currentPolygon->Vertices[j].x *= invW;
                    currentPolygon->Vertices[j].y *= invW;
                    currentPolygon->Vertices[j].z *= invW;
                }
                // simple triangulation
                for (int k = 2; k < currentPolygon->Vertices.Count(); k++)
                {
                    triangleBuffer.GrowToSize(id + 1);
                    if (SetupTriangle(triangleBuffer[id], constantId, triId, *(Vec3*)(void*)(currentPolygon->Vertices.Buffer()),
                        *(Vec3*)(void*)(currentPolygon->Vertices.Buffer() + k - 1), *(Vec3*)(void*)(currentPolygon->Vertices.Buffer() + k), state))
                    {
                        int pos = indexOutputBuffer.Count();
                        indexOutputBuffer.GrowToSize(pos + 3);
                        indexOutputBuffer[pos++] = clipVertIds[0];
                        indexOutputBuffer[pos++] = clipVertIds[k - 1];
                        indexOutputBuffer[pos] = clipVertIds[k];
                        id++;
                    }
                    else
                        triangleBuffer.UnsafeShrinkToSize(id);
                }
            }
            else
            {
                triangleBuffer.GrowToSize(id + 1);
                if (SetupTriangle(triangleBuffer[id], constantId, triId,
                    Vec3::FromHomogeneous(*(Vec4*)(void*)(currentPolygon->Vertices.Buffer())),
                    Vec3::FromHomogeneous(*(Vec4*)(void*)(currentPolygon->Vertices.Buffer() + 1)),
                    Vec3::FromHomogeneous(*(Vec4*)(void*)(currentPolygon->Vertices.Buffer() + 2)), state))
                {
                    int pos = indexOutputBuffer.Count();
                    indexOutputBuffer.GrowToSize(pos + 3);
                    indexOutputBuffer[pos++] = id0;
                    indexOutputBuffer[pos++] = id1;
                    indexOutputBuffer[pos] = id2;
                    id++;
                }
                else
                    triangleBuffer.UnsafeShrinkToSize(id);
            }
            return currentPolygon->Vertices.Count() - 2;
        }

        // consumes a segment of input index stream.

        template<bool UsePackageBuffer>
        int Input(RenderState & state, VertexBufferRef * vertBuffer, IndexBufferRef * indexBuffer, int * constantIndex, int start)
        {
            // Implements input stage shared by all renderers
            //
            std::atomic<int> consumePtr;
            consumePtr.store(start);
            int minVertId = INT_MAX;
            int maxVertId = 0;
            int end;
            int vertsPerPatch = indexBuffer->VertexCountPerPatch();
            end = Math::Min(indexBuffer->Count(), start + MaxBatchSize);

            int vertexOutputSize = state.Shader->GetVertexOutputSize();
            int tessellatedVertexOutputSize = state.Shader->GetTessellatedVertexOutputSize();
            if (vertexOutputSize > 512 || tessellatedVertexOutputSize > 512)
                throw InvalidOperationException(L"Too many vertex outputs!");
            int idealMaxVertexCount = IdealVertexDataBufferSize / vertexOutputSize;
            RefPtr<CoreLib::IO::StreamWriter> debugWriter;

            Parallel::For(0, Cores, [&, this](int threadId)
            {
                static int vertexCacheVersions[Cores];
                Array<Vec3, MaxClipPlanes> clipDistances;
                //TessellationResult tessResult;
                float vertexOutputData[512];
                triangleBuffer[threadId].Clear();
                vertexOutputBuffer[threadId].Clear();
                indexOutputBuffer[threadId].Clear();
                tessVertBuffer[threadId].Clear();

                const int vertexMapSize = 193;
                struct VertexMapEntry
                {
                    int CacheVersion : 8;
                    int VertexId : 24;
                    int Location;
                };

                // use a hash table to stored a cache of shaded vertexes.
                int vertexCacheVersion = (vertexCacheVersions[threadId]++) &(0xFF);
                VertexMapEntry vertexMap[vertexMapSize];
                for (int i = 0; i < vertexMapSize; i++)
                {
                    vertexMap[i].CacheVersion = vertexCacheVersion;
                    vertexMap[i].VertexId = -1;
                }


                int vertCount = 0;
                int tessVertCount = 0;
                float * vertexSource = (float*)vertBuffer->GetDataPointer();
                int vertexSize = vertBuffer->GetVertexSize();
                int segSize = state.TessellationEnabled ? 1 : SegmentSize;
                SegmentMask = ~(segSize - 1);
                int trianglesGenerated = 0;
                bool isQuad = indexBuffer->GetElementType() == ElementType::Quads;
                auto tessDomain = state.Shader->GetTessellationDomain();
                int lastPackageId = -1;
                int clipDistanceOutputIdx, clipDistanceOutputCount;
                state.Shader->GetClipDistanceOutput(clipDistanceOutputIdx, clipDistanceOutputCount);
                clipDistances.SetSize(clipDistanceOutputCount);
                BBox clipBounds;
                clipBounds.Init();
                while (trianglesGenerated < IdealTriangleBufferSize && vertCount < idealMaxVertexCount)
                {
                    int ptr = consumePtr.fetch_add(segSize, std::memory_order_relaxed);
                    int segStart, segEnd;

                    if (ptr >= end)
                        break;
                    segStart = ptr;
                    segEnd = Math::Min(ptr + segSize, end);

					int triId = ptr -start;
                    // consume this segment of indices
                    for (int j = segStart; j < segEnd; j++)
                    {
                        int index[IndexBuffer::MaxVerticesPerPatch];
                        for (int i = 0; i < vertsPerPatch; i++)
                        {
                            int vertIdx = (*indexBuffer)[j*vertsPerPatch + i];
                            int hashLoc = vertIdx%vertexMapSize;
                            auto vertMapEntry = vertexMap[hashLoc];
                            int vertAttribLoc = vertMapEntry.Location;
                            if (vertIdx != vertMapEntry.VertexId || vertMapEntry.CacheVersion != vertexCacheVersion)
                            {
                                vertAttribLoc = vertCount;
                                vertexMap[hashLoc].CacheVersion = vertexCacheVersion;
                                vertexMap[hashLoc].VertexId = vertIdx;
                                vertexMap[hashLoc].Location = vertAttribLoc;
                                // shade vertex
                                int outputPtr = vertexOutputBuffer[threadId].Count();
                                state.Shader->ExecuteVertexShader(state, vertBuffer->GetFormat(),
                                    vertexSource + vertIdx * vertexSize, clipBounds,
                                    vertexOutputData);
                                vertexOutputBuffer[threadId].AddRange(vertexOutputData, vertexOutputSize);
                                vertCount++;
                            }
                            index[i] = vertAttribLoc;
                        }
                        int constantId = -1;
                        if (constantIndex)
                            constantId = constantIndex[j];
                        if (!state.TessellationEnabled)
                        {
                            // no tessellation enabled, clip triangle and emit to output triangle stream.
                            for (int k = 0; k < clipDistanceOutputCount; k++)
                            {
                                clipDistances[k].x = vertexOutputBuffer[threadId][index[0] * vertexOutputSize + (clipDistanceOutputIdx + k)];
                                clipDistances[k].y = vertexOutputBuffer[threadId][index[1] * vertexOutputSize + (clipDistanceOutputIdx + k)];
                                clipDistances[k].z = vertexOutputBuffer[threadId][index[2] * vertexOutputSize + (clipDistanceOutputIdx + k)];
                            }

                            trianglesGenerated += ClipTriangle((List<ProjectedTriangle>&)triangleBuffer[threadId], (List<float>&)vertexOutputBuffer[threadId],
                                vertCount, (List<int>&)indexOutputBuffer[threadId], clipDistances, state, triId, constantId,
                                index, vertexOutputSize);

                            if (isQuad)
                            {
                                index[1] = index[2];
                                index[2] = index[3];
                                for (int k = 0; k < clipDistanceOutputCount; k++)
                                {
                                    clipDistances[k].x = vertexOutputBuffer[threadId][index[0] * vertexOutputSize + (clipDistanceOutputIdx + k)];
                                    clipDistances[k].y = vertexOutputBuffer[threadId][index[1] * vertexOutputSize + (clipDistanceOutputIdx + k)];
                                    clipDistances[k].z = vertexOutputBuffer[threadId][index[2] * vertexOutputSize + (clipDistanceOutputIdx + k)];
                                }
                                trianglesGenerated += ClipTriangle((List<ProjectedTriangle>&)triangleBuffer[threadId], (List<float>&)vertexOutputBuffer[threadId],
                                    vertCount, (List<int>&)indexOutputBuffer[threadId], clipDistances, state, triId, constantId,
                                    index, vertexOutputSize);
                            }
                        }
                        else
                        {
                            /*
                            // invoke tessellation shader

                            float * vertexInput[IndexBuffer::MaxVerticesPerPatch];
                            float tessLevels[6];
                            float perPatchBuffer[Shader::MaxPerPatchOutputSize];
                            for (int i = 0; i<vertsPerPatch; i++)
                            vertexInput[i] = vertexOutputBuffer[threadId].Buffer()+index[i]*vertexOutputSize;
                            state.Shader->ComputeTessellationControl(state, tessLevels, perPatchBuffer, vertexInput, vertsPerPatch, j);
                            if (tessDomain == TessellationDomain::Triangle)
                            tessellators[threadId].SubdivideTriangle(tessResult, tessLevels);
                            else
                            tessellators[threadId].SubdivideQuad(tessResult, tessLevels);
                            if (tessResult.Coordinates.Count())
                            {
                            int vertIndexStart = tessVertCount;
                            for (int i = 0; i<tessResult.Coordinates.Count(); i++)
                            {
                            auto coord = tessResult.Coordinates[i];
                            state.Shader->ComputeTessellationEvaluation(state, vertexOutputData, tessLevels, perPatchBuffer, vertexInput, 3, coord, j);
                            tessVertBuffer[threadId].AddRange(vertexOutputData, tessellatedVertexOutputSize);
                            tessVertCount++;
                            }
                            for (int i = 0; i<tessResult.TriangleCount; i++)
                            {
                            int vertIndex[3];
                            vertIndex[0] = tessResult.Indices[i*3] + vertIndexStart;
                            vertIndex[1] = tessResult.Indices[i*3 + 1] + vertIndexStart;
                            vertIndex[2] = tessResult.Indices[i*3 + 2] + vertIndexStart;
                            for (int k = 0; k < clipDistanceOutputCount; k++)
                            {
                            clipDistances[k].x = tessVertBuffer[threadId][vertIndex[0]*tessellatedVertexOutputSize + (clipDistanceOutputIdx+k)];
                            clipDistances[k].y = tessVertBuffer[threadId][vertIndex[1]*tessellatedVertexOutputSize + (clipDistanceOutputIdx+k)];
                            clipDistances[k].z = tessVertBuffer[threadId][vertIndex[2]*tessellatedVertexOutputSize + (clipDistanceOutputIdx+k)];
                            }
                            trianglesGenerated += ClipTriangle(triangleBuffer[threadId], tessVertBuffer[threadId],
                            tessVertCount, indexOutputBuffer[threadId], clipDistances, state, triId, constantId,
                            vertIndex, tessellatedVertexOutputSize);
                            }
                            }
                            */
                        }

                        triId++;
                    }
                }
                if (state.TessellationEnabled)
                {
                    ((List<float>&)tessVertBuffer[threadId]).SwapWith((List<float>&)vertexOutputBuffer[threadId]);
                    vertCount = tessVertCount;
                }
                for (int i = 0; i < vertCount; i++)
                {
                    vertexOutputBuffer[threadId][i*tessellatedVertexOutputSize + 3] = 1.0f / vertexOutputBuffer[threadId][i*tessellatedVertexOutputSize + 3];
                }
            });
            return Math::Min(end, consumePtr.load(std::memory_order_relaxed));
        }
    };

    // common part of different renderers.
    // the class implements IRasterRenderer and contains triangle set up and clipping stage.
    template<typename RenderAlgorithm>
    class RendererImplBase : public IRasterRenderer
    {
    private:
        RenderAlgorithm renderAlgorithm; // this class will define a set of template method that implements a specific renderer.
        FrameBuffer * frameBuffer;
        int screenWidth, screenHeight, padScreenWidth, padScreenHeight;
        float halfWidth, halfHeight;
        static const int batchSize = 1 << 12;
        // buffered triangle input
        ProjectedTriangleInput triangleInput;
    public:
        RendererImplBase()
        {
            renderAlgorithm.Init();
        }

        virtual TraceCollection * GetTraces()
        {
            return 0;
        }

        virtual void SetFrameBuffer(FrameBuffer * frameBuffer)
        {
            this->frameBuffer = frameBuffer;
            screenWidth = frameBuffer->GetWidth();
            screenHeight = frameBuffer->GetHeight();
            padScreenWidth = screenWidth << 7;
            padScreenHeight = screenHeight << 7;
            halfWidth = screenWidth*0.5f;
            halfHeight = screenHeight*0.5f;
            renderAlgorithm.SetFrameBuffer(frameBuffer);
        }

        virtual void Draw(RenderState & state, VertexBufferRef * vertBuffer, IndexBufferRef * indexBuffer, int * constantIndex)
        {
            IndexBufferRef & index = *indexBuffer;
            int i = 0;
            int vertexOutputSize = state.Shader->GetTessellatedVertexOutputSize();
            while (i < indexBuffer->Count())
            {
                i = triangleInput.Input<false>(state, vertBuffer, indexBuffer, constantIndex, i);
                renderAlgorithm.RenderProjectedBatch(state, triangleInput, vertexOutputSize);
            }
        }

        virtual void Finish()
        {
            // finish the frame
            renderAlgorithm.Finish();
        }

        virtual void Clear(const Vec4& clearColor, bool color, bool depth, bool mask)
        {
            // tell the renderer to clear framebuffer.
            renderAlgorithm.Clear(clearColor, color, depth);
            if (mask)
                frameBuffer->ClearMask();
        }
    };

    inline void ShadeFragment_DebugShowId(RenderState & state, float shadeResult[16], int id)
    {
        const float inv255 = 1.0f / 255.0f;
        float r = ((id >> 16) & 0xFF)*inv255;
        float g = ((id >> 8) & 0xFF)*inv255;
        float b = (id & 0xFF)*inv255;
        shadeResult[0] = shadeResult[1] = shadeResult[2] = shadeResult[3] = r;
        shadeResult[4] = shadeResult[5] = shadeResult[6] = shadeResult[7] = g;
        shadeResult[8] = shadeResult[9] = shadeResult[10] = shadeResult[11] = b;
        shadeResult[12] = shadeResult[13] = shadeResult[14] = shadeResult[15] = 1.0f;
    }

    inline void InterpolateVertexOutput(__m128 * interpolate, RenderState & state, __m128 beta, __m128 gamma, __m128 alpha, int triId, float * vertexOutputBuffer, int vertexSize, int * indexOutputBuffer)
    {
        static __m128 one = _mm_set1_ps(1.0f);

        float invW1 = *(vertexOutputBuffer + indexOutputBuffer[triId * 3] * vertexSize + 3);
        float invW2 = *(vertexOutputBuffer + indexOutputBuffer[triId * 3 + 1] * vertexSize + 3);
        float invW3 = *(vertexOutputBuffer + indexOutputBuffer[triId * 3 + 2] * vertexSize + 3);

        auto mInvW1 = _mm_set1_ps(invW1);
        auto mInvW2 = _mm_set1_ps(invW2);
        auto mInvW3 = _mm_set1_ps(invW3);

        __m128 alpha1 = _mm_mul_ps(alpha, mInvW1);
        __m128 beta1 = _mm_mul_ps(beta, mInvW2);
        __m128 gamma1 = _mm_mul_ps(gamma, mInvW3);
        __m128 interInvW = _mm_div_ps(one, _mm_add_ps(alpha1, _mm_add_ps(beta1, gamma1)));

        for (int k = 0; k < vertexSize; k++)
        {
            __m128 rs;
            __m128 tmp = _mm_load_ps1(vertexOutputBuffer + indexOutputBuffer[triId * 3] * vertexSize + k);
            rs = _mm_mul_ps(tmp, alpha1);
            tmp = _mm_load_ps1(vertexOutputBuffer + indexOutputBuffer[triId * 3 + 1] * vertexSize + k);
            rs = _mm_add_ps(rs, _mm_mul_ps(tmp, beta1));
            tmp = _mm_load_ps1(vertexOutputBuffer + indexOutputBuffer[triId * 3 + 2] * vertexSize + k);
            rs = _mm_add_ps(rs, _mm_mul_ps(tmp, gamma1));
            interpolate[k] = _mm_mul_ps(rs, interInvW);
        }
    }

    inline void ShadeFragment(RenderState & state, float* shadeResult, __m128 beta, __m128 gamma, __m128 alpha, int triId, int constId, float * vertexOutputBuffer, int vertexSize, int * indexOutputBuffer)
    {
        // interpolate vertex output
        __m128 interpolate[MaxVertexOutputSize];
        InterpolateVertexOutput(interpolate, state, beta, gamma, alpha, triId, vertexOutputBuffer, vertexSize, indexOutputBuffer);
        // run shader
        Vec4 color(0.0f, 0.0f, 0.0f, 0.0f);
		state.Shader->ShadeFragment(state, shadeResult, interpolate, constId);
    }

    inline void ShadeFragments(RenderState & state, int n, Fragment * fragmentBuffer, ProjectedTriangleInput & input, int vertexSize)
    {
        Parallel::For(0, n, 32, [&](int fid)
        {
            static __m128 one = _mm_set_ps1(1.0f);
            __m128 alpha = one;
            Fragment frag = fragmentBuffer[fid];
            alpha = _mm_sub_ps(_mm_sub_ps(one, frag.gamma), frag.beta);
            // interpolate vertex output
            __m128 interpolate[MaxVertexOutputSize];
            InterpolateVertexOutput(interpolate, state, frag.beta, frag.gamma, alpha, frag.TriangleId, input.vertexOutputBuffer[frag.CoreId].Buffer(), vertexSize, input.indexOutputBuffer[frag.CoreId].Buffer());
            // run shader
            Vec4 color(0.0f, 0.0f, 0.0f, 0.0f);
            state.Shader->ShadeFragment(state, fragmentBuffer[fid].ShadeResult, interpolate, frag.ConstId);
        });
    }
}

#endif
