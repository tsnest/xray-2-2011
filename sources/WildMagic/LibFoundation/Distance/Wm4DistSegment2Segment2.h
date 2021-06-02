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

#ifndef WM4DISTSEGMENT2SEGMENT2_H
#define WM4DISTSEGMENT2SEGMENT2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Distance.h"
#include "Wm4Segment2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM DistSegment2Segment2
    : public Distance<Real,Vector2<Real> >
{
public:
    DistSegment2Segment2 (const Segment2<Real>& rkSegment0,
        const Segment2<Real>& rkSegment1);

    // object access
    const Segment2<Real>& GetSegment0 () const;
    const Segment2<Real>& GetSegment1 () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);

private:
    using Distance<Real,Vector2<Real> >::m_kClosestPoint0;
    using Distance<Real,Vector2<Real> >::m_kClosestPoint1;

    const Segment2<Real>* m_pkSegment0;
    const Segment2<Real>* m_pkSegment1;
};

typedef DistSegment2Segment2<float> DistSegment2Segment2f;
typedef DistSegment2Segment2<double> DistSegment2Segment2d;

}

#endif
