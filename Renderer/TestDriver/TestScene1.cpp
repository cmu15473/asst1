#include "TestScene.h"

namespace RasterRenderer
{
    namespace Testing
    {
        TestScene * CreateTestScene1(ViewSettings & viewSettings, String baseDir)
        {
            auto rs = CreateTestSceneFromModel(viewSettings, baseDir + L"/plane.obj");
            Matrix4 modelView;
            Matrix4::Translation(modelView, 0.0f, -0.0f, -12.0f);
            rs->State.SetModelViewTransform(modelView);
            return rs;
        }
    }
}
