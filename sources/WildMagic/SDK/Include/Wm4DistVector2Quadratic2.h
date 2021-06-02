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

#ifndef WM4DISTVECTOR2QUADRATIC2_H
#define WM4DISTVECTOR2QUADRATIC2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Distance.h"
#include "Wm4Quadratic2.h"
#include "Wm4Polynomial1.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM DistVector2Quadratic2
    : public Distance<Real,Vector2<Real> >
{
public:
    DistVector2Quadratic2 (const Vector2<Real>& rkVector,
        const Quadratic2<Real>& rkQuadratic);

    // object access
    const Vector2<Real>& GetVector () const;
    const Quadratic2<Real>& GetQuadratic () const;

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

    static void ComputePoly (Real afA[2], Real afB[2], Real afD[2],
        Real fC, Polynomial1<Real>& rkPoly);

    const Vector2<Real>* m_pkVector;
    const Quadratic2<Real>* m_pkQuadratic;
};

typedef DistVector2Quadratic2<float> DistVector2Quadratic2f;
typedef DistVector2Quadratic2<double> DistVector2Quadratic2d;

}

#endif
