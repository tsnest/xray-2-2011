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
#include "Wm4IntrLine3Capsule3.h"
#include "Wm4DistLine3Segment3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Capsule3<Real>::IntrLine3Capsule3 (const Line3<Real>& rkLine,
    const Capsule3<Real>& rkCapsule)
    :
    m_pkLine(&rkLine),
    m_pkCapsule(&rkCapsule)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Capsule3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Capsule3<Real>& IntrLine3Capsule3<Real>::GetCapsule () const
{
    return *m_pkCapsule;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Capsule3<Real>::Test ()
{
    Real fDist = DistLine3Segment3<Real>(*m_pkLine,
        m_pkCapsule->Segment).Get();
    return fDist <= m_pkCapsule->Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Capsule3<Real>::Find ()
{
    Real afT[2];
    m_iQuantity = Find(m_pkLine->Origin,m_pkLine->Direction,*m_pkCapsule,
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
int IntrLine3Capsule3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Capsule3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Capsule3<Real>::Find (const Vector3<Real>& rkOrigin,
    const Vector3<Real>& rkDir, const Capsule3<Real>& rkCapsule, Real afT[2])
{
    // Create a coordinate system for the capsule.  In this system, the
    // capsule segment center C is the origin and the capsule axis direction
    // W is the z-axis.  U and V are the other coordinate axis directions.
    // If P = x*U+y*V+z*W, the cylinder containing the capsule wall is
    // x^2 + y^2 = r^2, where r is the capsule radius.  The finite cylinder
    // that makes up the capsule minus its hemispherical end caps has z-values
    // |z| <= e, where e is the extent of the capsule segment.  The top
    // hemisphere cap is x^2+y^2+(z-e)^2 = r^2 for z >= e, and the bottom
    // hemisphere cap is x^2+y^2+(z+e)^2 = r^2 for z <= -e.
    Vector3<Real> kU, kV, kW = rkCapsule.Segment.Direction;
    Vector3<Real>::GenerateComplementBasis(kU,kV,kW);
    Real fRSqr = rkCapsule.Radius*rkCapsule.Radius;
    Real fExtent = rkCapsule.Segment.Extent;

    // convert incoming line origin to capsule coordinates
    Vector3<Real> kDiff = rkOrigin - rkCapsule.Segment.Origin;
    Vector3<Real> kP(kU.Dot(kDiff),kV.Dot(kDiff),kW.Dot(kDiff));

    // Get the z-value, in capsule coordinates, of the incoming line's
    // unit-length direction.
    Real fDz = kW.Dot(rkDir);
    if (Math<Real>::FAbs(fDz) >= (Real)1.0 - Math<Real>::ZERO_TOLERANCE)
    {
        // The line is parallel to the capsule axis.  Determine if the line
        // intersects the capsule hemispheres.
        Real fRadialSqrDist = fRSqr - kP.X()*kP.X() - kP.Y()*kP.Y();
        if (fRadialSqrDist < (Real)0.0)
        {
            // line outside the cylinder of the capsule, no intersection
            return 0;
        }

        // line intersects the hemispherical caps
        Real fZOffset = Math<Real>::Sqrt(fRadialSqrDist) + fExtent;
        if (fDz > (Real)0.0)
        {
            afT[0] = -kP.Z() - fZOffset;
            afT[1] = -kP.Z() + fZOffset;
        }
        else
        {
            afT[0] = kP.Z() - fZOffset;
            afT[1] = kP.Z() + fZOffset;
        }
        return 2;
    }

    // convert incoming line unit-length direction to capsule coordinates
    Vector3<Real> kD(kU.Dot(rkDir),kV.Dot(rkDir),fDz);

    // Test intersection of line P+t*D with infinite cylinder x^2+y^2 = r^2.
    // This reduces to computing the roots of a quadratic equation.  If
    // P = (px,py,pz) and D = (dx,dy,dz), then the quadratic equation is
    //   (dx^2+dy^2)*t^2 + 2*(px*dx+py*dy)*t + (px^2+py^2-r^2) = 0
    Real fA0 = kP.X()*kP.X() + kP.Y()*kP.Y() - fRSqr;
    Real fA1 = kP.X()*kD.X() + kP.Y()*kD.Y();
    Real fA2 = kD.X()*kD.X() + kD.Y()*kD.Y();
    Real fDiscr = fA1*fA1 - fA0*fA2;
    if (fDiscr < (Real)0.0)
    {
        // line does not intersect infinite cylinder
        return 0;
    }

    Real fRoot, fInv, fT, fZValue;
    int iQuantity = 0;
    if (fDiscr > Math<Real>::ZERO_TOLERANCE)
    {
        // line intersects infinite cylinder in two places
        fRoot = Math<Real>::Sqrt(fDiscr);
        fInv = ((Real)1.0)/fA2;
        fT = (-fA1 - fRoot)*fInv;
        fZValue = kP.Z() + fT*kD.Z();
        if (Math<Real>::FAbs(fZValue) <= fExtent)
        {
            afT[iQuantity++] = fT;
        }

        fT = (-fA1 + fRoot)*fInv;
        fZValue = kP.Z() + fT*kD.Z();
        if (Math<Real>::FAbs(fZValue) <= fExtent)
        {
            afT[iQuantity++] = fT;
        }

        if (iQuantity == 2)
        {
            // line intersects capsule wall in two places
            return 2;
        }
    }
    else
    {
        // line is tangent to infinite cylinder
        fT = -fA1/fA2;
        fZValue = kP.Z() + fT*kD.Z();
        if (Math<Real>::FAbs(fZValue) <= fExtent)
        {
            afT[0] = fT;
            return 1;
        }
    }

    // Test intersection with bottom hemisphere.  The quadratic equation is
    //   t^2 + 2*(px*dx+py*dy+(pz+e)*dz)*t + (px^2+py^2+(pz+e)^2-r^2) = 0
    // Use the fact that currently fA1 = px*dx+py*dy and fA0 = px^2+py^2-r^2.
    // The leading coefficient is fA2 = 1, so no need to include in the
    // construction.
    Real fPZpE = kP.Z() + fExtent;
    fA1 += fPZpE*kD.Z();
    fA0 += fPZpE*fPZpE;
    fDiscr = fA1*fA1 - fA0;
    if (fDiscr > Math<Real>::ZERO_TOLERANCE)
    {
        fRoot = Math<Real>::Sqrt(fDiscr);
        fT = -fA1 - fRoot;
        fZValue = kP.Z() + fT*kD.Z();
        if (fZValue <= -fExtent)
        {
            afT[iQuantity++] = fT;
            if (iQuantity == 2)
            {
                if (afT[0] > afT[1])
                {
                    Real fSave = afT[0];
                    afT[0] = afT[1];
                    afT[1] = fSave;
                }
                return 2;
            }
        }

        fT = -fA1 + fRoot;
        fZValue = kP.Z() + fT*kD.Z();
        if (fZValue <= -fExtent)
        {
            afT[iQuantity++] = fT;
            if (iQuantity == 2)
            {
                if (afT[0] > afT[1])
                {
                    Real fSave = afT[0];
                    afT[0] = afT[1];
                    afT[1] = fSave;
                }
                return 2;
            }
        }
    }
    else if (Math<Real>::FAbs(fDiscr) <= Math<Real>::ZERO_TOLERANCE)
    {
        fT = -fA1;
        fZValue = kP.Z() + fT*kD.Z();
        if (fZValue <= -fExtent)
        {
            afT[iQuantity++] = fT;
            if (iQuantity == 2)
            {
                if (afT[0] > afT[1])
                {
                    Real fSave = afT[0];
                    afT[0] = afT[1];
                    afT[1] = fSave;
                }
                return 2;
            }
        }
    }

    // Test intersection with top hemisphere.  The quadratic equation is
    //   t^2 + 2*(px*dx+py*dy+(pz-e)*dz)*t + (px^2+py^2+(pz-e)^2-r^2) = 0
    // Use the fact that currently fA1 = px*dx+py*dy+(pz+e)*dz and
    // fA0 = px^2+py^2+(pz+e)^2-r^2.  The leading coefficient is fA2 = 1, so
    // no need to include in the construction.
    fA1 -= ((Real)2.0)*fExtent*kD.Z();
    fA0 -= ((Real)4.0)*fExtent*kP.Z();
    fDiscr = fA1*fA1 - fA0;
    if (fDiscr > Math<Real>::ZERO_TOLERANCE)
    {
        fRoot = Math<Real>::Sqrt(fDiscr);
        fT = -fA1 - fRoot;
        fZValue = kP.Z() + fT*kD.Z();
        if (fZValue >= fExtent)
        {
            afT[iQuantity++] = fT;
            if (iQuantity == 2)
            {
                if (afT[0] > afT[1])
                {
                    Real fSave = afT[0];
                    afT[0] = afT[1];
                    afT[1] = fSave;
                }
                return 2;
            }
        }

        fT = -fA1 + fRoot;
        fZValue = kP.Z() + fT*kD.Z();
        if (fZValue >= fExtent)
        {
            afT[iQuantity++] = fT;
            if (iQuantity == 2)
            {
                if (afT[0] > afT[1])
                {
                    Real fSave = afT[0];
                    afT[0] = afT[1];
                    afT[1] = fSave;
                }
                return 2;
            }
        }
    }
    else if (Math<Real>::FAbs(fDiscr) <= Math<Real>::ZERO_TOLERANCE)
    {
        fT = -fA1;
        fZValue = kP.Z() + fT*kD.Z();
        if (fZValue >= fExtent)
        {
            afT[iQuantity++] = fT;
            if (iQuantity == 2)
            {
                if (afT[0] > afT[1])
                {
                    Real fSave = afT[0];
                    afT[0] = afT[1];
                    afT[1] = fSave;
                }
                return 2;
            }
        }
    }

    return iQuantity;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLine3Capsule3<float>;

template WM4_FOUNDATION_ITEM
class IntrLine3Capsule3<double>;
//----------------------------------------------------------------------------
}
