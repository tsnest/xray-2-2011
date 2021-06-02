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

#ifndef WM4NATURALSPLINE3_H
#define WM4NATURALSPLINE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4MultipleCurve3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM NaturalSpline3 : public MultipleCurve3<Real>
{
public:
    enum BoundaryType
    {
        BT_FREE,
        BT_CLAMPED,
        BT_CLOSED
    };

    // Construction and destruction.  NaturalSpline3 accepts responsibility
    // for deleting the input arrays.
    NaturalSpline3 (BoundaryType eType, int iSegments, Real* afTime,
        Vector3<Real>* akPoint);

    virtual ~NaturalSpline3 ();

    const Vector3<Real>* GetPoints () const;

    virtual Vector3<Real> GetPosition (Real fTime) const;
    virtual Vector3<Real> GetFirstDerivative (Real fTime) const;
    virtual Vector3<Real> GetSecondDerivative (Real fTime) const;
    virtual Vector3<Real> GetThirdDerivative (Real fTime) const;

protected:
    using MultipleCurve3<Real>::m_iSegments;
    using MultipleCurve3<Real>::m_afTime;
    using MultipleCurve3<Real>::GetSpeedWithData;

    void CreateFreeSpline ();
    void CreateClampedSpline ();
    void CreateClosedSpline ();

    virtual Real GetSpeedKey (int iKey, Real fTime) const;
    virtual Real GetLengthKey (int iKey, Real fDT0, Real fDT1) const;
    virtual Real GetVariationKey (int iKey, Real fT0, Real fT1,
        const Vector3<Real>& rkA, const Vector3<Real>& rkB) const;

    Vector3<Real>* m_akA;
    Vector3<Real>* m_akB;
    Vector3<Real>* m_akC;
    Vector3<Real>* m_akD;

    class WM4_FOUNDATION_ITEM ThisPlusKey
    {
    public:
        ThisPlusKey (const NaturalSpline3* pkThis, int iKey)
            :
            This(pkThis),
            Key(iKey)
        {
        }

        const NaturalSpline3* This;
        int Key;
    };
};

typedef NaturalSpline3<float> NaturalSpline3f;
typedef NaturalSpline3<double> NaturalSpline3d;

}

#endif
