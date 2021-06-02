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
#include "Wm4IntrTriangle3Cone3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrTriangle3Cone3<Real>::IntrTriangle3Cone3 (
    const Triangle3<Real>& rkTriangle, const Cone3<Real>& rkCone)
    :
    m_pkTriangle(&rkTriangle),
    m_pkCone(&rkCone)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3<Real>& IntrTriangle3Cone3<Real>::GetTriangle () const
{
    return *m_pkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
const Cone3<Real>& IntrTriangle3Cone3<Real>::GetCone () const
{
    return *m_pkCone;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrTriangle3Cone3<Real>::Test ()
{
    // triangle is <P0,P1,P2>, edges are E0 = P1-P0, E1=P2-P0
    int iOnConeSide = 0;
    Real fP0Test = (Real)0.0, fP1Test = (Real)0.0, fP2Test = (Real)0.0;
    Real fAdE, fEdE, fEdD, fC1, fC2;

    Real fCosSqr = m_pkCone->CosAngle*m_pkCone->CosAngle;

    // test vertex P0
    Vector3<Real> kDiff0 = m_pkTriangle->V[0] - m_pkCone->Vertex;
    Real fAdD0 = m_pkCone->Axis.Dot(kDiff0);
    if (fAdD0 >= (Real)0.0)
    {
        // P0 is on cone side of plane
        fP0Test = fAdD0*fAdD0 - fCosSqr*(kDiff0.Dot(kDiff0));
        if (fP0Test >= (Real)0.0)
        {
            // P0 is inside the cone
            return true;
        }
        else
        {
            // P0 is outside the cone, but on cone side of plane
            iOnConeSide |= 1;
        }
    }
    // else P0 is not on cone side of plane

    // test vertex P1
    Vector3<Real> kEdge0 = m_pkTriangle->V[1] - m_pkTriangle->V[0];
    Vector3<Real> kDiff1 = kDiff0 + kEdge0;
    Real fAdD1 = m_pkCone->Axis.Dot(kDiff1);
    if (fAdD1 >= (Real)0.0)
    {
        // P1 is on cone side of plane
        fP1Test = fAdD1*fAdD1 - fCosSqr*(kDiff1.Dot(kDiff1));
        if (fP1Test >= (Real)0.0)
        {
            // P1 is inside the cone
            return true;
        }
        else
        {
            // P1 is outside the cone, but on cone side of plane
            iOnConeSide |= 2;
        }
    }
    // else P1 is not on cone side of plane

    // test vertex P2
    Vector3<Real> kEdge1 = m_pkTriangle->V[2] - m_pkTriangle->V[0];
    Vector3<Real> kDiff2 = kDiff0 + kEdge1;
    Real fAdD2 = m_pkCone->Axis.Dot(kDiff2);
    if (fAdD2 >= (Real)0.0)
    {
        // P2 is on cone side of plane
        fP2Test = fAdD2*fAdD2 - fCosSqr*(kDiff2.Dot(kDiff2));
        if (fP2Test >= (Real)0.0)
        {
            // P2 is inside the cone
            return true;
        }
        else
        {
            // P2 is outside the cone, but on cone side of plane
            iOnConeSide |= 4;
        }
    }
    // else P2 is not on cone side of plane

    // test edge <P0,P1> = E0
    if (iOnConeSide & 3)
    {
        fAdE = fAdD1 - fAdD0;
        fEdE = kEdge0.Dot(kEdge0);
        fC2 = fAdE*fAdE - fCosSqr*fEdE;
        if (fC2 < (Real)0.0)
        {
            fEdD = kEdge0.Dot(kDiff0);
            fC1 = fAdE*fAdD0 - fCosSqr*fEdD;
            if (iOnConeSide & 1)
            {
                if (iOnConeSide & 2)
                {
                    // <P0,P1> fully on cone side of plane, fC0 = fP0Test
                    if ((Real)0.0 <= fC1 && fC1 <= -fC2
                    &&  fC1*fC1 >= fP0Test*fC2)
                    {
                        return true;
                    }
                }
                else
                {
                    // P0 on cone side (Dot(A,P0-V) >= 0),
                    // P1 on opposite side (Dot(A,P1-V) <= 0)
                    // (Dot(A,E0) <= 0), fC0 = fP0Test
                    if ((Real)0.0 <= fC1 && fC2*fAdD0 <= fC1*fAdE
                    &&  fC1*fC1 >= fP0Test*fC2)
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P1 on cone side (Dot(A,P1-V) >= 0),
                // P0 on opposite side (Dot(A,P0-V) <= 0)
                // (Dot(A,E0) >= 0), fC0 = fP0Test (needs calculating)
                if (fC1 <= -fC2 && fC2*fAdD0 <= fC1*fAdE)
                {
                    fP0Test = fAdD0*fAdD0 - fCosSqr*(kDiff0.Dot(kDiff0));
                    if (fC1*fC1 >= fP0Test*fC2)
                    {
                        return true;
                    }
                }
            }
        }
    }
    // else <P0,P1> does not intersect cone half space

    // test edge <P0,P2> = E1
    if (iOnConeSide & 5)
    {
        fAdE = fAdD2 - fAdD0;
        fEdE = kEdge1.Dot(kEdge1);
        fC2 = fAdE*fAdE - fCosSqr*fEdE;
        if (fC2 < (Real)0.0)
        {
            fEdD = kEdge1.Dot(kDiff0);
            fC1 = fAdE*fAdD0 - fCosSqr*fEdD;
            if (iOnConeSide & 1)
            {
                if (iOnConeSide & 4)
                {
                    // <P0,P2> fully on cone side of plane, fC0 = fP0Test
                    if ((Real)0.0 <= fC1 && fC1 <= -fC2
                    &&  fC1*fC1 >= fP0Test*fC2)
                    {
                        return true;
                    }
                }
                else
                {
                    // P0 on cone side (Dot(A,P0-V) >= 0),
                    // P2 on opposite side (Dot(A,P2-V) <= 0)
                    // (Dot(A,E1) <= 0), fC0 = fP0Test
                    if ((Real)0.0 <= fC1 && fC2*fAdD0 <= fC1*fAdE
                    &&  fC1*fC1 >= fP0Test*fC2)
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P2 on cone side (Dot(A,P2-V) >= 0),
                // P0 on opposite side (Dot(A,P0-V) <= 0)
                // (Dot(A,E1) >= 0), fC0 = fP0Test (needs calculating)
                if (fC1 <= -fC2 && fC2*fAdD0 <= fC1*fAdE)
                {
                    fP0Test = fAdD0*fAdD0 - fCosSqr*(kDiff0.Dot(kDiff0));
                    if (fC1*fC1 >= fP0Test*fC2)
                    {
                        return true;
                    }
                }
            }
        }
    }
    // else <P0,P2> does not intersect cone half space

    // test edge <P1,P2> = E1-E0 = E2
    if (iOnConeSide & 6)
    {
        Vector3<Real> kE2 = kEdge1 - kEdge0;
        fAdE = fAdD2 - fAdD1;
        fEdE = kE2.Dot(kE2);
        fC2 = fAdE*fAdE - fCosSqr*fEdE;
        if (fC2 < (Real)0.0)
        {
            fEdD = kE2.Dot(kDiff1);
            fC1 = fAdE*fAdD1 - fCosSqr*fEdD;
            if (iOnConeSide & 2)
            {
                if (iOnConeSide & 4)
                {
                    // <P1,P2> fully on cone side of plane, fC0 = fP1Test
                    if ((Real)0.0 <= fC1 && fC1 <= -fC2
                    &&  fC1*fC1 >= fP1Test*fC2)
                    {
                        return true;
                    }
                }
                else
                {
                    // P1 on cone side (Dot(A,P1-V) >= 0),
                    // P2 on opposite side (Dot(A,P2-V) <= 0)
                    // (Dot(A,E2) <= 0), fC0 = fP1Test
                    if ((Real)0.0 <= fC1 && fC2*fAdD1 <= fC1*fAdE
                    &&  fC1*fC1 >= fP1Test*fC2)
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P2 on cone side (Dot(A,P2-V) >= 0),
                // P1 on opposite side (Dot(A,P1-V) <= 0)
                // (Dot(A,E2) >= 0), fC0 = fP1Test (needs calculating)
                if (fC1 <= -fC2 && fC2*fAdD1 <= fC1*fAdE)
                {
                    fP1Test = fAdD1*fAdD1 - fCosSqr*(kDiff1.Dot(kDiff1));
                    if (fC1*fC1 >= fP1Test*fC2)
                    {
                        return true;
                    }
                }
            }
        }
    }
    // else <P1,P2> does not intersect cone half space

    // Test triangle <P0,P1,P2>.  It is enough to handle only the case when
    // at least one Pi is on the cone side of the plane.  In this case and
    // after the previous testing, if the triangle intersects the cone, the
    // set of intersection must contain the point of intersection between
    // the cone axis and the triangle.
    if (iOnConeSide > 0)
    {
        Vector3<Real> kN = kEdge0.Cross(kEdge1);
        Real fNdA = kN.Dot(m_pkCone->Axis);
        Real fNdD = kN.Dot(kDiff0);
        Vector3<Real> kU = fNdD*m_pkCone->Axis - fNdA*kDiff0;
        Vector3<Real> kNcU = kN.Cross(kU);

        Real fNcUdE0 = kNcU.Dot(kEdge0), fNcUdE1, fNcUdE2, fNdN;
        if (fNdA >= (Real)0.0)
        {
            if (fNcUdE0 <= (Real)0.0)
            {
                fNcUdE1 = kNcU.Dot(kEdge1);
                if (fNcUdE1 >= (Real)0.0)
                {
                    fNcUdE2 = fNcUdE1 - fNcUdE0;
                    fNdN = kN.SquaredLength();
                    if (fNcUdE2 <= fNdA*fNdN)
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            if (fNcUdE0 >= (Real)0.0)
            {
                fNcUdE1 = kNcU.Dot(kEdge1);
                if (fNcUdE1 <= (Real)0.0)
                {
                    fNcUdE2 = fNcUdE1 - fNcUdE0;
                    fNdN = kN.SquaredLength();
                    if (fNcUdE2 >= fNdA*fNdN)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrTriangle3Cone3<float>;

template WM4_FOUNDATION_ITEM
class IntrTriangle3Cone3<double>;
//----------------------------------------------------------------------------
}
