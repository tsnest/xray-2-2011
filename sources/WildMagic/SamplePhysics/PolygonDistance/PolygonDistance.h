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

#ifndef POLYGONDISTANCE_H
#define POLYGONDISTANCE_H

#include "Wm4WindowApplication2.h"
using namespace Wm4;

class PolygonDistance : public WindowApplication2
{
    WM4_DECLARE_INITIALIZE;

public:
    PolygonDistance ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char ucKey, int iX, int iY);

protected:
    void InitialConfiguration ();
    void NextConfiguration ();
    void PolarRepresentation (int iNum, const Vector2f* akVertices,
        Vector2f& rkCentroid, Vector2f* akPolar);
    void CartesianRepresentation (int iNum, Vector2f* akVertices,
        const Vector2f& rkCentroid, const Vector2f* akPolar);
    void PolyRotate (int iNum, int iSign, Vector2f* akPolar);
    void DoEdgeNorm (int iNum, const Vector2f* akVertices,
        const Vector2f& rkClosest, Vector2f* akEnd);
    void DrawLineSegment (int iThick, Color kColor, const Vector2f& rkEnd1,
        const Vector2f& rkEnd2);
    void DrawPerps (const Vector2f* akEP);
    void DrawPoints (int iThick, Color kColor, const Vector2f& rkPoint);

    enum { PD_NUM_POLYS = 3 };

    class Polygon
    {
    public:
        int m_iNumVertices;
        int m_iSign;
        Vector2f* m_akVertices;
        Vector2f m_akCentroid;
        Vector2f* m_akPolar;
        TTuple<2,int>* m_akFaces;
    };

    Polygon* m_kPolygon;
    bool m_bDoPerps;
};

WM4_REGISTER_INITIALIZE(PolygonDistance);

#endif
