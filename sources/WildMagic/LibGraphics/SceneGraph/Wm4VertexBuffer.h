// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2009
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.1 (2007/06/16)

#ifndef WM4VERTEXBUFFER_H
#define WM4VERTEXBUFFER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4Attributes.h"
#include "Wm4Bindable.h"
#include "Wm4ColorRGB.h"
#include "Wm4ColorRGBA.h"
#include "Wm4Vector2.h"
#include "Wm4Vector3.h"
#include "Wm4Vector4.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM VertexBuffer : public Object, public Bindable
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    VertexBuffer (const Attributes& rkAttributes, int iVertexQuantity);
    VertexBuffer (const VertexBuffer* pkVBuffer);
    virtual ~VertexBuffer ();

    // The format of a single vertex in the buffer.
    const Attributes& GetAttributes () const;

    // The size of a single vertex in the buffer, measured as number of
    // 'float' values.  The number of bytes for a single vertex is
    // GetVertexSize()*sizeof(float).
    int GetVertexSize () const;

    // The number of vertices in the buffer.
    int GetVertexQuantity () const;

    // Access to positions.
    float* PositionTuple (int i);
    const float* PositionTuple (int i) const;

    // Access to blend weights.
    float* BlendWeightTuple (int i);
    const float* BlendWeightTuple (int i) const;

    // Access to normals.
    float* NormalTuple (int i);
    const float* NormalTuple (int i) const;

    // Access to colors.
    float* ColorTuple (int iUnit, int i);
    const float* ColorTuple (int iUnit, int i) const;

    // Access to fog.
    float* FogTuple (int i);
    const float* FogTuple (int i) const;

    // Access to psize.
    float* PSizeTuple (int i);
    const float* PSizeTuple (int i) const;

    // Access to blend indices.
    float* BlendIndicesTuple (int i);
    const float* BlendIndicesTuple (int i) const;

    // Access to texture coordinates.
    float* TCoordTuple (int iUnit, int i);
    const float* TCoordTuple (int iUnit, int i) const;

    // Access to tangents.
    float* TangentTuple (int i);
    const float* TangentTuple (int i) const;

    // Access to bitangents.
    float* BitangentTuple (int i);
    const float* BitangentTuple (int i) const;

    // Direct access to the vertex buffer data.  The quantity is the number of
    // float elements.  The number of bytes for the entire vertex buffer is
    // GetChannelQuantity()*sizeof(float).
    int GetChannelQuantity () const;
    float* GetData ();
    const float* GetData () const;

    // Use these accessors for convenience.  No range checking is performed,
    // so you should be sure that the attribute exists and that the number of
    // channels is correct.
    float& Position1 (int i);
    float Position1 (int i) const;
    Vector2f& Position2 (int i);
    Vector2f Position2 (int i) const;
    Vector3f& Position3 (int i);
    Vector3f Position3 (int i) const;
    Vector4f& Position4 (int i);
    Vector4f Position4 (int i) const;

    float& BlendWeight1 (int i);
    float BlendWeight1 (int i) const;
    Vector2f& BlendWeight2 (int i);
    Vector2f BlendWeight2 (int i) const;
    Vector3f& BlendWeight3 (int i);
    Vector3f BlendWeight3 (int i) const;
    Vector4f& BlendWeight4 (int i);
    Vector4f BlendWeight4 (int i) const;

    float& Normal1 (int i);
    float Normal1 (int i) const;
    Vector2f& Normal2 (int i);
    Vector2f Normal2 (int i) const;
    Vector3f& Normal3 (int i);
    Vector3f Normal3 (int i) const;
    Vector4f& Normal4 (int i);
    Vector4f Normal4 (int i) const;

    float& Color1 (int iUnit, int i);
    float Color1 (int iUnit, int i) const;
    Vector2f& Color2 (int iUnit, int i);
    Vector2f Color2 (int iUnit, int i) const;
    ColorRGB& Color3 (int iUnit, int i);
    ColorRGB Color3 (int iUnit, int i) const;
    ColorRGBA& Color4 (int iUnit, int i);
    ColorRGBA Color4 (int iUnit, int i) const;

    float& Fog1 (int i);
    float Fog1 (int i) const;

    float& PSize1 (int i);
    float PSize1 (int i) const;

    float& BlendIndices1 (int i);
    float BlendIndices1 (int i) const;
    Vector2f& BlendIndices2 (int i);
    Vector2f BlendIndices2 (int i) const;
    Vector3f& BlendIndices3 (int i);
    Vector3f BlendIndices3 (int i) const;
    Vector4f& BlendIndices4 (int i);
    Vector4f BlendIndices4 (int i) const;

    float& TCoord1 (int iUnit, int i);
    float TCoord1 (int iUnit, int i) const;
    Vector2f& TCoord2 (int iUnit, int i);
    Vector2f TCoord2 (int iUnit, int i) const;
    Vector3f& TCoord3 (int iUnit, int i);
    Vector3f TCoord3 (int iUnit, int i) const;
    Vector4f& TCoord4 (int iUnit, int i);
    Vector4f TCoord4 (int iUnit, int i) const;

    float& Tangent1 (int i);
    float Tangent1 (int i) const;
    Vector2f& Tangent2 (int i);
    Vector2f Tangent2 (int i) const;
    Vector3f& Tangent3 (int i);
    Vector3f Tangent3 (int i) const;
    Vector4f& Tangent4 (int i);
    Vector4f Tangent4 (int i) const;

    float& Bitangent1 (int i);
    float Bitangent1 (int i) const;
    Vector2f& Bitangent2 (int i);
    Vector2f Bitangent2 (int i) const;
    Vector3f& Bitangent3 (int i);
    Vector3f Bitangent3 (int i) const;
    Vector4f& Bitangent4 (int i);
    Vector4f Bitangent4 (int i) const;

    // Support for building an array from the vertex buffer data, but
    // compatible with the vertex program inputs.  The output array,
    // rafCompatible, if null on input is dynamically allocated.  The caller
    // is responsible for deleting it.  You may pass in an already allocated
    // array as long as you are certain it has enough channels to store the
    // data.
    //
    // TO DO.  The bPackARGB flag exists for DirectX because of its need
    // to have ARGB (8-bit channels) rather than floats in [0,1] like OpenGL
    // prefers.  Set bPackARGB to 'true' for DirectX and to 'false' for the
    // OpenGL and software renderers.  If we need other formats later, this
    // parameter must be expanded to include the new possibilities.
    void BuildCompatibleArray (const Attributes& rkIAttr, bool bPackARGB,
        int& riChannels, float*& rafCompatible) const;

    // An application might want to vary the "active quantity" of vertices.
    // Use this function to do so.  It does not change the data storage,
    // only the m_iVertexQuantity member.  The caller is responsible for
    // saving the full quantity of vertices and resetting this when finished
    // with the vertex buffer.  The caller also should not pass in a quantity
    // that is larger than the original full quantity.
    void SetVertexQuantity (int iVQuantity);

private:
    // streaming support
    VertexBuffer ();

    // The format of a single vertex in the buffer.
    Attributes m_kAttributes;

    // The size of a single vertex in the buffer.
    int m_iVertexSize;

    // The number of vertices in the buffer.
    int m_iVertexQuantity;

    // The vertex buffer data.
    int m_iChannelQuantity;  // = m_iVertexQuantity*m_iVertexSize
    float* m_afChannel;
};

WM4_REGISTER_STREAM(VertexBuffer);
typedef Pointer<VertexBuffer> VertexBufferPtr;
#include "Wm4VertexBuffer.inl"

}

#endif
