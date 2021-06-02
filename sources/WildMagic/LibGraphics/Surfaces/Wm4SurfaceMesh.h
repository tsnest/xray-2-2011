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
// Version: 4.0.0 (2006/06/28)

#ifndef WM4SURFACEMESH_H
#define WM4SURFACEMESH_H

#include "Wm4GraphicsLIB.h"
#include "Wm4TriMesh.h"
#include "Wm4SurfacePatch.h"
#include "Wm4Vector2Array.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM SurfaceMesh : public TriMesh
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  If there are T triangles, pkIBuffer
    // has 3*T elements, aspkPatch has T elements, and spkParams has 3*T
    // elements.  Each triangle has a patch for constructing vertices in the
    // subdivision.  The subdivision requires knowing the parameter values
    // at the vertices of the original triangles.  SurfaceMesh assumes
    // responsibility for deleting the aspkPatch array (so it must be
    // dynamically allocated).  The parameter bAllowDynamicChange should be
    // set to 'true' when you plan on dynamically changing the surface
    // patches.  For example, you might modify the control points in a Bezier
    // surface patch.
    SurfaceMesh (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer,
        Vector2fArray* pkParams, SurfacePatchPtr* aspkPatch,
        bool bAllowDynamicChange);

    virtual ~SurfaceMesh ();

    // level of subdivision
    void SetLevel (int iLevel);
    int GetLevel () const;

    // Call this function when one or more of your surface patches has
    // dynamically changed.
    void OnDynamicChange ();

    // Free up all memory used by the object, other than the TriMesh data.
    // The intent is that the surface mesh is in its final form.  Once this
    // function is called, SetLevel is ignored.
    void Lock ();
    bool IsLocked () const;

protected:
    SurfaceMesh ();

    VertexBufferPtr m_spkOrigVBuffer;
    IndexBufferPtr m_spkOrigIBuffer;
    Vector2fArrayPtr m_spkOrigParams;
    SurfacePatchPtr* m_aspkPatch;
    int m_iLevel, m_iFullVQuantity, m_iPatchQuantity;

private:
    // support for subdivision of the mesh
    class Edge
    {
    public:
        Edge (int iV0 = -1, int iV1 = -1);

        // support for hash sets
        static int HashFunction (const Edge& rkEdge);
        bool operator== (const Edge& rkEdge) const;
        bool operator!= (const Edge& rkEdge) const;

        SurfacePatchPtr Patch;  // surface for subdivision evaluations
        int V[2];               // vertex indices for end points
        Vector2f Param[2];      // surface parameter values for end points
        int VMid;               // index for new vertex
        Vector2f ParamMid;      // parameter values for new vertex
        int References;         // number of triangles sharing the edge
    };

    class Triangle
    {
    public:
        Triangle ();

        SurfacePatchPtr Patch;  // surface for subdivision evaluations
        int V[3];               // indices for vertices
    };

    void Allocate (int& riVQTotal, int& riEQTotal, THashSet<Edge>*& rpkESet,
        int& riTQTotal, Triangle*& rakTriangle);

    void Subdivide (int& riVQ, int& riEQ, THashSet<Edge>* pkESet,
        int& riTQ, Triangle* akTriangle);

    void InsertInto (THashSet<Edge>* pkESet, SurfacePatchPtr spkPatch,
        int iV0, int iV1, const Vector2f& rkParam0, const Vector2f& rkParam1,
        int iNewReferences);

    // support for dynamic changes in the surface patches
    class SurfaceInfo
    {
    public:
        SurfaceInfo ();

        SurfacePatchPtr Patch;
        Vector2f Param;
    };
    void InitializeSurfaceInfo ();
    bool m_bAllowDynamicChange;
    SurfaceInfo* m_akSInfo;
};

WM4_REGISTER_STREAM(SurfaceMesh);
typedef Pointer<SurfaceMesh> SurfaceMeshPtr;
#include "Wm4SurfaceMesh.inl"

}

#endif
