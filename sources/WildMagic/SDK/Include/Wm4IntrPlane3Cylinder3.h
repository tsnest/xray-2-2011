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
// Version: 4.0.2 (2008/08/17)

#ifndef WM4INTRPLANE3CYLINDER3_H
#define WM4INTRPLANE3CYLINDER3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Plane3.h"
#include "Wm4Cylinder3.h"
#include "Wm4Circle3.h"
#include "Wm4Ellipse3.h"
#include "Wm4Line3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrPlane3Cylinder3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Cylinder3 (const Plane3<Real>& rkPlane,
        const Cylinder3<Real>& rkCylinder);

    // object access
    const Plane3<Real>& GetPlane () const;
    const Cylinder3<Real>& GetCylinder () const;

    // Static intersection query for a *finite* cylinder.
    virtual bool Test ();

    // Static intersection query for an *infinite* cylinder.
    virtual bool Find ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The cylinder is culled if it is on the negative
    // side of the plane.
    bool CylinderIsCulled () const;

    // The intersection set for an *infinite* cylinder and the plane.
    enum
    {
        PC_EMPTY_SET,
        PC_ONE_LINE,
        PC_TWO_LINES,
        PC_CIRCLE,
        PC_ELLIPSE
    };

    int GetType () const;

    // Valid when GetType() returns PC_ONE_LINE.
    void GetOneLine (Line3<Real>& rkLine) const;

    // Valid when GetType() returns PC_TWO_LINES.
    void GetTwoLines (Line3<Real>& rkLine0, Line3<Real>& rkLine1) const;

    // Valid when GetType() returns PC_CIRCLE.
    void GetCircle (Circle3<Real>& rkCircle) const;

    // Valid when GetType() returns PC_ELLIPSE.
    void GetEllipse (Ellipse3<Real>& rkEllipse) const;

protected:
    // The objects to intersect.
    const Plane3<Real>* m_pkPlane;
    const Cylinder3<Real>* m_pkCylinder;

    // The intersection set when the cylinder is infinite.
    int m_iType;
    Line3<Real> m_kLine0, m_kLine1;
    Circle3<Real> m_kCircle;
    Ellipse3<Real> m_kEllipse;

};

typedef IntrPlane3Cylinder3<float> IntrPlane3Cylinder3f;
typedef IntrPlane3Cylinder3<double> IntrPlane3Cylinder3d;

}

#endif
