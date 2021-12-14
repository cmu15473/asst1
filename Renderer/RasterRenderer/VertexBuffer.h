#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "CoreLib/Basic.h"
#include "CoreLib/VectorMath.h"
#include "CoreLib/Graphics/BBox.h"

namespace RasterRenderer
{
    using namespace CoreLib::Basic;
    using namespace CoreLib::Graphics;
    using namespace VectorMath;

    enum class VertexFormat
    {
        Position = 0, PositionColor = 1, PositionColorNormal = 2, PositionNormalTex = 3, BezierPatchVertex = 4
    };

    class VertexBufferRef
    {
    protected:
        int * bufferRef;
        int count, vertexSize, colorOffset, normalOffset, texCoordOffset;
        VertexFormat format;
        inline int GetVertexSize(VertexFormat format)
        {
            switch (format)
            {
            case VertexFormat::Position:
                return 3;
            case VertexFormat::PositionColor:
                return 7;
            case VertexFormat::PositionColorNormal:
                return 10;
            case VertexFormat::PositionNormalTex:
                return 8;
            case VertexFormat::BezierPatchVertex:
                return 32;
            default:
                return -1;
            }
        }
        inline int GetTexCoordOffset(VertexFormat format)
        {
            switch (format)
            {
            case VertexFormat::PositionNormalTex:
                return 6;
            default:
                return -1;
            }
        }
        inline int GetColorOffset(VertexFormat format)
        {
            switch (format)
            {
            case VertexFormat::PositionColor:
            case VertexFormat::PositionColorNormal:
                return 3;
            default:
                return -1;
            }
        }
        inline int GetNormalOffset(VertexFormat format)
        {
            switch (format)
            {
            case VertexFormat::PositionColorNormal:
                return 7;
            case VertexFormat::PositionNormalTex:
                return 3;
            default:
                return -1;
            }
        }
    public:
        VertexBufferRef()
        {}
        VertexBufferRef(VertexFormat format, int n, void * data)
        {
            SetReference(format, n, data);
        }
        void SetReference(VertexFormat format, int n, void * data)
        {
            this->format = format;
            this->count = n;
            vertexSize = GetVertexSize(format);
            colorOffset = GetColorOffset(format);
            normalOffset = GetNormalOffset(format);
            texCoordOffset = GetTexCoordOffset(format);
            bufferRef = (int*)data;
        }
        
        VertexFormat GetFormat()
        {
            return format;
        }
        inline int GetVertexSize()
        {
            return vertexSize;
        }
        inline int GetNormalOffset()
        {
            return normalOffset;
        }
        inline int GetColorOffset()
        {
            return colorOffset;
        }
        void * GetDataPointer()
        {
            return bufferRef;
        }
        int Count()
        {
            return count;
        }
        Vec3 & GetPosition(int id)
        {
            return *(Vec3*)(void*)(bufferRef + vertexSize*id);
        }
        Vec4 GetColor(int id)
        {
            if (colorOffset == -1)
                return Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            return *(Vec4*)(void*)(bufferRef + vertexSize*id + colorOffset);
        }
        Vec3 GetNormal(int id)
        {
            if (normalOffset == -1)
                return Vec3(0.0f, 0.0f, 0.0f);
            return *(Vec3*)(void*)(bufferRef + vertexSize*id + normalOffset);
        }
        Vec2 GetTexCoord(int id)
        {
            if (texCoordOffset == -1)
                return Vec2(0.0f, 0.0f);
            return *(Vec2*)(void*)(bufferRef + vertexSize*id + texCoordOffset);
        }
    };

    class VertexBuffer : public VertexBufferRef
    {
    private:
        List<int> buffer;
    public:
        VertexBuffer(VertexFormat format, int n, void * data)
            : VertexBufferRef(format, n, 0)
        {
            buffer.SetSize(n * vertexSize);
            memcpy_s(buffer.Buffer(), buffer.Count()*sizeof(int), data, buffer.Count()*sizeof(int));
            bufferRef = buffer.Buffer();
        }

        VertexBuffer(VertexFormat format, int n)
            : VertexBufferRef(format, n, 0)
        {
            buffer.SetSize(n * vertexSize);
            bufferRef = buffer.Buffer();
        }
    };

    enum class ElementType
    {
        Triangles, Quads, Patches
    };

    class IndexBufferRef
    {
    protected:
        int * bufferRef;
        int count;
        int vertsPerPatch;
        ElementType elementType;
    public:
        static const int MaxVerticesPerPatch = 16;
        IndexBufferRef()
        {
            bufferRef = 0;
            count = 0;
            vertsPerPatch = 0;
            elementType = ElementType::Triangles;
        }
        IndexBufferRef(ElementType elementType, int numElements, int * buffer, int vertsPerPatch=-1)
        {
            if (elementType == ElementType::Triangles)
                vertsPerPatch = 3;
            else if (elementType == ElementType::Quads)
                vertsPerPatch = 4;
            this->bufferRef = buffer;
            count = numElements;
            this->vertsPerPatch = vertsPerPatch;
            this->elementType = elementType;
        }
        inline int Count()
        {
            return count;
        }
        inline int VertexCountPerPatch()
        {
            return vertsPerPatch;
        }
        inline int operator[](int id)
        {
            return bufferRef[id];
        }
        inline ElementType GetElementType()
        {
            return elementType;
        }
    };

    class IndexBuffer : public IndexBufferRef
    {
    private:
        List<int> buffer;
    public:
        static const int MaxVerticesPerPatch = 16;
        IndexBuffer(ElementType elementType, int numElements, int * buffer, int vertsPerPatch=-1)
            : IndexBufferRef(elementType, numElements, buffer, vertsPerPatch)
        {
            this->buffer.AddRange(buffer, numElements * this->vertsPerPatch);
            bufferRef = this->buffer.Buffer();
        }
    };

    class Package
    {
    public:
        BBox Bounds;
        int IndexStart;
        int IndexEnd; // exclusive
    };
}

#endif