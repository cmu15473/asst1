#include "Shader.h"
#include "RenderState.h"

namespace RasterRenderer
{
    int DefaultShader::GetVertexOutputSize()
    {
        return 12;
    }

    int DefaultShader::GetTessellatedVertexOutputSize()
    {
        return 12;
    }

    void DefaultShader::GetClipDistanceOutput(int & startIndex, int & count)
    {
        startIndex = -1;
        count = 0;
    }

    void DefaultShader::ComputeVertex(RenderState & state, const Vec3 & pos, const Vec3 & normal, const Vec4 & color, const Vec2 & texCoord, const BBox & bounds, float * inputBuffer, float * output)
    {
        state.ModelViewProjectionTransform.Transform(GetVec4(output, 0), Vec4(pos, 1.0f));
        state.NormalTransform.TransformNormal(GetVec3(output, 4), normal);
        state.ModelViewTransform.Transform(GetVec3(output, 7), pos);
        //float invW = 1.0f/output[3];
        //output[12] = invW;
        GetVec2(output, 10) = texCoord;//*invW;
    }

	void DefaultShader::ShadeFragment(RenderState & state, float * result, __m128 * input, int id)
    {
        __m128 zero = _mm_setzero_ps();
        __m128 x2 = _mm_mul_ps(input[7], input[7]);
        __m128 y2 = _mm_mul_ps(input[8], input[8]);
        __m128 z2 = _mm_mul_ps(input[9], input[9]);
        __m128 w = _mm_rsqrt_ps(_mm_add_ps(_mm_add_ps(x2, y2), z2));
        __m128 viewX = _mm_mul_ps(input[7], w);
        __m128 viewY = _mm_mul_ps(input[8], w);
        __m128 viewZ = _mm_mul_ps(input[9], w);

        __m128 brightness = _mm_neg(_mm_add_ps(_mm_add_ps(_mm_mul_ps(viewX, input[4]),  _mm_mul_ps(viewY, input[5])), _mm_mul_ps(viewZ, input[6])));
        brightness = _mm_max_ps(zero, brightness);
        brightness = _mm_mul_ps(brightness, _mm_set_ps1(0.7f));
        brightness = _mm_add_ps(brightness, _mm_set_ps1(0.3f));
        _mm_store_ps(result, brightness);
        _mm_store_ps(result+4, brightness);
        _mm_store_ps(result+8, brightness);
        result[12] = result[13] = result[14] = result[15] = 1.0f;
    }

    TessellationDomain DefaultShader::GetTessellationDomain()
    {
        return TessellationDomain::None;
    }

    void DefaultShader::ComputeTessellationControl(RenderState & state, float * tessLevelsOutput, float * perPatchOutputBuffer, float ** vertexInput, int vertCount, int id)
    {
        tessLevelsOutput[0] = tessLevelsOutput[1] = tessLevelsOutput[2] = tessLevelsOutput[3] = 1.0f;
    }
    void DefaultShader::ComputeTessellationEvaluation(RenderState & state, float * vertexOutput, float * tessLevels, float * perPatchInput, float ** vertexInput, int vertCount, Vec3 tessCoord, int id)
    {
        for (int j = 0; j<GetTessellatedVertexOutputSize(); j++)
            vertexOutput[j] = vertexInput[0][j]*tessCoord.x + vertexInput[1][j]*tessCoord.y + vertexInput[2][j]*tessCoord.z;
    }

	void TextureShader::ShadeFragment(RenderState & state, float * output, __m128 * input, int id)
    {
        __m128 zero = _mm_setzero_ps();
        __m128 x2 = _mm_mul_ps(input[7], input[7]);
        __m128 y2 = _mm_mul_ps(input[8], input[8]);
        __m128 z2 = _mm_mul_ps(input[9], input[9]);
        __m128 w = _mm_rsqrt_ps(_mm_add_ps(_mm_add_ps(x2, y2), z2));
        __m128 viewX = _mm_mul_ps(input[7], w);
        __m128 viewY = _mm_mul_ps(input[8], w);
        __m128 viewZ = _mm_mul_ps(input[9], w);

         x2 = _mm_mul_ps(input[4], input[4]);
         y2 = _mm_mul_ps(input[5], input[5]);
         z2 = _mm_mul_ps(input[6], input[6]);
         w = _mm_rsqrt_ps(_mm_add_ps(_mm_add_ps(x2, y2), z2));
         auto normalX = _mm_mul_ps(input[4], w);
         auto normalY = _mm_mul_ps(input[5], w);
         auto normalZ = _mm_mul_ps(input[6], w);

        __m128 brightness = _mm_neg(_mm_add_ps(_mm_add_ps(_mm_mul_ps(viewX, normalX), _mm_mul_ps(viewY, normalY)), _mm_mul_ps(viewZ, normalZ)));
        brightness = _mm_max_ps(zero, brightness);
        brightness = _mm_mul_ps(brightness, _mm_set_ps1(0.7f));
        brightness = _mm_add_ps(brightness, _mm_set_ps1(0.3f));

        CORE_LIB_ALIGN_16(float u[4]);
        CORE_LIB_ALIGN_16(float v[4]);
        CORE_LIB_ALIGN_16(float lighting[4]);

        _mm_store_ps(u, input[10]);
        _mm_store_ps(v, input[11]);
        _mm_store_ps(lighting, brightness);
        float dudx = fabs(u[1]-u[0]);
        float dudy = fabs(u[2]-u[0]);
        float dvdx = fabs(v[1]-v[0]);
        float dvdy = fabs(v[2]-v[0]);

        for (int i = 0; i<4; i++)
        {
            Vec2 uv = Vec2(u[i], v[i]);
            // sample texture
            Vec4 diffuseMap = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            TextureData * texture = *(TextureData **)((int*)state.ConstantBuffer+id*(4+sizeof(TextureData*)/4));
            if (texture)
            {
                uv.x -= floor(uv.x);
                uv.y -= floor(uv.y);
                state.SampleTexture(&diffuseMap, texture, 16, dudx, dvdx, dudy, dvdy, uv);
            }
            Vec4 diffRate = *(Vec4*)((int*)state.ConstantBuffer+id*(4+sizeof(TextureData*)/4)+sizeof(TextureData*)/4);
            diffuseMap *= Vec4(lighting[i], lighting[i], lighting[i], 1.0f);
            diffuseMap *= diffRate;
            output[i] = diffuseMap.x;
            output[i+4] = diffuseMap.y;
            output[i+8] = diffuseMap.z;
            output[i+12] = diffuseMap.w;
        }
    }
}
