#ifndef RASTER_RENDERER_RENDER_STATE_H
#define RASTER_RENDERER_RENDER_STATE_H

#include "CoreLib/VectorMath.h"
#include "Shader.h"
#include "CoreLib/Imaging/TextureData.h"

using namespace VectorMath;

namespace RasterRenderer
{
    using namespace CoreLib::Imaging;

    enum class TextureFilter
    {
        Nearest = 0,
        Linear = 1,
        TriLinear = 2,
        Anisotropic4x = 4,
        Anisotropic8x = 8,
        Anisotropic16x = 16
    };

    enum class TransparencyOrder
    {
        Unchanged,
        BackToFront
    };

    enum class PackageCullingType
    {
        None, BitMask, ZBuffer
    };

    class RenderState
    {
    public:
        Matrix4 ModelViewTransform;
        Matrix4 ProjectionTransform;
        Matrix4 InverseProjectionTransform;
        Matrix4 ModelViewProjectionTransform;
        Matrix4 NormalTransform;
        RasterRenderer::TextureFilter TextureFilter;
        int ViewportWidth, ViewportHeight;
        float HalfWidth, HalfHeight;
        float zMin, zMax;
    public:
        RasterRenderer::Shader * Shader;
        TextureData * TextureBinding[16];
        RasterRenderer::TransparencyOrder TransparencyOrder;
        void * ConstantBuffer;
        bool BackfaceCulling;
        bool AlphaBlend;
        bool TessellationEnabled;
        bool TraceEnabled;
        PackageCullingType PackageCulling;
        bool EnableDebugDump;
        RenderState()
        {
            Matrix4::CreateIdentityMatrix(ModelViewTransform);
            Matrix4::CreateIdentityMatrix(ProjectionTransform);
            Matrix4::CreateIdentityMatrix(ModelViewProjectionTransform);
            Matrix4::CreateIdentityMatrix(NormalTransform);
            ViewportWidth = ViewportHeight = 0;
            HalfWidth = HalfHeight = 0.0f;
            Shader = 0;
            TraceEnabled = false;
            BackfaceCulling = true;
            AlphaBlend = false;
            TessellationEnabled = false;
            PackageCulling = PackageCullingType::None;
            EnableDebugDump = false;
            memset(TextureBinding, 0, sizeof(TextureData*) * 16);
            this->TextureFilter = RasterRenderer::TextureFilter::TriLinear;
            this->TransparencyOrder = RasterRenderer::TransparencyOrder::Unchanged;
        }
        void SetViewport(int width, int height)
        {
            ViewportWidth = width;
            ViewportHeight = height;
            HalfWidth = width * 0.5f;
            HalfHeight = height * 0.5f;
        }
        void SetModelViewTransform(Matrix4 & transform)
        {
            ModelViewTransform = transform;
            Matrix4::Multiply(ModelViewProjectionTransform, ProjectionTransform, transform);
            transform.Inverse(NormalTransform);
            NormalTransform.Transpose();
        }
        void SetProjectionTransform(Matrix4 & transform)
        {
            ProjectionTransform = transform;
            Matrix4::Multiply(ModelViewProjectionTransform, transform, ModelViewTransform);
            transform.Inverse(InverseProjectionTransform);
            Vec3 rs;
            InverseProjectionTransform.TransformHomogeneous(rs, Vec3(0.0f, 0.0f, 0.0f));
            zMin = rs.z;
            InverseProjectionTransform.TransformHomogeneous(rs, Vec3(0.0f, 0.0f, 1.0f));
            zMax = rs.z;
        }
        inline void SampleTexture(Vec4 * result, TextureData * texture, int maxRate, float dudx, float dvdx, float dudy, float dvdy, Vec2 uv)
        {
			if (this->TextureFilter == RasterRenderer::TextureFilter::Linear)
			{
				LinearSampling(result, texture, uv);
			}
			else if (this->TextureFilter == RasterRenderer::TextureFilter::TriLinear)
			{
				TrilinearSampling(result, texture, sqrt(dudx*dudx + dudy*dudy), sqrt(dvdx*dvdx + dvdy*dvdy), uv);
			}
			else if (this->TextureFilter == RasterRenderer::TextureFilter::Nearest)
			{
				NearestSampling(result, texture, uv);
			}
			else if(this->TextureFilter == RasterRenderer::TextureFilter::Anisotropic16x)
			{
				AnisotropicSampling(result, texture, 16, dudx, dvdx, dudy, dvdy, uv);
			}
			else if (this->TextureFilter == RasterRenderer::TextureFilter::Anisotropic8x)
			{
				AnisotropicSampling(result, texture, 8, dudx, dvdx, dudy, dvdy, uv);
			}
			else if (this->TextureFilter == RasterRenderer::TextureFilter::Anisotropic4x)
			{
				AnisotropicSampling(result, texture, 4, dudx, dvdx, dudy, dvdy, uv);
			}
        }
    };
}

#endif
