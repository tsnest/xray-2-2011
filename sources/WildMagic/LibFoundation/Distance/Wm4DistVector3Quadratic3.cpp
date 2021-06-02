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
// Version: 4.0.1 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4DistVector3Quadratic3.h"
#include "Wm4Eigen.h"
#include "Wm4PolynomialRoots.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Quadratic3<Real>::DistVector3Quadratic3 (
    const Vector3<Real>& rkVector, const Quadratic3<Real>& rkQuadratic)
    :
    m_pkVector(&rkVector),
    m_pkQuadratic(&rkQuadratic)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& DistVector3Quadratic3<Real>::GetVector () const
{
    return *m_pkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Quadratic3<Real>& DistVector3Quadratic3<Real>::GetQuadratic () const
{
    return *m_pkQuadratic;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Quadratic3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Quadratic3<Real>::GetSquared ()
{
    Eigen<Real> kES(3);
    kES(0,0) = (*m_pkQuadratic)[4];
    kES(0,1) = ((Real)0.5)*(*m_pkQuadratic)[5];
    kES(0,2) = ((Real)0.5)*(*m_pkQuadratic)[6];
    kES(1,0) = kES(0,1);
    kES(1,1) = (*m_pkQuadratic)[7];
    kES(1,2) = ((Real)0.5)*(*m_pkQuadratic)[8];
    kES(2,0) = kES(0,2);
    kES(2,1) = kES(1,2);
    kES(2,2) = (*m_pkQuadratic)[9];

    kES.IncrSortEigenStuff3();
    Vector3<Real> akEVec[3];
    kES.GetEigenvector(0,akEVec[0]);
    kES.GetEigenvector(1,akEVec[1]);
    kES.GetEigenvector(2,akEVec[2]);

    Real afA[3], afB[3], afD[3], fC = (*m_pkQuadratic)[0];
    int i;
    for (i = 0; i < 3; i++)
    {
        afA[i] = akEVec[i].X()*m_pkVector->X() + akEVec[i].Y()*m_pkVector->Y()
            + akEVec[i].Z()*m_pkVector->Z();
        afB[i] = akEVec[i].X()*(*m_pkQuadratic)[1] +
            akEVec[i].Y()*(*m_pkQuadratic)[2] +
            akEVec[i].Z()*(*m_pkQuadratic)[3];
        afD[i] = kES.GetEigenvalue(i);
    }

    Polynomial1<Real> kPoly(6);
    ComputePoly(afA,afB,afD,fC,kPoly);

    PolynomialRoots<Real> kPR(Math<Real>::ZERO_TOLERANCE);
    kPR.FindB(kPoly,6);
    int iCount = kPR.GetCount();
    const Real* afRoot = kPR.GetRoots();

    if (iCount > 0)
    {
        Real fMinDistSqr = Math<Real>::MAX_REAL;
        int iMinIndex = -1;
        Real afV[3], fDenom;
        for (int iIndex = 0; iIndex < iCount; iIndex++)
        {
            // compute closest point for this root
            for (i = 0; i < 3; i++)
            {
                fDenom = (Real)1.0 + ((Real)2.0)*afRoot[iIndex]*afD[i];
                afV[i] = (afA[i]-afRoot[iIndex]*afB[i])/fDenom;
            }

            m_kClosestPoint1.X() = akEVec[0][0]*afV[0] + akEVec[1][0]*afV[1]
                + akEVec[2][0]*afV[2];
            m_kClosestPoint1.Y() = akEVec[0][1]*afV[0] + akEVec[1][1]*afV[1]
                + akEVec[2][1]*afV[2];
            m_kClosestPoint1.Z() = akEVec[0][2]*afV[0] + akEVec[1][2]*afV[1]
                + akEVec[2][2]*afV[2];

            // compute squared distance from point to quadric
            Vector3<Real> kDiff = m_kClosestPoint1 - *m_pkVector;
            Real fDistSqr = kDiff.SquaredLength();
            if (fDistSqr < fMinDistSqr)
            {
                fMinDistSqr = fDistSqr;
                iMinIndex = iIndex;
            }
        }

        for (i = 0; i < 3; i++)
        {
            fDenom = (Real)1.0+((Real)2.0)*afRoot[iMinIndex]*afD[i];
            afV[i] = (afA[i]-afRoot[iMinIndex]*afB[i])/fDenom;
        }

        m_kClosestPoint0 = *m_pkVector;
        m_kClosestPoint1.X() = akEVec[0][0]*afV[0] + akEVec[1][0]*afV[1] +
            akEVec[2][0]*afV[2];
        m_kClosestPoint1.Y() = akEVec[0][1]*afV[0] + akEVec[1][1]*afV[1] +
            akEVec[2][1]*afV[2];
        m_kClosestPoint1.Z() = akEVec[0][2]*afV[0] + akEVec[1][2]*afV[1] +
            akEVec[2][2]*afV[2];
        return fMinDistSqr;
    }
    else
    {
        // should not happen
        assert(false);
        return -(Real)1.0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Quadratic3<Real>::Get (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Quadratic3<Real> kMQuadratic = m_pkQuadratic->Translate(fT*rkVelocity1);
    return DistVector3Quadratic3<Real>(kMVector,kMQuadratic).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Quadratic3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMVector = *m_pkVector + fT*rkVelocity0;
    Quadratic3<Real> kMQuadratic = m_pkQuadratic->Translate(fT*rkVelocity1);
    return DistVector3Quadratic3<Real>(kMVector,kMQuadratic).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
void DistVector3Quadratic3<Real>::ComputePoly (Real afA[3], Real afB[3],
    Real afD[3], Real fC, Polynomial1<Real>& rkPoly)
{
    Real afBPad[3] =
    {
        afB[0]+afA[0]*afD[0],
        afB[1]+afA[1]*afD[1],
        afB[2]+afA[2]*afD[2]
    };

    Real afBSqr[3] =
    {
        afB[0]*afB[0],
        afB[1]*afB[1],
        afB[2]*afB[2]
    };

    Real afDSum[4] =
    {
        afD[0]+afD[1],
        afD[0]+afD[2],
        afD[1]+afD[2],
        afD[0]+afD[1]+afD[2]
    };

    Real afDPrd[4] =
    {
        afD[0]*afD[1],
        afD[0]*afD[2],
        afD[1]*afD[2],
        afD[0]*afD[1]*afD[2]
    };

    Real afDSqr[3] =
    {
        afD[0]*afD[0],
        afD[1]*afD[1],
        afD[2]*afD[2]
    };

    rkPoly[0] = afA[0]*afBPad[0]+afA[1]*afBPad[1]+afA[2]*afBPad[2]+fC;

    rkPoly[1] = - afBSqr[0] - afBSqr[1] - afBSqr[2] + ((Real)4.0)*(
        afA[0]*afBPad[0]*afDSum[2] + afA[1]*afBPad[1]*afDSum[1] +
        afA[2]*afBPad[2]*afDSum[0] + fC*afDSum[3]);

    rkPoly[2] = - afBSqr[0]*(afD[0] + ((Real)4.0)*afDSum[2])
        - afBSqr[1]*(afD[1] + ((Real)4.0)*afDSum[1]) - afBSqr[2]*(afD[2] +
        ((Real)4.0)*afDSum[0]) + ((Real)4.0)*(afA[0]*afBPad[0]*(
        afDSum[2]*afDSum[2]+2*afDPrd[2]) +
        afA[1]*afBPad[1]*(afDSum[1]*afDSum[1]+2*afDPrd[1]) +
        afA[2]*afBPad[2]*(afDSum[0]*afDSum[0]+2*afDPrd[0]) +
        fC*(afDSqr[0]+afDSqr[1]+afDSqr[2]+((Real)4.0)*(
        afDPrd[0]+afDPrd[1]+afDPrd[2])));

    rkPoly[3] =
        - afBSqr[0]*(afD[1]*afDSum[0]+afD[2]*afDSum[1]+((Real)4.0)*afDPrd[2])
        - afBSqr[1]*(afD[0]*afDSum[0]+afD[2]*afDSum[2]+((Real)4.0)*afDPrd[1])
        - afBSqr[2]*(afD[0]*afDSum[1]+afD[1]*afDSum[2]+((Real)4.0)*afDPrd[0])
        + ((Real)4.0)*(afA[0]*afDPrd[2]*afBPad[0]*afDSum[2] +
        afA[1]*afDPrd[1]*afBPad[1]*afDSum[1] +
        afA[2]*afDPrd[0]*afBPad[2]*afDSum[0] +
        fC*(afDSqr[0]*afDSum[2]+afDSqr[1]*afDSum[1]+afDSqr[2]*afDSum[0]+
        ((Real)4.0)*afDPrd[3]));

    rkPoly[3] *= (Real)4.0;

    rkPoly[4] =
        - afBSqr[0]*(afD[0]*(afDSqr[1]+afDSqr[2])
        + ((Real)4.0)*afDPrd[2]*afDSum[3])
        - afBSqr[1]*(afD[1]*(afDSqr[0]+afDSqr[2])
        + ((Real)4.0)*afDPrd[1]*afDSum[3])
        - afBSqr[2]*(afD[2]*(afDSqr[0]+afDSqr[1])
        + ((Real)4.0)*afDPrd[0]*afDSum[3])
        + ((Real)4.0)*(afA[0]*afDSqr[1]*afDSqr[2]*afBPad[0] +
        afA[1]*afDSqr[0]*afDSqr[2]*afBPad[1] +
        afA[2]*afDSqr[0]*afDSqr[1]*afBPad[2] +
        fC*(afDSqr[0]*afDSqr[1]+afDSqr[0]*afDSqr[2]+afDSqr[1]*afDSqr[2]
        + ((Real)4.0)*afDPrd[3]*afDSum[3]));

    rkPoly[4] *= (Real)4.0;

    rkPoly[5] = ((Real)16.0)*(afDPrd[0]+afDPrd[1]+afDPrd[2])*(
        - afBSqr[0]*afDPrd[2] - afBSqr[1]*afDPrd[1] - afBSqr[2]*afDPrd[0] +
        ((Real)4.0)*fC*afDPrd[3]);

    rkPoly[6] = ((Real)16.0)*afDPrd[3]*(- afBSqr[0]*afDPrd[2]
        - afBSqr[1]*afDPrd[1] - afBSqr[2]*afDPrd[0]
        + ((Real)4.0)*fC*afDPrd[3]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistVector3Quadratic3<float>;

template WM4_FOUNDATION_ITEM
class DistVector3Quadratic3<double>;
//----------------------------------------------------------------------------
}
