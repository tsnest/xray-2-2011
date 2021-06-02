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
// Version: 4.0.0 (2006/09/06)

//----------------------------------------------------------------------------
template <typename Real>
RealV<Real,2> Perp (const RealV<Real,2>& rkV)
{
    return RealV<Real,2>(rkV[1],-rkV[0]);
}
//----------------------------------------------------------------------------
template <typename Real>
RealV<Real,2> UnitPerp (const RealV<Real,2>& rkV)
{
    RealV<Real,2> kPerp(rkV[1],-rkV[0]);
    Normalize(kPerp);
    return kPerp;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DotPerp (const RealV<Real,2>& rkU, const RealV<Real,2>& rkV)
{
    return rkU[0]*rkV[1] - rkU[1]*rkV[0];
}
//----------------------------------------------------------------------------
template <typename Real>
void GetBarycentrics (const RealV<Real,2>& rkP, const RealV<Real,2>& rkV0,
    const RealV<Real,2>& rkV1, const RealV<Real,2>& rkV2, Real afBary[3])
{
    // Compute the vectors relative to V2 of the triangle.
    RealV<Real,2> akDiff[3] =
    {
        rkV0 - rkV2,
        rkV1 - rkV2,
        rkP  - rkV2
    };

    // If the vertices have large magnitude, the linear system of equations
    // for computing barycentric coordinates can be ill-conditioned.  To avoid
    // this, uniformly scale the triangle edges to be of order 1.  The scaling
    // of all differences does not change the barycentric coordinates.
    Real fMax = (Real)0;
    int i;
    for (i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            Real fValue = Math<Real>::FAbs(akDiff[i][j]);
            if (fValue > fMax)
            {
                fMax = fValue;
            }
        }
    }

    // scale down only large data
    if (fMax > (Real)1)
    {
        Real fInvMax = ((Real)1)/fMax;
        for (i = 0; i < 3; i++)
        {
            akDiff[i] *= fInvMax;
        }
    }

    Real fDet = DotPerp(akDiff[0],akDiff[1]);
    if (Math<Real>::FAbs(fDet) > Math<Real>::ZERO_TOLERANCE)
    {
        Real fInvDet = ((Real)1)/fDet;
        afBary[0] = DotPerp(akDiff[2],akDiff[1])*fInvDet;
        afBary[1] = DotPerp(akDiff[0],akDiff[2])*fInvDet;
        afBary[2] = (Real)1 - afBary[0] - afBary[1];
    }
    else
    {
        // The triangle is a sliver.  Determine the longest edge and
        // compute barycentric coordinates with respect to that edge.
        RealV<Real,2> kE2 = rkV0 - rkV1;
        Real fMaxSqrLength = SquaredLength(kE2);
        int iMaxIndex = 2;
        Real fSqrLength = SquaredLength(akDiff[1]);
        if (fSqrLength > fMaxSqrLength)
        {
            iMaxIndex = 1;
            fMaxSqrLength = fSqrLength;
        }
        fSqrLength = SquaredLength(akDiff[0]);
        if (fSqrLength > fMaxSqrLength)
        {
            iMaxIndex = 0;
            fMaxSqrLength = fSqrLength;
        }

        if (fMaxSqrLength > Math<Real>::ZERO_TOLERANCE)
        {
            Real fInvSqrLength = ((Real)1)/fMaxSqrLength;
            if (iMaxIndex == 0)
            {
                // P-V2 = t(V0-V2)
                afBary[0] = Dot(akDiff[2],akDiff[0])*fInvSqrLength;
                afBary[1] = (Real)0;
                afBary[2] = (Real)1 - afBary[0];
            }
            else if (iMaxIndex == 1)
            {
                // P-V2 = t(V1-V2)
                afBary[0] = (Real)0;
                afBary[1] = Dot(akDiff[2],akDiff[1])*fInvSqrLength;
                afBary[2] = (Real)1 - afBary[1];
            }
            else
            {
                // P-V1 = t(V0-V1)
                akDiff[2] = rkP - rkV1;
                afBary[0] = Dot(akDiff[2],kE2)*fInvSqrLength;
                afBary[1] = (Real)1 - afBary[0];
                afBary[2] = (Real)0;
            }
        }
        else
        {
            // triangle is a nearly a point, just return equal weights
            afBary[0] = ((Real)1)/(Real)3;
            afBary[1] = afBary[0];
            afBary[2] = afBary[0];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool Orthonormalize (RealV<Real,2>& rkV0, RealV<Real,2>& rkV1)
{
    // Gram-Schmidt orthonormalization produces vectors u0 and u1 as follows:
    //   u0 = v0/|v0|
    //   u1 = (v1-Dot(u0,v1)*u0)/|v1-Dot(u0,v1)*u0|

    // Compute u0.
    Real fLength = Normalize(rkV0);
    if (fLength == (Real)0)
    {
        return false;
    }

    // Compute u1.
    Real fDot0 = Dot(rkV0,rkV1); 
    rkV1 -= rkV0*fDot0;
    fLength = Normalize(rkV1);
    return (fLength != (Real)0);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Orthonormalize (RealV<Real,2> akV[2])
{
    return Orthonormalize(akV[0],akV[1]);
}
//----------------------------------------------------------------------------
template <typename Real>
bool GenerateOrthonormalBasis (RealV<Real,2>& rkV0, RealV<Real,2>& rkV1)
{
    Real fLength = Normalize(rkV1);
    if (fLength == (Real)0)
    {
        return false;
    }

    rkV0 = Perp(rkV1);
    return true;
}
//----------------------------------------------------------------------------
