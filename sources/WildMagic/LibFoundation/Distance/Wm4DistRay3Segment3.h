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

#ifndef WM4DISTRAY3SEGMENT3_H
#define WM4DISTRAY3SEGMENT3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Distance.h"
#include "Wm4Ray3.h"
#include "Wm4Segment3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM DistRay3Segment3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistRay3Segment3 (const Ray3<Real>& rkRay,
        const Segment3<Real>& rkSegment);

    // object access
    const Ray3<Real>& GetRay () const;
    const Segment3<Real>& GetSegment () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // Information about the closest points.
    Real GetRayParameter () const;
    Real GetSegmentParameter () const;

private:
    using Distance<Real,Vector3<Real> >::m_kClosestPoint0;
    using Distance<Real,Vector3<Real> >::m_kClosestPoint1;

    const Ray3<Real>* m_pkRay;
    const Segment3<Real>* m_pkSegment;

    // Information about the closest points.
    Real m_fRayParameter;  // closest0 = ray.origin+param*ray.direction
    Real m_fSegmentParameter;  // closest1 = seg.origin+param*seg.direction
};

typedef DistRay3Segment3<float> DistRay3Segment3f;
typedef DistRay3Segment3<double> DistRay3Segment3d;

}

#endif
