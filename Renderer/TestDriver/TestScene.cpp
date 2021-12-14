#include "TestScene.h"
#include "ModelResource.h"

namespace RasterRenderer
{
    namespace Testing
    {
        class ModelTestScene : public TestScene
        {
        private:
            ModelResource model;
        public:
            ModelTestScene(CoreLib::Basic::String fileName, ViewSettings & viewSettings)
                : model(ModelResource::FromObjModel(fileName)), TestScene(viewSettings)
            {
                printf("Loaded scene: %d triangles\n", model.TriangleCount());
            }
            virtual void Draw(IRasterRenderer * renderer)
            {
                model.Draw(State, renderer);
            }
            virtual void SetShader(Shader * shader)
            {
                model.SetShader(shader);
            }
        };

        TestScene * CreateTestSceneFromModel(ViewSettings & viewSettings, CoreLib::Basic::String fileName)
        {
            return new ModelTestScene(fileName, viewSettings);
        }
    }
}
