#ifndef RASTER_RENDERER_SHADER_H
#define RASTER_RENDERER_SHADER_H

#include "CoreLib/Basic.h"
#include "CoreLib/VectorMath.h"
#include "VertexBuffer.h"

namespace RasterRenderer
{
    using namespace VectorMath;

    class RenderState;
    
    const int MaxVertexOutputSize = 256;

    enum class TessellationDomain
    {
        None, Triangle, Quad
    };

    // this is the shader interface. All shaders must implement all functions defined in this class.

    class Shader : public CoreLib::Basic::Object
    {
    protected:
        inline Vec2 & GetVec2(float * input, int offset)
        {
            return *(Vec2*)(input+offset);
        }
        inline Vec3 & GetVec3(float * input, int offset)
        {
            return *(Vec3*)(input+offset);
        }
        inline Vec4 & GetVec4(float * input, int offset)
        {
            return *(Vec4*)(input+offset);
        }
    public:
        static const int MaxPerPatchOutputSize = 256;  // the maximum number of output values of a tessellation control shader
        virtual void GetClipDistanceOutput(int & startIndex, int & count) = 0; // tells the pipeline to treat specific vertex output as clip distances that can be used in triangle clipping stage. 
        virtual int GetVertexOutputSize() = 0; // returns the number of vertex shader outputs 
        virtual int GetTessellatedVertexOutputSize() = 0; // returns the number of tessellation shader outputs, if no tessellation shader, return the number of vertex shader outputs.
        virtual TessellationDomain GetTessellationDomain() = 0; // return the tessellation domain

        // specify the body of tessellation control shader
        virtual void ComputeTessellationControl(RenderState & state, float * tessLevelsOutput, float * perPatchOutputBuffer, float ** vertexInput, int vertCount, int id) = 0;
        
        // specify the body of tesselation evaluation shader
        virtual void ComputeTessellationEvaluation(RenderState & state, float * vertexOutput, float * tessLevels, float * perPatchInput, float ** vertexInput, int vertCount, Vec3 tessCoord, int id) = 0;
        
        // specify the vertex shader
        virtual void ComputeVertex(RenderState & state, const Vec3 & pos, const Vec3 & normal, const Vec4 & color, const Vec2 & texCoord, const BBox & bounds, float * inputBuffer, float * output) = 0;

        // specify the fragment shader in SIMD form. (used by tiled renderer)
		virtual void ShadeFragment(RenderState & state, float * result, __m128 * input, int id) = 0;

        // a helper function that calls the vertex shader
        void ExecuteVertexShader(RenderState & state, VertexFormat format, float * input, const BBox & bounds, float * output)
        {
            VertexBufferRef buffer(format, 1, input);
            ComputeVertex(state, buffer.GetPosition(0), buffer.GetNormal(0), buffer.GetColor(0), buffer.GetTexCoord(0), bounds, input, output);
        };
    };

    // a default shader implementation

    class DefaultShader : public Shader
    {
    public:
        virtual int GetVertexOutputSize();
        virtual void GetClipDistanceOutput(int & startIndex, int & count);
        virtual int GetTessellatedVertexOutputSize();
        virtual void ComputeVertex(RenderState & state, const Vec3 & pos, const Vec3 & normal, const Vec4 & color, const Vec2 & texCoord, const BBox & bounds, float * inputBuffer, float * output);
		virtual void ShadeFragment(RenderState & state, float * result, __m128 * input, int id);
        virtual void ComputeTessellationControl(RenderState & state, float * tessLevelsOutput, float * perPatchOutputBuffer, float ** vertexInput, int vertCount, int id);
        virtual void ComputeTessellationEvaluation(RenderState & state, float * vertexOutput, float * tessLevels, float * perPatchInput, float ** vertexInput, int vertCount, Vec3 tessCoord, int id);
        virtual TessellationDomain GetTessellationDomain();
    };

    class TextureShader : public DefaultShader
    {
    public:
		virtual void ShadeFragment(RenderState & state, float * output, __m128 * input, int id);
    };

    inline void NormalToRGB(__m128 *result, __m128 *normal)
    {
        static __m128 half = _mm_set_ps1(0.5f);
        result[0] = _mm_add_ps(_mm_mul_ps(normal[0], half), half);
        result[1] = _mm_add_ps(_mm_mul_ps(normal[1], half), half);
        result[2] = _mm_add_ps(_mm_mul_ps(normal[2], half), half);
    }

    inline void NormalToRGB(Vec3 & result, Vec3 &normal)
    {
        result.x = normal.x*0.5f + 0.5f;
        result.y = normal.y*0.5f + 0.5f;
        result.z = normal.z*0.5f + 0.5f;
    }
}

#endif
