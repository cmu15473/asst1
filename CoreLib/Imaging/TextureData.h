#ifndef RAY_TRACE_PRO_TEXTURE_DATA_H
#define RAY_TRACE_PRO_TEXTURE_DATA_H

#include "../Basic.h"
#include "../VectorMath.h"
#include "../LibMath.h"
#include "Bitmap.h"

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline
#define _finite(x) ((x)==(x))
#endif

namespace CoreLib
{
	namespace Imaging
	{
		using namespace VectorMath;
		struct Color
		{
			union
			{
				struct
				{
					unsigned char R, G, B, A;
				};
				int Value;
			};
			Color()
			{}
			Color(int r, int g, int b, int a)
			{
				R = r;
				G = g;
				B = b;
				A = a;
			}
		};

		struct TextureLevel
		{
			Basic::List<Color> Pixels;
			int Width, Height;
		};
		class TextureData
		{
		public:
			int RefCount;
			Basic::String FileName;
			int Width, Height;
			float InvWidth, InvHeight;
			bool IsTransparent;
			Basic::List<TextureLevel> Levels;
			TextureData();
			TextureData(const Basic::String & fileName);
		};
		class Cubemap
		{
		public:
			RefPtr<TextureData> Maps[6]; // {+x, -x, +y, -y, +z, -z}
			Matrix4 Transforms[6];
		};

		inline void SampleTextureLevel_Neareast(VectorMath::Vec4 * result, TextureData * texture, int lod, VectorMath::Vec2 & _uv)
		{
			Vec2 uv = _uv;
			TextureLevel & level = texture->Levels[lod];
			uv.x *= level.Width;
			uv.y *= level.Height;
			if (uv.x < 0.0f) uv.x += level.Width;
			if (uv.y < 0.0f) uv.y += level.Height;
			int i0 = (int)(uv.x);
			int j0 = (int)(uv.y);
			i0 = Basic::Math::Min(i0, level.Width - 1);
			j0 = Basic::Math::Min(j0, level.Height - 1);
			Color c = level.Pixels[j0 * level.Width + i0];
			const float inv255 = 1.0f / 255.0f;
			result->x = c.R * inv255;
			result->y = c.G * inv255;
			result->z = c.B * inv255;
			result->w = c.A * inv255;
		}

		FORCE_INLINE void SampleTextureLevel(VectorMath::Vec4 * result, TextureData * texture, int lod, VectorMath::Vec2 & _uv)
		{
			Vec2 uv = _uv;
			TextureLevel & level = texture->Levels[lod];
			uv.x -= floor(uv.x);
			uv.y -= floor(uv.y);
			uv.x *= level.Width;
			uv.y *= level.Height;
			/*if (uv.x < 0.0f) uv.x += level.Width;
			if (uv.y < 0.0f) uv.y += level.Height;*/
			int i0 = (int)(uv.x);
			int i1 = i0 + 1 >= level.Width ? 0 : i0 + 1;
			int j0 = (int)(uv.y);
			int j1 = j0 + 1 >= level.Height ? 0 : j0 + 1;
			i0 = Basic::Math::Min(i0, level.Width - 1);
			j0 = Basic::Math::Min(j0, level.Height - 1);
			Color c1, c2, c3, c4;
			c1 = level.Pixels[j0 * level.Width + i0];
			c2 = level.Pixels[j0 * level.Width + i1];
			c3 = level.Pixels[j1 * level.Width + i0];
			c4 = level.Pixels[j1 * level.Width + i1];
			Color ci0, ci1;
			float it = uv.x - i0;
			float jt = uv.y - j0;
			float invIt = 1.0f - it;
			float invJt = 1.0f - jt;
			ci0.R = (unsigned char)(c1.R * invIt + c2.R * it);
			ci0.G = (unsigned char)(c1.G * invIt + c2.G * it);
			ci0.B = (unsigned char)(c1.B * invIt + c2.B * it);
			ci0.A = (unsigned char)(c1.A * invIt + c2.A * it);
			ci1.R = (unsigned char)(c3.R * invIt + c4.R * it);
			ci1.G = (unsigned char)(c3.G * invIt + c4.G * it);
			ci1.B = (unsigned char)(c3.B * invIt + c4.B * it);
			ci1.A = (unsigned char)(c3.A * invIt + c4.A * it);
			Color c;
			c.R = (unsigned char)(ci0.R * invJt + ci1.R * jt);
			c.G = (unsigned char)(ci0.G * invJt + ci1.G * jt);
			c.B = (unsigned char)(ci0.B * invJt + ci1.B * jt);
			c.A = (unsigned char)(ci0.A * invJt + ci1.A * jt);

			const float inv255 = 1.0f / 255.0f;

			result->x = c.R * inv255;
			result->y = c.G * inv255;
			result->z = c.B * inv255;
			result->w = c.A * inv255;
		}

		// Texture lookup: return value of nearest texel
		inline void NearestSampling(Vec4 * result, TextureData * texture, Vec2 uv)
		{
			SampleTextureLevel_Neareast(result, texture, 0, uv);
		}

		inline void LinearSampling(Vec4 * result, TextureData * texture, Vec2 uv)
		{
			if (_finite(uv.x) && _finite(uv.y))
				SampleTextureLevel(result, texture, 0, uv);
		}

		inline float fast_log2(float val)
		{
			int * const    exp_ptr = reinterpret_cast <int *> (&val);
			int            x = *exp_ptr;
			const int      log_2 = ((x >> 23) & 255) - 128;
			x &= ~(255 << 23);
			x += 127 << 23;
			*exp_ptr = x;

			val = ((-1.0f / 3) * val + 2) * val - 2.0f / 3;   // (1)

			return (val + log_2);
		}

