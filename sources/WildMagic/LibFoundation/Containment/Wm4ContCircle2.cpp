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
#include "Wm4ContCircle2.h"

// All internal minimal circle calculations store the squared radius in the
// radius member of Circle2.  Only at the end is a sqrt computed.

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
MinCircle2<Real>::MinCircle2 (int iQuantity, const Vector2<Real>* akPoint,
    Circle2<Real>& rkMinimal, Real fEpsilon)
{
    m_fEpsilon = fEpsilon;
    m_aoUpdate[0] = 0;
    m_aoUpdate[1] = &MinCircle2<Real>::UpdateSupport1;
    m_aoUpdate[2] = &MinCircle2<Real>::UpdateSupport2;
    m_aoUpdate[3] = &MinCircle2<Real>::UpdateSupport3;

    Support kSupp;
    Real fDistDiff;

    if (iQuantity >= 1)
    {
        // create identity permutation (0,1,...,iQuantity-1)
        Vector2<Real>** apkPerm = WM4_NEW Vector2<Real>*[iQuantity];
        int i;
        for (i = 0; i < iQuantity; i++)
        {
            apkPerm[i] = (Vector2<Real>*) &akPoint[i];
        }
        
        // generate random permutation
        for (i = iQuantity-1; i > 0; i--)
        {
            int j = rand() % (i+1);
            if (j != i)
            {
                Vector2<Real>* pSave = apkPerm[i];
                apkPerm[i] = apkPerm[j];
                apkPerm[j] = pSave;
            }
        }
        
        rkMinimal = ExactCircle1(*apkPerm[0]);
        kSupp.Quantity = 1;
        kSupp.Index[0] = 0;
        i = 1;
        while (i < iQuantity)
        {
            if (!kSupp.Contains(i,apkPerm,m_fEpsilon))
            {
                if (!Contains(*apkPerm[i],rkMinimal,fDistDiff))
                {
                    UpdateFunction oUpdate = m_aoUpdate[kSupp.Quantity];
                    Circle2<Real> kCir = (this->*oUpdate)(i,apkPerm,kSupp);
                    if (kCir.Radius > rkMinimal.Radius)
                    {
                        rkMinimal = kCir;
                        i = 0;
                        continue;
                    }
                }
            }
            i++;
        }
        
        WM4_DELETE[] apkPerm;
    }
    else
    {
        assert(false);
    }

    rkMinimal.Radius = Math<Real>::Sqrt(rkMinimal.Radius);
}
//----------------------------------------------------------------------------
template <class Real>
bool MinCircle2<Real>::Contains (const Vector2<Real>& rkP,
    const Circle2<Real>& rkC, Real& rfDistDiff)
{
    Vector2<Real> kDiff = rkP - rkC.Center;
    Real fTest = kDiff.SquaredLength();

    // NOTE:  In this algorithm, Circle2 is storing the *squared radius*,
    // so the next line of code is not in error.
    rfDistDiff = fTest - rkC.Radius;

    return rfDistDiff <= (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
Circle2<Real> MinCircle2<Real>::ExactCircle1 (const Vector2<Real>& rkP)
{
    Circle2<Real> kMinimal;
    kMinimal.Center = rkP;
    kMinimal.Radius = (Real)0.0;
    return kMinimal;
}
//----------------------------------------------------------------------------
template <class Real>
Circle2<Real> MinCircle2<Real>::ExactCircle2 (const Vector2<Real>& rkP0,
    const Vector2<Real>& rkP1)
{
    Vector2<Real> kDiff = rkP1 - rkP0;
    Circle2<Real> kMinimal;
    kMinimal.Center = ((Real)0.5)*(rkP0 + rkP1);
    kMinimal.Radius = ((Real)0.25)*kDiff.SquaredLength();
    return kMinimal;
}
//----------------------------------------------------------------------------
template <class Real>
Circle2<Real> MinCircle2<Real>::ExactCircle3 (const Vector2<Real>& rkP0,
    const Vector2<Real>& rkP1, const Vector2<Real>& rkP2)
{
    Vector2<Real> kE10 = rkP1 - rkP0;
    Vector2<Real> kE20 = rkP2 - rkP0;

    Real aafA[2][2] =
    {
        { kE10.X(), kE10.Y() },
        { kE20.X(), kE20.Y() }
    };

    Real afB[2] =
    {
        ((Real)0.5)*kE10.SquaredLength(),
        ((Real)0.5)*kE20.SquaredLength()
    };

    Circle2<Real> kMinimal;
    Real fDet = aafA[0][0]*aafA[1][1] - aafA[0][1]*aafA[1][0];

    if (Math<Real>::FAbs(fDet) > m_fEpsilon)
    {
        Real fInvDet = ((Real)1.0)/fDet;
        Vector2<Real> kQ;
        kQ.X() = (aafA[1][1]*afB[0]-aafA[0][1]*afB[1])*fInvDet;
        kQ.Y() = (aafA[0][0]*afB[1]-aafA[1][0]*afB[0])*fInvDet;
        kMinimal.Center = rkP0 + kQ;
        kMinimal.Radius = kQ.SquaredLength();
    }
    else
    {
        kMinimal.Center = Vector2<Real>::ZERO;
        kMinimal.Radius = Math<Real>::MAX_REAL;
    }

    return kMinimal;
}
//----------------------------------------------------------------------------
template <class Real>
Circle2<Real> MinCircle2<Real>::UpdateSupport1 (int i,
    Vector2<Real>** apkPerm, Support& rkSupp)
{
    const Vector2<Real>& rkP0 = *apkPerm[rkSupp.Index[0]];
    const Vector2<Real>& rkP1 = *apkPerm[i];

    Circle2<Real> kMinimal = ExactCircle2(rkP0,rkP1);
    rkSupp.Quantity = 2;
    rkSupp.Index[1] = i;

    return kMinimal;
}
//----------------------------------------------------------------------------
template <class Real>
Circle2<Real> MinCircle2<Real>::UpdateSupport2 (int i,
    Vector2<Real>** apkPerm, Support& rkSupp)
{
    const Vector2<Real>& rkP0 = *apkPerm[rkSupp.Index[0]];
    const Vector2<Real>& rkP1 = *apkPerm[rkSupp.Index[1]];
    const Vector2<Real>& rkP2 = *apkPerm[i];

    Circle2<Real> akC[3];
    Real fMinRSqr = Math<Real>::MAX_REAL;
    Real fDistDiff;
    int iIndex = -1;

    akC[0] = ExactCircle2(rkP0,rkP2);
    if (Contains(rkP1,akC[0],fDistDiff))
    {
        fMinRSqr = akC[0].Radius;
        iIndex = 0;
    }

    akC[1] = ExactCircle2(rkP1,rkP2);
    if (akC[1].Radius < fMinRSqr && Contains(rkP0,akC[1],fDistDiff))
    {
        fMinRSqr = akC[1].Radius;
        iIndex = 1;
    }

    Circle2<Real> kMinimal;

    if (iIndex != -1)
    {
        kMinimal = akC[iIndex];
        rkSupp.Index[1-iIndex] = i;
    }
    else
    {
        kMinimal = ExactCircle3(rkP0,rkP1,rkP2);
        assert( kMinimal.Radius <= fMinRSqr );
        rkSupp.Quantity = 3;
        rkSupp.Index[2] = i;
    }

    return kMinimal;
}
//----------------------------------------------------------------------------
template <class Real>
Circle2<Real> MinCircle2<Real>::UpdateSupport3 (int i,
    Vector2<Real>** apkPerm, Support& rkSupp)
{
    const Vector2<Real>* apkPt[3] =
    {
        apkPerm[rkSupp.Index[0]],
        apkPerm[rkSupp.Index[1]],
        apkPerm[rkSupp.Index[2]]
    };

    const Vector2<Real>& rkP3 = *apkPerm[i];

    // permutations of type 1
    int aiT1[3][3] =
    {
        {0, /*3*/ 1,2},
        {1, /*3*/ 0,2},
        {2, /*3*/ 0,1}
    };

    // permutations of type 2
    int aiT2[3][3] =
    {
        {0,1, /*3*/ 2},
        {0,2, /*3*/ 1},
        {1,2, /*3*/ 0}
    };

    Circle2<Real> akC[6];
    Real fMinRSqr = Math<Real>::MAX_REAL;
    int iIndex = -1;
    Real fDistDiff, fMinDistDiff = Math<Real>::MAX_REAL;
    int iMinIndex = -1;
    int k = 0;  // sphere index

    // permutations of type 1
    int j;
    for (j = 0; j < 3; j++, k++)
    {
        akC[k] = ExactCircle2(*apkPt[aiT1[j][0]],rkP3);
        if (akC[k].Radius < fMinRSqr)
        {
            if (Contains(*apkPt[aiT1[j][1]],akC[k],fDistDiff)
            &&  Contains(*apkPt[aiT1[j][2]],akC[k],fDistDiff))
            {
                fMinRSqr = akC[k].Radius;
                iIndex = k;
            }
            else if (fDistDiff < fMinDistDiff)
            {
                fMinDistDiff = fDistDiff;
                iMinIndex = k;
            }
        }
    }

    // permutations of type 2
    for (j = 0; j < 3; j++, k++)
    {
        akC[k] = ExactCircle3(*apkPt[aiT2[j][0]],*apkPt[aiT2[j][1]],rkP3);
        if (akC[k].Radius < fMinRSqr)
        {
            if (Contains(*apkPt[aiT2[j][2]],akC[k],fDistDiff))
            {
                fMinRSqr = akC[k].Radius;
                iIndex = k;
            }
            else if (fDistDiff < fMinDistDiff)
            {
                fMinDistDiff = fDistDiff;
                iMinIndex = k;
            }
        }
    }

    // Theoretically, iIndex >= 0 should happen, but floating point round-off
    // error can lead to this.  When this happens, the circle is chosen that
    // has the minimum absolute errors between points (barely) outside the
    // circle and the circle.
    if (iIndex == -1)
    {
        iIndex = iMinIndex;
    }

    Circle2<Real> kMinimal = akC[iIndex];

    switch (iIndex)
    {
    case 0:
        rkSupp.Quantity = 2;
        rkSupp.Index[1] = i;
        break;
    case 1:
        rkSupp.Quantity = 2;
        rkSupp.Index[0] = i;
        break;
    case 2:
        rkSupp.Quantity = 2;
        rkSupp.Index[0] = rkSupp.Index[2];
        rkSupp.Index[1] = i;
        break;
    case 3:
        rkSupp.Index[2] = i;
        break;
    case 4:
        rkSupp.Index[1] = i;
        break;
    case 5:
        rkSupp.Index[0] = i;
        break;
    }

    return kMinimal;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class MinCircle2<float>;

template WM4_FOUNDATION_ITEM
class MinCircle2<double>;
//----------------------------------------------------------------------------
}
