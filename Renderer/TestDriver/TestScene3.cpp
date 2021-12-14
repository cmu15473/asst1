#include "TestScene.h"

namespace RasterRenderer
{
    namespace Testing
    {
        TestScene * CreateTestScene3(ViewSettings & viewSettings, String baseDir)
        {
            auto rs = CreateTestSceneFromModel(viewSettings, baseDir + L"/bunny.obj");
            Matrix4 modelView;
            Matrix4 translation, rotation;
            Matrix4::Translation(translation, 0.0f, -8.0f, -15.0f);
            rs->State.SetModelViewTransform(translation);
            return rs;
        }
    }
}
