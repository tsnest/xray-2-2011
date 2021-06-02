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

//----------------------------------------------------------------------------
template <class Real>
Arc2<Real>::Arc2 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Arc2<Real>::Arc2 (const Vector2<Real>& rkCenter, Real fRadius,
    const Vector2<Real>& rkEnd0, const Vector2<Real>& rkEnd1)
    :
    Circle2<Real>(rkCenter,fRadius),
    End0(rkEnd0),
    End1(rkEnd1)
{
}
//----------------------------------------------------------------------------
template <class Real>
bool Arc2<Real>::Contains (const Vector2<Real>& rkP) const
{
    // Assert: |P-C| = R where P is the input point, C is the circle center,
    // and R is the circle radius.  For P to be on the arc from A to B, it
    // must be on the side of the plane containing A with normal N = Perp(B-A)
    // where Perp(u,v) = (v,-u).

    Vector2<Real> kPmE0 = rkP - End0;
    Vector2<Real> kE1mE0 = End1 - End0;
    Real fDotPerp = kPmE0.DotPerp(kE1mE0);
    return fDotPerp >= (Real)0.0;
}
//----------------------------------------------------------------------------
