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

#ifndef WM4INTPBSPLINE4_H
#define WM4INTPBSPLINE4_H

#include "Wm4FoundationLIB.h"
#include "Wm4IntpBSplineUniform.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntpBSplineUniform4
    : public IntpBSplineUniform<Real>
{
public:
    // Construction.  IntpBSplineUniform4 accepts responsibility for
    // deleting the input array afData.
    IntpBSplineUniform4 (int iDegree, const int* aiDim, Real* afData);

    int Index (int iX, int iY, int iZ, int iW) const;

    // spline evaluation for function interpolation (no derivatives)
    Real operator() (Real fX, Real fY, Real fZ, Real fW);
    virtual Real operator() (Real* afX);

    // spline evaluation, derivative counts given in iDx, iDy, iDz, iDw,
    // aiDx[]
    Real operator() (int iDx, int iDy, int iDz, int iDw, Real fX, Real fY,
        Real fZ, Real fW);
    virtual Real operator() (int* aiDx, Real* afX);

private:
    using IntpBSplineUniform<Real>::m_iDegree;
    using IntpBSplineUniform<Real>::m_iDp1;
    using IntpBSplineUniform<Real>::m_iDp1ToN;
    using IntpBSplineUniform<Real>::m_aiDim;
    using IntpBSplineUniform<Real>::m_afData;
    using IntpBSplineUniform<Real>::m_aiGridMin;
    using IntpBSplineUniform<Real>::m_aiGridMax;
    using IntpBSplineUniform<Real>::m_aiBase;
    using IntpBSplineUniform<Real>::m_aiOldBase;
    using IntpBSplineUniform<Real>::m_aafMatrix;
    using IntpBSplineUniform<Real>::m_afCache;
    using IntpBSplineUniform<Real>::m_afInter;
    using IntpBSplineUniform<Real>::m_aafPoly;
    using IntpBSplineUniform<Real>::m_afProduct;
    using IntpBSplineUniform<Real>::m_aiSkip;
    using IntpBSplineUniform<Real>::m_oEvaluateCallback;

    void EvaluateUnknownData ();
    void ComputeIntermediate ();
};

typedef IntpBSplineUniform4<float> IntpBSplineUniform4f;
typedef IntpBSplineUniform4<double> IntpBSplineUniform4d;

}

#endif
