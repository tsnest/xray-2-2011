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

#ifndef WM4DISTLINE2SEGMENT2_H
#define WM4DISTLINE2SEGMENT2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Distance.h"
#include "Wm4Line2.h"
#include "Wm4Segment2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM DistLine2Segment2
    : public Distance<Real,Vector2<Real> >
{
public:
    DistLine2Segment2 (const Line2<Real>& rkLine,
        const Segment2<Real>& rkSegment);

    // object access
    const Line2<Real>& GetLine () const;
    const Segment2<Real>& GetSegment () const;

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

    const Line2<Real>* m_pkLine;
    const Segment2<Real>* m_pkSegment;
};

typedef DistLine2Segment2<float> DistLine2Segment2f;
typedef DistLine2Segment2<double> DistLine2Segment2d;

}

#endif
