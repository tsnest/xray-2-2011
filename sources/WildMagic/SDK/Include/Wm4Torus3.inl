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
Torus3<Real>::Torus3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Torus3<Real>::Torus3 (Real fRo, Real fRi)
{
    assert(fRo > (Real)0.0 && fRi > (Real)0.0);

    Ro = fRo;
    Ri = fRi;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Torus3<Real>::Position (Real fS, Real fT)
{
    Real fTwoPiS = Math<Real>::TWO_PI*fS;
    Real fTwoPiT = Math<Real>::TWO_PI*fT;
    Real fCosTwoPiS = Math<Real>::Cos(fTwoPiS);
    Real fSinTwoPiS = Math<Real>::Sin(fTwoPiS);
    Real fCosTwoPiT = Math<Real>::Cos(fTwoPiT);
    Real fSinTwoPiT = Math<Real>::Sin(fTwoPiT);
    Real fRc = Ro + Ri*fCosTwoPiT;
    Vector3<Real> kPos(fRc*fCosTwoPiS,fRc*fSinTwoPiS,Ri*fSinTwoPiT);
    return kPos;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Torus3<Real>::Normal (Real fS, Real fT)
{
    Real fTwoPiS = Math<Real>::TWO_PI*fS;
    Real fCosTwoPiS = Math<Real>::Cos(fTwoPiS);
    Real fSinTwoPiS = Math<Real>::Sin(fTwoPiS);
    Vector3<Real> kPos = Position(fS,fT);
    Vector3<Real> kNor(kPos.X()-Ro*fCosTwoPiS,kPos.Y()-Ro*fSinTwoPiS,
        kPos.Z());
    kNor.Normalize();
    return kNor;
}
//----------------------------------------------------------------------------
template <class Real>
void Torus3<Real>::GetParameters (const Vector3<Real>& rkPos, Real& rfS,
    Real& rfT) const
{
    Real fRc = Math<Real>::Sqrt(rkPos.X()*rkPos.X() + rkPos.Y()*rkPos.Y());
    Real fAngle;

    if (fRc < Math<Real>::ZERO_TOLERANCE)
    {
        rfS = (Real)0.0;
    }
    else
    {
        fAngle = Math<Real>::ATan2(rkPos.Y(),rkPos.X());
        if (fAngle >= (Real)0.0)
        {
            rfS = fAngle*Math<Real>::INV_TWO_PI;
        }
        else
        {
            rfS = (Real)1.0 + fAngle*Math<Real>::INV_TWO_PI;
        }
    }

    Real fDiff = fRc - Ro;
    if (Math<Real>::FAbs(fDiff) < Math<Real>::ZERO_TOLERANCE
    &&  Math<Real>::FAbs(rkPos.Z()) < Math<Real>::ZERO_TOLERANCE)
    {
        rfT = (Real)0.0;
    }
    else
    {
        fAngle = Math<Real>::ATan2(rkPos.Z(),fDiff);
        if (fAngle >= (Real)0.0)
        {
            rfT = fAngle*Math<Real>::INV_TWO_PI;
        }
        else
        {
            rfT = (Real)1.0 + fAngle*Math<Real>::INV_TWO_PI;
        }
    }
}
//----------------------------------------------------------------------------
