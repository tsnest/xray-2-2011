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
// Version: 4.0.0 (2006/09/21)

#include "Wm4ImagicsPCH.h"
#include "Wm4GradientAnisotropic3.h"
using namespace Wm4;

//----------------------------------------------------------------------------
GradientAnisotropic3::GradientAnisotropic3 (int iXBound, int iYBound,
    int iZBound, float fXSpacing, float fYSpacing, float fZSpacing,
    const float* afData, const bool* abMask, float fBorderValue,
    ScaleType eType, float fK)
    :
    PdeFilter3(iXBound,iYBound,iZBound,fXSpacing,fYSpacing,fZSpacing,afData,
        abMask,fBorderValue,eType)
{
    m_fK = fK;
    ComputeParam();
}
//----------------------------------------------------------------------------
GradientAnisotropic3::~GradientAnisotropic3 ()
{
}
//----------------------------------------------------------------------------
void GradientAnisotropic3::ComputeParam ()
{
    float fGradMagSqr = 0.0f;
    for (int iZ = 1; iZ <= m_iZBound; iZ++)
    {
        for (int iY = 1; iY <= m_iYBound; iY++)
        {
            for (int iX = 1; iX <= m_iXBound; iX++)
            {
                float fUx = GetUx(iX,iY,iZ);
                float fUy = GetUy(iX,iY,iZ);
                float fUz = GetUz(iX,iY,iZ);
                fGradMagSqr += fUx*fUx + fUy*fUy + fUz*fUz;
            }
        }
    }
    fGradMagSqr /= (float)m_iQuantity;

    m_fParam = 1.0f/(m_fK*m_fK*fGradMagSqr);
    m_fMHalfParam = -0.5f*m_fParam;
}
//----------------------------------------------------------------------------
void GradientAnisotropic3::OnPreUpdate ()
{
    ComputeParam();
}
//----------------------------------------------------------------------------
void GradientAnisotropic3::OnUpdate (int iX, int iY, int iZ)
{
    // ITK's algorithm, which is implemented in the files
    //   itkGradientMagnitudeImageFilter.{h,txx}
    // This implementation appears to ignore the pixel sizes; that is, they
    // assume square pixels.  In comparison, the files
    //   itkCurvatureNDAnisotropicDiffusionFunction.{h,txx}
    // do adjust for pixel sizes via m_ScaleCoefficients[] values.

    LookUp27(iX,iY,iZ);

    // one-sided U-derivative estimates
    float fUxFwd = m_fInvDx*(m_fUpzz - m_fUzzz);
    float fUxBwd = m_fInvDx*(m_fUzzz - m_fUmzz);
    float fUyFwd = m_fInvDy*(m_fUzpz - m_fUzzz);
    float fUyBwd = m_fInvDy*(m_fUzzz - m_fUzmz);
    float fUzFwd = m_fInvDz*(m_fUzzp - m_fUzzz);
    float fUzBwd = m_fInvDz*(m_fUzzz - m_fUzzm);

    // centered U-derivative estimates
    float fDUvzz = m_fHalfInvDx*(m_fUpzz - m_fUmzz);
    float fDUvpz = m_fHalfInvDx*(m_fUppz - m_fUmpz);
    float fDUvmz = m_fHalfInvDx*(m_fUpmz - m_fUmmz);
    float fDUvzp = m_fHalfInvDx*(m_fUpzp - m_fUmzp);
    float fDUvzm = m_fHalfInvDx*(m_fUpzm - m_fUmzm);

    float fDUzvz = m_fHalfInvDy*(m_fUzpz - m_fUzmz);
    float fDUpvz = m_fHalfInvDy*(m_fUppz - m_fUpmz);
    float fDUmvz = m_fHalfInvDy*(m_fUmpz - m_fUmmz);
    float fDUzvp = m_fHalfInvDy*(m_fUzpp - m_fUzmp);
    float fDUzvm = m_fHalfInvDy*(m_fUzpm - m_fUzmm);

    float fDUzzv = m_fHalfInvDz*(m_fUzzp - m_fUzzm);
    float fDUpzv = m_fHalfInvDz*(m_fUpzp - m_fUpzm);
    float fDUmzv = m_fHalfInvDz*(m_fUmzp - m_fUmzm);
    float fDUzpv = m_fHalfInvDz*(m_fUzpp - m_fUzpm);
    float fDUzmv = m_fHalfInvDz*(m_fUzmp - m_fUzmm);

    float fUxCenSqr = fDUvzz*fDUvzz;
    float fUyCenSqr = fDUzvz*fDUzvz;
    float fUzCenSqr = fDUzzv*fDUzzv;

    float fUxEst, fUyEst, fUzEst, fGradMagSqr;

    // estimate for C(x+1,y,z)
    fUyEst = 0.5f*(fDUzvz + fDUpvz);
    fUzEst = 0.5f*(fDUzzv + fDUpzv);
    fGradMagSqr = fUxCenSqr + fUyEst*fUyEst + fUzEst*fUzEst;
    float fCXP = expf(m_fMHalfParam*fGradMagSqr);

    // estimate for C(x-1,y,z)
    fUyEst = 0.5f*(fDUzvz + fDUmvz);
    fUzEst = 0.5f*(fDUzzv + fDUmzv);
    fGradMagSqr = fUxCenSqr + fUyEst*fUyEst + fUzEst*fUzEst;
    float fCXM = expf(m_fMHalfParam*fGradMagSqr);

    // estimate for C(x,y+1,z)
    fUxEst = 0.5f*(fDUvzz + fDUvpz);
    fUzEst = 0.5f*(fDUzzv + fDUzpv);
    fGradMagSqr = fUxEst*fUxEst + fUyCenSqr + fUzEst*fUzEst;
    float fCYP = expf(m_fMHalfParam*fGradMagSqr);

    // estimate for C(x,y-1,z)
    fUxEst = 0.5f*(fDUvzz + fDUvmz);
    fUzEst = 0.5f*(fDUzzv + fDUzmv);
    fGradMagSqr = fUxEst*fUxEst + fUyCenSqr + fUzEst*fUzEst;
    float fCYM = expf(m_fMHalfParam*fGradMagSqr);

    // estimate for C(x,y,z+1)
    fUxEst = 0.5f*(fDUvzz + fDUvzp);
    fUyEst = 0.5f*(fDUzvz + fDUzvp);
    fGradMagSqr = fUxEst*fUxEst + fUyEst*fUyEst + fUzCenSqr;
    float fCZP = expf(m_fMHalfParam*fGradMagSqr);

    // estimate for C(x,y,z-1)
    fUxEst = 0.5f*(fDUvzz + fDUvzm);
    fUyEst = 0.5f*(fDUzvz + fDUzvm);
    fGradMagSqr = fUxEst*fUxEst + fUyEst*fUyEst + fUzCenSqr;
    float fCZM = expf(m_fMHalfParam*fGradMagSqr);

    m_aaafDst[iZ][iY][iX] = m_fUzzz + m_fTimeStep*(
        fCXP*fUxFwd - fCXM*fUxBwd +
        fCYP*fUyFwd - fCYM*fUyBwd +
        fCZP*fUzFwd - fCZM*fUzBwd);
}
//----------------------------------------------------------------------------
