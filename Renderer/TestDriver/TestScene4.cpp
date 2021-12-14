#include "TestScene.h"
#include "LightingShader.h"

namespace RasterRenderer
{
    namespace Testing
    {
        TestScene * CreateTestScene4(ViewSettings & viewSettings, String baseDir)
        {
            auto rs = CreateTestSceneFromModel(viewSettings, baseDir + L"/crytek-sponza/sponza.obj");
            Matrix4 modelView;
            float trans[16] = {
                0.258877f, -0.025814f, 0.965565f, 0.000000f,
                -0.000000f, 0.999643f, 0.026725f, 0.000000f,
                -0.965910f, -0.006919f, 0.258785f, 0.000000f,
                -215.546417f, -602.996277f, -1047.017212f, 1.000000f
            };
            for (int i = 0; i < 16; i++)
                modelView.values[i] = trans[i];
            rs->State.SetModelViewTransform(modelView);
            auto shader = new LightingShader();
            Light light;
            light.Ambient = 0.3f;
            light.Color = Vec3(1.8f, 1.8f, 1.8f);
            light.Decay = 0.0f;
            light.Position = Vec3(-147.60965f, 1129.266113f, -40.055214f);
            shader->Lights.Add(light);
            rs->SetShader(shader);
            return rs;
        }
    }
}
