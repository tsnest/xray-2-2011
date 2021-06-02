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

#ifndef WM4CUBICPOLYNOMIALCURVE3_H
#define WM4CUBICPOLYNOMIALCURVE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4PolynomialCurve3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM CubicPolynomialCurve3
    : public PolynomialCurve3<Real>
{
public:
    // Construction and destruction.  CubicPolynomialCurve3 accepts
    // responsibility for deleting the input polynomials.
    CubicPolynomialCurve3 (Polynomial1<Real>* pkXPoly,
        Polynomial1<Real>* pkYPoly, Polynomial1<Real>* pkZPoly);

    virtual ~CubicPolynomialCurve3 ();

    // tessellation data
    int GetVertexQuantity () const;
    Vector3<Real>* Vertices ();

    // tessellation by recursive subdivision
    void Tessellate (int iLevel);

protected:
    using PolynomialCurve3<Real>::m_fTMin;
    using PolynomialCurve3<Real>::m_fTMax;

    // precomputation
    class WM4_FOUNDATION_ITEM IntervalParameters
    {
    public:
        int m_iI0, m_iI1;
        Vector3<Real> m_akXuu[2];
    };

    // subdivide curve into two halves
    void Subdivide (int iLevel, Real fDSqr, Vector3<Real>* akX,
        IntervalParameters& rkIP);

    // tessellation data
    int m_iVertexQuantity;
    Vector3<Real>* m_akVertex;
};

typedef CubicPolynomialCurve3<float> CubicPolynomialCurve3f;
typedef CubicPolynomialCurve3<double> CubicPolynomialCurve3d;

}

#endif
