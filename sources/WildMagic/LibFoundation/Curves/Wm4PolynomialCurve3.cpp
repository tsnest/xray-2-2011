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

#include "Wm4FoundationPCH.h"
#include "Wm4PolynomialCurve3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
PolynomialCurve3<Real>::PolynomialCurve3 (Polynomial1<Real>* pkXPoly,
    Polynomial1<Real>* pkYPoly, Polynomial1<Real>* pkZPoly)
    :
    SingleCurve3<Real>((Real)0.0,(Real)1.0)
{
    assert(pkXPoly && pkYPoly && pkZPoly);
    assert(pkXPoly->GetDegree() == pkYPoly->GetDegree());
    assert(pkXPoly->GetDegree() == pkZPoly->GetDegree());

    m_pkXPoly = pkXPoly;
    m_pkYPoly = pkYPoly;
    m_pkZPoly = pkZPoly;
    m_kXDer1 = m_pkXPoly->GetDerivative();
    m_kYDer1 = m_pkYPoly->GetDerivative();
    m_kZDer1 = m_pkZPoly->GetDerivative();
    m_kXDer2 = m_kXDer1.GetDerivative();
    m_kYDer2 = m_kYDer1.GetDerivative();
    m_kZDer2 = m_kZDer1.GetDerivative();
    m_kXDer3 = m_kXDer2.GetDerivative();
    m_kYDer3 = m_kYDer2.GetDerivative();
    m_kZDer3 = m_kZDer2.GetDerivative();
}
//----------------------------------------------------------------------------
template <class Real>
PolynomialCurve3<Real>::~PolynomialCurve3 ()
{
    WM4_DELETE m_pkXPoly;
    WM4_DELETE m_pkYPoly;
    WM4_DELETE m_pkZPoly;
}
//----------------------------------------------------------------------------
template <class Real>
int PolynomialCurve3<Real>::GetDegree () const
{
    return m_pkXPoly->GetDegree();
}
//----------------------------------------------------------------------------
template <class Real>
const Polynomial1<Real>* PolynomialCurve3<Real>::GetXPolynomial () const
{
    return m_pkXPoly;
}
//----------------------------------------------------------------------------
template <class Real>
const Polynomial1<Real>* PolynomialCurve3<Real>::GetYPolynomial () const
{
    return m_pkYPoly;
}
//----------------------------------------------------------------------------
template <class Real>
const Polynomial1<Real>* PolynomialCurve3<Real>::GetZPolynomial () const
{
    return m_pkZPoly;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> PolynomialCurve3<Real>::GetPosition (Real fTime) const
{
    Vector3<Real> kResult((*m_pkXPoly)(fTime),(*m_pkYPoly)(fTime),
        (*m_pkZPoly)(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> PolynomialCurve3<Real>::GetFirstDerivative (Real fTime) const
{
    Vector3<Real> kResult(m_kXDer1(fTime),m_kYDer1(fTime),m_kZDer1(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> PolynomialCurve3<Real>::GetSecondDerivative (Real fTime) const
{
    Vector3<Real> kResult(m_kXDer2(fTime),m_kYDer2(fTime),m_kZDer2(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> PolynomialCurve3<Real>::GetThirdDerivative (Real fTime) const
{
    Vector3<Real> kResult(m_kXDer3(fTime),m_kYDer3(fTime),m_kZDer3(fTime));
    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real PolynomialCurve3<Real>::GetVariation (Real fT0, Real fT1,
    const Vector3<Real>* pkP0, const Vector3<Real>* pkP1) const
{
    Vector3<Real> kP0, kP1;
    if (!pkP0)
    {
        kP0 = GetPosition(fT0);
        pkP0 = &kP0;
    }
    if (!pkP1)
    {
        kP1 = GetPosition(fT1);
        pkP1 = &kP1;
    }

    // construct line segment A + t*B
    Real fInvDT = ((Real)1.0)/(fT1 - fT0);
    Vector3<Real> kB = fInvDT*(*pkP1 - *pkP0);
    Vector3<Real> kA = *pkP0 - fT0*kB;
    Polynomial1<Real> kLx(1), kLy(1), kLz(1);
    kLx[0] = kA.X();
    kLx[1] = kB.X();
    kLy[0] = kA.Y();
    kLy[1] = kB.Y();
    kLz[0] = kA.Z();
    kLz[1] = kB.Z();

    // compute |X(t) - L(t)|^2
    Polynomial1<Real> kDx = *m_pkXPoly - kLx;
    Polynomial1<Real> kDy = *m_pkYPoly - kLy;
    Polynomial1<Real> kDz = *m_pkZPoly - kLz;
    Polynomial1<Real> kNormSqr = kDx*kDx + kDy*kDy + kDz*kDz;

    // compute indefinite integral of |X(t)-L(t)|^2
    Polynomial1<Real> kIntegral(kNormSqr.GetDegree()+1);
    kIntegral[0] = (Real)0.0;
    for (int i = 1; i <= kIntegral.GetDegree(); i++)
    {
        kIntegral[i] = kNormSqr[i-1]/i;
    }

    // return definite Integral(t0,t1,|X(t)-L(t)|^2)
    Real fResult = kIntegral(fT1) - kIntegral(fT0);
    return fResult;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class PolynomialCurve3<float>;

template WM4_FOUNDATION_ITEM
class PolynomialCurve3<double>;
//----------------------------------------------------------------------------
}
