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

#ifndef WM4CUBICPOLYNOMIALCURVE2_H
#define WM4CUBICPOLYNOMIALCURVE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4PolynomialCurve2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM CubicPolynomialCurve2
    : public PolynomialCurve2<Real>
{
public:
    // Construction and destruction.  CubicPolynomialCurve2 accepts
    // responsibility for deleting the input polynomials.
    CubicPolynomialCurve2 (Polynomial1<Real>* pkXPoly,
        Polynomial1<Real>* pkYPoly);
    virtual ~CubicPolynomialCurve2 ();

    // tessellation data
    int GetVertexQuantity () const;
    Vector2<Real>* Vertices ();

    // tessellation by recursive subdivision
    void Tessellate (int iLevel);

protected:
    using PolynomialCurve2<Real>::m_fTMin;
    using PolynomialCurve2<Real>::m_fTMax;

    // precomputation
    class WM4_FOUNDATION_ITEM IntervalParameters
    {
    public:
        int m_iI0, m_iI1;
        Vector2<Real> m_akXuu[2];
    };

    // subdivide curve into two halves
    void Subdivide (int iLevel, Real fDSqr, Vector2<Real>* akX,
        IntervalParameters& rkIP);

    // tessellation data
    int m_iVertexQuantity;
    Vector2<Real>* m_akVertex;
};

typedef CubicPolynomialCurve2<float> CubicPolynomialCurve2f;
typedef CubicPolynomialCurve2<double> CubicPolynomialCurve2d;

}

#endif
