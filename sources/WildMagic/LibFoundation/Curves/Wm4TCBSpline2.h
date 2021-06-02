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

#ifndef WM4TCBSPLINE2_H
#define WM4TCBSPLINE2_H

#include "Wm4FoundationLIB.h"
#include "Wm4MultipleCurve2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM TCBSpline2 : public MultipleCurve2<Real>
{
public:
    // Construction and destruction.  TCBSpline2 accepts responsibility for
    // deleting the input arrays.
    TCBSpline2 (int iSegments, Real* afTime, Vector2<Real>* akPoint,
        Real* afTension, Real* afContinuity, Real* afBias);

    virtual ~TCBSpline2 ();

    const Vector2<Real>* GetPoints () const;
    const Real* GetTensions () const;
    const Real* GetContinuities () const;
    const Real* GetBiases () const;

    virtual Vector2<Real> GetPosition (Real fTime) const;
    virtual Vector2<Real> GetFirstDerivative (Real fTime) const;
    virtual Vector2<Real> GetSecondDerivative (Real fTime) const;
    virtual Vector2<Real> GetThirdDerivative (Real fTime) const;

protected:
    using MultipleCurve2<Real>::m_iSegments;
    using MultipleCurve2<Real>::m_afTime;
    using MultipleCurve2<Real>::GetSpeedWithData;

    void ComputePoly (int i0, int i1, int i2, int i3);

    virtual Real GetSpeedKey (int iKey, Real fTime) const;
    virtual Real GetLengthKey (int iKey, Real fT0, Real fT1) const;
    virtual Real GetVariationKey (int iKey, Real fT0, Real fT1,
        const Vector2<Real>& rkA, const Vector2<Real>& rkB) const;

    Vector2<Real>* m_akPoint;
    Real* m_afTension;
    Real* m_afContinuity;
    Real* m_afBias;
    Vector2<Real>* m_akA;
    Vector2<Real>* m_akB;
    Vector2<Real>* m_akC;
    Vector2<Real>* m_akD;

    class WM4_FOUNDATION_ITEM ThisPlusKey
    {
    public:
        ThisPlusKey (const TCBSpline2* pkThis, int iKey)
            :
            This(pkThis),
            Key(iKey)
        {
        }

        const TCBSpline2* This;
        int Key;
    };
};

typedef TCBSpline2<float> TCBSpline2f;
typedef TCBSpline2<double> TCBSpline2d;

}

#endif
