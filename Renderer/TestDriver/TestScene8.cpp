#include "TestScene.h"

namespace RasterRenderer
{
	namespace Testing
	{
		class TestShader8 : public DefaultShader
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

		class TestScene8 : public TestScene
		{
		private:
			RefPtr<VertexBuffer> vertBuffer;
			RefPtr<IndexBuffer> indexBuffer;
			List<int> materials;
		public:
			TestScene8(ViewSettings & viewSettings)
				: TestScene(viewSettings)
			{
				List<float> vertData;
				List<int> indices;
				int vid = 0;
				for (int x = 0; x < 100; x++)
				{
					for (int y = 0; y < 100; y++)
					{
						for (int z = 0; z < 10; z++)
						{
							float vx = 1.2f * x / (100.0f) - 0.6f;
							float vy = 1.2f * y / (100.0f) - 0.6f;
							float vz = 0.9f - z / 20.0f;
							float vx1 = 1.2f * (x + 1) / (100.0f) - 0.6f;
							float vy1 = 1.2f * (y + 1) / (100.0f) - 0.6f;
							vertData.Add(vx); vertData.Add(vy); vertData.Add(vz); vertData.Add(0.0f); vertData.Add(0.0f); vertData.Add(1.0f); vertData.Add(0.0f); vertData.Add(0.0f);
							vertData.Add(vx1); vertData.Add(vy); vertData.Add(vz); vertData.Add(0.0f); vertData.Add(0.0f); vertData.Add(1.0f); vertData.Add(0.0f); vertData.Add(0.0f);
							vertData.Add(vx1); vertData.Add(vy1); vertData.Add(vz); vertData.Add(0.0f); vertData.Add(0.0f); vertData.Add(1.0f); vertData.Add(0.0f); vertData.Add(0.0f);
							vertData.Add(vx); vertData.Add(vy1); vertData.Add(vz); vertData.Add(0.0f); vertData.Add(0.0f); vertData.Add(1.0f); vertData.Add(0.0f); vertData.Add(0.0f);
							indices.Add(vid); 
							indices.Add(vid + 1);
							indices.Add(vid + 2);
							indices.Add(vid);
							indices.Add(vid + 2);
							indices.Add(vid + 3);
							vid += 4;
							materials.Add(0);
							materials.Add(0);
						}
					}
				}
				vertBuffer = new VertexBuffer(VertexFormat::PositionNormalTex, vertData.Count() / 8, vertData.Buffer());
				indexBuffer = new IndexBuffer(ElementType::Triangles, indices.Count()/3, indices.Buffer());
				
			}

			virtual void Draw(IRasterRenderer * renderer)
			{
				renderer->Draw(State, vertBuffer.Ptr(), indexBuffer.Ptr(), materials.Buffer());
			}
		};


		TestScene * CreateTestScene8(ViewSettings & viewSettings)
		{
			auto rs = new TestScene8(viewSettings);
			Matrix4 identity;
			Matrix4::CreateIdentityMatrix(identity);
			rs->State.SetProjectionTransform(identity);
			rs->State.SetModelViewTransform(identity);
			auto shader = new TestShader8();
			shader->ColorOutput = Vec4(0.12f, 0.86f, 0.34f, 0.05f);
			rs->SetShader(shader);
			return rs;
		}
	}
}
