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

#ifndef WM4CURVEMESH_H
#define WM4CURVEMESH_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Polyline.h"
#include "Wm4CurveSegment.h"
#include "Wm4FloatArray.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM CurveMesh : public Polyline
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  The input pkVBuffer has N >= 2 vertices
    // which are assumed to be ordered to form a continuous curve of N-1
    // curve segments.  The curve segments are stored in aspkSegment, an
    // array with N-1 elements.  CurveMesh assumes responsibility for deleting
    // this array, so it must be dynamically allocated.  The array pkParams
    // stores the parameter values for the curve segment end points.  This
    // array must have 2*(N-1) values.  Curve segment P[i] has domain
    // [spkParams[2*i],spkParams[2*i+1]].  Continuity requires that
    // P[i](spkParams[2*i+1]) = P[i+1](spkParams[2*(i+1)]).  The polyline
    // produced by the subdivision is flagged as "open".  If you want a
    // closed polyline, you should make certain that the first end point of
    // P[0] matches the last end point of P[N-1].
    //
    // The parameter bAllowDynamicChange should be set to 'true' when you plan
    // on dynamically changing the curve segments.  For example, you might
    // modify the control points in a Bezier curve segment.
    CurveMesh (VertexBuffer* pkVBuffer, CurveSegmentPtr* aspkSegment,
        FloatArray* pkParams, bool bAllowDynamicChange);

    virtual ~CurveMesh ();

    // level of subdivision
    void SetLevel (int iLevel);
    int GetLevel () const;

    // Call this function when one or more of your curve segments has
    // dynamically changed.
    void OnDynamicChange ();

    // Free up all memory used by the object, other than the Polyline data.
    // The intent is that the curve mesh is in its final form.  Once this
    // function is called, SetLevel is ignored.
    void Lock ();
    bool IsLocked () const;

protected:
    CurveMesh ();

    VertexBufferPtr m_spkOrigVBuffer;
    FloatArrayPtr m_spkOrigParams;
    CurveSegmentPtr* m_aspkSegment;
    int m_iLevel, m_iFullVQuantity, m_iSegmentQuantity;

private:
    // support for subdivision of the mesh
    class WM4_GRAPHICS_ITEM Edge
    {
    public:
        Edge ();

        CurveSegmentPtr Segment;  // curve for subdivision evaluations
        int V[2];                 // indices for vertices
        float Param[2];           // curve parameter values for end points
    };

    void Allocate (int& riVQTotal, int& riEQTotal, Edge*& rakEdge);
    void Subdivide (int& riVQ, int& riEQ, Edge* akEdge);

    // support for dynamic changes in the curve segments
    class WM4_GRAPHICS_ITEM CurveInfo
    {
    public:
        CurveInfo ();

        CurveSegmentPtr Segment;
        float Param;
    };
    void InitializeCurveInfo ();
    bool m_bAllowDynamicChange;
    CurveInfo* m_akCInfo;
};

WM4_REGISTER_STREAM(CurveMesh);
typedef Pointer<CurveMesh> CurveMeshPtr;
#include "Wm4CurveMesh.inl"

}

#endif
