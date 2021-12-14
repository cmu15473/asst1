#include "TestScene.h"

namespace RasterRenderer
{
    namespace Testing
    {
        class Light
        {
        public:
            Vec3 Position;
            Vec3 Color;
            float Ambient, Decay;
        };
        class LightingShader : public DefaultShader
        {
        public:
            List<Light> Lights;
            virtual void ShadeFragment(RenderState & state, float * output, __m128 * input, int id)
            {
                __m128 zero = _mm_setzero_ps();
                __m128 one = _mm_set1_ps(1.0f);
                __m128 sumLightingR = zero, sumLightingG = zero, sumLightingB = zero;
                for (auto &light : Lights)
                {
                    __m128 lightX = _mm_sub_ps(_mm_set1_ps(light.Position.x), input[7]);
                    __m128 lightY = _mm_sub_ps(_mm_set1_ps(light.Position.y), input[8]);
                    __m128 lightZ = _mm_sub_ps(_mm_set1_ps(light.Position.z), input[9]);
                    __m128 x2 = _mm_mul_ps(lightX, lightX);
                    __m128 y2 = _mm_mul_ps(lightY, lightY);
                    __m128 z2 = _mm_mul_ps(lightZ, lightZ);
                    __m128 length = _mm_sqrt_ps(_mm_add_ps(_mm_add_ps(x2, y2), z2));
                    __m128 invLength = _mm_div_ps(one, length);
                    lightX = _mm_mul_ps(lightX, invLength);
                    lightY = _mm_mul_ps(lightY, invLength);
                    lightZ = _mm_mul_ps(lightZ, invLength);
                    __m128 decay = one;
                    if (light.Decay > 0.01f)
                    {
                        decay = _mm_max_ps(zero, _mm_sub_ps(one, _mm_div_ps(length, _mm_set_ps1(light.Decay))));
                    }
                    x2 = _mm_mul_ps(input[4], input[4]);
                    y2 = _mm_mul_ps(input[5], input[5]);
                    z2 = _mm_mul_ps(input[6], input[6]);
                    __m128 w = _mm_rsqrt_ps(_mm_add_ps(_mm_add_ps(x2, y2), z2));
                    auto normalX = _mm_mul_ps(input[4], w);
                    auto normalY = _mm_mul_ps(input[5], w);
                    auto normalZ = _mm_mul_ps(input[6], w);

                    __m128 lighting = _mm_add_ps(_mm_add_ps(_mm_mul_ps(lightX, normalX), _mm_mul_ps(lightY, normalY)), _mm_mul_ps(lightZ, normalZ));
                    lighting = _mm_max_ps(zero, lighting);
                    lighting = _mm_mul_ps(lighting, decay);
                    lighting = _mm_mul_ps(lighting, _mm_set_ps1(1.0f - light.Ambient));
                    lighting = _mm_add_ps(lighting, _mm_set_ps1(light.Ambient));
                    sumLightingR = _mm_add_ps(sumLightingR, _mm_mul_ps(_mm_set1_ps(light.Color.x), lighting));
                    sumLightingG = _mm_add_ps(sumLightingG, _mm_mul_ps(_mm_set1_ps(light.Color.y), lighting));
                    sumLightingB = _mm_add_ps(sumLightingB, _mm_mul_ps(_mm_set1_ps(light.Color.z), lighting));
                }
                CORE_LIB_ALIGN_16(float u[4]);
                CORE_LIB_ALIGN_16(float v[4]);
                CORE_LIB_ALIGN_16(float lighting[3][4]);

                _mm_store_ps(u, input[10]);
                _mm_store_ps(v, input[11]);
                _mm_store_ps(lighting[0], sumLightingR);
                _mm_store_ps(lighting[1], sumLightingG);
                _mm_store_ps(lighting[2], sumLightingB);
                float dudx = fabs(u[1] - u[0]);
                float dudy = fabs(u[2] - u[0]);
                float dvdx = fabs(v[1] - v[0]);
                float dvdy = fabs(v[2] - v[0]);

                for (int i = 0; i<4; i++)
                {
                    Vec2 uv = Vec2(u[i], v[i]);
                    // sample texture
                    Vec4 diffuseMap = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    TextureData * texture = *(TextureData **)((int*)state.ConstantBuffer + id*(4 + sizeof(TextureData*) / 4));
                    if (texture)
                    {
                        uv.x -= floor(uv.x);
                        uv.y -= floor(uv.y);
                        state.SampleTexture(&diffuseMap, texture, 16, dudx, dvdx, dudy, dvdy, uv);
                    }
                    Vec4 diffRate = *(Vec4*)((int*)state.ConstantBuffer + id*(4 + sizeof(TextureData*) / 4) + sizeof(TextureData*) / 4);
                    diffuseMap *= Vec4(lighting[0][i], lighting[1][i], lighting[2][i], 1.0f);
                    diffuseMap *= diffRate;
                    output[i] = diffuseMap.x;
                    output[i + 4] = diffuseMap.y;
                    output[i + 8] = diffuseMap.z;
                    output[i + 12] = diffuseMap.w;
                }
            }
        };
    }
}
