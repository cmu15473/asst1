#include "ModelResource.h"
#include "CoreLib/LibIO.h"
#include <xmmintrin.h>
#include <float.h>
using namespace CoreLib::Graphics;
using namespace CoreLib::IO;

namespace RasterRenderer
{
    void ComputeBBox(ObjModel & obj, float & minX, float & minY, float & minZ, float & maxX, float & maxY, float & maxZ)
    {
        minX = minY = minZ = FLT_MAX;
        maxX = maxY = maxZ = -FLT_MAX;
        for (auto & v : obj.Vertices)
        {
            if (v.x < minX) minX = v.x;
            if (v.x > maxX) maxX = v.x;
            if (v.y < minY) minY = v.y;
            if (v.y > maxY) maxY = v.y;
            if (v.z < minZ) minZ = v.z;
            if (v.z > maxZ) maxZ = v.z;
        }
    }

    ModelResource ModelResource::FromObjModel(String fileName)
    {
        ObjModel obj;
        if (Path::GetFileExt(fileName).ToLower() == L"obj")
        {
            if (LoadObj(obj, fileName.ToMultiByteString()))
            {
                RecomputeNormals(obj);
                
            }
            else
                throw CoreLib::IO::IOException(L"Cannot load model file '" + fileName + L"'");
        }
        else
        {
            try
            {
                BinaryReader reader(new FileStream(fileName, FileMode::Open));
                if (!obj.LoadFromBinary(reader))
                {
                    throw CoreLib::IO::IOException(L"Cannot load model file '" + fileName + L"', version mismatch.");
                }
            }
            catch (IOException &)
            {
                throw CoreLib::IO::IOException(L"Cannot load model file '" + fileName + L"'");
            }
        }
        String basePath = Path::GetDirectoryName(fileName);
        return FromObjModel(basePath, obj);
    }

    struct MdlVertex
    {
        Vec3 Position;
        Vec3 Normal;
        Vec2 TexCoord;
    };

    struct IndexVertex
    {
        int Pos, Norm, Tex;
        IndexVertex()
        {
            Pos = 0;
            Norm = Tex = -1;
        }
        IndexVertex(int pos, int norm, int tex)
        {
            Pos = pos;
            Norm = norm;
            Tex = tex;
        }
        inline bool operator == (const IndexVertex & v)
        {
            return Pos == v.Pos && Tex == v.Tex && Norm == v.Norm;
        }
        inline int GetHashCode()
        {
            return (Pos<<20)+(Norm<<10) + Tex;
        }
    };

