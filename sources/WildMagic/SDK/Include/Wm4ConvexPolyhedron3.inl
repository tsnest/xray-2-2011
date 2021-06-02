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

//----------------------------------------------------------------------------
template <class Real>
ConvexPolyhedron3<Real>::ConvexPolyhedron3 (int iVQuantity,
    Vector3<Real>* akVertex, int iTQuantity, int* aiIndex,
    Plane3<Real>* akPlane, bool bOwner)
    :
    Polyhedron3<Real>(iVQuantity,akVertex,iTQuantity,aiIndex,bOwner)
{
    if (akPlane)
    {
        m_akPlane = akPlane;
        m_bPlaneOwner = bOwner;
    }
    else
    {
        m_akPlane = WM4_NEW Plane3<Real>[m_iTQuantity];
        m_bPlaneOwner = true;
        UpdatePlanes();
    }
}
//----------------------------------------------------------------------------
template <class Real>
ConvexPolyhedron3<Real>::ConvexPolyhedron3 (const ConvexPolyhedron3& rkPoly)
    :
    Polyhedron3<Real>(rkPoly)
{
    m_akPlane = 0;
    m_bPlaneOwner = false;
    *this = rkPoly;
}
//----------------------------------------------------------------------------
template <class Real>
ConvexPolyhedron3<Real>::~ConvexPolyhedron3 ()
{
    if (m_bPlaneOwner)
    {
        WM4_DELETE[] m_akPlane;
    }
}
//----------------------------------------------------------------------------
template <class Real>
ConvexPolyhedron3<Real>& ConvexPolyhedron3<Real>::operator= (
    const ConvexPolyhedron3& rkPoly)
{
    Polyhedron3<Real>::operator=(rkPoly);

    if (m_bPlaneOwner)
    {
        WM4_DELETE[] m_akPlane;
    }

    m_bPlaneOwner = rkPoly.m_bPlaneOwner;

    if (m_bPlaneOwner)
    {
        m_akPlane = WM4_NEW Plane3<Real>[m_iTQuantity];
        size_t uiSize = m_iTQuantity*sizeof(Plane3<Real>);
        System::Memcpy(m_akPlane,uiSize,rkPoly.m_akPlane,uiSize);
    }
    else
    {
        m_akPlane = rkPoly.m_akPlane;
    }

    m_kTModified = rkPoly.m_kTModified;
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>* ConvexPolyhedron3<Real>::GetPlanes () const
{
    return m_akPlane;
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& ConvexPolyhedron3<Real>::GetPlane (int i) const
{
    assert(0 <= i && i < m_iTQuantity);
    return m_akPlane[i];
}
//----------------------------------------------------------------------------
template <class Real>
void ConvexPolyhedron3<Real>::SetVertex (int i, const Vector3<Real>& rkV)
{
    Polyhedron3<Real>::SetVertex(i,rkV);

    const int* piIndex = m_aiIndex;
    for (int j = 0; j < m_iTQuantity; j++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;
        if (i == iV0 || i == iV1 || i == iV2)
        {
            m_kTModified.insert(j);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void ConvexPolyhedron3<Real>::UpdatePlane (int i,
    const Vector3<Real>& rkAverage)
{
    int iBase = 3*i;
    int iV0 = m_aiIndex[iBase++];
    int iV1 = m_aiIndex[iBase++];
    int iV2 = m_aiIndex[iBase];

    Vector3<Real>& rkV0 = m_akVertex[iV0];
    Vector3<Real>& rkV1 = m_akVertex[iV1];
    Vector3<Real>& rkV2 = m_akVertex[iV2];

    Vector3<Real> kDiff = rkAverage - rkV0;
    Vector3<Real> kE1 = rkV1 - rkV0;
    Vector3<Real> kE2 = rkV2 - rkV0;
    Vector3<Real> kNormal = kE2.Cross(kE1);
    Real fLength = kNormal.Length();
    if (fLength > Math<Real>::ZERO_TOLERANCE)
    {
        kNormal /= fLength;
        Real fDot = kNormal.Dot(kDiff);
        assert( fDot >= (Real)0.0 );
        if (fDot < (Real)0.0)
        {
            kNormal = -kNormal;
        }
    }
    else
    {
        // The triangle is degenerate.  Use a "normal" that points towards
        // the average.
        kNormal = kDiff;
        kNormal.Normalize();
    }

    // inner pointing normal
    m_akPlane[i] = Plane3<Real>(kNormal,kNormal.Dot(rkV0));
}
//----------------------------------------------------------------------------
template <class Real>
void ConvexPolyhedron3<Real>::UpdatePlanes ()
{
    Vector3<Real> kAverage = this->ComputeVertexAverage();
    int i;

    if (m_kTModified.empty())
    {
        for (i = 0; i < m_iTQuantity; i++)
        {
            UpdatePlane(i,kAverage);
        }
    }
    else
    {
        std::set<int>::iterator pkIter = m_kTModified.begin();
        for (/**/; pkIter != m_kTModified.end(); pkIter++)
        {
            i = *pkIter;
            UpdatePlane(i,kAverage);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool ConvexPolyhedron3<Real>::IsConvex (Real fThreshold) const
{
    Real fMax = -Math<Real>::MAX_REAL, fMin = Math<Real>::MAX_REAL;
    for (int j = 0; j < m_iTQuantity; j++)
    {
        const Plane3<Real>& rkPlane = m_akPlane[j];
        for (int i = 0; i < m_iVQuantity; i++)
        {
            Real fDistance = rkPlane.DistanceTo(m_akVertex[i]);
            if (fDistance < fMin)
            {
                fMin = fDistance;
            }
            if (fDistance > fMax)
            {
                fMax = fDistance;
            }
            if (fDistance < fThreshold)
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool ConvexPolyhedron3<Real>::ContainsPoint (const Vector3<Real>& rkP,
    Real fThreshold) const
{
    for (int i = 0; i < m_iTQuantity; i++)
    {
        Real fDistance = m_akPlane[i].DistanceTo(rkP);
        if (fDistance < fThreshold)
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
