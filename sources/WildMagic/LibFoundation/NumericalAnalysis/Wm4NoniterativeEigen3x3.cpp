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

#include "Wm4FoundationPCH.h"
#include "Wm4NoniterativeEigen3x3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
NoniterativeEigen3x3<Real>::NoniterativeEigen3x3 (const Matrix3<Real>& rkA)
{
    // Scale the matrix so its entries are in [-1,1].  The scaling is applied
    // only when at least one matrix entry has magnitude larger than 1.
    Matrix3<Real> kAScaled = rkA;
    Real* afAScaledEntry = (Real*)kAScaled;
    Real fMax = Math<Real>::FAbs(afAScaledEntry[0]);
    Real fAbs = Math<Real>::FAbs(afAScaledEntry[1]);
    if (fAbs > fMax)
    {
        fMax = fAbs;
    }
    fAbs = Math<Real>::FAbs(afAScaledEntry[2]);
    if (fAbs > fMax)
    {
        fMax = fAbs;
    }
    fAbs = Math<Real>::FAbs(afAScaledEntry[4]);
    if (fAbs > fMax)
    {
        fMax = fAbs;
    }
    fAbs = Math<Real>::FAbs(afAScaledEntry[5]);
    if (fAbs > fMax)
    {
        fMax = fAbs;
    }
    fAbs = Math<Real>::FAbs(afAScaledEntry[8]);
    if (fAbs > fMax)
    {
        fMax = fAbs;
    }

    int i;
    if (fMax > (Real)1)
    {
        Real fInvMax = ((Real)1)/fMax;
        for (i = 0; i < 9; i++)
        {
            afAScaledEntry[i] *= fInvMax;
        }
    }

    // Compute the eigenvalues using double-precision arithmetic.
    double adRoot[3];
    ComputeRoots(kAScaled,adRoot);
    m_afEigenvalue[0] = (Real)adRoot[0];
    m_afEigenvalue[1] = (Real)adRoot[1];
    m_afEigenvalue[2] = (Real)adRoot[2];

    Real afMax[3];
    Vector3<Real> akMaxRow[3];
    for (i = 0; i < 3; i++)
    {
        Matrix3<Real> kM = kAScaled;
        kM[0][0] -= m_afEigenvalue[i];
        kM[1][1] -= m_afEigenvalue[i];
        kM[2][2] -= m_afEigenvalue[i];
        if (!PositiveRank(kM,afMax[i],akMaxRow[i]))
        {
            // Rescale back to the original size.
            if (fMax > (Real)1)
            {
                for (int j = 0; j < 3; j++)
                {
                    m_afEigenvalue[j] *= fMax;
                }
            }

            m_akEigenvector[0] = Vector3<Real>::UNIT_X;
            m_akEigenvector[1] = Vector3<Real>::UNIT_Y;
            m_akEigenvector[2] = Vector3<Real>::UNIT_Z;
            return;
        }
    }

    Real fTotalMax = afMax[0];
    i = 0;
    if (afMax[1] > fTotalMax)
    {
        fTotalMax = afMax[1];
        i = 1;
    }
    if (afMax[2] > fTotalMax)
    {
        i = 2;
    }

    if (i == 0)
    {
        akMaxRow[0].Normalize();
        ComputeVectors(kAScaled,akMaxRow[0],1,2,0);
    }
    else if (i == 1)
    {
        akMaxRow[1].Normalize();
        ComputeVectors(kAScaled,akMaxRow[1],2,0,1);
    }
    else
    {
        akMaxRow[2].Normalize();
        ComputeVectors(kAScaled,akMaxRow[2],0,1,2);
    }

    // Rescale back to the original size.
    if (fMax > (Real)1)
    {
        for (i = 0; i < 3; i++)
        {
            m_afEigenvalue[i] *= fMax;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
NoniterativeEigen3x3<Real>::~NoniterativeEigen3x3 ()
{
}
//----------------------------------------------------------------------------
template <class Real>
const Real NoniterativeEigen3x3<Real>::GetEigenvalue (int i) const
{
    assert(0 <= i && i < 3);
    return m_afEigenvalue[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Real* NoniterativeEigen3x3<Real>::GetEigenvalues () const
{
    return m_afEigenvalue;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& NoniterativeEigen3x3<Real>::GetEigenvector (int i) const
{
    assert(0 <= i && i < 3);
    return m_akEigenvector[i];
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>* NoniterativeEigen3x3<Real>::GetEigenvectors () const
{
    return m_akEigenvector;
}
//----------------------------------------------------------------------------
template <class Real>
void NoniterativeEigen3x3<Real>::ComputeRoots (const Matrix3<Real>& rkA,
    double adRoot[3])
{
    // Convert the unique matrix entries to double precision.
    double dA00 = (double)rkA[0][0];
    double dA01 = (double)rkA[0][1];
    double dA02 = (double)rkA[0][2];
    double dA11 = (double)rkA[1][1];
    double dA12 = (double)rkA[1][2];
    double dA22 = (double)rkA[2][2];

    // The characteristic equation is x^3 - c2*x^2 + c1*x - c0 = 0.  The
    // eigenvalues are the roots to this equation, all guaranteed to be
    // real-valued, because the matrix is symmetric.
    double dC0 = dA00*dA11*dA22 + 2.0*dA01*dA02*dA12 - dA00*dA12*dA12 -
        dA11*dA02*dA02 - dA22*dA01*dA01;

    double dC1 = dA00*dA11 - dA01*dA01 + dA00*dA22 - dA02*dA02 +
        dA11*dA22 - dA12*dA12;

    double dC2 = dA00 + dA11 + dA22;

    // Construct the parameters used in classifying the roots of the equation
    // and in solving the equation for the roots in closed form.
    double dC2Div3 = dC2*ms_dInv3;
    double dADiv3 = (dC1 - dC2*dC2Div3)*ms_dInv3;
    if (dADiv3 > 0.0)
    {
        dADiv3 = 0.0;
    }

    double dMBDiv2 = 0.5*(dC0 + dC2Div3*(2.0*dC2Div3*dC2Div3 - dC1));

    double dQ = dMBDiv2*dMBDiv2 + dADiv3*dADiv3*dADiv3;
    if (dQ > 0.0)
    {
        dQ = 0.0;
    }

    // Compute the eigenvalues by solving for the roots of the polynomial.
    double dMagnitude = Mathd::Sqrt(-dADiv3);
    double dAngle = Mathd::ATan2(Mathd::Sqrt(-dQ),dMBDiv2)*ms_dInv3;
    double dCos = Mathd::Cos(dAngle);
    double dSin = Mathd::Sin(dAngle);
    double dRoot0 = dC2Div3 + 2.0*dMagnitude*dCos;
    double dRoot1 = dC2Div3 - dMagnitude*(dCos + ms_dRoot3*dSin);
    double dRoot2 = dC2Div3 - dMagnitude*(dCos - ms_dRoot3*dSin);

    // Sort in increasing order.
    if (dRoot1 >= dRoot0)
    {
        adRoot[0] = dRoot0;
        adRoot[1] = dRoot1;
    }
    else
    {
        adRoot[0] = dRoot1;
        adRoot[1] = dRoot0;
    }

    if (dRoot2 >= adRoot[1])
    {
        adRoot[2] = dRoot2;
    }
    else
    {
        adRoot[2] = adRoot[1];
        if (dRoot2 >= adRoot[0])
        {
            adRoot[1] = dRoot2;
        }
        else
        {
            adRoot[1] = adRoot[0];
            adRoot[0] = dRoot2;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool NoniterativeEigen3x3<Real>::PositiveRank (Matrix3<Real>& rkM,
    Real& rfMax, Vector3<Real>& rkMaxRow) const
{
    // Locate the maximum-magnitude entry of the matrix.
    rfMax = (Real)-1;
    int iRow, iCol, iMaxRow = -1;
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = iRow; iCol < 3; iCol++)
        {
            Real fAbs = Math<Real>::FAbs(rkM[iRow][iCol]);
            if (fAbs > rfMax)
            {
                rfMax = fAbs;
                iMaxRow = iRow;
            }
        }
    }

    // Return the row containing the maximum, to be used for eigenvector
    // construction.
    rkMaxRow = rkM.GetRow(iMaxRow);

    return rfMax >= Math<Real>::ZERO_TOLERANCE;
}
//----------------------------------------------------------------------------
template <class Real>
void NoniterativeEigen3x3<Real>::ComputeVectors (const Matrix3<Real>& rkA,
    Vector3<Real>& rkU2, int i0, int i1, int i2)
{
    Vector3<Real> kU0, kU1;
    Vector3<Real>::GenerateComplementBasis (kU0,kU1,rkU2);

    // V[i2] = c0*U0 + c1*U1,  c0^2 + c1^2=1
    // e2*V[i2] = c0*A*U0 + c1*A*U1
    // e2*c0 = c0*U0.Dot(A*U0) + c1*U0.Dot(A*U1) = d00*c0 + d01*c1
    // e2*c1 = c0*U1.Dot(A*U0) + c1*U1.Dot(A*U1) = d01*c0 + d11*c1
    Vector3<Real> kTmp = rkA*kU0;
    Real fP00 = m_afEigenvalue[i2] - kU0.Dot(kTmp);
    Real fP01 = kU1.Dot(kTmp);
    Real fP11 = m_afEigenvalue[i2] - kU1.Dot(rkA*kU1);
    Real fInvLength;
    Real fMax = Math<Real>::FAbs(fP00);
    int iRow = 0;
    Real fAbs = Math<Real>::FAbs(fP01);
    if (fAbs > fMax)
    {
        fMax = fAbs;
    }
    fAbs = Math<Real>::FAbs(fP11);
    if (fAbs > fMax)
    {
        fMax = fAbs;
        iRow = 1;
    }

    if (fMax >= Math<Real>::ZERO_TOLERANCE)
    {
        if (iRow == 0)
        {
            fInvLength = Math<Real>::InvSqrt(fP00*fP00 + fP01*fP01);
            fP00 *= fInvLength;
            fP01 *= fInvLength;
            m_akEigenvector[i2] = fP01*kU0 + fP00*kU1;
        }
        else
        {
            fInvLength = Math<Real>::InvSqrt(fP11*fP11 + fP01*fP01);
            fP11 *= fInvLength;
            fP01 *= fInvLength;
            m_akEigenvector[i2] = fP11*kU0 + fP01*kU1;
        }
    }
    else
    {
        if (iRow == 0)
        {
            m_akEigenvector[i2] = kU1;
        }
        else
        {
            m_akEigenvector[i2] = kU0;
        }
    }

    // V[i0] = c0*U2 + c1*Cross(U2,V[i2]) = c0*R + c1*S
    // e0*V[i0] = c0*A*R + c1*A*S
    // e0*c0 = c0*R.Dot(A*R) + c1*R.Dot(A*S) = d00*c0 + d01*c1
    // e0*c1 = c0*S.Dot(A*R) + c1*S.Dot(A*S) = d01*c0 + d11*c1
    Vector3<Real> kS = rkU2.Cross(m_akEigenvector[i2]);
    kTmp = rkA*rkU2;
    fP00 = m_afEigenvalue[i0] - rkU2.Dot(kTmp);
    fP01 = kS.Dot(kTmp);
    fP11 = m_afEigenvalue[i0] - kS.Dot(rkA*kS);
    fMax = Math<Real>::FAbs(fP00);
    iRow = 0;
    fAbs = Math<Real>::FAbs(fP01);
    if (fAbs > fMax)
    {
        fMax = fAbs;
    }
    fAbs = Math<Real>::FAbs(fP11);
    if (fAbs > fMax)
    {
        fMax = fAbs;
        iRow = 1;
    }

    if (fMax >= Math<Real>::ZERO_TOLERANCE)
    {
        if (iRow == 0)
        {
            fInvLength = Math<Real>::InvSqrt(fP00*fP00 + fP01*fP01);
            fP00 *= fInvLength;
            fP01 *= fInvLength;
            m_akEigenvector[i0] = fP01*rkU2 + fP00*kS;
        }
        else
        {
            fInvLength = Math<Real>::InvSqrt(fP11*fP11 + fP01*fP01);
            fP11 *= fInvLength;
            fP01 *= fInvLength;
            m_akEigenvector[i0] = fP11*rkU2 + fP01*kS;
        }
    }
    else
    {
        if (iRow == 0)
        {
            m_akEigenvector[i0] = kS;
        }
        else
        {
            m_akEigenvector[i0] = rkU2;
        }
    }

    // V[i1] = Cross(V[i2],V[i0])
    m_akEigenvector[i1] = m_akEigenvector[i2].Cross(m_akEigenvector[i0]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM class NoniterativeEigen3x3<float>;
template<> const double NoniterativeEigen3x3<float>::ms_dInv3 = 1.0/3.0;
template<> const double NoniterativeEigen3x3<float>::ms_dRoot3 =
    Mathd::Sqrt(3.0);

template WM4_FOUNDATION_ITEM class NoniterativeEigen3x3<double>;
template<> const double NoniterativeEigen3x3<double>::ms_dInv3 = 1.0/3.0;
template<> const double NoniterativeEigen3x3<double>::ms_dRoot3 =
    Mathd::Sqrt(3.0);
//----------------------------------------------------------------------------
}
