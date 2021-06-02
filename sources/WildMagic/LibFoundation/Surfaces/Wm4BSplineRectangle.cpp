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
#include "Wm4BSplineRectangle.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
BSplineRectangle<Real>::BSplineRectangle (int iNumUCtrlPoints,
    int iNumVCtrlPoints, Vector3<Real>** aakCtrlPoint, int iUDegree,
    int iVDegree, bool bULoop, bool bVLoop, bool bUOpen, bool bVOpen)
    :
    ParametricSurface<Real>((Real)0.0,(Real)1.0,(Real)0.0,(Real)1.0,true)
{
    assert(iNumUCtrlPoints >= 2);
    assert(1 <= iUDegree && iUDegree <= iNumUCtrlPoints-1);
    assert(iNumVCtrlPoints >= 2);
    assert(1 <= iVDegree && iVDegree <= iNumVCtrlPoints-1);

    m_abLoop[0] = bULoop;
    m_abLoop[1] = bVLoop;

    m_iNumUCtrlPoints = iNumUCtrlPoints;
    m_iNumVCtrlPoints = iNumVCtrlPoints;
    m_iUReplicate = (bULoop ? (bUOpen ? 1 : iUDegree) : 0);
    m_iVReplicate = (bVLoop ? (bVOpen ? 1 : iVDegree) : 0);
    CreateControl(aakCtrlPoint);

    m_akBasis[0].Create(m_iNumUCtrlPoints+m_iUReplicate,iUDegree,bUOpen);
    m_akBasis[1].Create(m_iNumVCtrlPoints+m_iVReplicate,iVDegree,bVOpen);
}
//----------------------------------------------------------------------------
template <class Real>
BSplineRectangle<Real>::BSplineRectangle (int iNumUCtrlPoints,
    int iNumVCtrlPoints, Vector3<Real>** aakCtrlPoint, int iUDegree,
    int iVDegree, bool bULoop, bool bVLoop, bool bUOpen, Real* afVKnot)
    :
    ParametricSurface<Real>((Real)0.0,(Real)1.0,(Real)0.0,(Real)1.0,true)
{
    assert(iNumUCtrlPoints >= 2);
    assert(1 <= iUDegree && iUDegree <= iNumUCtrlPoints-1);
    assert(iNumVCtrlPoints >= 2);
    assert(1 <= iVDegree && iVDegree <= iNumVCtrlPoints-1);

    m_abLoop[0] = bULoop;
    m_abLoop[1] = bVLoop;

    m_iNumUCtrlPoints = iNumUCtrlPoints;
    m_iNumVCtrlPoints = iNumVCtrlPoints;
    m_iUReplicate = (bULoop ? (bUOpen ? 1 : iUDegree) : 0);
    m_iVReplicate = (bVLoop ? 1 : 0);
    CreateControl(aakCtrlPoint);

    m_akBasis[0].Create(m_iNumUCtrlPoints+m_iUReplicate,iUDegree,bUOpen);
    m_akBasis[1].Create(m_iNumVCtrlPoints+m_iVReplicate,iVDegree,afVKnot);
}
//----------------------------------------------------------------------------
template <class Real>
BSplineRectangle<Real>::BSplineRectangle (int iNumUCtrlPoints,
    int iNumVCtrlPoints, Vector3<Real>** aakCtrlPoint, int iUDegree,
    int iVDegree, bool bULoop, bool bVLoop, Real* afUKnot, bool bVOpen)
    :
    ParametricSurface<Real>((Real)0.0,(Real)1.0,(Real)0.0,(Real)1.0,true)
{
    assert(iNumUCtrlPoints >= 2);
    assert(1 <= iUDegree && iUDegree <= iNumUCtrlPoints-1);
    assert(iNumVCtrlPoints >= 2);
    assert(1 <= iVDegree && iVDegree <= iNumVCtrlPoints-1);

    m_abLoop[0] = bULoop;
    m_abLoop[1] = bVLoop;

    m_iNumUCtrlPoints = iNumUCtrlPoints;
    m_iNumVCtrlPoints = iNumVCtrlPoints;
    m_iUReplicate = (bULoop ? 1 : 0);
    m_iVReplicate = (bVLoop ? (bVOpen ? 1 : iVDegree) : 0);
    CreateControl(aakCtrlPoint);

    m_akBasis[0].Create(m_iNumUCtrlPoints+m_iUReplicate,iUDegree,afUKnot);
    m_akBasis[1].Create(m_iNumVCtrlPoints+m_iVReplicate,iVDegree,bVOpen);
}
//----------------------------------------------------------------------------
template <class Real>
BSplineRectangle<Real>::BSplineRectangle (int iNumUCtrlPoints,
    int iNumVCtrlPoints, Vector3<Real>** aakCtrlPoint, int iUDegree,
    int iVDegree, bool bULoop, bool bVLoop, Real* afUKnot, Real* afVKnot)
    :
    ParametricSurface<Real>((Real)0.0,(Real)1.0,(Real)0.0,(Real)1.0,true)
{
    assert(iNumUCtrlPoints >= 2);
    assert(1 <= iUDegree && iUDegree <= iNumUCtrlPoints-1);
    assert(iNumVCtrlPoints >= 2);
    assert(1 <= iVDegree && iVDegree <= iNumVCtrlPoints-1);

    m_abLoop[0] = bULoop;
    m_abLoop[1] = bVLoop;

    m_iNumUCtrlPoints = iNumUCtrlPoints;
    m_iNumVCtrlPoints = iNumVCtrlPoints;
    m_iUReplicate = (bULoop ? 1 : 0);
    m_iVReplicate = (bVLoop ? 1 : 0);
    CreateControl(aakCtrlPoint);

    m_akBasis[0].Create(m_iNumUCtrlPoints+m_iUReplicate,iUDegree,afUKnot);
    m_akBasis[1].Create(m_iNumVCtrlPoints+m_iVReplicate,iVDegree,afVKnot);
}
//----------------------------------------------------------------------------
template <class Real>
BSplineRectangle<Real>::~BSplineRectangle ()
{
    WM4_DELETE[] m_aakCtrlPoint[0];
    WM4_DELETE[] m_aakCtrlPoint;
}
//----------------------------------------------------------------------------
template <class Real>
void BSplineRectangle<Real>::CreateControl (Vector3<Real>** aakCtrlPoint)
{
    int iNewNumUCtrlPoints = m_iNumUCtrlPoints + m_iUReplicate;
    int iNewNumVCtrlPoints = m_iNumVCtrlPoints + m_iVReplicate;
    m_aakCtrlPoint = WM4_NEW Vector3<Real>*[iNewNumUCtrlPoints];
    m_aakCtrlPoint[0] =
        WM4_NEW Vector3<Real>[iNewNumUCtrlPoints*iNewNumVCtrlPoints];
    for (int i = 1; i < iNewNumUCtrlPoints; i++)
    {
        m_aakCtrlPoint[i] = &m_aakCtrlPoint[0][i*iNewNumVCtrlPoints];
    }

    for (int iU = 0; iU < iNewNumUCtrlPoints; iU++)
    {
        int iUOld = iU % m_iNumUCtrlPoints;
        for (int iV = 0; iV < iNewNumVCtrlPoints; iV++)
        {
            int iVOld = iV % m_iNumVCtrlPoints;
            m_aakCtrlPoint[iU][iV] = aakCtrlPoint[iUOld][iVOld];
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineRectangle<Real>::GetNumCtrlPoints (int iDim) const
{
    assert(0 <= iDim && iDim <= 1);
    return m_akBasis[iDim].GetNumCtrlPoints();
}
//----------------------------------------------------------------------------
template <class Real>
int BSplineRectangle<Real>::GetDegree (int iDim) const
{
    assert(0 <= iDim && iDim <= 1);
    return m_akBasis[iDim].GetDegree();
}
//----------------------------------------------------------------------------
template <class Real>
bool BSplineRectangle<Real>::IsOpen (int iDim) const
{
    assert(0 <= iDim && iDim <= 1);
    return m_akBasis[iDim].IsOpen();
}
//----------------------------------------------------------------------------
template <class Real>
bool BSplineRectangle<Real>::IsUniform (int iDim) const
{
    assert(0 <= iDim && iDim <= 1);
    return m_akBasis[iDim].IsUniform();
}
//----------------------------------------------------------------------------
template <class Real>
bool BSplineRectangle<Real>::IsLoop (int iDim) const
{
    assert(0 <= iDim && iDim <= 1);
    return m_abLoop[iDim];
}
//----------------------------------------------------------------------------
template <class Real>
void BSplineRectangle<Real>::SetControlPoint (int iUIndex, int iVIndex,
    const Vector3<Real>& rkCtrl)
{
    if (0 <= iUIndex && iUIndex < m_iNumUCtrlPoints
    &&  0 <= iVIndex && iVIndex < m_iNumVCtrlPoints)
    {
        // set the control point
        m_aakCtrlPoint[iUIndex][iVIndex] = rkCtrl;

        // set the replicated control point
        bool bDoUReplicate = (iUIndex < m_iUReplicate);
        bool bDoVReplicate = (iVIndex < m_iVReplicate);
        int iUExt = 0, iVExt = 0;

        if (bDoUReplicate)
        {
            iUExt = m_iNumUCtrlPoints + iUIndex;
            m_aakCtrlPoint[iUExt][iVIndex] = rkCtrl;
        }
        if (bDoVReplicate)
        {
            iVExt = m_iNumVCtrlPoints + iVIndex;
            m_aakCtrlPoint[iUIndex][iVExt] = rkCtrl;
        }
        if (bDoUReplicate && bDoVReplicate)
        {
            m_aakCtrlPoint[iUExt][iVExt] = rkCtrl;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> BSplineRectangle<Real>::GetControlPoint (int iUIndex,
    int iVIndex) const
{
    if (0 <= iUIndex && iUIndex < m_iNumUCtrlPoints
    &&  0 <= iVIndex && iVIndex < m_iNumVCtrlPoints)
    {
        return m_aakCtrlPoint[iUIndex][iVIndex];
    }

    return Vector3<Real>(Math<Real>::MAX_REAL,Math<Real>::MAX_REAL,
        Math<Real>::MAX_REAL);
}
//----------------------------------------------------------------------------
template <class Real>
void BSplineRectangle<Real>::SetKnot (int iDim, int i, Real fKnot)
{
    if (0 <= iDim && iDim <= 1)
    {
        m_akBasis[iDim].SetKnot(i,fKnot);
    }
}
//----------------------------------------------------------------------------
template <class Real>
Real BSplineRectangle<Real>::GetKnot (int iDim, int i) const
{
    if (0 <= iDim && iDim <= 1)
    {
        return m_akBasis[iDim].GetKnot(i);
    }

    return Math<Real>::MAX_REAL;
}
//----------------------------------------------------------------------------
template <class Real>
void BSplineRectangle<Real>::Get (Real fU, Real fV, Vector3<Real>* pkPos,
    Vector3<Real>* pkDerU, Vector3<Real>* pkDerV, Vector3<Real>* pkDerUU,
    Vector3<Real>* pkDerUV, Vector3<Real>* pkDerVV) const
{
    int iU, iUMin, iUMax;
    if (pkDerUU)
    {
        m_akBasis[0].Compute(fU,2,iUMin,iUMax);
    }
    else if (pkDerUV || pkDerU)
    {
        m_akBasis[0].Compute(fU,1,iUMin,iUMax);
    }
    else
    {
        m_akBasis[0].Compute(fU,0,iUMin,iUMax);
    }

    int iV, iVMin, iVMax;
    if (pkDerVV)
    {
        m_akBasis[1].Compute(fV,2,iVMin,iVMax);
    }
    else if (pkDerUV || pkDerV)
    {
        m_akBasis[1].Compute(fV,1,iVMin,iVMax);
    }
    else
    {
        m_akBasis[1].Compute(fV,0,iVMin,iVMax);
    }

    Real fTmp;

    if (pkPos)
    {
        *pkPos = Vector3<Real>::ZERO;
        for (iU = iUMin; iU <= iUMax; iU++)
        {
            for (iV = iVMin; iV <= iVMax; iV++)
            {
                fTmp = m_akBasis[0].GetD0(iU)*m_akBasis[1].GetD0(iV);
                *pkPos += fTmp*m_aakCtrlPoint[iU][iV];
            }
        }
    }

    if (pkDerU)
    {
        *pkDerU = Vector3<Real>::ZERO;
        for (iU = iUMin; iU <= iUMax; iU++)
        {
            for (iV = iVMin; iV <= iVMax; iV++)
            {
                fTmp = m_akBasis[0].GetD1(iU)*m_akBasis[1].GetD0(iV);
                *pkDerU += fTmp*m_aakCtrlPoint[iU][iV];
            }
        }
    }

    if (pkDerV)
    {
        *pkDerV = Vector3<Real>::ZERO;
        for (iU = iUMin; iU <= iUMax; iU++)
        {
            for (iV = iVMin; iV <= iVMax; iV++)
            {
                fTmp = m_akBasis[0].GetD0(iU)*m_akBasis[1].GetD1(iV);
                *pkDerV += fTmp*m_aakCtrlPoint[iU][iV];
            }
        }
    }

    if (pkDerUU)
    {
        *pkDerUU = Vector3<Real>::ZERO;
        for (iU = iUMin; iU <= iUMax; iU++)
        {
            for (iV = iVMin; iV <= iVMax; iV++)
            {
                fTmp = m_akBasis[0].GetD2(iU)*m_akBasis[1].GetD0(iV);
                *pkDerUU += fTmp*m_aakCtrlPoint[iU][iV];
            }
        }
    }

    if (pkDerUV)
    {
        *pkDerUV = Vector3<Real>::ZERO;
        for (iU = iUMin; iU <= iUMax; iU++)
        {
            for (iV = iVMin; iV <= iVMax; iV++)
            {
                fTmp = m_akBasis[0].GetD1(iU)*m_akBasis[1].GetD1(iV);
                *pkDerUV += fTmp*m_aakCtrlPoint[iU][iV];
            }
        }
    }

    if (pkDerVV)
    {
        *pkDerVV = Vector3<Real>::ZERO;
        for (iU = iUMin; iU <= iUMax; iU++)
        {
            for (iV = iVMin; iV <= iVMax; iV++)
            {
                fTmp = m_akBasis[0].GetD0(iU)*m_akBasis[1].GetD2(iV);
                *pkDerVV += fTmp*m_aakCtrlPoint[iU][iV];
            }
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> BSplineRectangle<Real>::P (Real fU, Real fV) const
{
    Vector3<Real> kP;
    Get(fU,fV,&kP,0,0,0,0,0);
    return kP;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> BSplineRectangle<Real>::PU (Real fU, Real fV) const
{
    Vector3<Real> kPU;
    Get(fU,fV,0,&kPU,0,0,0,0);
    return kPU;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> BSplineRectangle<Real>::PV (Real fU, Real fV) const
{
    Vector3<Real> kPV;
    Get(fU,fV,0,0,&kPV,0,0,0);
    return kPV;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> BSplineRectangle<Real>::PUU (Real fU, Real fV) const
{
    Vector3<Real> kPUU;
    Get(fU,fV,0,0,0,&kPUU,0,0);
    return kPUU;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> BSplineRectangle<Real>::PUV (Real fU, Real fV) const
{
    Vector3<Real> kPUV;
    Get(fU,fV,0,0,0,0,&kPUV,0);
    return kPUV;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> BSplineRectangle<Real>::PVV (Real fU, Real fV) const
{
    Vector3<Real> kPVV;
    Get(fU,fV,0,0,0,0,0,&kPVV);
    return kPVV;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class BSplineRectangle<float>;

template WM4_FOUNDATION_ITEM
class BSplineRectangle<double>;
//----------------------------------------------------------------------------
}
