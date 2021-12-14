#ifndef RASTER_RENDERER_TEST_SCENE_H
#define RASTER_RENDERER_TEST_SCENE_H

#include "IRasterRenderer.h"
#include "CoreLib/Basic.h"
#include "ViewSettings.h"

namespace RasterRenderer
{
    namespace Testing
    {
        class TestScene : public Object
        {
        private:
            RefPtr<Shader> defaultShader;
        public:
            RenderState State;
            Vec4 ClearColor;
            TestScene(ViewSettings & viewSettings)
            {
                defaultShader = new DefaultShader();
                ClearColor.SetZero();
                State.Shader = defaultShader.Ptr();
                State.ViewportHeight = viewSettings.WindowHeight;
                State.ViewportWidth = viewSettings.WindowWidth;
                State.HalfHeight = State.ViewportHeight * 0.5f;
                State.HalfWidth = State.ViewportWidth * 0.5f;
                State.TextureFilter = TextureFilter::Anisotropic16x;
                //State.TextureFilter = TextureFilter::TriLinear;
                Matrix4 proj;
                Matrix4::CreatePerspectiveMatrixFromViewAngle(proj, viewSettings.FovY, viewSettings.WindowWidth/(float)viewSettings.WindowHeight, viewSettings.zNear, viewSettings.zFar);
                State.SetProjectionTransform(proj);
            }
            virtual void Draw(IRasterRenderer * renderer) = 0;
            virtual void Draw(IRasterRenderer * renderer, RenderState & state)
            {
                State = state;
                Draw(renderer);
            }
            virtual void SetShader(Shader * shader)
            {
                defaultShader = shader;
				State.Shader = shader;
            }
        };

        TestScene * CreateTestScene0(ViewSettings & viewSettings);
        TestScene * CreateTestScene1(ViewSettings & viewSettings, String baseDir);
        TestScene * CreateTestScene2(ViewSettings & viewSettings, String baseDir);
        TestScene * CreateTestScene3(ViewSettings & viewSettings, String baseDir);
        TestScene * CreateTestScene4(ViewSettings & viewSettings, String baseDir);
        TestScene * CreateTestScene5(ViewSettings & viewSettings, String baseDir);
        TestScene * CreateTestScene6(ViewSettings & viewSettings);
        TestScene * CreateTestScene7(ViewSettings & viewSettings, String baseDir);
		TestScene * CreateTestScene8(ViewSettings & viewSettings);
        TestScene * CreateTestSceneFromModel(ViewSettings & viewSettings, CoreLib::Basic::String fileName);
    }
}
#endif