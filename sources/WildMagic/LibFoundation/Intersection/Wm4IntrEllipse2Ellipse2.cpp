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
// Version: 4.0.4 (2007/07/22)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrEllipse2Ellipse2.h"
#include "Wm4IntrBox2Box2.h"
#include "Wm4PolynomialRoots.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrEllipse2Ellipse2<Real>::IntrEllipse2Ellipse2 (
    const Ellipse2<Real>& rkEllipse0, const Ellipse2<Real>& rkEllipse1)
    :
    DIGITS_ACCURACY(10),
    m_pkEllipse0(&rkEllipse0),
    m_pkEllipse1(&rkEllipse1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetEllipse0 () const
{
    return *m_pkEllipse0;
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetEllipse1 () const
{
    return *m_pkEllipse1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrEllipse2Ellipse2<Real>::Test ()
{
    return Find();
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrEllipse2Ellipse2<Real>::Find ()
{
    m_iQuantity = 0;

    // Test for separation of oriented bounding boxes of ellipses.  This is
    // a fast-out attempt.
    Box2<Real> kBox0, kBox1;
    kBox0.Center = m_pkEllipse0->Center;
    kBox0.Axis[0] = m_pkEllipse0->Axis[0];
    kBox0.Axis[1] = m_pkEllipse0->Axis[1];
    kBox0.Extent[0] = m_pkEllipse0->Extent[0];
    kBox0.Extent[1] = m_pkEllipse0->Extent[1];
    kBox1.Center = m_pkEllipse1->Center;
    kBox1.Axis[0] = m_pkEllipse1->Axis[0];
    kBox1.Axis[1] = m_pkEllipse1->Axis[1];
    kBox1.Extent[0] = m_pkEllipse1->Extent[0];
    kBox1.Extent[1] = m_pkEllipse1->Extent[1];
    if(!IntrBox2Box2<Real>(kBox0,kBox1).Test())
    {
        // The boxes do not overlap, so neither do the ellipses.
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    // Compute the 4th-degree polynomial whose roots lead to intersections of
    // the ellipses, and then compute its roots.
    Polynomial1<Real> kPoly = GetQuartic(*m_pkEllipse0,*m_pkEllipse1);
    PolynomialRoots<Real> kPR(Math<Real>::ZERO_TOLERANCE);
    kPR.FindB(kPoly,DIGITS_ACCURACY);
    int iYCount = kPR.GetCount();
    const Real* afYRoot = kPR.GetRoots();
    if (iYCount == 0)
    {
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    // TODO:  Adjust the comments.
    // Compute the coefficients of a polynomial in s = sin(angle) and
    // c = cos(angle) that relates ellipse0 to ellipse1
    // affinely transformed to a circle.  The polynomial is
    //   d0 + d1*c + d2*s + d3*c^2 + d4*c*s + d5*s^2 = 0
    // where c^2 + s^2 = 1.
    Vector2<Real> kC0mC1 = m_pkEllipse0->Center - m_pkEllipse1->Center;
    Matrix2<Real> kM1;
    m_pkEllipse1->GetM(kM1);
    Vector2<Real> kM1C0mC1 = kM1*kC0mC1;
    Vector2<Real> kM1A0 = kM1*m_pkEllipse0->Axis[0];
    Vector2<Real> kM1A1 = kM1*m_pkEllipse0->Axis[1];
    Real afCoeff[6];
    afCoeff[0] = kM1C0mC1.Dot(kC0mC1) - (Real)1.0;
    afCoeff[1] = ((Real)2.0)*m_pkEllipse0->Extent[0]*(kM1A0.Dot(kC0mC1));
    afCoeff[2] = ((Real)2.0)*m_pkEllipse0->Extent[1]*(kM1A1.Dot(kC0mC1));
    afCoeff[3] = m_pkEllipse0->Extent[0]*m_pkEllipse0->Extent[0]*
        (kM1A0.Dot(m_pkEllipse0->Axis[0]));
    afCoeff[4] = ((Real)2.0)*m_pkEllipse0->Extent[0]*m_pkEllipse0->Extent[1]*
        (kM1A0.Dot(m_pkEllipse0->Axis[1]));
    afCoeff[5] = m_pkEllipse0->Extent[1]*m_pkEllipse0->Extent[1]*
        (kM1A1.Dot(m_pkEllipse0->Axis[1]));

    // Evaluate the quadratics, saving the values to test later for closeness
    // to zero and for root polishing.
    Real afQP0[6], afQP1[6];
    m_pkEllipse0->ToCoefficients(afQP0);
    m_pkEllipse1->ToCoefficients(afQP1);
    std::vector<Measurement> kMeasure(8);  // store <x,y,sqrt(Q0^2+S1^2)>
    Vector2<Real> kPoint;
    int i;
    for (int iY = 0; iY < iYCount; iY++)
    {
        kPoint[1] = afYRoot[iY];
        PolynomialRoots<Real> kAR(Math<Real>::ZERO_TOLERANCE);
        Polynomial1<Real> kAPoly(2);
        kAPoly[0] = afQP0[0] + kPoint[1]*(afQP0[2] + kPoint[1]*afQP0[5]);
        kAPoly[1] = afQP0[1] + kPoint[1]*afQP0[4];
        kAPoly[2] = afQP0[3];
        kAR.FindB(kAPoly,DIGITS_ACCURACY);
        int iXCount = kAR.GetCount();
        const Real* afXRoot = kAR.GetRoots();
        for (int iX = 0; iX < iXCount; iX++)
        {
            kPoint[0] = afXRoot[iX];
            Real fQ0 = m_pkEllipse0->Evaluate(kPoint);
            Real fQ1 = m_pkEllipse1->Evaluate(kPoint);

            Real fAngle0;
            bool bTransverse = RefinePoint(afCoeff,kPoint,fQ0,fQ1,fAngle0);

            i = iX + 2*iY;
            kMeasure[i].Point = kPoint;
            kMeasure[i].Q0 = fQ0;
            kMeasure[i].Q1 = fQ1;
            kMeasure[i].Norm = Math<Real>::Sqrt(fQ0*fQ0 + fQ1*fQ1);
            kMeasure[i].Angle0 = fAngle0;
            kMeasure[i].Transverse = bTransverse;
        }
    }
    std::sort(kMeasure.begin(),kMeasure.end());

    for (i = 0; i < 8; i++)
    {
        if (kMeasure[i].Norm < Math<Real>::ZERO_TOLERANCE)
        {
            int j;
            Real fADiff;
            for (j = 0; j < m_iQuantity; j++)
            {
                fADiff = kMeasure[i].Angle0 - kMeasure[j].Angle0;
                if (Math<Real>::FAbs(fADiff) < Math<Real>::ZERO_TOLERANCE)
                {
                    break;
                }
            }
            if (j == m_iQuantity)
            {
                m_akPoint[m_iQuantity] = kMeasure[i].Point;
                m_abTransverse[m_iQuantity] = kMeasure[i].Transverse;
                if (++m_iQuantity == 4)
                {
                    break;
                }
            }
        }
    }

    if (m_iQuantity == 0)
    {
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    m_iIntersectionType = IT_POINT;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrEllipse2Ellipse2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrEllipse2Ellipse2<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipse2<Real>& IntrEllipse2Ellipse2<Real>::GetIntersectionEllipse ()
    const
{
    return *m_pkEllipse0;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrEllipse2Ellipse2<Real>::IsTransverseIntersection (int i) const
{
    return m_abTransverse[i];
}
//----------------------------------------------------------------------------
template <class Real>
Polynomial1<Real> IntrEllipse2Ellipse2<Real>::GetQuartic (
    const Ellipse2<Real>& rkEllipse0, const Ellipse2<Real>& rkEllipse1)
{
    Real afP0[6], afP1[6];
    rkEllipse0.ToCoefficients(afP0);
    rkEllipse1.ToCoefficients(afP1);

    // The polynomials are
    //   P0 = a0 + a1*x + a2*y + a3*x^2 + a4*x*y + a5*y^2
    //      = (a0 + a2*y + a5*y^2) + (a1 + a4*y)*x + (a3)*x^2
    //      = u0(y) + u1(y)*x + u2(y)*x^2
    //   P1 = b0 + b1*x + b2*y + b3*x^2 + b4*x*y + b5*y^2
    //      = (b0 + b2*y + b5*y^2) + (b1 + b4*y)*x + (b3)*x^2
    //      = v0(y) + v1(y)*x + v2(y)*x^2
    // The Bezout determinant eliminates the variable x when solving the
    // equations P0(x,y) = 0 and P1(x,y) = 0.  We have
    //   0 = P0 = u0 + u1*x + u2*x^2
    //   0 = P1 = v0 + v1*x + v2*x^2
    //   0 = v2*P0 - u2*P1 = (u0*v2 - u2*v0) + (u1*v2 - u2*v1)*x
    //   0 = v1*P0 - u1*P1 = (u0*v1 - u1*v0) + (u2*v1 - u1*v2)*x^2
    // Solve the equation 0 = v2*P0-u2*P1 for x and substitute in the other
    // equation and simplify to
    //   Q(y) = (u0*v1-v1*u0)*(u1*v2-u2*v1) - (u0*v2-u2*v0)^2 = 0
    //        = c0 + c1*y + c2*y^2 + c3*y^3 + c4*y^4
    // Define dij = ai*bj - aj*bi for various indices i and j.  For example,
    // d01 = a0*b1-b1*a0.  The coefficients of Q(y) are
    //   c0 = d01*d13 - d30^2
    //   c1 = d01*d43 + (d04+d21)*d13 - 2*d30*d32
    //   c2 = (d04+d21)*d43 + (d24+d51)*d13 - 2*d30*d35 - d32^2
    //   c3 = (d24+d51)*d43 + d54*d13 - 2*d32*d35
    //   c4 = d54*d43 - d35^2

    Real fD01 = afP0[0]*afP1[1] - afP0[1]*afP1[0];
    Real fD04 = afP0[0]*afP1[4] - afP0[4]*afP1[0];
    Real fD13 = afP0[1]*afP1[3] - afP0[3]*afP1[1];
    Real fD21 = afP0[2]*afP1[1] - afP0[1]*afP1[2];
    Real fD24 = afP0[2]*afP1[4] - afP0[4]*afP1[2];
    Real fD30 = afP0[3]*afP1[0] - afP0[0]*afP1[3];
    Real fD32 = afP0[3]*afP1[2] - afP0[2]*afP1[3];
    Real fD35 = afP0[3]*afP1[5] - afP0[5]*afP1[3];
    Real fD43 = afP0[4]*afP1[3] - afP0[3]*afP1[4];
    Real fD51 = afP0[5]*afP1[1] - afP0[1]*afP1[5];
    Real fD54 = afP0[5]*afP1[4] - afP0[4]*afP1[5];
    Real fD04p21 = fD04+fD21;
    Real fD24p51 = fD24+fD51;

    Polynomial1<Real> kPoly(4);
    kPoly[0] = fD01*fD13-fD30*fD30;
    kPoly[1] = fD01*fD43+fD04p21*fD13-((Real)2.0)*fD30*fD32;
    kPoly[2] = fD04p21*fD43+fD24p51*fD13-((Real)2.0)*fD30*fD35-fD32*fD32;
    kPoly[3] = fD24p51*fD43+fD54*fD13-((Real)2.0)*fD32*fD35;
    kPoly[4] = fD54*fD43-fD35*fD35;

    return kPoly;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrEllipse2Ellipse2<Real>::RefinePoint (const Real afCoeff[6],
    Vector2<Real>& rkPoint, Real& rfQ0, Real& rfQ1, Real& rfAngle0)
{
    // The incoming polynomial is
    //   f(angle) = d0 + d1*c + d2*s + d3*c^2 + d4*c*s + d5*s^2
    // where s = sin(angle) and c = cos(angle).  The derivative is
    //   f'(angle) = -d1*s + d2*c + (d5 - d3)*2*c*s + d4*(c^2 - s^2)

    Vector2<Real> kDiff = rkPoint - m_pkEllipse0->Center;
    Real fCos = kDiff.Dot(m_pkEllipse0->Axis[0])/m_pkEllipse0->Extent[0];
    Real fSin = kDiff.Dot(m_pkEllipse0->Axis[1])/m_pkEllipse0->Extent[1];
    Real fA0 = Math<Real>::ATan2(fSin,fCos);
    Real fF0 = afCoeff[0] + afCoeff[1]*fCos + afCoeff[2]*fSin +
        afCoeff[3]*fCos*fCos + afCoeff[4]*fCos*fSin + afCoeff[5]*fSin*fSin;
    Real fDF0 = -afCoeff[1]*fSin + afCoeff[2]*fCos +
        ((Real)2.0)*(afCoeff[5] - afCoeff[3])*fCos*fSin +
        afCoeff[4]*(fCos*fCos - fSin*fSin);

    Real fA1 = (Real)0.0, fF1, fDF1;

    // The value fFValue should match rfQ1 (to within floating-point
    // round-off error).  Try to force fF0 to zero using bisection.  This
    // requires finding an angle such that the corresponding function value
    // is opposite in sign to fF0.  If the search fails, the input point
    // is either a tangential intersection or not an intersection at all.
    int iMaxIterations = 32;
    int i;
    for (i = 0; i < iMaxIterations; i++)
    {
        fA1 = fA0 - fF0/fDF0;
        fCos = Math<Real>::Cos(fA1);
        fSin = Math<Real>::Sin(fA1);
        fF1 = afCoeff[0] + afCoeff[1]*fCos + afCoeff[2]*fSin +
            afCoeff[3]*fCos*fCos + afCoeff[4]*fCos*fSin +
            afCoeff[5]*fSin*fSin;
        if (fF0*fF1 < (Real)0.0)
        {
            // Switch to bisection.
            break;
        }

        fDF1 = -afCoeff[1]*fSin + afCoeff[2]*fCos +
            ((Real)2.0)*(afCoeff[5] - afCoeff[3])*fCos*fSin +
            afCoeff[4]*(fCos*fCos - fSin*fSin);
        if (fDF1*fDF0 < (Real)0.0)
        {
            // Try a steeper slope in hopes of finding an opposite sign
            // value.
            fDF0 *= (Real)2.0;
            continue;
        }

        if (Math<Real>::FAbs(fF1) < Math<Real>::FAbs(fF0))
        {
            // We failed to find an opposite-sign value, but the new
            // function value is closer to zero, so try again with the
            // new value.
            fA0 = fA1;
            fF0 = fF1;
            fDF0 = fDF1;
        }
    }

    Real fAngle = fA0;
    bool bTransverse;
    if (i < iMaxIterations)
    {
        // Apply bisection.  Determine number of iterations to get 10 digits
        // of accuracy.
        Real fTmp0 = Math<Real>::Log(Math<Real>::FAbs(fA1 - fA0));
        Real fTmp1 = ((Real)DIGITS_ACCURACY)*Math<Real>::Log((Real)10.0);
        Real fArg = (fTmp0 + fTmp1)/Math<Real>::Log((Real)2.0);
        iMaxIterations = (int)(fArg + (Real)0.5);
        for (i = 0; i < iMaxIterations; i++)
        {
            fAngle = ((Real)0.5)*(fA0 + fA1);
            fCos = Math<Real>::Cos(fAngle);
            fSin = Math<Real>::Sin(fAngle);
            fF1 = afCoeff[0] + afCoeff[1]*fCos + afCoeff[2]*fSin +
                afCoeff[3]*fCos*fCos + afCoeff[4]*fCos*fSin +
                afCoeff[5]*fSin*fSin;

            Real fProduct = fF0*fF1;
            if (fProduct < (Real)0.0)
            {
                fA1 = fAngle;
            }
            else if (fProduct > (Real)0.0)
            {
                fA0 = fAngle;
                fF0 = fF1;
            }
            else
            {
                break;
            }
        }
        bTransverse = true;
    }
    else
    {
        bTransverse = false;
    }

    rkPoint = m_pkEllipse0->Center +
        m_pkEllipse0->Extent[0]*fCos*m_pkEllipse0->Axis[0] +
        m_pkEllipse0->Extent[1]*fSin*m_pkEllipse0->Axis[1];

    rfQ0 = m_pkEllipse0->Evaluate(rkPoint);
    rfQ1 = m_pkEllipse1->Evaluate(rkPoint);
    rfAngle0 = fAngle;

    return bTransverse;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrEllipse2Ellipse2<float>;

template WM4_FOUNDATION_ITEM
class IntrEllipse2Ellipse2<double>;
//----------------------------------------------------------------------------
}
