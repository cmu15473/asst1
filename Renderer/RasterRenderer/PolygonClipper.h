#ifndef POLYGON_CLIPPER_H
#define POLYGON_CLIPPER_H

#include "CoreLib/VectorMath.h"
#include "CoreLib/Basic.h"

namespace RasterRenderer
{
    using namespace CoreLib::Basic;
    using namespace VectorMath;

    const int MaxClipPlanes = 15;
    const int MaxClipPolygonVertexCount = 18;

    class Polygon
    {
    public:
        Array<Vec4_Struct, MaxClipPolygonVertexCount> Vertices;
        Array<Vec3_Struct, MaxClipPolygonVertexCount> ClipWeights;
        void FromTriangle(Vec4 & v0, Vec4 & v1, Vec4 & v2)
        {
            Vertices.SetSize(3);
            ClipWeights.SetSize(3);
            Vertices[0] = v0; Vertices[1] = v1; Vertices[2] = v2;
            ClipWeights[0] = Vec3(1.0f, 0.0f, 0.0f);
            ClipWeights[1] = Vec3(0.0f, 1.0f, 0.0f);
            ClipWeights[2] = Vec3(0.0f, 0.0f, 1.0f);
        }
    };
    class PolygonClipper
    {
    private:
        template<typename PredicateFunc, typename ComputeTFunc, typename ClipFunc>
        static bool ClipByPlane(Polygon *& input, Polygon *& buffer, PredicateFunc predicate, ComputeTFunc computeT, ClipFunc clip)
        {
            bool shouldClip = false;
            for (int i = 0; i<input->Vertices.Count(); i++)
            {
                if (!predicate(input->Vertices[i], i))
                {
                    shouldClip = true;
                    break;
                }
            }
            if (!shouldClip)
                return false;
            buffer->Vertices.Clear();
            buffer->ClipWeights.Clear();
            for (int i = 0; i<input->Vertices.Count(); i++)
            {
                int i1 = i+1;
                if (i1 == input->Vertices.Count()) i1 = 0;
                Vec4 v0 = input->Vertices[i];
                Vec4 v1 = input->Vertices[i1];
                if (predicate(v0, i))
                {
                    if (predicate(v1, i1))
                    {
                        buffer->Vertices.Add(v1);
                        buffer->ClipWeights.Add(input->ClipWeights[i1]);
                    }
                    else
                    {
                        float t = computeT(v0, v1, i, i1);
                        Vec4 pos = v0*(1-t)+v1*t;
                        clip(pos);
                        Vec3 weight = *(Vec3*)(input->ClipWeights.Buffer()+i)*(1-t) + *(Vec3*)(input->ClipWeights.Buffer() + i1)*t;
                        buffer->Vertices.Add(pos);
                        buffer->ClipWeights.Add(weight);
                    }
                }
                else
                {
                    if (predicate(v1, i1))
                    {
                        float t = computeT(v0, v1, i, i1);
                        Vec4 pos = v0*(1-t)+v1*t;
                        clip(pos);
                        Vec3 weight = *(Vec3*)(input->ClipWeights.Buffer()+i)*(1-t) + *(Vec3*)(input->ClipWeights.Buffer() + i1)*t;
                        buffer->Vertices.Add(pos);
                        buffer->ClipWeights.Add(weight);
                        buffer->Vertices.Add(v1);
                        buffer->ClipWeights.Add(input->ClipWeights[i1]);
                    }
                }
            }
            Polygon * tmp = input;
            input = buffer;
            buffer = tmp;
            return true;
        }
    public:
        static bool Clip(Polygon *& input, Polygon *& buffer, Array<Vec3, MaxClipPlanes> & clipDistances)
        {
            bool rs = false;
            for (int i = 0; i<clipDistances.Count(); i++)
            {
                float clipD[18];
                for (int j = 0; j<input->Vertices.Count(); j++)
                {
                    clipD[j] = Vec3::Dot(*(Vec3*)(void*)(input->ClipWeights.Buffer() + j), clipDistances[i]);
                }
                rs = ClipByPlane(input, buffer, [&clipD](const Vec4 & v, int vid)->bool {return clipD[vid] >= 0.0f;}, 
                    [&clipD](const Vec4 & v0, const Vec4 & v1, int i0, int i1)->float
                    {
                        return -clipD[i0]/(clipD[i1]-clipD[i0]);
                    },
                    [=](Vec4 & v){}) || rs;
                if (input->Vertices.Count() < 3) return rs;
            }
            rs = ClipByPlane(input, buffer, [](const Vec4 & v, int vid)->bool {return v.x>=-v.w;}, 
                [](const Vec4 & v0, const Vec4 & v1, int i0, int i1)->float {return (v0.w + v0.x)/((v0.x+v0.w)-(v1.x+v1.w));},
                [](Vec4 & v){v.x = -v.w;}) || rs;
            if (input->Vertices.Count() < 3) return rs;
            rs = ClipByPlane(input, buffer, [](const Vec4 & v, int vid)->bool {return v.x<=v.w;}, 
                [](const Vec4 & v0, const Vec4 & v1, int i0, int i1)->float {return (-v0.w + v0.x)/((v0.x-v0.w)-(v1.x-v1.w));},
                [](Vec4 & v){v.x = v.w;}) || rs;
            if (input->Vertices.Count() < 3) return rs;
            rs = ClipByPlane(input, buffer, [](const Vec4 & v, int vid)->bool {return v.y>=-v.w;}, 
                [](const Vec4 & v0, const Vec4 & v1, int i0, int i1)->float {return (v0.w + v0.y)/((v0.y+v0.w)-(v1.y+v1.w));},
                [](Vec4 & v){v.y = -v.w;}) || rs;
            if (input->Vertices.Count() < 3) return rs;
            rs = ClipByPlane(input, buffer, [](const Vec4 & v, int vid)->bool {return v.y<=v.w;}, 
                [](const Vec4 & v0, const Vec4 & v1, int i0, int i1)->float {return (-v0.w + v0.y)/((v0.y-v0.w)-(v1.y-v1.w));},
                [](Vec4 & v){v.y = v.w;}) || rs;
            if (input->Vertices.Count() < 3) return rs;
            rs = ClipByPlane(input, buffer, [=](const Vec4 & v, int vid)->bool {return v.z<=v.w;}, 
                [](const Vec4 & v0, const Vec4 & v1, int i0, int i1)->float {return (-v0.w + v0.z)/((v0.z-v0.w)-(v1.z-v1.w));},
                [=](Vec4 & v){v.z = v.w;}) || rs;
            if (input->Vertices.Count() < 3) return rs;
            rs = ClipByPlane(input, buffer, [=](const Vec4 & v, int vid)->bool {return v.z>=-v.w;}, 
                [](const Vec4 & v0, const Vec4 & v1, int i0, int i1)->float {return (v0.w + v0.z)/((v0.z+v0.w)-(v1.z+v1.w));},
                [=](Vec4 & v){v.z = -v.w;}) || rs;
            if (input->Vertices.Count() < 3) return rs;
            for (int i = 0; i<input->Vertices.Count(); i++)
            {
                if (input->Vertices[i].w <= 0.0f)
                {
                    input->Vertices.Clear();
                    input->ClipWeights.Clear();
                    return true;
                }
            }
            return rs;
        }
    };
}

#endif