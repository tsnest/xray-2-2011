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
// Version: 4.0.1 (2007/05/06)

#ifndef WM4DISTVECTOR3CIRCLE3_H
#define WM4DISTVECTOR3CIRCLE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Distance.h"
#include "Wm4Circle3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM DistVector3Circle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistVector3Circle3 (const Vector3<Real>& rkVector,
        const Circle3<Real>& rkCircle);

    // object access
    const Vector3<Real>& GetVector () const;
    const Circle3<Real>& GetCircle () const;

    // Static distance queries.  Compute the distance from the point P to the
    // circle.  When P is on the normal line C+t*N where C is the circle
    // center and N is the normal to the plane containing the circle, then
    // all circle points are equidistant from P.  In this case the returned
    // point is (infinity,infinity,infinity).
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    using Distance<Real,Vector3<Real> >::m_kClosestPoint0;
    using Distance<Real,Vector3<Real> >::m_kClosestPoint1;

    const Vector3<Real>* m_pkVector;
    const Circle3<Real>* m_pkCircle;
};

typedef DistVector3Circle3<float> DistVector3Circle3f;
typedef DistVector3Circle3<double> DistVector3Circle3d;

}

#endif
