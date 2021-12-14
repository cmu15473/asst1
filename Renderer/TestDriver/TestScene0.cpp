#include "TestScene.h"

namespace RasterRenderer
{
    namespace Testing
    {
        class TestScene0 : public TestScene
        {
        private:
            RefPtr<VertexBuffer> vertBuffer;
            RefPtr<IndexBuffer> indexBuffer;
        public:
            TestScene0(ViewSettings & viewSettings)
                : TestScene(viewSettings)
            {
                float vertData[] = {  0.0f,  1.0f, -4.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                      -2.0f, 0.0f, -3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                      2.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
                int indexData[] = {0, 1, 2};
                vertBuffer = new VertexBuffer(VertexFormat::PositionNormalTex, 3, vertData);
                indexBuffer = new IndexBuffer(ElementType::Triangles, 1, indexData);
            }

            virtual void Draw(IRasterRenderer * renderer)
            {
                int index[] = {0,0};
                renderer->Draw(State, vertBuffer.Ptr(), indexBuffer.Ptr(), index);
            }
        };

        TestScene * CreateTestScene0(ViewSettings & viewSettings)
        {
            return new TestScene0(viewSettings);
        }
    }
}