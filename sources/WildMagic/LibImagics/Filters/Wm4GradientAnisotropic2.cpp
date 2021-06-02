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
#include "Wm4GradientAnisotropic2.h"
using namespace Wm4;

//----------------------------------------------------------------------------
GradientAnisotropic2::GradientAnisotropic2 (int iXBound, int iYBound,
    float fXSpacing, float fYSpacing, const float* afData, const bool* abMask,
    float fBorderValue, ScaleType eType, float fK)
    :
    PdeFilter2(iXBound,iYBound,fXSpacing,fYSpacing,afData,abMask,fBorderValue,
        eType)
{
    m_fK = fK;
    ComputeParam();
}
//----------------------------------------------------------------------------
GradientAnisotropic2::~GradientAnisotropic2 ()
{
}
//----------------------------------------------------------------------------
void GradientAnisotropic2::ComputeParam ()
{
    float fGradMagSqr = 0.0f;
    for (int iY = 1; iY <= m_iYBound; iY++)
    {
        for (int iX = 1; iX <= m_iXBound; iX++)
        {
            float fUx = GetUx(iX,iY);
            float fUy = GetUy(iX,iY);
            fGradMagSqr += fUx*fUx + fUy*fUy;
        }
    }
    fGradMagSqr /= (float)m_iQuantity;

    m_fParam = 1.0f/(m_fK*m_fK*fGradMagSqr);
    m_fMHalfParam = -0.5f*m_fParam;
}
//----------------------------------------------------------------------------
void GradientAnisotropic2::OnPreUpdate ()
{
    ComputeParam();
}
//----------------------------------------------------------------------------
void GradientAnisotropic2::OnUpdate (int iX, int iY)
{
    // ITK's algorithm, which is implemented in the files
    //   itkGradientMagnitudeImageFilter.{h,txx}
    // This implementation appears to ignore the pixel sizes; that is, they
    // assume square pixels.  In comparison, the files
    //   itkCurvatureNDAnisotropicDiffusionFunction.{h,txx}
    // do adjust for pixel sizes via m_ScaleCoefficients[] values.

    LookUp9(iX,iY);

    // one-sided U-derivative estimates
    float fUxFwd = m_fInvDx*(m_fUpz - m_fUzz);
    float fUxBwd = m_fInvDx*(m_fUzz - m_fUmz);
    float fUyFwd = m_fInvDy*(m_fUzp - m_fUzz);
    float fUyBwd = m_fInvDy*(m_fUzz - m_fUzm);

    // centered U-derivative estimates
    float fUxCenM = m_fHalfInvDx*(m_fUpm - m_fUmm);
    float fUxCenZ = m_fHalfInvDx*(m_fUpz - m_fUmz);
    float fUxCenP = m_fHalfInvDx*(m_fUpp - m_fUmp);
    float fUyCenM = m_fHalfInvDy*(m_fUmp - m_fUmm);
    float fUyCenZ = m_fHalfInvDy*(m_fUzp - m_fUzm);
    float fUyCenP = m_fHalfInvDy*(m_fUpp - m_fUpm);

    float fUxCenZSqr = fUxCenZ*fUxCenZ;
    float fUyCenZSqr = fUyCenZ*fUyCenZ;
    float fGradMagSqr;

    // estimate for C(x+1,y)
    float fUyEstP = 0.5f*(fUyCenZ + fUyCenP);
    fGradMagSqr = fUxCenZSqr + fUyEstP*fUyEstP;
    float fCXP = expf(m_fMHalfParam*fGradMagSqr);

    // estimate for C(x-1,y)
    float fUyEstM = 0.5f*(fUyCenZ + fUyCenM);
    fGradMagSqr = fUxCenZSqr + fUyEstM*fUyEstM;
    float fCXM = expf(m_fMHalfParam*fGradMagSqr);

    // estimate for C(x,y+1)
    float fUxEstP = 0.5f*(fUxCenZ + fUxCenP);
    fGradMagSqr = fUyCenZSqr + fUxEstP*fUxEstP;
    float fCYP = expf(m_fMHalfParam*fGradMagSqr);

    // estimate for C(x,y-1)
    float fUxEstM = 0.5f*(fUxCenZ + fUxCenM);
    fGradMagSqr = fUyCenZSqr + fUxEstM*fUxEstM;
    float fCYM = expf(m_fMHalfParam*fGradMagSqr);

    m_aafDst[iY][iX] = m_fUzz + m_fTimeStep*(
        fCXP*fUxFwd - fCXM*fUxBwd + fCYP*fUyFwd - fCYM*fUyBwd);
}
//----------------------------------------------------------------------------
