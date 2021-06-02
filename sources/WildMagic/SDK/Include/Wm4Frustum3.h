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

#ifndef WM4FRUSTUM3_H
#define WM4FRUSTUM3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class Frustum3
{
public:
    // Orthogonal frustum.  Let E be the origin, D be the direction vector,
    // U be the up vector, and R be the right vector.  Let u > 0 and r > 0 be
    // the extents in the U and R directions, respectively.  Let n and f be
    // the extents in the D direction with 0 < n < f.  The four corners of the
    // frustum in the near plane are E + n*D + s0*u*U + s1*r*R where |s0| =
    // |s1| = 1 (four choices).  The four corners of the frustum in the far
    // plane are E + f*D + (f/n)*(s0*u*U + s1*r*R) where |s0| = |s1| = 1 (four
    // choices).

    // The default frustum is E = (0,0,0), D = (0,0,-1), U = (0,1,0), and
    // R = (1,0,0), DMin = 1, DMax = 2, UBound = 1, RBound = 1.
    Frustum3 ();
    Frustum3 (const Vector3<Real>& rkOrigin, const Vector3<Real>& rkDVector,
        const Vector3<Real>& rkUVector, const Vector3<Real>& rkRVector,
        Real fDMin, Real fDMax, Real fUBound, Real fRBound);

    // The Update() function must be called whenever changes are made to
    // DMin, DMac, UBound, or RBound.  The values DRatio, MTwoUF, and MTwoRF
    // are dependent on the changes, so call the Get accessors only after the
    // Update() call.
    void Update ();
    Real GetDRatio () const;
    Real GetMTwoUF () const;
    Real GetMTwoRF () const;

    void ComputeVertices (Vector3<Real> akVertex[8]) const;

    Vector3<Real> Origin, DVector, UVector, RVector;
    Real DMin, DMax, UBound, RBound;

protected:
    // derived quantities
    Real m_fDRatio, m_fMTwoUF, m_fMTwoRF;
};

#include "Wm4Frustum3.inl"

typedef Frustum3<float> Frustum3f;
typedef Frustum3<double> Frustum3d;

}

#endif
