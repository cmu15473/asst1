#include "TestScene.h"
#include "LightingShader.h"

namespace RasterRenderer
{
    namespace Testing
    {
        TestScene * CreateTestScene7(ViewSettings & viewSettings, String baseDir)
        {
            auto rs = CreateTestSceneFromModel(viewSettings, baseDir + L"/station/trainstation2.obj");
            Matrix4 modelView;
            float transformMatrix[] = { 0.968971f, -0.049833f, -0.242101f, 0.000000f,
                                        0.000000f, 0.979466f, -0.201607f, 0.00000f,
                                        0.247176f, 0.195351f, 0.949074f, 0.000000f,
                                        1965.519287f, -851.207275f, -3578.858887f, 1.000000f };
            for (int i = 0; i < 16; i++)
                modelView.values[i] = transformMatrix[i];
            Matrix4 flip;
            Matrix4::CreateIdentityMatrix(flip);
            flip.m[1][1] = 0.0f;
            flip.m[1][2] = -1.0f;
            flip.m[2][1] = 1.0f;
            flip.m[2][2] = 0.0f;
            Matrix4 flipped;
            Matrix4::Multiply(flipped, modelView, flip);
            modelView = flipped;
            rs->State.SetModelViewTransform(modelView);

            auto shader = new LightingShader();

            Light light;
            Vec3 lightPos = Vec3(-351.89325f, -1102.160522f, 3587.382568f);
            modelView.Transform(light.Position, lightPos);
            light.Ambient = 0.4f;
            light.Color = Vec3(1.4f, 1.4f, 1.4f);
            light.Decay = 0.0f;
            shader->Lights.Add(light);

            rs->SetShader(shader);
            rs->ClearColor = Vec4(0.78f, 0.843f, 0.922f, 1.0f);
            return rs;
        }
    }
}
