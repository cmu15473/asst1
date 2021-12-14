#ifndef CORE_LIB_GRAPHICS_BBOX_H
#define CORE_LIB_GRAPHICS_BBOX_H

#include "../VectorMath.h"
#include "../LibMath.h"
#include <float.h>
namespace CoreLib
{
	namespace Graphics
	{
		using namespace VectorMath;

		class BBox
		{
		public:
			struct
			{
				float xMin, yMin, zMin, xMax, yMax, zMax;
			};
			Vec3 & Min()
			{
				return *(Vec3*)this;
			}
			Vec3 & Max()
			{
				return *((Vec3*)this+1);
			}
			inline int MaxDimension()
			{
				float xsize = xMax-xMin;
				float ysize = yMax-yMin;
				float zsize = zMax-zMin;
				if (xsize>ysize)
				{
					if (xsize>zsize)
						return 0;
					else
						return 2;
				}
				else
				{
					if (ysize > zsize)
						return 1;
					else
						return 2;
				}
			}
			inline void Init()
			{
				xMin = yMin = zMin = FLT_MAX;
				xMax = yMax = zMax = -FLT_MAX;
			}
			inline void Union(const BBox & box)
			{
				xMin = Math::Min(box.xMin, xMin);
				yMin = Math::Min(box.yMin, yMin);
				zMin = Math::Min(box.zMin, zMin);

				xMax = Math::Max(box.xMax, xMax);
				yMax = Math::Max(box.yMax, yMax);
				zMax = Math::Max(box.zMax, zMax);
			}
			inline void Union(const Vec3 &v)
			{
				xMin = Math::Min(v.x, xMin);
				yMin = Math::Min(v.y, yMin);
				zMin = Math::Min(v.z, zMin);

				xMax = Math::Max(v.x, xMax);
				yMax = Math::Max(v.y, yMax);
				zMax = Math::Max(v.z, zMax);
			}
			inline bool Contains(const Vec3 & v)
			{
				return v.x >= xMin && v.x <= xMax &&
					   v.y >= yMin && v.y <= yMax &&
					   v.z >= zMin && v.z <= zMax;
			}
			inline void GetCornerPoints(Vec3 cornerPoints[8])
			{
				cornerPoints[0] = Vec3(xMin, yMin, zMin);
				cornerPoints[1] = Vec3(xMax, yMin, zMin);
				cornerPoints[2] = Vec3(xMin, yMax, zMin);
				cornerPoints[3] = Vec3(xMax, yMax, zMin);
				cornerPoints[4] = Vec3(xMin, yMin, zMax);
				cornerPoints[5] = Vec3(xMax, yMin, zMax);
				cornerPoints[6] = Vec3(xMin, yMax, zMax);
				cornerPoints[7] = Vec3(xMax, yMax, zMax);
			}
		};

		inline void TransformBBox(BBox & bboxOut, const Matrix4 & mat, const BBox & bboxIn)
		{
			Vec3 v, v_t;
			bboxOut.Init();
			for (int i = 0; i < 8; i++)
			{
				if (i & 1)
					v.x = bboxIn.xMax;
				else
					v.x = bboxIn.xMin;
				if (i & 2)
					v.y = bboxIn.yMax;
				else
					v.y = bboxIn.yMin;
				if (i & 4)
					v.z = bboxIn.zMax;
				else
					v.z = bboxIn.zMin;
				mat.Transform(v_t, v);
				bboxOut.Union(v_t);
			}
		}
	}
}

#endif
