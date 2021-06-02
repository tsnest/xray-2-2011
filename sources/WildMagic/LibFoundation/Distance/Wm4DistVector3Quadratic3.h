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

#ifndef WM4DISTVECTOR3QUADRATIC3_H
#define WM4DISTVECTOR3QUADRATIC3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Distance.h"
#include "Wm4Quadratic3.h"
#include "Wm4Polynomial1.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM DistVector3Quadratic3
    : public Distance<Real,Vector3<Real> >
{
public:
    DistVector3Quadratic3 (const Vector3<Real>& rkVector,
        const Quadratic3<Real>& rkQuadratic);

    // object access
    const Vector3<Real>& GetVector () const;
    const Quadratic3<Real>& GetQuadratic () const;

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

    static void ComputePoly (Real afA[3], Real afB[3], Real afD[3], Real fC,
        Polynomial1<Real>& rkPoly);

    const Vector3<Real>* m_pkVector;
    const Quadratic3<Real>* m_pkQuadratic;
};

typedef DistVector3Quadratic3<float> DistVector3Quadratic3f;
typedef DistVector3Quadratic3<double> DistVector3Quadratic3d;

}

#endif
