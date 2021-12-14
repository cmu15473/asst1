#ifndef RAY_TRACE_PRO_OBJ_MODEL_H
#define RAY_TRACE_PRO_OBJ_MODEL_H

#include "../Basic.h"
#include "../VectorMath.h"
#include "../LibIO.h"

namespace CoreLib
{
	namespace Graphics
	{
		enum class PolygonType
		{
			Triangle, Quad
		};
		struct ObjFace
		{
			int VertexIds[4];
			int NormalIds[4];
			int TexCoordIds[4];
			unsigned int SmoothGroup;
			int MaterialId;
		};
		const int ObjMaterialVersion = 1;
		struct ObjMaterial
		{
			float SpecularRate;
			VectorMath::Vec3 Diffuse, Specular;
			Basic::String DiffuseMap, BumpMap, AlphaMap;
		};
		struct ObjModel
		{
			Basic::List<Basic::RefPtr<ObjMaterial>> Materials;
			Basic::List<VectorMath::Vec3> Vertices, Normals;
			Basic::List<VectorMath::Vec2> TexCoords;
			Basic::List<ObjFace> Faces;
			void ConstructPerVertexFaceList(Basic::List<int> & faceCountAtVert, Basic::List<int> & vertFaceList);
			void SaveToBinary(IO::BinaryWriter & writer);
			bool LoadFromBinary(IO::BinaryReader & reader);
		};
		bool LoadObj(ObjModel & mdl, const char * fileName, PolygonType polygonType = PolygonType::Triangle);
		void RecomputeNormals(ObjModel & mdl);
	}
}

#endif