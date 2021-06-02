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
#include "Wm4DistLine3Box3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
DistLine3Box3<Real>::DistLine3Box3 (const Line3<Real>& rkLine,
    const Box3<Real>& rkBox)
    :
    m_pkLine(&rkLine),
    m_pkBox(&rkBox)
{
    m_fLParam = (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& DistLine3Box3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Box3<Real>& DistLine3Box3<Real>::GetBox () const
{
    return *m_pkBox;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Box3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Box3<Real>::GetSquared ()
{
    // compute coordinates of line in box coordinate system
    Vector3<Real> kDiff = m_pkLine->Origin - m_pkBox->Center;
    Vector3<Real> kPnt(
        kDiff.Dot(m_pkBox->Axis[0]),
        kDiff.Dot(m_pkBox->Axis[1]),
        kDiff.Dot(m_pkBox->Axis[2]));
    Vector3<Real> kDir(
        m_pkLine->Direction.Dot(m_pkBox->Axis[0]),
        m_pkLine->Direction.Dot(m_pkBox->Axis[1]),
        m_pkLine->Direction.Dot(m_pkBox->Axis[2]));

    // Apply reflections so that direction vector has nonnegative components.
    bool bReflect[3];
    int i;
    for (i = 0; i < 3; i++)
    {
        if (kDir[i] < (Real)0.0)
        {
            kPnt[i] = -kPnt[i];
            kDir[i] = -kDir[i];
            bReflect[i] = true;
        }
        else
        {
            bReflect[i] = false;
        }
    }

    Real fSqrDistance = (Real)0.0;
    m_fLParam = (Real)0.0;  // parameter for closest point on line

    if (kDir.X() > (Real)0.0)
    {
        if (kDir.Y() > (Real)0.0)
        {
            if (kDir.Z() > (Real)0.0)  // (+,+,+)
            {
                CaseNoZeros(kPnt,kDir,fSqrDistance);
            }
            else  // (+,+,0)
            {
                Case0(0,1,2,kPnt,kDir,fSqrDistance);
            }
        }
        else
        {
            if (kDir.Z() > (Real)0.0)  // (+,0,+)
            {
                Case0(0,2,1,kPnt,kDir,fSqrDistance);
            }
            else  // (+,0,0)
            {
                Case00(0,1,2,kPnt,kDir,fSqrDistance);
            }
        }
    }
    else
    {
        if (kDir.Y() > (Real)0.0)
        {
            if (kDir.Z() > (Real)0.0)  // (0,+,+)
            {
                Case0(1,2,0,kPnt,kDir,fSqrDistance);
            }
            else  // (0,+,0)
            {
                Case00(1,0,2,kPnt,kDir,fSqrDistance);
            }
        }
        else
        {
            if (kDir.Z() > (Real)0.0)  // (0,0,+)
            {
                Case00(2,0,1,kPnt,kDir,fSqrDistance);
            }
            else  // (0,0,0)
            {
                Case000(kPnt,fSqrDistance);
            }
        }
    }

    // compute closest point on line
    m_kClosestPoint0 = m_pkLine->Origin + m_fLParam*m_pkLine->Direction;

    // compute closest point on box
    m_kClosestPoint1 = m_pkBox->Center;
    for (i = 0; i < 3; i++)
    {
        // undo the reflections applied previously
        if (bReflect[i])
        {
            kPnt[i] = -kPnt[i];
        }

        m_kClosestPoint1 += kPnt[i]*m_pkBox->Axis[i];
    }

    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Box3<Real>::Get (Real fT, const Vector3<Real>& rkVelocity0,
    const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkLine->Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkBox->Center + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_pkLine->Direction);
    Box3<Real> kMBox(kMCenter,m_pkBox->Axis,m_pkBox->Extent);
    return DistLine3Box3<Real>(kMLine,kMBox).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Box3<Real>::GetSquared (Real fT,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin = m_pkLine->Origin + fT*rkVelocity0;
    Vector3<Real> kMCenter = m_pkBox->Center + fT*rkVelocity1;
    Line3<Real> kMLine(kMOrigin,m_pkLine->Direction);
    Box3<Real> kMBox(kMCenter,m_pkBox->Axis,m_pkBox->Extent);
    return DistLine3Box3<Real>(kMLine,kMBox).GetSquared();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistLine3Box3<Real>::GetLineParameter () const
{
    return m_fLParam;
}
//----------------------------------------------------------------------------
template <class Real>
void DistLine3Box3<Real>::Face (int i0, int i1, int i2, Vector3<Real>& rkPnt,
    const Vector3<Real>& rkDir, const Vector3<Real>& rkPmE,
    Real& rfSqrDistance)
{
    Vector3<Real> kPpE;
    Real fLSqr, fInv, fTmp, fParam, fT, fDelta;

    kPpE[i1] = rkPnt[i1] + m_pkBox->Extent[i1];
    kPpE[i2] = rkPnt[i2] + m_pkBox->Extent[i2];
    if (rkDir[i0]*kPpE[i1] >= rkDir[i1]*rkPmE[i0])
    {
        if (rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0])
        {
            // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
            rkPnt[i0] = m_pkBox->Extent[i0];
            fInv = ((Real)1.0)/rkDir[i0];
            rkPnt[i1] -= rkDir[i1]*rkPmE[i0]*fInv;
            rkPnt[i2] -= rkDir[i2]*rkPmE[i0]*fInv;
            m_fLParam = -rkPmE[i0]*fInv;
        }
        else
        {
            // v[i1] >= -e[i1], v[i2] < -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i2]*rkDir[i2];
            fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i2]*kPpE[i2]);
            if (fTmp <= ((Real)2.0)*fLSqr*m_pkBox->Extent[i1])
            {
                fT = fTmp/fLSqr;
                fLSqr += rkDir[i1]*rkDir[i1];
                fTmp = kPpE[i1] - fT;
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
                    rkDir[i2]*kPpE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
                    kPpE[i2]*kPpE[i2] + fDelta*fParam;

                m_fLParam = fParam;
                rkPnt[i0] = m_pkBox->Extent[i0];
                rkPnt[i1] = fT - m_pkBox->Extent[i1];
                rkPnt[i2] = -m_pkBox->Extent[i2];
            }
            else
            {
                fLSqr += rkDir[i1]*rkDir[i1];
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
                    rkDir[i2]*kPpE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1] +
                    kPpE[i2]*kPpE[i2] + fDelta*fParam;

                m_fLParam = fParam;
                rkPnt[i0] = m_pkBox->Extent[i0];
                rkPnt[i1] = m_pkBox->Extent[i1];
                rkPnt[i2] = -m_pkBox->Extent[i2];
            }
        }
    }
    else
    {
        if (rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0])
        {
            // v[i1] < -e[i1], v[i2] >= -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
            fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i1]*kPpE[i1]);
            if (fTmp <= ((Real)2.0)*fLSqr*m_pkBox->Extent[i2])
            {
                fT = fTmp/fLSqr;
                fLSqr += rkDir[i2]*rkDir[i2];
                fTmp = kPpE[i2] - fT;
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                    rkDir[i2]*fTmp;
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                    fTmp*fTmp + fDelta*fParam;

                m_fLParam = fParam;
                rkPnt[i0] = m_pkBox->Extent[i0];
                rkPnt[i1] = -m_pkBox->Extent[i1];
                rkPnt[i2] = fT - m_pkBox->Extent[i2];
            }
            else
            {
                fLSqr += rkDir[i2]*rkDir[i2];
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                    rkDir[i2]*rkPmE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                    rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

                m_fLParam = fParam;
                rkPnt[i0] = m_pkBox->Extent[i0];
                rkPnt[i1] = -m_pkBox->Extent[i1];
                rkPnt[i2] = m_pkBox->Extent[i2];
            }
        }
        else
        {
            // v[i1] < -e[i1], v[i2] < -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0]+rkDir[i2]*rkDir[i2];
            fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i2]*kPpE[i2]);
            if (fTmp >= (Real)0.0)
            {
                // v[i1]-edge is closest
                if (fTmp <= ((Real)2.0)*fLSqr*m_pkBox->Extent[i1])
                {
                    fT = fTmp/fLSqr;
                    fLSqr += rkDir[i1]*rkDir[i1];
                    fTmp = kPpE[i1] - fT;
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
                        rkDir[i2]*kPpE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
                        kPpE[i2]*kPpE[i2] + fDelta*fParam;

                    m_fLParam = fParam;
                    rkPnt[i0] = m_pkBox->Extent[i0];
                    rkPnt[i1] = fT - m_pkBox->Extent[i1];
                    rkPnt[i2] = -m_pkBox->Extent[i2];
                }
                else
                {
                    fLSqr += rkDir[i1]*rkDir[i1];
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
                        rkDir[i2]*kPpE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1]
                        + kPpE[i2]*kPpE[i2] + fDelta*fParam;

                    m_fLParam = fParam;
                    rkPnt[i0] = m_pkBox->Extent[i0];
                    rkPnt[i1] = m_pkBox->Extent[i1];
                    rkPnt[i2] = -m_pkBox->Extent[i2];
                }
                return;
            }

            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
            fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i1]*kPpE[i1]);
            if (fTmp >= (Real)0.0)
            {
                // v[i2]-edge is closest
                if (fTmp <= ((Real)2.0)*fLSqr*m_pkBox->Extent[i2])
                {
                    fT = fTmp/fLSqr;
                    fLSqr += rkDir[i2]*rkDir[i2];
                    fTmp = kPpE[i2] - fT;
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                        rkDir[i2]*fTmp;
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                        fTmp*fTmp + fDelta*fParam;

                    m_fLParam = fParam;
                    rkPnt[i0] = m_pkBox->Extent[i0];
                    rkPnt[i1] = -m_pkBox->Extent[i1];
                    rkPnt[i2] = fT - m_pkBox->Extent[i2];
                }
                else
                {
                    fLSqr += rkDir[i2]*rkDir[i2];
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                        rkDir[i2]*rkPmE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                        rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

                    m_fLParam = fParam;
                    rkPnt[i0] = m_pkBox->Extent[i0];
                    rkPnt[i1] = -m_pkBox->Extent[i1];
                    rkPnt[i2] = m_pkBox->Extent[i2];
                }
                return;
            }

            // (v[i1],v[i2])-corner is closest
            fLSqr += rkDir[i2]*rkDir[i2];
            fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                rkDir[i2]*kPpE[i2];
            fParam = -fDelta/fLSqr;
            rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                kPpE[i2]*kPpE[i2] + fDelta*fParam;

            m_fLParam = fParam;
            rkPnt[i0] = m_pkBox->Extent[i0];
            rkPnt[i1] = -m_pkBox->Extent[i1];
            rkPnt[i2] = -m_pkBox->Extent[i2];
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void DistLine3Box3<Real>::CaseNoZeros (Vector3<Real>& rkPnt,
    const Vector3<Real>& rkDir, Real& rfSqrDistance)
{
    Vector3<Real> kPmE(
        rkPnt.X() - m_pkBox->Extent[0],
        rkPnt.Y() - m_pkBox->Extent[1],
        rkPnt.Z() - m_pkBox->Extent[2]);

    Real fProdDxPy = rkDir.X()*kPmE.Y();
    Real fProdDyPx = rkDir.Y()*kPmE.X();
    Real fProdDzPx, fProdDxPz, fProdDzPy, fProdDyPz;

    if (fProdDyPx >= fProdDxPy)
    {
        fProdDzPx = rkDir.Z()*kPmE.X();
        fProdDxPz = rkDir.X()*kPmE.Z();
        if (fProdDzPx >= fProdDxPz)
        {
            // line intersects x = e0
            Face(0,1,2,rkPnt,rkDir,kPmE,rfSqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(2,0,1,rkPnt,rkDir,kPmE,rfSqrDistance);
        }
    }
    else
    {
        fProdDzPy = rkDir.Z()*kPmE.Y();
        fProdDyPz = rkDir.Y()*kPmE.Z();
        if (fProdDzPy >= fProdDyPz)
        {
            // line intersects y = e1
            Face(1,2,0,rkPnt,rkDir,kPmE,rfSqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(2,0,1,rkPnt,rkDir,kPmE,rfSqrDistance);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void DistLine3Box3<Real>::Case0 (int i0, int i1, int i2, Vector3<Real>& rkPnt,
    const Vector3<Real>& rkDir, Real& rfSqrDistance)
{
    Real fPmE0 = rkPnt[i0] - m_pkBox->Extent[i0];
    Real fPmE1 = rkPnt[i1] - m_pkBox->Extent[i1];
    Real fProd0 = rkDir[i1]*fPmE0;
    Real fProd1 = rkDir[i0]*fPmE1;
    Real fDelta, fInvLSqr, fInv;

    if (fProd0 >= fProd1)
    {
        // line intersects P[i0] = e[i0]
        rkPnt[i0] = m_pkBox->Extent[i0];

        Real fPpE1 = rkPnt[i1] + m_pkBox->Extent[i1];
        fDelta = fProd0 - rkDir[i0]*fPpE1;
        if (fDelta >= (Real)0.0)
        {
            fInvLSqr = ((Real)1.0)/(rkDir[i0]*rkDir[i0]+rkDir[i1]*rkDir[i1]);
            rfSqrDistance += fDelta*fDelta*fInvLSqr;
            rkPnt[i1] = -m_pkBox->Extent[i1];
            m_fLParam = -(rkDir[i0]*fPmE0+rkDir[i1]*fPpE1)*fInvLSqr;
        }
        else
        {
            fInv = ((Real)1.0)/rkDir[i0];
            rkPnt[i1] -= fProd0*fInv;
            m_fLParam = -fPmE0*fInv;
        }
    }
    else
    {
        // line intersects P[i1] = e[i1]
        rkPnt[i1] = m_pkBox->Extent[i1];

        Real fPpE0 = rkPnt[i0] + m_pkBox->Extent[i0];
        fDelta = fProd1 - rkDir[i1]*fPpE0;
        if (fDelta >= (Real)0.0)
        {
            fInvLSqr = ((Real)1.0)/(rkDir[i0]*rkDir[i0]+rkDir[i1]*rkDir[i1]);
            rfSqrDistance += fDelta*fDelta*fInvLSqr;
            rkPnt[i0] = -m_pkBox->Extent[i0];
            m_fLParam = -(rkDir[i0]*fPpE0+rkDir[i1]*fPmE1)*fInvLSqr;
        }
        else
        {
            fInv = ((Real)1.0)/rkDir[i1];
            rkPnt[i0] -= fProd1*fInv;
            m_fLParam = -fPmE1*fInv;
        }
    }

    if (rkPnt[i2] < -m_pkBox->Extent[i2])
    {
        fDelta = rkPnt[i2] + m_pkBox->Extent[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = -m_pkBox->Extent[i2];
    }
    else if (rkPnt[i2] > m_pkBox->Extent[i2])
    {
        fDelta = rkPnt[i2] - m_pkBox->Extent[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = m_pkBox->Extent[i2];
    }
}
//----------------------------------------------------------------------------
template <class Real>
void DistLine3Box3<Real>::Case00 (int i0, int i1, int i2,
    Vector3<Real>& rkPnt, const Vector3<Real>& rkDir,  Real& rfSqrDistance)
{
    Real fDelta;

    m_fLParam = (m_pkBox->Extent[i0] - rkPnt[i0])/rkDir[i0];

    rkPnt[i0] = m_pkBox->Extent[i0];

    if (rkPnt[i1] < -m_pkBox->Extent[i1])
    {
        fDelta = rkPnt[i1] + m_pkBox->Extent[i1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = -m_pkBox->Extent[i1];
    }
    else if (rkPnt[i1] > m_pkBox->Extent[i1])
    {
        fDelta = rkPnt[i1] - m_pkBox->Extent[i1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = m_pkBox->Extent[i1];
    }

    if (rkPnt[i2] < -m_pkBox->Extent[i2])
    {
        fDelta = rkPnt[i2] + m_pkBox->Extent[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = -m_pkBox->Extent[i2];
    }
    else if (rkPnt[i2] > m_pkBox->Extent[i2])
    {
        fDelta = rkPnt[i2] - m_pkBox->Extent[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = m_pkBox->Extent[i2];
    }
}
//----------------------------------------------------------------------------
template <class Real>
void DistLine3Box3<Real>::Case000 (Vector3<Real>& rkPnt, Real& rfSqrDistance)
{
    Real fDelta;

    if (rkPnt.X() < -m_pkBox->Extent[0])
    {
        fDelta = rkPnt.X() + m_pkBox->Extent[0];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.X() = -m_pkBox->Extent[0];
    }
    else if (rkPnt.X() > m_pkBox->Extent[0])
    {
        fDelta = rkPnt.X() - m_pkBox->Extent[0];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.X() = m_pkBox->Extent[0];
    }

    if (rkPnt.Y() < -m_pkBox->Extent[1])
    {
        fDelta = rkPnt.Y() + m_pkBox->Extent[1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.Y() = -m_pkBox->Extent[1];
    }
    else if (rkPnt.Y() > m_pkBox->Extent[1])
    {
        fDelta = rkPnt.Y() - m_pkBox->Extent[1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.Y() = m_pkBox->Extent[1];
    }

    if (rkPnt.Z() < -m_pkBox->Extent[2])
    {
        fDelta = rkPnt.Z() + m_pkBox->Extent[2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.Z() = -m_pkBox->Extent[2];
    }
    else if (rkPnt.Z() > m_pkBox->Extent[2])
    {
        fDelta = rkPnt.Z() - m_pkBox->Extent[2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt.Z() = m_pkBox->Extent[2];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class DistLine3Box3<float>;

template WM4_FOUNDATION_ITEM
class DistLine3Box3<double>;
//----------------------------------------------------------------------------
}
