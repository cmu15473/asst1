#include "TestScene.h"

namespace RasterRenderer
{
    namespace Testing
    {
        class AlphaColorShader : public DefaultShader
        {
        public:
            Vec4 ColorOutput;
            virtual void ShadeFragment(RenderState & state, float * output, __m128 * input, int id)
            {
                for (int i = 0; i<4; i++)
                {
                    output[i] = ColorOutput.x;
                    output[i + 4] = ColorOutput.y;
                    output[i + 8] = ColorOutput.z;
                    output[i + 12] = ColorOutput.w;
                }
            }
        }; 
        class TestScene6 : public TestScene
        {
        private:
            RefPtr<VertexBuffer> vertBuffer;
            RefPtr<IndexBuffer> indexBuffer;
            RefPtr<AlphaColorShader> shader;
        public:
            TestScene6(ViewSettings & viewSettings)
                : TestScene(viewSettings)
            {
                float vertData[] = { 0.0f, 1.2f, -4.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                    -1.0f, 0.0f, -4.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                    1.0f, 0.0f, -4.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                };
                int indexData[] = { 0, 1, 2 };
                vertBuffer = new VertexBuffer(VertexFormat::PositionNormalTex, 3, vertData);
                indexBuffer = new IndexBuffer(ElementType::Triangles, 1, indexData);
                shader = new AlphaColorShader();
            }

            virtual void Draw(IRasterRenderer * renderer)
            {
                int index[] = { 0, 0 };
                Matrix4 trans;
                State.AlphaBlend = true;
                State.Shader = shader.Ptr();
                shader->ColorOutput = Vec4(1.0f, 0.2f, 0.3f, 1.0f);
                Matrix4::CreateIdentityMatrix(trans);
                State.SetModelViewTransform(trans);
                renderer->Draw(State, vertBuffer.Ptr(), indexBuffer.Ptr(), index);
                Matrix4::Translation(trans, 0.3f, 0.2f, 0.1f);
                State.SetModelViewTransform(trans);
                shader->ColorOutput = Vec4(0.3f, 0.8f, 0.2f, 0.6f);
                renderer->Draw(State, vertBuffer.Ptr(), indexBuffer.Ptr(), index);
                Matrix4::Translation(trans, 0.6f, 0.4f, 0.2f);
                State.SetModelViewTransform(trans);
                shader->ColorOutput = Vec4(0.3f, 0.2f, 0.9f, 0.6f);
                renderer->Draw(State, vertBuffer.Ptr(), indexBuffer.Ptr(), index);
            }
        };

        TestScene * CreateTestScene6(ViewSettings & viewSettings)
        {
            return new TestScene6(viewSettings);
        }
    }
}