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
RealV<Real,3> Cross (const RealV<Real,3>& rkV0, const RealV<Real,3>& rkV1)
{
    return RealV<Real,3>(
        rkV0[1]*rkV1[2] - rkV0[2]*rkV1[1],
        rkV0[2]*rkV1[0] - rkV0[0]*rkV1[2],
        rkV0[0]*rkV1[1] - rkV0[1]*rkV1[0]);
}
//----------------------------------------------------------------------------
template <typename Real>
RealV<Real,3> UnitCross (const RealV<Real,3>& rkV0, const RealV<Real,3>& rkV1)
{
    RealV<Real,3> kCross(
        rkV0[1]*rkV1[2] - rkV0[2]*rkV1[1],
        rkV0[2]*rkV1[0] - rkV0[0]*rkV1[2],
        rkV0[0]*rkV1[1] - rkV0[1]*rkV1[0]);
    Normalize(kCross);
    return kCross;
}
//----------------------------------------------------------------------------
template <typename Real>
void GetBarycentrics (const RealV<Real,3>& rkP, const RealV<Real,3>& rkV0,
    const RealV<Real,3>& rkV1, const RealV<Real,3>& rkV2,
    const RealV<Real,3>& rkV3, Real afBary[4])
{
    // Compute the vectors relative to V3 of the tetrahedron.
    RealV<Real,3> akDiff[4] =
    {
        rkV0 - rkV3,
        rkV1 - rkV3,
        rkV2 - rkV3,
        rkP  - rkV3
    };

    // If the vertices have large magnitude, the linear system of equations
    // for computing barycentric coordinates can be ill-conditioned.  To
    // avoid this, uniformly scale the tetrahedron edges to be of order 1.
    // The scaling of all differences does not change the barycentric
    // coordinates.
    Real fMax = (Real)0;
    int i;
    for (i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
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
        for (i = 0; i < 4; i++)
        {
            akDiff[i] *= fInvMax;
        }
    }

    Real fDet = Dot(akDiff[0],Cross(akDiff[1],akDiff[2]));
    RealV<Real,3> kE1cE2 = Cross(akDiff[1],akDiff[2]);
    RealV<Real,3> kE2cE0 = Cross(akDiff[2],akDiff[0]);
    RealV<Real,3> kE0cE1 = Cross(akDiff[0],akDiff[1]);
    if (Math<Real>::FAbs(fDet) > Math<Real>::ZERO_TOLERANCE)
    {
        Real fInvDet = ((Real)1)/fDet;
        afBary[0] = Dot(akDiff[3],kE1cE2)*fInvDet;
        afBary[1] = Dot(akDiff[3],kE2cE0)*fInvDet;
        afBary[2] = Dot(akDiff[3],kE0cE1)*fInvDet;
        afBary[3] = (Real)1 - afBary[0] - afBary[1] - afBary[2];
    }
    else
    {
        // The tetrahedron is potentially flat.  Determine the face of
        // maximum area and compute barycentric coordinates with respect
        // to that face.
        RealV<Real,3> kE02 = rkV0 - rkV2;
        RealV<Real,3> kE12 = rkV1 - rkV2;
        RealV<Real,3> kE02cE12 = Cross(kE02,kE12);
        Real fMaxSqrArea = SquaredLength(kE02cE12);
        int iMaxIndex = 3;
        Real fSqrArea = SquaredLength(kE0cE1);
        if (fSqrArea > fMaxSqrArea)
        {
            iMaxIndex = 0;
            fMaxSqrArea = fSqrArea;
        }
        fSqrArea = SquaredLength(kE1cE2);
        if (fSqrArea > fMaxSqrArea)
        {
            iMaxIndex = 1;
            fMaxSqrArea = fSqrArea;
        }
        fSqrArea = SquaredLength(kE2cE0);
        if (fSqrArea > fMaxSqrArea)
        {
            iMaxIndex = 2;
            fMaxSqrArea = fSqrArea;
        }

        if (fMaxSqrArea > Math<Real>::ZERO_TOLERANCE)
        {
            Real fInvSqrArea = ((Real)1)/fMaxSqrArea;
            RealV<Real,3> kTmp;
            if (iMaxIndex == 0)
            {
                kTmp = Cross(akDiff[3],akDiff[1]);
                afBary[0] = Dot(kE0cE1,kTmp)*fInvSqrArea;
                kTmp = Cross(akDiff[0],akDiff[3]);
                afBary[1] = Dot(kE0cE1,kTmp)*fInvSqrArea;
                afBary[2] = (Real)0;
                afBary[3] = (Real)1 - afBary[0] - afBary[1];
            }
            else if (iMaxIndex == 1)
            {
                afBary[0] = (Real)0;
                kTmp = Cross(akDiff[3],akDiff[2]);
                afBary[1] = Dot(kE1cE2,kTmp)*fInvSqrArea;
                kTmp = Cross(akDiff[1],akDiff[3]);
                afBary[2] = Dot(kE1cE2,kTmp)*fInvSqrArea;
                afBary[3] = (Real)1 - afBary[1] - afBary[2];
            }
            else if (iMaxIndex == 2)
            {
                kTmp = Cross(akDiff[2],akDiff[3]);
                afBary[0] = Dot(kE2cE0,kTmp)*fInvSqrArea;
                afBary[1] = (Real)0;
                kTmp = Cross(akDiff[3],akDiff[0]);
                afBary[2] = Dot(kE2cE0,kTmp)*fInvSqrArea;
                afBary[3] = (Real)1 - afBary[0] - afBary[2];
            }
            else
            {
                akDiff[3] = rkP - rkV2;
                kTmp = Cross(akDiff[3],kE12);
                afBary[0] = Dot(kE02cE12,kTmp)*fInvSqrArea;
                kTmp = Cross(kE02,akDiff[3]);
                afBary[1] = Dot(kE02cE12,kTmp)*fInvSqrArea;
                afBary[2] = (Real)1 - afBary[0] - afBary[1];
                afBary[3] = (Real)0;
            }
        }
        else
        {
            // The tetrahedron is potentially a sliver.  Determine the edge of
            // maximum length and compute barycentric coordinates with respect
            // to that edge.
            Real fMaxSqrLength = SquaredLength(akDiff[0]);
            iMaxIndex = 0;  // <V0,V3>
            Real fSqrLength = SquaredLength(akDiff[1]);
            if (fSqrLength > fMaxSqrLength)
            {
                iMaxIndex = 1;  // <V1,V3>
                fMaxSqrLength = fSqrLength;
            }
            fSqrLength = SquaredLength(akDiff[2]);
            if (fSqrLength > fMaxSqrLength)
            {
                iMaxIndex = 2;  // <V2,V3>
                fMaxSqrLength = fSqrLength;
            }
            fSqrLength = SquaredLength(kE02);
            if (fSqrLength > fMaxSqrLength)
            {
                iMaxIndex = 3;  // <V0,V2>
                fMaxSqrLength = fSqrLength;
            }
            fSqrLength = SquaredLength(kE12);
            if (fSqrLength > fMaxSqrLength)
            {
                iMaxIndex = 4;  // <V1,V2>
                fMaxSqrLength = fSqrLength;
            }
            RealV<Real,3> kE01 = rkV0 - rkV1;
            fSqrLength = SquaredLength(kE01);
            if (fSqrLength > fMaxSqrLength)
            {
                iMaxIndex = 5;  // <V0,V1>
                fMaxSqrLength = fSqrLength;
            }

            if (fMaxSqrLength > Math<Real>::ZERO_TOLERANCE)
            {
                Real fInvSqrLength = ((Real)1)/fMaxSqrLength;
                if (iMaxIndex == 0)
                {
                    // P-V3 = t*(V0-V3)
                    afBary[0] = Dot(akDiff[3],akDiff[0])*fInvSqrLength;
                    afBary[1] = (Real)0;
                    afBary[2] = (Real)0;
                    afBary[3] = (Real)1 - afBary[0];
                }
                else if (iMaxIndex == 1)
                {
                    // P-V3 = t*(V1-V3)
                    afBary[0] = (Real)0;
                    afBary[1] = Dot(akDiff[3],akDiff[1])*fInvSqrLength;
                    afBary[2] = (Real)0;
                    afBary[3] = (Real)1 - afBary[1];
                }
                else if (iMaxIndex == 2)
                {
                    // P-V3 = t*(V2-V3)
                    afBary[0] = (Real)0;
                    afBary[1] = (Real)0;
                    afBary[2] = Dot(akDiff[3],akDiff[2])*fInvSqrLength;
                    afBary[3] = (Real)1 - afBary[2];
                }
                else if (iMaxIndex == 3)
                {
                    // P-V2 = t*(V0-V2)
                    akDiff[3] = rkP - rkV2;
                    afBary[0] = Dot(akDiff[3],kE02)*fInvSqrLength;
                    afBary[1] = (Real)0;
                    afBary[2] = (Real)1 - afBary[0];
                    afBary[3] = (Real)0;
                }
                else if (iMaxIndex == 4)
                {
                    // P-V2 = t*(V1-V2)
                    akDiff[3] = rkP - rkV2;
                    afBary[0] = (Real)0;
                    afBary[1] = Dot(akDiff[3],kE12)*fInvSqrLength;
                    afBary[2] = (Real)1 - afBary[1];
                    afBary[3] = (Real)0;
                }
                else
                {
                    // P-V1 = t*(V0-V1)
                    akDiff[3] = rkP - rkV1;
                    afBary[0] = Dot(akDiff[3],kE01)*fInvSqrLength;
                    afBary[1] = (Real)1 - afBary[0];
                    afBary[2] = (Real)0;
                    afBary[3] = (Real)0;
                }
            }
            else
            {
                // tetrahedron is a nearly a point, just return equal weights
                afBary[0] = ((Real)1)/(Real)4;
                afBary[1] = afBary[0];
                afBary[2] = afBary[0];
                afBary[3] = afBary[0];
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool Orthonormalize (RealV<Real,3>& rkV0, RealV<Real,3>& rkV1,
    RealV<Real,3>& rkV2)
{
    // Gram-Schmidt orthonormalization produces vectors u0, u1, and u2 as
    // follows:
    //
    //   u0 = v0/|v0|
    //   u1 = (v1 - Dot(u0,v1)*u0)/|v1 - Dot(u0,v1)*u0|
    //   u2 = (v2 - Dot(u0,v2)*u0 - Dot(u1,v2)*u1) /
    //          |v2 - Dot(u0,v2)*u0 - Dot(u1,v2)*u1|

    // Compute u0.
    Real fLength = Normalize(rkV0);
    if (fLength == (Real)0)
    {
        return false;
    }

    // Compute u1.
    Real fDot0 = Dot(rkV0,rkV1); 
    rkV1 -= fDot0*rkV0;
    fLength = Normalize(rkV1);
    if (fLength == (Real)0)
    {
        return false;
    }

    // Compute u2.
    Real fDot1 = Dot(rkV1,rkV2);
    fDot0 = Dot(rkV0,rkV2);
    rkV2 -= fDot0*rkV0 + fDot1*rkV1;
    fLength = Normalize(rkV2);
    return (fLength != (Real)0);
}
//----------------------------------------------------------------------------
template <typename Real>
bool Orthonormalize (RealV<Real,3> akV[3])
{
    return Orthonormalize(akV[0],akV[1],akV[2]);
}
//----------------------------------------------------------------------------
template <typename Real>
bool GenerateOrthonormalBasis (RealV<Real,3>& rkV0, RealV<Real,3>& rkV1,
    RealV<Real,3>& rkV2)
{
    Real fLength = Normalize(rkV2);
    if (fLength == (Real)0)
    {
        return false;
    }

    GenerateComplementBasis(rkV0,rkV1,rkV2);
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void GenerateComplementBasis (RealV<Real,3>& rkV0, RealV<Real,3>& rkV1,
    const RealV<Real,3>& rkV2)
{
    Real fInvLength;

    if (Math<Real>::FAbs(rkV2[0]) >= Math<Real>::FAbs(rkV2[1]) )
    {
        // W.x or W.z is the largest magnitude component, swap them
        fInvLength = Math<Real>::InvSqrt(rkV2[0]*rkV2[0] + rkV2[2]*rkV2[2]);
        rkV0[0] = -rkV2[2]*fInvLength;
        rkV0[1] = (Real)0;
        rkV0[2] = rkV2[0]*fInvLength;
        rkV1[0] = rkV2[1]*rkV0[2];
        rkV1[1] = rkV2[2]*rkV0[0] - rkV2[0]*rkV0[2];
        rkV1[2] = -rkV2[1]*rkV0[0];
    }
    else
    {
        // W.y or W.z is the largest magnitude component, swap them
        fInvLength = Math<Real>::InvSqrt(rkV2[1]*rkV2[1] + rkV2[2]*rkV2[2]);
        rkV0[0] = (Real)0;
        rkV0[1] = rkV2[2]*fInvLength;
        rkV0[2] = -rkV2[1]*fInvLength;
        rkV1[0] = rkV2[1]*rkV0[2] - rkV2[2]*rkV0[1];
        rkV1[1] = -rkV2[0]*rkV0[2];
        rkV1[2] = rkV2[0]*rkV0[1];
    }
}
//----------------------------------------------------------------------------
