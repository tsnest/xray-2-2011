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

#ifndef WM4DISTTRIANGLE3RECTANGLE3_H
#define WM4DISTTRIANGLE3RECTANGLE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Distance.h"
#include "Wm4Triangle3.h"
#include "Wm4Rectangle3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM DistTriangle3Rectangle3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistTriangle3Rectangle3 (const Triangle3<Real>& rkTriangle,
        const Rectangle3<Real>& rkRectangle);

    // object access
    const Triangle3<Real>& GetTriangle () const;
    const Rectangle3<Real>& GetRectangle () const;

    // static distance queries
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

    const Triangle3<Real>* m_pkTriangle;
    const Rectangle3<Real>* m_pkRectangle;
};

typedef DistTriangle3Rectangle3<float> DistTriangle3Rectangle3f;
typedef DistTriangle3Rectangle3<double> DistTriangle3Rectangle3d;

}

#endif
