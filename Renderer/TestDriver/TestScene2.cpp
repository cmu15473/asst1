#include "TestScene.h"

namespace RasterRenderer
{
    namespace Testing
    {
        TestScene * CreateTestScene2(ViewSettings & viewSettings, String baseDir)
        {
            auto rs = CreateTestSceneFromModel(viewSettings, baseDir + L"/sibenik.obj");
            Matrix4 modelView;
            Matrix4 translation, rotation;
            Matrix4::RotationY(rotation, Math::Pi/2.0f);
            Matrix4::Translation(translation, 0.0f, 11.0f, -6.0f);
            Matrix4::Multiply(modelView, translation, rotation);
            rs->State.SetModelViewTransform(modelView);
            return rs;
        }
    }
}
