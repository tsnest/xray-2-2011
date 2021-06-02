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
// Version: 4.0.2 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrLine3Cylinder3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Cylinder3<Real>::IntrLine3Cylinder3 (const Line3<Real>& rkLine,
    const Cylinder3<Real>& rkCylinder)
    :
    m_pkLine(&rkLine),
    m_pkCylinder(&rkCylinder)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Cylinder3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Cylinder3<Real>& IntrLine3Cylinder3<Real>::GetCylinder () const
{
    return *m_pkCylinder;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Cylinder3<Real>::Find ()
{
    Real afT[2];
    m_iQuantity = Find(m_pkLine->Origin,m_pkLine->Direction,*m_pkCylinder,
        afT);
    for (int i = 0; i < m_iQuantity; i++)
    {
        m_akPoint[i] = m_pkLine->Origin + afT[i]*m_pkLine->Direction;
    }

    if (m_iQuantity == 2)
    {
        m_iIntersectionType = IT_SEGMENT;
    }
    else if (m_iQuantity == 1)
    {
        m_iIntersectionType = IT_POINT;
    }
    else
    {
        m_iIntersectionType = IT_EMPTY;
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Cylinder3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Cylinder3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Cylinder3<Real>::Find (const Vector3<Real>& rkOrigin,
    const Vector3<Real>& rkDir, const Cylinder3<Real>& rkCylinder,
    Real afT[2])
{
    // Create a coordinate system for the cylinder.  In this system, the
    // cylinder segment center C is the origin and the cylinder axis direction
    // W is the z-axis.  U and V are the other coordinate axis directions.
    // If P = x*U+y*V+z*W, the cylinder is x^2 + y^2 = r^2, where r is the
    // cylinder radius.  The end caps are |z| = h/2, where h is the cylinder
    // height.
    Vector3<Real> kU, kV, kW = rkCylinder.Segment.Direction;
    Vector3<Real>::GenerateComplementBasis(kU,kV,kW);
    Real fHalfHeight = ((Real)0.5)*rkCylinder.Height;
    Real fRSqr = rkCylinder.Radius*rkCylinder.Radius;

    // convert incoming line origin to cylinder coordinates
    Vector3<Real> kDiff = rkOrigin - rkCylinder.Segment.Origin;
    Vector3<Real> kP(kU.Dot(kDiff),kV.Dot(kDiff),kW.Dot(kDiff));

    // Get the z-value, in cylinder coordinates, of the incoming line's
    // unit-length direction.
    Real fDz = kW.Dot(rkDir);

    if (Math<Real>::FAbs(fDz) >= (Real)1.0 - Math<Real>::ZERO_TOLERANCE)
    {
        // The line is parallel to the cylinder axis.  Determine if the line
        // intersects the cylinder end disks.
        Real fRadialSqrDist = fRSqr - kP.X()*kP.X() - kP.Y()*kP.Y();
        if (fRadialSqrDist < (Real)0.0)
        {
            // line outside the cylinder, no intersection
            return 0;
        }

        // line intersects the cylinder end disks
        if (fDz > (Real)0.0)
        {
            afT[0] = -kP.Z() - fHalfHeight;
            afT[1] = -kP.Z() + fHalfHeight;
        }
        else
        {
            afT[0] = kP.Z() - fHalfHeight;
            afT[1] = kP.Z() + fHalfHeight;
        }
        return 2;
    }

    // convert incoming line unit-length direction to cylinder coordinates
    Vector3<Real> kD(kU.Dot(rkDir),kV.Dot(rkDir),fDz);

    Real fA0, fA1, fA2, fDiscr, fRoot, fInv, fT;

    if (Math<Real>::FAbs(kD.Z()) <= Math<Real>::ZERO_TOLERANCE)
    {
        // The line is perpendicular to the cylinder axis.
        if (Math<Real>::FAbs(kP.Z()) > fHalfHeight)
        {
            // line is outside the planes of the cylinder end disks
            return 0;
        }

        // Test intersection of line P+t*D with infinite cylinder
        // x^2+y^2 = r^2.  This reduces to computing the roots of a
        // quadratic equation.  If P = (px,py,pz) and D = (dx,dy,dz),
        // then the quadratic equation is
        //   (dx^2+dy^2)*t^2 + 2*(px*dx+py*dy)*t + (px^2+py^2-r^2) = 0
        fA0 = kP.X()*kP.X() + kP.Y()*kP.Y() - fRSqr;
        fA1 = kP.X()*kD.X() + kP.Y()*kD.Y();
        fA2 = kD.X()*kD.X() + kD.Y()*kD.Y();
        fDiscr = fA1*fA1 - fA0*fA2;
        if (fDiscr < (Real)0.0)
        {
            // line does not intersect cylinder
            return 0;
        }
        else if (fDiscr > Math<Real>::ZERO_TOLERANCE)
        {
            // line intersects cylinder in two places
            fRoot = Math<Real>::Sqrt(fDiscr);
            fInv = ((Real)1.0)/fA2;
            afT[0] = (-fA1 - fRoot)*fInv;
            afT[1] = (-fA1 + fRoot)*fInv;
            return 2;
        }
        else
        {
            // line is tangent to the cylinder
            afT[0] = -fA1/fA2;
            return 1;
        }
    }

    // test plane intersections first
    int iQuantity = 0;
    fInv = ((Real)1.0)/kD.Z();

    Real fT0 = (-fHalfHeight - kP.Z())*fInv;
    Real fXTmp = kP.X() + fT0*kD.X();
    Real fYTmp = kP.Y() + fT0*kD.Y();
    if (fXTmp*fXTmp + fYTmp*fYTmp <= fRSqr)
    {
        // planar intersection inside the top cylinder end disk
        afT[iQuantity++] = fT0;
    }

    Real fT1 = (+fHalfHeight - kP.Z())*fInv;
    fXTmp = kP.X() + fT1*kD.X();
    fYTmp = kP.Y() + fT1*kD.Y();
    if (fXTmp*fXTmp + fYTmp*fYTmp <= fRSqr)
    {
        // planar intersection inside the bottom cylinder end disk
        afT[iQuantity++] = fT1;
    }

    if (iQuantity == 2)
    {
        // line intersects both top and bottom cylinder end disks
        if (afT[0] > afT[1])
        {
            Real fSave = afT[0];
            afT[0] = afT[1];
            afT[1] = fSave;
        }
        return 2;
    }

    // If iQuantity == 1, then the line must intersect cylinder wall in a
    // single point somewhere between the end disks.  This case is detected
    // in the following code that tests for intersection between line and
    // cylinder wall.
    fA0 = kP.X()*kP.X() + kP.Y()*kP.Y() - fRSqr;
    fA1 = kP.X()*kD.X() + kP.Y()*kD.Y();
    fA2 = kD.X()*kD.X() + kD.Y()*kD.Y();
    fDiscr = fA1*fA1 - fA0*fA2;
    if (fDiscr < (Real)0.0)
    {
        // line does not intersect cylinder wall
        assert( iQuantity == 0 );
        return 0;
    }
    else if (fDiscr > Math<Real>::ZERO_TOLERANCE)
    {
        fRoot = Math<Real>::Sqrt(fDiscr);
        fInv = ((Real)1.0)/fA2;
        fT = (-fA1 - fRoot)*fInv;
        if (fT0 <= fT1)
        {
            if (fT0 <= fT && fT <= fT1)
            {
                afT[iQuantity++] = fT;
            }
        }
        else
        {
            if (fT1 <= fT && fT <= fT0)
            {
                afT[iQuantity++] = fT;
            }
        }

        if (iQuantity == 2)
        {
            // Line intersects one of the cylinder end disks and once on the
            // cylinder wall.
            if (afT[0] > afT[1])
            {
                Real fSave = afT[0];
                afT[0] = afT[1];
                afT[1] = fSave;
            }
            return 2;
        }

        fT = (-fA1 + fRoot)*fInv;
        if (fT0 <= fT1)
        {
            if (fT0 <= fT && fT <= fT1)
            {
                afT[iQuantity++] = fT;
            }
        }
        else
        {
            if (fT1 <= fT && fT <= fT0)
            {
                afT[iQuantity++] = fT;
            }
        }
    }
    else
    {
        fT = -fA1/fA2;
        if (fT0 <= fT1)
        {
            if (fT0 <= fT && fT <= fT1)
            {
                afT[iQuantity++] = fT;
            }
        }
        else
        {
            if (fT1 <= fT && fT <= fT0)
            {
                afT[iQuantity++] = fT;
            }
        }
    }

    if (iQuantity == 2)
    {
        if (afT[0] > afT[1])
        {
            Real fSave = afT[0];
            afT[0] = afT[1];
            afT[1] = fSave;
        }
    }

    return iQuantity;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLine3Cylinder3<float>;

template WM4_FOUNDATION_ITEM
class IntrLine3Cylinder3<double>;
//----------------------------------------------------------------------------
}
