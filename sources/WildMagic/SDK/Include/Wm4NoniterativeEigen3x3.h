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
// Version: 4.0.0 (2006/10/09)

#ifndef WM4NONITERATIVEEIGEN3X3_H
#define WM4NONITERATIVEEIGEN3X3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Matrix3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM NoniterativeEigen3x3
{
public:
    // The input matrix A must be symmetric.
    NoniterativeEigen3x3 (const Matrix3<Real>& rkA);
    ~NoniterativeEigen3x3 ();

    // Get the eigenvalues and eigenvectors.  The eigenvalues are stored in
    // increasing order.
    const Real GetEigenvalue (int i) const;
    const Real* GetEigenvalues () const;
    const Vector3<Real>& GetEigenvector (int i) const;
    const Vector3<Real>* GetEigenvectors () const;

private:
    // Compute the roots of the cubic polynomial.  Double precision arithmetic
    // is used to minimize the effects due to subtractive cancellation.  The
    // roots are returned in increasing order.
    void ComputeRoots (const Matrix3<Real>& rkA, double adRoot[3]);

    // Determine if M has positive rank.  The maximum-magnitude entry of M is
    // returned.  The row in which it is contained is also returned.
    bool PositiveRank (Matrix3<Real>& rkM, Real& rfMax,
        Vector3<Real>& rkMaxRow) const;

    // Compute the eigenvectors.
    void ComputeVectors (const Matrix3<Real>& rkA, Vector3<Real>& rkU2,
        int i0, int i1, int i2);

    Real m_afEigenvalue[3];
    Vector3<Real> m_akEigenvector[3];

    // For use by ComputeRoots.
    static const double ms_dInv3;
    static const double ms_dRoot3;
};

typedef NoniterativeEigen3x3<float> NoniterativeEigen3x3f;
typedef NoniterativeEigen3x3<double> NoniterativeEigen3x3d;

}

#endif