		inline void TrilinearSampling(Vec4 * result, TextureData * texture, float du, float dv, Vec2 & uv, int minLod = 0)
		{
			if (_finite(du) && _finite(dv) && _finite(uv.x) && _finite(uv.y))
			{
				du *= texture->Width;
				dv *= texture->Height;
				float maxDudv = Basic::Math::Max(du, dv);
				float lod;
				int lod1, lod2;
				if (maxDudv < 0.0001f)
				{
					lod = 0.0f;
					lod1 = lod2 = 0;
				}
				else
				{
					lod = Basic::Math::Max(0.0f, fast_log2(maxDudv));
					lod1 = Basic::Math::Min((int)lod, texture->Levels.Count() - 1);
					lod2 = (lod == lod1 ? lod1 : Basic::Math::Min(lod1 + 1, texture->Levels.Count() - 1));
				}
				lod1 = Basic::Math::Max(lod1, minLod);
				lod2 = Basic::Math::Max(lod2, minLod);
				if (lod1 == lod2)
				{
					SampleTextureLevel(result, texture, lod1, uv);
				}
				else
				{
					float lodt = lod - lod1;
					float invLodt = 1.0f - lodt;
					Vec4 v1, v2;
					SampleTextureLevel(&v1, texture, lod1, uv);
					SampleTextureLevel(&v2, texture, lod2, uv);
					result->x = v1.x * invLodt + v2.x * lodt;
					result->y = v1.y * invLodt + v2.y * lodt;
					result->z = v1.z * invLodt + v2.z * lodt;
					result->w = v1.w * invLodt + v2.w * lodt;
				}
			}
			else
			{
				result->x = 0.0f;
				result->y = 0.0f;
				result->z = 0.0f;
				result->w = 0.0f;
			}
		}

		inline void AnisotropicSampling(Vec4 * result, TextureData * texture, int maxRate, float dudx, float dvdx, float dudy, float dvdy, Vec2 & uv)
		{
			float A = dvdx * dvdx + dvdy * dvdy;
			float B = -2.0f * (dudx * dvdx + dudy * dvdy);
			float C = dudx * dudx + dudy * dudy;
			float F = (dudx * dvdy - dudy * dvdx);
			F *= F;
			float p = A - C;
			float q = A + C;
			float t = sqrt(p * p + B * B);

			dudx *= texture->Width; dudy *= texture->Width;
			dvdx *= texture->Height; dvdy *= texture->Height;

			float squaredLengthX = dudx*dudx + dvdx*dvdx;
			float squaredLengthY = dudy*dudy + dvdy*dvdy;
			float determinant = abs(dudx*dvdy - dvdx*dudy);
			bool isMajorX = squaredLengthX > squaredLengthY;
			float squaredLengthMajor = isMajorX ? squaredLengthX : squaredLengthY;
			float lengthMajor = sqrt(squaredLengthMajor);
			float normMajor = 1.f / lengthMajor;

			Vec2 anisoDir;
			anisoDir.x = (isMajorX ? dudx : dudy) * normMajor;
			anisoDir.y = (isMajorX ? dvdx : dvdy) * normMajor;

			float ratioOfAnisotropy = squaredLengthMajor / determinant;

			// clamp ratio and compute LOD
			float lengthMinor;
			if (ratioOfAnisotropy > maxRate) // maxAniso comes from a Sampler state.
			{
				// ratio is clamped - LOD is based on ratio (preserves area)
				ratioOfAnisotropy = (float)maxRate;
				lengthMinor = lengthMajor / ratioOfAnisotropy;
			}
			else
			{
				// ratio not clamped - LOD is based on area
				lengthMinor = determinant / lengthMajor;
			}

			// clamp to top LOD
			if (lengthMinor < 1.0f)
			{
				ratioOfAnisotropy = Math::Max(1.0f, ratioOfAnisotropy*lengthMinor);
				lengthMinor = 1.0f;
			}

			float LOD = log(lengthMinor)* 1.442695f;
			float invRate = 1.0f / (int)ratioOfAnisotropy;
			float startU = uv.x * texture->Width - lengthMajor*anisoDir.x*0.5f;
			float startV = uv.y * texture->Height - lengthMajor*anisoDir.y*0.5f;
			float stepU = lengthMajor*anisoDir.x*invRate;
			float stepV = lengthMajor*anisoDir.y*invRate;
			result->SetZero();
			int lod1, lod2;
			lod1 = Basic::Math::Min((int)LOD, texture->Levels.Count() - 1);
			lod2 = Basic::Math::Min((int)ceil(LOD), texture->Levels.Count() - 1);
			float lodt = LOD - lod1;
			float invLodt = 1.0f - lodt;
			for (int i = 0; i<(int)ratioOfAnisotropy; i++)
			{
				uv.x = (startU + stepU * (i + 0.5f)) * texture->InvWidth;
				uv.y = (startV + stepV * (i + 0.5f)) * texture->InvHeight;
				if (lod1 == lod2)
				{
					Vec4 rs;
					SampleTextureLevel(&rs, texture, lod1, uv);
					(*result) += rs;
				}
				else
				{
					Vec4 v1, v2;
					SampleTextureLevel(&v1, texture, lod1, uv);
					SampleTextureLevel(&v2, texture, lod2, uv);
					result->x += v1.x * invLodt + v2.x * lodt;
					result->y += v1.y * invLodt + v2.y * lodt;
					result->z += v1.z * invLodt + v2.z * lodt;
					result->w += v1.w * invLodt + v2.w * lodt;
				}
			}
			(*result) *= invRate;
		}
	}
}

#endif
