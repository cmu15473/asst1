#ifndef RASTER_RENDERER_MODEL_RESOURCE_H
#define RASTER_RENDERER_MODEL_RESOURCE_H

#include "VertexBuffer.h"
#include "CoreLib/Graphics/ObjModel.h"
#include "IRasterRenderer.h"

namespace RasterRenderer
{
    class RenderBatch
    {
    public:
        RasterRenderer::IndexBuffer IndexBuffer;
        List<int> ConstantIndex;
        bool AlphaBlend;
        RenderBatch(int n, int * data, int * constIdx, bool alpha)
            :IndexBuffer(ElementType::Triangles, n, data)
        {
            AlphaBlend = alpha;
            ConstantIndex.AddRange(constIdx, n);
        }
    };

    class ModelMaterial
    {
    public:
        RefPtr<TextureData> DiffuseMap;
        
        Vec4 DiffuseRate, SpecularRate, AmbientRate;
        float SpecularPower;
        ModelMaterial()
        {
            DiffuseRate = SpecularRate = AmbientRate = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            SpecularPower = 0.0f;
        }
    };

    class ModelResource
    {
    private:
        RefPtr<VertexBuffer> vertexBuffer;
        //RefPtr<IndexBuffer> indexBuffer;
        List<RefPtr<RenderBatch>> batches;
        List<ModelMaterial> materials;
        RefPtr<Shader> shader;
        List<int> constBuffer;
        int Count;
    public:
        float Radius;
        ModelResource()
        {}
        static ModelResource FromObjModel(String fileName);
        static ModelResource FromObjModel(String basePath, CoreLib::Graphics::ObjModel & model);
        inline int TriangleCount()
        {
            return Count;
        }
        void SetShader(Shader * shader)
        {
            this->shader = shader;
        }
        inline void Draw(RenderState & state, IRasterRenderer * renderer)
        {
            state.ConstantBuffer = constBuffer.Buffer();
            state.Shader = this->shader.Ptr();
            for (int i = 0; i<batches.Count(); i++)
            {
                state.AlphaBlend = batches[i]->AlphaBlend;
                renderer->Draw(state, vertexBuffer.Ptr(), &batches[i]->IndexBuffer, batches[i]->ConstantIndex.Buffer());
            }
        }
    };
}

#endif
