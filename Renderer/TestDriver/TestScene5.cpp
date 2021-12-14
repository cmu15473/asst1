#include "TestScene.h"
#include "LightingShader.h"

namespace RasterRenderer
{
    namespace Testing
    {
        TestScene * CreateTestScene5(ViewSettings & viewSettings, String baseDir)
        {
            auto rs = CreateTestSceneFromModel(viewSettings, baseDir + L"/warehouse/ep2_indoor.obj");
            Matrix4 modelView;
            float transformMatrix[] = { -0.669132f, 0.023659f, -0.742766f, 0.000000f,
                                        0.000000f, 0.999493f, 0.031837f, 0.000000f,
                                        0.743143f, 0.021303f, -0.668793f, 0.000000f,
                                        -7027.120605f, 1115.192993f, 6001.621094f, 1.000000f };
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
            Vec3 lightPos = Vec3(282.73f, -9336.99f, -1269.75f);
            modelView.Transform(light.Position, lightPos);
            light.Ambient = 0.2f;
            light.Color = Vec3(1.65f, 1.65f, 1.70f);
            light.Decay = 850.0f;
            shader->Lights.Add(light);

            lightPos = Vec3(-196.555801f, -9157.416992f, -1255.329346f);
            modelView.Transform(light.Position, lightPos);
            light.Ambient = 0.0f;
            light.Color = Vec3(1.5f, 0.5f, 0.28f);
            light.Decay = 400.0f;
            shader->Lights.Add(light);

            rs->SetShader(shader);

            return rs;
        }
    }
}