    ModelResource ModelResource::FromObjModel(String basePath, ObjModel & model)
    {
        ModelResource rs;
        float minX, minY, minZ, maxX, maxY, maxZ;
        ComputeBBox(model, minX, minY, minZ, maxX, maxY, maxZ);
        rs.Radius = sqrt((maxX-minX)*(maxX-minX) + (maxY-minY)*(maxY-minY) + (maxZ-minZ)*(maxZ-minZ));
        
        List<MdlVertex> verts;
        verts.Reserve(model.Vertices.Count());
        List<List<int>> groups;
        groups.SetSize(model.Materials.Count()+1);
        List<IndexVertex> vertexDict;
        vertexDict.SetSize(model.Vertices.Count());
        for (int i = 0; i<vertexDict.Count(); i++)
        {
            vertexDict[i].Norm = -1;
            vertexDict[i].Tex = -1;
            vertexDict[i].Pos = -1;
        };
        int indexCount = 0;
        for (auto & face : model.Faces)
        {
            int indices[3];
            for (int i = 0; i<3; i++)
            {
                auto index = IndexVertex(face.VertexIds[i], face.NormalIds[i], face.TexCoordIds[i]);
                if (vertexDict[index.Pos].Tex == index.Tex && vertexDict[index.Pos].Norm == index.Norm /*!vertexDict.TryGetValue(index, indices[i])*/)
                {
                    indices[i] = vertexDict[index.Pos].Pos;
                }
                else
                {
                    MdlVertex vert;
                    vert.Position = model.Vertices[index.Pos];
                    if (index.Norm != -1)
                        vert.Normal = model.Normals[index.Norm];
                    else
                        vert.Normal.SetZero();
                    if (index.Tex != -1)
                        vert.TexCoord = model.TexCoords[index.Tex];
                    else
                        vert.TexCoord.SetZero();
                    verts.Add(vert);
                    indices[i] = verts.Count()-1;
                    vertexDict[index.Pos] = index;
                    vertexDict[index.Pos].Pos = verts.Count()-1;
                }
            }
            groups[face.MaterialId+1].AddRange(indices, 3);
            indexCount+=3;
        }
        rs.Count = indexCount/3;

        rs.vertexBuffer = new VertexBuffer(VertexFormat::PositionNormalTex, verts.Count(), verts.Buffer());

        // load materials
        const int pointerSize = sizeof(TextureData*)/4;
        const int constSize = pointerSize + 4;
        rs.constBuffer.Reserve(constSize * model.Materials.Count()+1);
        rs.materials.Add(ModelMaterial());
        for (int i = 0; i<pointerSize; i++)
            rs.constBuffer.Add(0);
        rs.constBuffer.Add(FloatAsInt(0.7f));
        rs.constBuffer.Add(FloatAsInt(0.7f));
        rs.constBuffer.Add(FloatAsInt(0.7f));
        rs.constBuffer.Add(FloatAsInt(1.0f));
        Dictionary<String, RefPtr<TextureData>> textures;
        for (int i = 0; i<model.Materials.Count(); i++)
        {
            ModelMaterial mat;
            if (model.Materials[i]->DiffuseMap.Length())
            {
                String diffuseMap = Path::Combine(basePath,  model.Materials[i]->DiffuseMap);
                try
                {
                    RefPtr<TextureData> texture;
                    if (textures.TryGetValue(diffuseMap, texture))
                        mat.DiffuseMap = texture;
                    else
                    {
                        mat.DiffuseMap = new TextureData(diffuseMap);
                        textures[diffuseMap] = mat.DiffuseMap;
                    }
                }
                catch (IOException&)
                {
					printf("Cannot load texture \"%s\"\n", diffuseMap.ToMultiByteString());
                    mat.DiffuseMap = new TextureData();
                }
                
            }
            else
                mat.DiffuseMap = 0;
            for (int z = 0; z<pointerSize; z++)
                rs.constBuffer.Add(0);
            *(TextureData**)(rs.constBuffer.Buffer()+rs.constBuffer.Count()-pointerSize) = mat.DiffuseMap.Ptr();
            
            mat.DiffuseRate = Vec4(model.Materials[i]->Diffuse, 1.0f);
            rs.constBuffer.Add(FloatAsInt(mat.DiffuseRate.x));
            rs.constBuffer.Add(FloatAsInt(mat.DiffuseRate.y));
            rs.constBuffer.Add(FloatAsInt(mat.DiffuseRate.z));
            rs.constBuffer.Add(FloatAsInt(1.0f));
            mat.AmbientRate = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            mat.SpecularRate = Vec4(model.Materials[i]->Specular, 1.0f);
            mat.SpecularPower = model.Materials[i]->SpecularRate;
            rs.materials.Add(mat);
        }

        List<int> index;
        List<int> constIndex;
        index.Reserve(indexCount);
        constIndex.SetSize(indexCount/3);
        for (int x = 0; x<2; x++)
        {
            index.Clear();
            int pos = 0;
            for (int i = 0; i<groups.Count(); i++)
            {
                if (x == 0)
                {
                    if (i>0 && rs.materials[i].DiffuseMap && rs.materials[i].DiffuseMap->IsTransparent)
                        continue;
                }
                else
                {
                    if (i == 0 || !rs.materials[i].DiffuseMap ||!rs.materials[i].DiffuseMap->IsTransparent)
                        continue;
                }
                index.AddRange(groups[i]);
                for (int j = 0; j<groups[i].Count()/3; j++)
                {
                    constIndex[pos++] = i;
                }
            }
            rs.batches.Add(new RenderBatch(pos, index.Buffer(), constIndex.Buffer(), x==1));
        }

        rs.shader = new TextureShader();
        return rs;
    }
}
