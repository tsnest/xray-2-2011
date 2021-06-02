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
template <typename Real, int N>
void ComputeExtremes (int iQuantity, const RealV<Real,N>* akVector,
    RealV<Real,N>& rkMin, RealV<Real,N>& rkMax)
{
    assert(iQuantity > 0 && akVector);

    rkMin = akVector[0];
    rkMax = rkMin;
    for (int i = 1; i < iQuantity; i++)
    {
        const RealV<Real,N>& rkVector = akVector[i];
        for (int j = 0; j < N; j++)
        {
            if (rkVector[j] < rkMin[j])
            {
                rkMin[j] = rkVector[j];
            }
            else if (rkVector[j] > rkMax[j])
            {
                rkMax[j] = rkVector[j];
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Real Length (const RealV<Real,N>& rkV)
{
    Real fResult = rkV[0]*rkV[0];
    for (int i = 1; i < N; i++)
    {
        fResult += rkV[i]*rkV[i];
    }
    return Math<Real>::Sqrt(fResult);
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Real SquaredLength (const RealV<Real,N>& rkV)
{
    Real fResult = rkV[0]*rkV[0];
    for (int i = 1; i < N; i++)
    {
        fResult += rkV[i]*rkV[i];
    }
    return fResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Real Dot (const RealV<Real,N>& rkV0, const RealV<Real,N>& rkV1)
{
    Real fResult = rkV0[0]*rkV1[0];
    for (int i = 1; i < N; i++)
    {
        fResult += rkV0[i]*rkV1[i];
    }
    return fResult;
}
//----------------------------------------------------------------------------
template <typename Real, int N>
Real Normalize (RealV<Real,N>& rkV)
{
    Real fLength = Length(rkV);

    if (fLength > Math<Real>::ZERO_TOLERANCE)
    {
        Real fInvLength = ((Real)1)/fLength;
        rkV[0] *= fInvLength;
        rkV[1] *= fInvLength;
    }
    else
    {
        fLength = (Real)0;
        rkV[0] = (Real)0;
        rkV[1] = (Real)0;
    }

    return fLength;
}
//----------------------------------------------------------------------------
