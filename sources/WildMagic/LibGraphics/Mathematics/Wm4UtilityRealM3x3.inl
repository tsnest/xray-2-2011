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
void MakeDiagonal (Real fM00, Real fM11, Real fM22, RealM<Real,3,3>& rkM)
{
    rkM[0][0] = fM00;
    rkM[0][1] = (Real)0;
    rkM[0][2] = (Real)0;
    rkM[1][0] = (Real)0;
    rkM[1][1] = fM11;
    rkM[1][2] = (Real)0;
    rkM[2][0] = (Real)0;
    rkM[2][1] = (Real)0;
    rkM[2][2] = fM22;
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,3,3> Adjugate (const RealM<Real,3,3>& rkM)
{
    RealM<Real,3,3> kAdjugate;
    kAdjugate[0][0] = rkM[1][1]*rkM[2][2] - rkM[1][2]*rkM[2][1];
    kAdjugate[0][1] = rkM[0][2]*rkM[2][1] - rkM[0][1]*rkM[2][2];
    kAdjugate[0][2] = rkM[0][1]*rkM[1][2] - rkM[0][2]*rkM[1][1];
    kAdjugate[1][0] = rkM[1][2]*rkM[2][0] - rkM[1][0]*rkM[2][2];
    kAdjugate[1][1] = rkM[0][0]*rkM[2][2] - rkM[0][2]*rkM[2][0];
    kAdjugate[1][2] = rkM[0][2]*rkM[1][0] - rkM[0][0]*rkM[1][2];
    kAdjugate[2][0] = rkM[1][0]*rkM[2][1] - rkM[1][1]*rkM[2][0];
    kAdjugate[2][1] = rkM[0][1]*rkM[2][0] - rkM[0][0]*rkM[2][1];
    kAdjugate[2][2] = rkM[0][0]*rkM[1][1] - rkM[0][1]*rkM[1][0];
    return kAdjugate;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Determinant (const RealM<Real,3,3>& rkM)
{
    Real fC00 = rkM[1][1]*rkM[2][2] - rkM[1][2]*rkM[2][1];
    Real fC10 = rkM[1][2]*rkM[2][0] - rkM[1][0]*rkM[2][2];
    Real fC20 = rkM[1][0]*rkM[2][1] - rkM[1][1]*rkM[2][0];
    Real fDet = rkM[0][0]*fC00 + rkM[0][1]*fC10 + rkM[0][2]*fC20;
    return fDet;
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,3,3> Inverse (const RealM<Real,3,3>& rkM)
{
    RealM<Real,3,3> kInverse;

    Real fC00 = rkM[1][1]*rkM[2][2] - rkM[1][2]*rkM[2][1];
    Real fC10 = rkM[1][2]*rkM[2][0] - rkM[1][0]*rkM[2][2];
    Real fC20 = rkM[1][0]*rkM[2][1] - rkM[1][1]*rkM[2][0];
    Real fDet = rkM[0][0]*fC00 + rkM[0][1]*fC10 + rkM[0][2]*fC20;
    if (Math<Real>::FAbs(fDet) > Math<Real>::ZERO_TOLERANCE)
    {
        kInverse[0][0] = fC00;
        kInverse[0][1] = rkM[0][2]*rkM[2][1] - rkM[0][1]*rkM[2][2];
        kInverse[0][2] = rkM[0][1]*rkM[1][2] - rkM[0][2]*rkM[1][1];
        kInverse[1][0] = fC10;
        kInverse[1][1] = rkM[0][0]*rkM[2][2] - rkM[0][2]*rkM[2][0];
        kInverse[1][2] = rkM[0][2]*rkM[1][0] - rkM[0][0]*rkM[1][2];
        kInverse[2][0] = fC20;
        kInverse[2][1] = rkM[0][1]*rkM[2][0] - rkM[0][0]*rkM[2][1];
        kInverse[2][2] = rkM[0][0]*rkM[1][1] - rkM[0][1]*rkM[1][0];

        Real fInvDet = ((Real)1)/fDet;
        kInverse[0][0] *= fInvDet;
        kInverse[0][1] *= fInvDet;
        kInverse[0][2] *= fInvDet;
        kInverse[1][0] *= fInvDet;
        kInverse[1][1] *= fInvDet;
        kInverse[1][2] *= fInvDet;
        kInverse[2][0] *= fInvDet;
        kInverse[2][1] *= fInvDet;
        kInverse[2][2] *= fInvDet;
    }
    else
    {
        MakeZero(kInverse);
    }

    return kInverse;
}
//----------------------------------------------------------------------------
template <typename Real>
void FromAxisAngle (const RealV<Real,3>& rkAxis, Real fAngle,
    RealM<Real,3,3>& rkRot)
{
    Real fCos = Math<Real>::Cos(fAngle);
    Real fSin = Math<Real>::Sin(fAngle);
    Real fOneMinusCos = ((Real)1)-fCos;
    Real fX2 = rkAxis[0]*rkAxis[0];
    Real fY2 = rkAxis[1]*rkAxis[1];
    Real fZ2 = rkAxis[2]*rkAxis[2];
    Real fXYM = rkAxis[0]*rkAxis[1]*fOneMinusCos;
    Real fXZM = rkAxis[0]*rkAxis[2]*fOneMinusCos;
    Real fYZM = rkAxis[1]*rkAxis[2]*fOneMinusCos;
    Real fXSin = rkAxis[0]*fSin;
    Real fYSin = rkAxis[1]*fSin;
    Real fZSin = rkAxis[2]*fSin;

    rkRot[0][0] = fX2*fOneMinusCos + fCos;
    rkRot[0][1] = fXYM - fZSin;
    rkRot[0][2] = fXZM + fYSin;
    rkRot[1][0] = fXYM + fZSin;
    rkRot[1][1] = fY2*fOneMinusCos + fCos;
    rkRot[1][2] = fYZM - fXSin;
    rkRot[2][0] = fXZM - fYSin;
    rkRot[2][1] = fYZM + fXSin;
    rkRot[2][2] = fZ2*fOneMinusCos + fCos;
}
//----------------------------------------------------------------------------
template <typename Real>
void ToAxisAngle (const RealM<Real,3,3>& rkRot, RealV<Real,3>& rkAxis,
    Real& rfAngle)
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise rotation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    Real fTrace = rkRot[0][0] + rkRot[1][1] + rkRot[2][2];
    Real fCos = ((Real)0.5)*(fTrace-(Real)1);
    rfAngle = Math<Real>::ACos(fCos);  // in [0,PI]

    if (rfAngle > (Real)0)
    {
        if (rfAngle < Math<Real>::PI)
        {
            rkAxis[0] = rkRot[2][1] - rkRot[1][2];
            rkAxis[1] = rkRot[0][2] - rkRot[2][0];
            rkAxis[2] = rkRot[1][0] - rkRot[0][1];
            Normalize(rkAxis);
        }
        else
        {
            // angle is PI
            Real fHalfInverse;
            if (rkRot[0][0] >= rkRot[1][1])
            {
                // r00 >= r11
                if (rkRot[0][0] >= rkRot[2][2])
                {
                    // r00 is maximum diagonal term
                    rkAxis[0] = ((Real)0.5)*Math<Real>::Sqrt(rkRot[0][0] -
                        rkRot[1][1] - rkRot[2][2] + (Real)1);
                    fHalfInverse = ((Real)0.5)/rkAxis[0];
                    rkAxis[1] = fHalfInverse*rkRot[0][1];
                    rkAxis[2] = fHalfInverse*rkRot[0][2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis[2] = ((Real)0.5)*Math<Real>::Sqrt(rkRot[2][2] -
                        rkRot[0][0] - rkRot[1][1] + (Real)1);
                    fHalfInverse = ((Real)0.5)/rkAxis[2];
                    rkAxis[0] = fHalfInverse*rkRot[0][2];
                    rkAxis[1] = fHalfInverse*rkRot[1][2];
                }
            }
            else
            {
                // r11 > r00
                if (rkRot[1][1] >= rkRot[2][2])
                {
                    // r11 is maximum diagonal term
                    rkAxis[1] = ((Real)0.5)*Math<Real>::Sqrt(rkRot[1][1] -
                        rkRot[0][0] - rkRot[2][2] + (Real)1);
                    fHalfInverse  = ((Real)0.5)/rkAxis[1];
                    rkAxis[0] = fHalfInverse*rkRot[0][1];
                    rkAxis[2] = fHalfInverse*rkRot[1][2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis[2] = ((Real)0.5)*Math<Real>::Sqrt(rkRot[2][2] -
                        rkRot[0][0] - rkRot[1][1] + (Real)1);
                    fHalfInverse = ((Real)0.5)/rkAxis[2];
                    rkAxis[0] = fHalfInverse*rkRot[0][2];
                    rkAxis[1] = fHalfInverse*rkRot[1][2];
                }
            }
        }
    }
    else
    {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        rkAxis[0] = (Real)1;
        rkAxis[1] = (Real)0;
        rkAxis[2] = (Real)0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void MakeXRotation (Real fAngle, RealM<Real,3,3>& rkRot)
{
    Real fCos = Math<Real>::Cos(fAngle);
    Real fSin = Math<Real>::Sin(fAngle);
    rkRot[0][0] = (Real)1;
    rkRot[0][1] = (Real)0;
    rkRot[0][2] = (Real)0;
    rkRot[1][0] = (Real)0;
    rkRot[1][1] = fCos;
    rkRot[1][2] = -fSin;
    rkRot[2][0] = (Real)0;
    rkRot[2][1] = fSin;
    rkRot[2][2] = fCos;
}
//----------------------------------------------------------------------------
template <typename Real>
void MakeYRotation (Real fAngle, RealM<Real,3,3>& rkRot)
{
    Real fCos = Math<Real>::Cos(fAngle);
    Real fSin = Math<Real>::Sin(fAngle);
    rkRot[0][0] = fCos;
    rkRot[0][1] = (Real)0;
    rkRot[0][2] = fSin;
    rkRot[1][0] = (Real)0;
    rkRot[1][1] = (Real)1;
    rkRot[1][2] = (Real)0;
    rkRot[2][0] = -fSin;
    rkRot[2][1] = (Real)0;
    rkRot[2][2] = fCos;
}
//----------------------------------------------------------------------------
template <typename Real>
void MakeZRotation (Real fAngle, RealM<Real,3,3>& rkRot)
{
    Real fCos = Math<Real>::Cos(fAngle);
    Real fSin = Math<Real>::Sin(fAngle);
    rkRot[0][0] = fCos;
    rkRot[0][1] = -fSin;
    rkRot[0][2] = (Real)0;
    rkRot[1][0] = fSin;
    rkRot[1][1] = fCos;
    rkRot[1][2] = (Real)0;
    rkRot[2][0] = (Real)0;
    rkRot[2][1] = (Real)0;
    rkRot[2][2] = (Real)1;
}
//----------------------------------------------------------------------------
template <typename Real>
void FromEulerAnglesXYZ (Real fXAngle, Real fYAngle, Real fZAngle,
    RealM<Real,3,3>& rkRot)
{
    RealM<Real,3,3> kXRot, kYRot, kZRot;
    MakeXRotation(fXAngle,kXRot);
    MakeYRotation(fYAngle,kYRot);
    MakeZRotation(fZAngle,kZRot);
    rkRot = kXRot*(kYRot*kZRot);
}
//----------------------------------------------------------------------------
template <typename Real>
void FromEulerAnglesXZY (Real fXAngle, Real fZAngle, Real fYAngle,
    RealM<Real,3,3>& rkRot)
{
    RealM<Real,3,3> kXRot, kYRot, kZRot;
    MakeXRotation(fXAngle,kXRot);
    MakeYRotation(fYAngle,kYRot);
    MakeZRotation(fZAngle,kZRot);
    rkRot = kXRot*(kZRot*kYRot);
}
//----------------------------------------------------------------------------
template <typename Real>
void FromEulerAnglesYXZ (Real fYAngle, Real fXAngle, Real fZAngle,
    RealM<Real,3,3>& rkRot)
{
    RealM<Real,3,3> kXRot, kYRot, kZRot;
    MakeXRotation(fXAngle,kXRot);
    MakeYRotation(fYAngle,kYRot);
    MakeZRotation(fZAngle,kZRot);
    rkRot = kYRot*(kXRot*kZRot);
}
//----------------------------------------------------------------------------
template <typename Real>
void FromEulerAnglesYZX (Real fYAngle, Real fZAngle, Real fXAngle,
    RealM<Real,3,3>& rkRot)
{
    RealM<Real,3,3> kXRot, kYRot, kZRot;
    MakeXRotation(fXAngle,kXRot);
    MakeYRotation(fYAngle,kYRot);
    MakeZRotation(fZAngle,kZRot);
    rkRot = kYRot*(kZRot*kXRot);
}
//----------------------------------------------------------------------------
template <typename Real>
void FromEulerAnglesZXY (Real fZAngle, Real fXAngle, Real fYAngle,
    RealM<Real,3,3>& rkRot)
{
    RealM<Real,3,3> kXRot, kYRot, kZRot;
    MakeXRotation(fXAngle,kXRot);
    MakeYRotation(fYAngle,kYRot);
    MakeZRotation(fZAngle,kZRot);
    rkRot = kZRot*(kXRot*kYRot);
}
//----------------------------------------------------------------------------
template <typename Real>
void FromEulerAnglesZYX (Real fZAngle, Real fYAngle, Real fXAngle,
    RealM<Real,3,3>& rkRot)
{
    RealM<Real,3,3> kXRot, kYRot, kZRot;
    MakeXRotation(fXAngle,kXRot);
    MakeYRotation(fYAngle,kYRot);
    MakeZRotation(fZAngle,kZRot);
    rkRot = kZRot*(kYRot*kXRot);
}
//----------------------------------------------------------------------------
template <typename Real>
bool ToEulerAnglesXYZ (const RealM<Real,3,3>& rkRot, Real& rfXAngle,
    Real& rfYAngle, Real& rfZAngle)
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if (rkRot[0][2] < (Real)1)
    {
        if (rkRot[0][2] > -(Real)1)
        {
            rfXAngle = Math<Real>::ATan2(-rkRot[1][2],rkRot[2][2]);
            rfYAngle = (Real)asin((double)rkRot[0][2]);
            rfZAngle = Math<Real>::ATan2(-rkRot[0][1],rkRot[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = -Math<Real>::ATan2(rkRot[1][0],rkRot[1][1]);
            rfYAngle = -Math<Real>::HALF_PI;
            rfZAngle = (Real)0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = Math<Real>::ATan2(rkRot[1][0],rkRot[1][1]);
        rfYAngle = Math<Real>::HALF_PI;
        rfZAngle = (Real)0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ToEulerAnglesXZY (const RealM<Real,3,3>& rkRot, Real& rfXAngle,
    Real& rfZAngle, Real& rfYAngle)
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    if (rkRot[0][1] < (Real)1)
    {
        if (rkRot[0][1] > -(Real)1)
        {
            rfXAngle = Math<Real>::ATan2(rkRot[2][1],rkRot[1][1]);
            rfZAngle = (Real)asin(-(double)rkRot[0][1]);
            rfYAngle = Math<Real>::ATan2(rkRot[0][2],rkRot[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - YA = atan2(r20,r22)
            rfXAngle = Math<Real>::ATan2(rkRot[2][0],rkRot[2][2]);
            rfZAngle = Math<Real>::HALF_PI;
            rfYAngle = (Real)0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
        rfXAngle = Math<Real>::ATan2(-rkRot[2][0],rkRot[2][2]);
        rfZAngle = -Math<Real>::HALF_PI;
        rfYAngle = (Real)0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ToEulerAnglesYXZ (const RealM<Real,3,3>& rkRot, Real& rfYAngle,
    Real& rfXAngle, Real& rfZAngle)
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    if (rkRot[1][2] < (Real)1)
    {
        if (rkRot[1][2] > -(Real)1)
        {
            rfYAngle = Math<Real>::ATan2(rkRot[0][2],rkRot[2][2]);
            rfXAngle = (Real)asin(-(double)rkRot[1][2]);
            rfZAngle = Math<Real>::ATan2(rkRot[1][0],rkRot[1][1]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
            rfYAngle = Math<Real>::ATan2(rkRot[0][1],rkRot[0][0]);
            rfXAngle = Math<Real>::HALF_PI;
            rfZAngle = (Real)0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
        rfYAngle = Math<Real>::ATan2(-rkRot[0][1],rkRot[0][0]);
        rfXAngle = -Math<Real>::HALF_PI;
        rfZAngle = (Real)0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ToEulerAnglesYZX (const RealM<Real,3,3>& rkRot, Real& rfYAngle,
    Real& rfZAngle, Real& rfXAngle)
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    if (rkRot[1][0] < (Real)1)
    {
        if (rkRot[1][0] > -(Real)1)
        {
            rfYAngle = Math<Real>::ATan2(-rkRot[2][0],rkRot[0][0]);
            rfZAngle = (Real)asin((double)rkRot[1][0]);
            rfXAngle = Math<Real>::ATan2(-rkRot[1][2],rkRot[1][1]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
            rfYAngle = -Math<Real>::ATan2(rkRot[2][1],rkRot[2][2]);
            rfZAngle = -Math<Real>::HALF_PI;
            rfXAngle = (Real)0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + XA = atan2(r21,r22)
        rfYAngle = Math<Real>::ATan2(rkRot[2][1],rkRot[2][2]);
        rfZAngle = Math<Real>::HALF_PI;
        rfXAngle = (Real)0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ToEulerAnglesZXY (const RealM<Real,3,3>& rkRot, Real& rfZAngle,
    Real& rfXAngle, Real& rfYAngle)
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    if (rkRot[2][1] < (Real)1)
    {
        if (rkRot[2][1] > -(Real)1)
        {
            rfZAngle = Math<Real>::ATan2(-rkRot[0][1],rkRot[1][1]);
            rfXAngle = (Real)asin((double)rkRot[2][1]);
            rfYAngle = Math<Real>::ATan2(-rkRot[2][0],rkRot[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
            rfZAngle = -Math<Real>::ATan2(rkRot[0][2],rkRot[0][0]);
            rfXAngle = -Math<Real>::HALF_PI;
            rfYAngle = (Real)0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
        rfZAngle = Math<Real>::ATan2(rkRot[0][2],rkRot[0][0]);
        rfXAngle = Math<Real>::HALF_PI;
        rfYAngle = (Real)0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool ToEulerAnglesZYX (const RealM<Real,3,3>& rkRot, Real& rfZAngle,
    Real& rfYAngle, Real& rfXAngle)
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    if (rkRot[2][0] < (Real)1)
    {
        if (rkRot[2][0] > -(Real)1)
        {
            rfZAngle = Math<Real>::ATan2(rkRot[1][0],rkRot[0][0]);
            rfYAngle = (Real)asin(-(double)rkRot[2][0]);
            rfXAngle = Math<Real>::ATan2(rkRot[2][1],rkRot[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            rfZAngle = -Math<Real>::ATan2(rkRot[0][1],rkRot[0][2]);
            rfYAngle = Math<Real>::HALF_PI;
            rfXAngle = (Real)0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        rfZAngle = Math<Real>::ATan2(-rkRot[0][1],-rkRot[0][2]);
        rfYAngle = -Math<Real>::HALF_PI;
        rfXAngle = (Real)0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool Orthonormalize (RealM<Real,3,3>& rkM)
{
    // The algorithm uses Gram-Schmidt orthogonalization applied to the
    // columns of M.
    RealV<Real,3> kQ0(rkM[0][0],rkM[1][0],rkM[2][0]);
    RealV<Real,3> kQ1(rkM[0][1],rkM[1][1],rkM[2][1]);
    RealV<Real,3> kQ2(rkM[0][2],rkM[1][2],rkM[2][2]);
    if (Orthonormalize(kQ0,kQ1,kQ2))
    {
        rkM[0][0] = kQ0[0];
        rkM[1][0] = kQ0[1];
        rkM[2][0] = kQ0[2];
        rkM[0][1] = kQ1[0];
        rkM[1][1] = kQ1[1];
        rkM[2][1] = kQ1[2];
        rkM[0][2] = kQ2[0];
        rkM[1][2] = kQ2[1];
        rkM[2][2] = kQ2[2];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool Tridiagonalize (RealM<Real,3,3>& rkM, Real afDiag[3], Real afSubd[2])
{
    // Householder reduction T = Q^t M Q
    //   Input:   
    //     mat, symmetric 3x3 matrix M
    //   Output:  
    //     mat, orthogonal matrix Q (a reflection)
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    // M is symmetric, so these are the 6 unique elements of the matrix.
    Real fM00 = rkM[0][0];
    Real fM01 = rkM[0][1];
    Real fM02 = rkM[0][2];
    Real fM11 = rkM[1][1];
    Real fM12 = rkM[1][2];
    Real fM22 = rkM[2][2];

    afDiag[0] = fM00;
    if (Math<Real>::FAbs(fM02) >= Math<Real>::ZERO_TOLERANCE)
    {
        afSubd[0] = Math<Real>::Sqrt(fM01*fM01+fM02*fM02);
        Real fInvLength = ((Real)1)/afSubd[0];
        fM01 *= fInvLength;
        fM02 *= fInvLength;
        Real fTmp = ((Real)2)*fM01*fM12 + fM02*(fM22 - fM11);
        afDiag[1] = fM11 + fM02*fTmp;
        afDiag[2] = fM22 - fM02*fTmp;
        afSubd[1] = fM12 - fM01*fTmp;

        rkM[0][0] = (Real)1;
        rkM[0][1] = (Real)0;
        rkM[0][2] = (Real)0;
        rkM[1][0] = (Real)0;
        rkM[1][1] = fM01;
        rkM[1][2] = fM02;
        rkM[2][0] = (Real)0;
        rkM[2][1] = fM02;
        rkM[2][2] = -fM01;
        return true;
    }

    afDiag[1] = fM11;
    afDiag[2] = fM22;
    afSubd[0] = fM01;
    afSubd[1] = fM12;
    MakeIdentity(rkM);
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool QLAlgorithm (RealM<Real,3,3>& rkM, Real afDiag[3], Real afSubd[2])
{
    // This is an implementation of the symmetric QR algorithm from the book
    // "Matrix Computations" by Gene H. Golub and Charles F. Van Loan, second
    // edition.  The algorithm is 8.2.3.  The implementation has a slight
    // variation to actually make it a QL algorithm, and it traps the case
    // when either of the subdiagonal terms s0 or s1 is zero and reduces the
    // 2-by-2 subblock directly.

    const int iMax = 32;
    for (int i = 0; i < iMax; i++)
    {
        Real fSum, fDiff, fDiscr, fEValue0, fEValue1, fCos, fSin, fTmp;
        int iRow;

        fSum = Math<Real>::FAbs(afDiag[0]) + Math<Real>::FAbs(afDiag[1]);
        if (Math<Real>::FAbs(afSubd[0]) + fSum == fSum)
        {
            // The matrix is effectively
            //       +-        -+
            //   M = | d0  0  0 |
            //       | 0  d1 s1 |
            //       | 0  s1 d2 |
            //       +-        -+

            // Compute the eigenvalues as roots of a quadratic equation.
            fSum = afDiag[1] + afDiag[2];
            fDiff = afDiag[1] - afDiag[2];
            fDiscr = Math<Real>::Sqrt(fDiff*fDiff +
                ((Real)4)*afSubd[1]*afSubd[1]);
            fEValue0 = ((Real)0.5)*(fSum - fDiscr);
            fEValue1 = ((Real)0.5)*(fSum + fDiscr);

            // Compute the Givens rotation.
            if (fDiff >= (Real)0)
            {
                fCos = afSubd[1];
                fSin = afDiag[1] - fEValue0;
            }
            else
            {
                fCos = afDiag[2] - fEValue0;
                fSin = afSubd[1];
            }
            fTmp = Math<Real>::InvSqrt(fCos*fCos + fSin*fSin);
            fCos *= fTmp;
            fSin *= fTmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // rotation.
            for (iRow = 0; iRow < 3; iRow++)
            {
                fTmp = rkM[iRow][2];
                rkM[iRow][2] = fSin*rkM[iRow][1] + fCos*fTmp;
                rkM[iRow][1] = fCos*rkM[iRow][1] - fSin*fTmp;
            }

            // Update the tridiagonal matrix.
            afDiag[1] = fEValue0;
            afDiag[2] = fEValue1;
            afSubd[0] = (Real)0;
            afSubd[1] = (Real)0;
            return true;
        }

        fSum = Math<Real>::FAbs(afDiag[1]) + Math<Real>::FAbs(afDiag[2]);
        if (Math<Real>::FAbs(afSubd[1]) + fSum == fSum)
        {
            // The matrix is effectively
            //       +-         -+
            //   M = | d0  s0  0 |
            //       | s0  d1  0 |
            //       | 0   0  d2 |
            //       +-         -+

            // Compute the eigenvalues as roots of a quadratic equation.
            fSum = afDiag[0] + afDiag[1];
            fDiff = afDiag[0] - afDiag[1];
            fDiscr = Math<Real>::Sqrt(fDiff*fDiff +
                ((Real)4)*afSubd[0]*afSubd[0]);
            fEValue0 = ((Real)0.5)*(fSum - fDiscr);
            fEValue1 = ((Real)0.5)*(fSum + fDiscr);

            // Compute the Givens rotation.
            if (fDiff >= (Real)0)
            {
                fCos = afSubd[0];
                fSin = afDiag[0] - fEValue0;
            }
            else
            {
                fCos = afDiag[1] - fEValue0;
                fSin = afSubd[0];
            }
            fTmp = Math<Real>::InvSqrt(fCos*fCos + fSin*fSin);
            fCos *= fTmp;
            fSin *= fTmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // rotation.
            for (iRow = 0; iRow < 3; iRow++)
            {
                fTmp = rkM[iRow][1];
                rkM[iRow][1] = fSin*rkM[iRow][0] + fCos*fTmp;
                rkM[iRow][0] = fCos*rkM[iRow][0] - fSin*fTmp;
            }

            // Update the tridiagonal matrix.
            afDiag[0] = fEValue0;
            afDiag[1] = fEValue1;
            afSubd[0] = (Real)0;
            afSubd[1] = (Real)0;
            return true;
        }

        // The matrix is
        //       +-        -+
        //   M = | d0 s0  0 |
        //       | s0 d1 s1 |
        //       | 0  s1 d2 |
        //       +-        -+

        // Set up the parameters for the first pass of the QL step.  The
        // value for A is the difference between diagonal term D[2] and the
        // implicit shift suggested by Wilkinson.
        Real fRatio = (afDiag[1]-afDiag[0])/(((Real)2)*afSubd[0]);
        Real fRoot = Math<Real>::Sqrt((Real)1 + fRatio*fRatio);
        Real fB = afSubd[1];
        Real fA = afDiag[2] - afDiag[0];
        if (fRatio >= (Real)0)
        {
            fA += afSubd[0]/(fRatio + fRoot);
        }
        else
        {
            fA += afSubd[0]/(fRatio - fRoot);
        }

        // Compute the Givens rotation for the first pass.
        if (Math<Real>::FAbs(fB) >= Math<Real>::FAbs(fA))
        {
            fRatio = fA/fB;
            fSin = Math<Real>::InvSqrt((Real)1 + fRatio*fRatio);
            fCos = fRatio*fSin;
        }
        else
        {
            fRatio = fB/fA;
            fCos = Math<Real>::InvSqrt((Real)1 + fRatio*fRatio);
            fSin = fRatio*fCos;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // rotation.
        for (iRow = 0; iRow < 3; iRow++)
        {
            fTmp = rkM[iRow][2];
            rkM[iRow][2] = fSin*rkM[iRow][1] + fCos*fTmp;
            rkM[iRow][1] = fCos*rkM[iRow][1] - fSin*fTmp;
        }

        // Set up the parameters for the second pass of the QL step.  The
        // values tmp0 and tmp1 are required to fully update the tridiagonal
        // matrix at the end of the second pass.
        Real fTmp0 = (afDiag[1] - afDiag[2])*fSin + ((Real)2)*afSubd[1]*fCos;
        Real fTmp1 = fCos*afSubd[0];
        fB = fSin*afSubd[0];
        fA = fCos*fTmp0 - afSubd[1];
        fTmp0 *= fSin;

        // Compute the Givens rotation for the second pass.  The subdiagonal
        // term S[1] in the tridiagonal matrix is updated at this time.
        if (Math<Real>::FAbs(fB) >= Math<Real>::FAbs(fA))
        {
            fRatio = fA/fB;
            fRoot = Math<Real>::Sqrt((Real)1 + fRatio*fRatio);
            afSubd[1] = fB*fRoot;
            fSin = ((Real)1)/fRoot;
            fCos = fRatio*fSin;
        }
        else
        {
            fRatio = fB/fA;
            fRoot = Math<Real>::Sqrt((Real)1 + fRatio*fRatio);
            afSubd[1] = fA*fRoot;
            fCos = ((Real)1)/fRoot;
            fSin = fRatio*fCos;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // rotation.
        for (iRow = 0; iRow < 3; iRow++)
        {
            fTmp = rkM[iRow][1];
            rkM[iRow][1] = fSin*rkM[iRow][0] + fCos*fTmp;
            rkM[iRow][0] = fCos*rkM[iRow][0] - fSin*fTmp;
        }

        // Complete the update of the tridiagonal matrix.
        Real fTmp2 = afDiag[1] - fTmp0;
        afDiag[2] += fTmp0;
        fTmp0 = (afDiag[0] - fTmp2)*fSin + ((Real)2)*fTmp1*fCos;
        afSubd[0] = fCos*fTmp0 - fTmp1;
        fTmp0 *= fSin;
        afDiag[1] = fTmp2 + fTmp0;
        afDiag[0] -= fTmp0;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
void EigenDecomposition (const RealM<Real,3,3>& rkM, RealM<Real,3,3>& rkRot,
    RealM<Real,3,3>& rkDiag)
{
    // Factor M = R*D*R^T.  The columns of R are the eigenvectors.  The
    // diagonal entries of D are the corresponding eigenvalues.
    Real afDiag[3], afSubd[2];
    rkRot = rkM;
    bool bReflection = Tridiagonalize(rkRot,afDiag,afSubd);
    bool bConverged = QLAlgorithm(rkRot,afDiag,afSubd);
    assert(bConverged);

    // Sort the eigenvalues in increasing order, d0 <= d1 <= d2.
    int i;
    Real fSave;

    if (afDiag[1] < afDiag[0])
    {
        // Swap d0 and d1.
        fSave = afDiag[0];
        afDiag[0] = afDiag[1];
        afDiag[1] = fSave;

        // Swap V0 and V1.
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][0];
            rkRot[i][0] = rkRot[i][1];
            rkRot[i][1] = fSave;
        }
        bReflection = !bReflection;
    }

    if (afDiag[2] < afDiag[1])
    {
        // Swap d1 and d2.
        fSave = afDiag[1];
        afDiag[1] = afDiag[2];
        afDiag[2] = fSave;

        // Swap V1 and V2.
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][1];
            rkRot[i][1] = rkRot[i][2];
            rkRot[i][2] = fSave;
        }
        bReflection = !bReflection;
    }

    if (afDiag[1] < afDiag[0])
    {
        // Swap d0 and d1.
        fSave = afDiag[0];
        afDiag[0] = afDiag[1];
        afDiag[1] = fSave;

        // Swap V0 and V1.
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][0];
            rkRot[i][0] = rkRot[i][1];
            rkRot[i][1] = fSave;
        }
        bReflection = !bReflection;
    }

    MakeDiagonal(afDiag[0],afDiag[1],afDiag[2],rkDiag);

    if (bReflection)
    {
        // The orthogonal transformation that diagonalizes M is a reflection.
        // Make the eigenvectors a right-handed system by changing sign on
        // the last column.
        rkRot[0][2] = -rkRot[0][2];
        rkRot[1][2] = -rkRot[1][2];
        rkRot[2][2] = -rkRot[2][2];
    }
}
//----------------------------------------------------------------------------
template <typename Real>
bool QDUDecomposition (const RealM<Real,3,3>& rkM, RealM<Real,3,3>& rkQ,
    RealM<Real,3,3>& rkD, RealM<Real,3,3>& rkU)
{
    // Factor M = QR = QDU where Q is orthogonal (rotation), D is diagonal
    // (scaling), and U is upper triangular with ones on its diagonal
    // (shear).  Algorithm uses Gram-Schmidt orthogonalization (the QR
    // algorithm).
    //
    // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The 'right' matrix R has entries
    //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
    //   r10 = 0      r11 = q1*m1  r12 = q1*m2
    //   r20 = 0      r21 = 0      r22 = q2*m2
    // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
    // u02 = r02/r00, and u12 = r12/r11.

    // The algorithm uses Gram-Schmidt orthogonalization applied to the
    // columns of M to obtain Q.
    rkQ = rkM;
    if (!Orthonormalize(rkQ))
    {
        return false;
    }

    // Ensure that Q has determinant 1 (a rotation, not a reflection).
    Real fDet = Determinant(rkQ);
    if (fDet < (Real)0)
    {
        rkQ *= (Real)-1;
    }

    // Build the 'right' matrix R.
    RealM<Real,3,3> kR;
    kR[0][0] = rkQ[0][0]*rkM[0][0]+rkQ[1][0]*rkM[1][0]+rkQ[2][0]*rkM[2][0];
    kR[0][1] = rkQ[0][0]*rkM[0][1]+rkQ[1][0]*rkM[1][1]+rkQ[2][0]*rkM[2][1];
    kR[1][1] = rkQ[0][1]*rkM[0][1]+rkQ[1][1]*rkM[1][1]+rkQ[2][1]*rkM[2][1];
    kR[0][2] = rkQ[0][0]*rkM[0][2]+rkQ[1][0]*rkM[1][2]+rkQ[2][0]*rkM[2][2];
    kR[1][2] = rkQ[0][1]*rkM[0][2]+rkQ[1][1]*rkM[1][2]+rkQ[2][1]*rkM[2][2];
    kR[2][2] = rkQ[0][2]*rkM[0][2]+rkQ[1][2]*rkM[1][2]+rkQ[2][2]*rkM[2][2];

    // Build the diagonal matrix D.
    MakeDiagonal(kR[0][0],kR[1][1],kR[2][2],rkD);

    // Build the shear component U.
    Real fInvD0 = ((Real)1)/rkD[0][0];
    rkU[0][0] = (Real)1;
    rkU[0][1] = kR[0][1]*fInvD0;
    rkU[0][2] = kR[0][2]*fInvD0;
    rkU[1][0] = (Real)0;
    rkU[1][1] = (Real)1;
    rkU[1][2] = kR[1][2]/rkD[1][1];
    rkU[2][0] = (Real)0;
    rkU[2][1] = (Real)0;
    rkU[2][2] = (Real)1;

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
void Bidiagonalize (RealM<Real,3,3>& rkA, RealM<Real,3,3>& rkL,
    RealM<Real,3,3>& rkR)
{
    Real afV[3], afW[3];
    Real fLength, fSign, fT1, fInvT1, fT2;
    bool bIdentity;

    // map first column to (*,0,0)
    fLength = Math<Real>::Sqrt(rkA[0][0]*rkA[0][0] + rkA[1][0]*rkA[1][0] +
        rkA[2][0]*rkA[2][0]);
    if (fLength > (Real)0)
    {
        fSign = (rkA[0][0] > (Real)0 ? (Real)1 : (Real)-1);
        fT1 = rkA[0][0] + fSign*fLength;
        fInvT1 = ((Real)1)/fT1;
        afV[1] = rkA[1][0]*fInvT1;
        afV[2] = rkA[2][0]*fInvT1;

        fT2 = -((Real)2)/(((Real)1)+afV[1]*afV[1]+afV[2]*afV[2]);
        afW[0] = fT2*(rkA[0][0]+rkA[1][0]*afV[1]+rkA[2][0]*afV[2]);
        afW[1] = fT2*(rkA[0][1]+rkA[1][1]*afV[1]+rkA[2][1]*afV[2]);
        afW[2] = fT2*(rkA[0][2]+rkA[1][2]*afV[1]+rkA[2][2]*afV[2]);
        rkA[0][0] += afW[0];
        rkA[0][1] += afW[1];
        rkA[0][2] += afW[2];
        rkA[1][1] += afV[1]*afW[1];
        rkA[1][2] += afV[1]*afW[2];
        rkA[2][1] += afV[2]*afW[1];
        rkA[2][2] += afV[2]*afW[2];

        rkL[0][0] = ((Real)1)+fT2;
        rkL[0][1] = fT2*afV[1];
        rkL[1][0] = rkL[0][1];
        rkL[0][2] = fT2*afV[2];
        rkL[2][0] = rkL[0][2];
        rkL[1][1] = ((Real)1.0)+fT2*afV[1]*afV[1];
        rkL[1][2] = fT2*afV[1]*afV[2];
        rkL[2][1] = rkL[1][2];
        rkL[2][2] = ((Real)1.0)+fT2*afV[2]*afV[2];
        bIdentity = false;
    }
    else
    {
        MakeIdentity(rkL);
        bIdentity = true;
    }

    // map first row to (*,*,0)
    fLength = Math<Real>::Sqrt(rkA[0][1]*rkA[0][1]+rkA[0][2]*rkA[0][2]);
    if (fLength > (Real)0)
    {
        fSign = (rkA[0][1] > (Real)0 ? (Real)1 : (Real)-1);
        fT1 = rkA[0][1] + fSign*fLength;
        afV[2] = rkA[0][2]/fT1;

        fT2 = -((Real)2)/(((Real)1)+afV[2]*afV[2]);
        afW[0] = fT2*(rkA[0][1]+rkA[0][2]*afV[2]);
        afW[1] = fT2*(rkA[1][1]+rkA[1][2]*afV[2]);
        afW[2] = fT2*(rkA[2][1]+rkA[2][2]*afV[2]);
        rkA[0][1] += afW[0];
        rkA[1][1] += afW[1];
        rkA[1][2] += afW[1]*afV[2];
        rkA[2][1] += afW[2];
        rkA[2][2] += afW[2]*afV[2];
        
        rkR[0][0] = (Real)1;
        rkR[0][1] = (Real)0;
        rkR[1][0] = (Real)0;
        rkR[0][2] = (Real)0;
        rkR[2][0] = (Real)0;
        rkR[1][1] = ((Real)1)+fT2;
        rkR[1][2] = fT2*afV[2];
        rkR[2][1] = rkR[1][2];
        rkR[2][2] = ((Real)1)+fT2*afV[2]*afV[2];
    }
    else
    {
        MakeIdentity(rkR);
    }

    // map second column to (*,*,0)
    fLength = Math<Real>::Sqrt(rkA[1][1]*rkA[1][1]+rkA[2][1]*rkA[2][1]);
    if (fLength > (Real)0)
    {
        fSign = (rkA[1][1] > (Real)0 ? (Real)1 : (Real)-1);
        fT1 = rkA[1][1] + fSign*fLength;
        afV[2] = rkA[2][1]/fT1;

        fT2 = -((Real)2)/(((Real)1)+afV[2]*afV[2]);
        afW[1] = fT2*(rkA[1][1]+rkA[2][1]*afV[2]);
        afW[2] = fT2*(rkA[1][2]+rkA[2][2]*afV[2]);
        rkA[1][1] += afW[1];
        rkA[1][2] += afW[2];
        rkA[2][2] += afV[2]*afW[2];

        Real fA = ((Real)1)+fT2;
        Real fB = fT2*afV[2];
        Real fC = ((Real)1)+fB*afV[2];

        if (bIdentity)
        {
            rkL[0][0] = (Real)1;
            rkL[0][1] = (Real)0;
            rkL[1][0] = (Real)0;
            rkL[0][2] = (Real)0;
            rkL[2][0] = (Real)0;
            rkL[1][1] = fA;
            rkL[1][2] = fB;
            rkL[2][1] = fB;
            rkL[2][2] = fC;
        }
        else
        {
            for (int iRow = 0; iRow < 3; iRow++)
            {
                Real fTmp0 = rkL[iRow][1];
                Real fTmp1 = rkL[iRow][2];
                rkL[iRow][1] = fA*fTmp0+fB*fTmp1;
                rkL[iRow][2] = fB*fTmp0+fC*fTmp1;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void GolubKahanStep (RealM<Real,3,3>& rkA, RealM<Real,3,3>& rkL,
    RealM<Real,3,3>& rkR)
{
    Real fT11 = rkA[0][1]*rkA[0][1]+rkA[1][1]*rkA[1][1];
    Real fT22 = rkA[1][2]*rkA[1][2]+rkA[2][2]*rkA[2][2];
    Real fT12 = rkA[1][1]*rkA[1][2];
    Real fTrace = fT11+fT22;
    Real fDiff = fT11-fT22;
    Real fDiscr = Math<Real>::Sqrt(fDiff*fDiff+((Real)4)*fT12*fT12);
    Real fRoot1 = ((Real)0.5)*(fTrace+fDiscr);
    Real fRoot2 = ((Real)0.5)*(fTrace-fDiscr);

    // adjust right
    Real fY = rkA[0][0] - (Math<Real>::FAbs(fRoot1-fT22) <=
        Math<Real>::FAbs(fRoot2-fT22) ? fRoot1 : fRoot2);
    Real fZ = rkA[0][1];
    Real fInvLength = Math<Real>::InvSqrt(fY*fY+fZ*fZ);
    Real fSin = fZ*fInvLength;
    Real fCos = -fY*fInvLength;

    Real fTmp0 = rkA[0][0];
    Real fTmp1 = rkA[0][1];
    rkA[0][0] = fCos*fTmp0-fSin*fTmp1;
    rkA[0][1] = fSin*fTmp0+fCos*fTmp1;
    rkA[1][0] = -fSin*rkA[1][1];
    rkA[1][1] *= fCos;

    int iRow;
    for (iRow = 0; iRow < 3; iRow++)
    {
        fTmp0 = rkR[0][iRow];
        fTmp1 = rkR[1][iRow];
        rkR[0][iRow] = fCos*fTmp0-fSin*fTmp1;
        rkR[1][iRow] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust left
    fY = rkA[0][0];
    fZ = rkA[1][0];
    fInvLength = Math<Real>::InvSqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    rkA[0][0] = fCos*rkA[0][0]-fSin*rkA[1][0];
    fTmp0 = rkA[0][1];
    fTmp1 = rkA[1][1];
    rkA[0][1] = fCos*fTmp0-fSin*fTmp1;
    rkA[1][1] = fSin*fTmp0+fCos*fTmp1;
    rkA[0][2] = -fSin*rkA[1][2];
    rkA[1][2] *= fCos;

    int iCol;
    for (iCol = 0; iCol < 3; iCol++)
    {
        fTmp0 = rkL[iCol][0];
        fTmp1 = rkL[iCol][1];
        rkL[iCol][0] = fCos*fTmp0-fSin*fTmp1;
        rkL[iCol][1] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust right
    fY = rkA[0][1];
    fZ = rkA[0][2];
    fInvLength = Math<Real>::InvSqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    rkA[0][1] = fCos*rkA[0][1]-fSin*rkA[0][2];
    fTmp0 = rkA[1][1];
    fTmp1 = rkA[1][2];
    rkA[1][1] = fCos*fTmp0-fSin*fTmp1;
    rkA[1][2] = fSin*fTmp0+fCos*fTmp1;
    rkA[2][1] = -fSin*rkA[2][2];
    rkA[2][2] *= fCos;

    for (iRow = 0; iRow < 3; iRow++)
    {
        fTmp0 = rkR[1][iRow];
        fTmp1 = rkR[2][iRow];
        rkR[1][iRow] = fCos*fTmp0-fSin*fTmp1;
        rkR[2][iRow] = fSin*fTmp0+fCos*fTmp1;
    }

    // adjust left
    fY = rkA[1][1];
    fZ = rkA[2][1];
    fInvLength = Math<Real>::InvSqrt(fY*fY+fZ*fZ);
    fSin = fZ*fInvLength;
    fCos = -fY*fInvLength;

    rkA[1][1] = fCos*rkA[1][1]-fSin*rkA[2][1];
    fTmp0 = rkA[1][2];
    fTmp1 = rkA[2][2];
    rkA[1][2] = fCos*fTmp0-fSin*fTmp1;
    rkA[2][2] = fSin*fTmp0+fCos*fTmp1;

    for (iCol = 0; iCol < 3; iCol++)
    {
        fTmp0 = rkL[iCol][1];
        fTmp1 = rkL[iCol][2];
        rkL[iCol][1] = fCos*fTmp0-fSin*fTmp1;
        rkL[iCol][2] = fSin*fTmp0+fCos*fTmp1;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void SingularValueDecomposition (const RealM<Real,3,3>& rkM,
    RealM<Real,3,3>& rkL, RealM<Real,3,3>& rkS, RealM<Real,3,3>& rkR)
{
    int iRow, iCol;

    RealM<Real,3,3> kA = rkM;
    Bidiagonalize(kA,rkL,rkR);
    MakeZero(rkS);

    const int iMax = 32;
    const Real fEpsilon = (Real)1e-04;
    for (int i = 0; i < iMax; i++)
    {
        Real fTmp, fTmp0, fTmp1;
        Real fSin0, fCos0, fTan0;
        Real fSin1, fCos1, fTan1;

        bool bTest1 = (Math<Real>::FAbs(kA[0][1]) <=
            fEpsilon*(Math<Real>::FAbs(kA[0][0]) +
            Math<Real>::FAbs(kA[1][1])));
        bool bTest2 = (Math<Real>::FAbs(kA[1][2]) <=
            fEpsilon*(Math<Real>::FAbs(kA[1][1]) +
            Math<Real>::FAbs(kA[2][2])));
        if (bTest1)
        {
            if (bTest2)
            {
                rkS[0][0] = kA[0][0];
                rkS[1][1] = kA[1][1];
                rkS[2][2] = kA[2][2];
                break;
            }
            else
            {
                // 2x2 closed form factorization
                fTmp = (kA[1][1]*kA[1][1] - kA[2][2]*kA[2][2] +
                    kA[1][2]*kA[1][2])/(kA[1][2]*kA[2][2]);
                fTan0 = ((Real)0.5)*(fTmp + Math<Real>::Sqrt(fTmp*fTmp +
                    ((Real)4)));
                fCos0 = Math<Real>::InvSqrt(((Real)1)+fTan0*fTan0);
                fSin0 = fTan0*fCos0;

                for (iCol = 0; iCol < 3; iCol++)
                {
                    fTmp0 = rkL[iCol][1];
                    fTmp1 = rkL[iCol][2];
                    rkL[iCol][1] = fCos0*fTmp0-fSin0*fTmp1;
                    rkL[iCol][2] = fSin0*fTmp0+fCos0*fTmp1;
                }
                
                fTan1 = (kA[1][2]-kA[2][2]*fTan0)/kA[1][1];
                fCos1 = Math<Real>::InvSqrt(((Real)1)+fTan1*fTan1);
                fSin1 = -fTan1*fCos1;

                for (iRow = 0; iRow < 3; iRow++)
                {
                    fTmp0 = rkR[1][iRow];
                    fTmp1 = rkR[2][iRow];
                    rkR[1][iRow] = fCos1*fTmp0-fSin1*fTmp1;
                    rkR[2][iRow] = fSin1*fTmp0+fCos1*fTmp1;
                }

                rkS[0][0] = kA[0][0];
                rkS[1][1] = fCos0*fCos1*kA[1][1] -
                    fSin1*(fCos0*kA[1][2]-fSin0*kA[2][2]);
                rkS[2][2] = fSin0*fSin1*kA[1][1] +
                    fCos1*(fSin0*kA[1][2]+fCos0*kA[2][2]);
                break;
            }
        }
        else 
        {
            if (bTest2)
            {
                // 2x2 closed form factorization 
                fTmp = (kA[0][0]*kA[0][0] + kA[1][1]*kA[1][1] -
                    kA[0][1]*kA[0][1])/(kA[0][1]*kA[1][1]);
                fTan0 = ((Real)0.5)*(-fTmp + Math<Real>::Sqrt(fTmp*fTmp +
                    ((Real)4.0)));
                fCos0 = Math<Real>::InvSqrt(((Real)1)+fTan0*fTan0);
                fSin0 = fTan0*fCos0;

                for (iCol = 0; iCol < 3; iCol++)
                {
                    fTmp0 = rkL[iCol][0];
                    fTmp1 = rkL[iCol][1];
                    rkL[iCol][0] = fCos0*fTmp0-fSin0*fTmp1;
                    rkL[iCol][1] = fSin0*fTmp0+fCos0*fTmp1;
                }
                
                fTan1 = (kA[0][1]-kA[1][1]*fTan0)/kA[0][0];
                fCos1 = Math<Real>::InvSqrt(((Real)1)+fTan1*fTan1);
                fSin1 = -fTan1*fCos1;

                for (iRow = 0; iRow < 3; iRow++)
                {
                    fTmp0 = rkR[0][iRow];
                    fTmp1 = rkR[1][iRow];
                    rkR[0][iRow] = fCos1*fTmp0-fSin1*fTmp1;
                    rkR[1][iRow] = fSin1*fTmp0+fCos1*fTmp1;
                }

                rkS[0][0] = fCos0*fCos1*kA[0][0] -
                    fSin1*(fCos0*kA[0][1]-fSin0*kA[1][1]);
                rkS[1][1] = fSin0*fSin1*kA[0][0] +
                    fCos1*(fSin0*kA[0][1]+fCos0*kA[1][1]);
                rkS[2][2] = kA[2][2];
                break;
            }
            else
            {
                GolubKahanStep(kA,rkL,rkR);
            }
        }
    }

    // positize diagonal
    for (iRow = 0; iRow < 3; iRow++)
    {
        if (rkS[iRow][iRow] < (Real)0.0)
        {
            rkS[iRow][iRow] = -rkS[iRow][iRow];
            for (iCol = 0; iCol < 3; iCol++)
                rkR[iRow][iCol] = -rkR[iRow][iCol];
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
RealM<Real,3,3> Slerp (Real fT, const RealM<Real,3,3>& rkR0,
    const RealM<Real,3,3>& rkR1)
{
    RealM<Real,3,3> kProd = MulATrnB(rkR0,rkR1);

    RealV<Real,3> kAxis;
    Real fAngle;
    ToAxisAngle(kProd,kAxis,fAngle);

    RealM<Real,3,3> kResult;
    FromAxisAngle(kAxis,fT*fAngle,kResult);
    return kResult;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateScaling (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU0,
    const RealV<Real,3>& rkU1, const RealV<Real,3>& rkU2, Real fS0, Real fS1,
    Real fS2, RealM<Real,3,3>& rkM, RealV<Real,3>& rkB)
{
    RealM<Real,3,3> kU0U0Trn, kU1U1Trn, kU2U2Trn;
    MakeTensorProduct(rkU0,rkU0,kU0U0Trn);
    MakeTensorProduct(rkU1,rkU1,kU1U1Trn);
    MakeTensorProduct(rkU2,rkU2,kU2U2Trn);
    rkM = fS0*kU0U0Trn + fS1*kU1U1Trn + fS2*kU2U2Trn;
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateRotation (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU2,
    Real fAngle, RealM<Real,3,3>& rkM, RealV<Real,2>& rkB)
{
    FromAxisAngle(rkU2,fAngle,rkM);
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateReflection (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU0,
    RealM<Real,3,3>& rkM, RealV<Real,3>& rkB)
{
    RealM<Real,3,3> kU0U0Trn, kIdentity;
    MakeTensorProduct(rkU0,rkU0,kU0U0Trn);
    MakeIdentity(kIdentity);
    rkM = ((Real)2)*kU0U0Trn - kIdentity;
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateReflection (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU0,
    const RealV<Real,3>& rkU1, RealM<Real,3,3>& rkM, RealV<Real,3>& rkB)
{
    RealM<Real,3,3> kU0U0Trn, kU1U1Trn, kIdentity;
    MakeTensorProduct(rkU0,rkU0,kU0U0Trn);
    MakeTensorProduct(rkU1,rkU1,kU1U1Trn);
    MakeIdentity(kIdentity);
    rkM = ((Real)2)*(kU0U0Trn + kU1U1Trn) - kIdentity;
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
template <typename Real>
void CreateShear (const RealV<Real,3>& rkP, const RealV<Real,3>& rkU0,
    const RealV<Real,3>& rkU1, Real fS, RealM<Real,3,3>& rkM,
    RealV<Real,3>& rkB)
{
    RealM<Real,3,3> kU0U1Trn, kIdentity;
    MakeTensorProduct(rkU0,rkU1,kU0U1Trn);
    MakeIdentity(kIdentity);
    rkM = kIdentity + fS*kU0U1Trn;
    rkB = rkP - rkM*rkP;
}
//----------------------------------------------------------------------------
