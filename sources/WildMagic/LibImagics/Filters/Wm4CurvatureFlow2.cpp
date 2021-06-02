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
#include "Wm4CurvatureFlow2.h"
using namespace Wm4;

//----------------------------------------------------------------------------
CurvatureFlow2::CurvatureFlow2 (int iXBound, int iYBound, float fXSpacing,
    float fYSpacing, const float* afData, const bool* abMask,
    float fBorderValue, ScaleType eType)
    :
    PdeFilter2(iXBound,iYBound,fXSpacing,fYSpacing,afData,abMask,fBorderValue,
        eType)
{
}
//----------------------------------------------------------------------------
CurvatureFlow2::~CurvatureFlow2 ()
{
}
//----------------------------------------------------------------------------
void CurvatureFlow2::OnUpdate (int iX, int iY)
{
    LookUp9(iX,iY);

    float fUx = m_fHalfInvDx*(m_fUpz - m_fUmz);
    float fUy = m_fHalfInvDy*(m_fUzp - m_fUzm);
    float fUxx = m_fInvDxDx*(m_fUpz - 2.0f*m_fUzz + m_fUmz);
    float fUxy = m_fFourthInvDxDy*(m_fUmm + m_fUpp - m_fUmp - m_fUpm);
    float fUyy = m_fInvDyDy*(m_fUzp - 2.0f*m_fUzz + m_fUzm);

    float fSqrUx = fUx*fUx;
    float fSqrUy = fUy*fUy;
    float fSqrLength = fSqrUx + fSqrUy + 1e-08f;  // prevent zero division
    m_aafDst[iY][iX] = m_fUzz + m_fTimeStep*(fUxx*fSqrUy +
        fUyy*fSqrUx - 0.5f*fUxy*fUx*fUy)/fSqrLength;
}
//----------------------------------------------------------------------------
