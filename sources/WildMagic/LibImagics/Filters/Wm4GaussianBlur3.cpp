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
#include "Wm4GaussianBlur3.h"
using namespace Wm4;

//----------------------------------------------------------------------------
GaussianBlur3::GaussianBlur3 (int iXBound, int iYBound, int iZBound,
    float fXSpacing, float fYSpacing, float fZSpacing, const float* afData,
    const bool* abMask, float fBorderValue, ScaleType eType)
    :
    PdeFilter3(iXBound,iYBound,iZBound,fXSpacing,fYSpacing,fZSpacing,afData,
        abMask,fBorderValue,eType)
{
    m_fMaximumTimeStep = 0.5f/(m_fInvDxDx + m_fInvDyDy + m_fInvDzDz);
}
//----------------------------------------------------------------------------
GaussianBlur3::~GaussianBlur3 ()
{
}
//----------------------------------------------------------------------------
float GaussianBlur3::GetMaximumTimeStep () const
{
    return m_fMaximumTimeStep;
}
//----------------------------------------------------------------------------
void GaussianBlur3::OnUpdate (int iX, int iY, int iZ)
{
    LookUp7(iX,iY,iZ);

    float fUxx = m_fInvDxDx*(m_fUpzz - 2.0f*m_fUzzz + m_fUmzz);
    float fUyy = m_fInvDyDy*(m_fUzpz - 2.0f*m_fUzzz + m_fUzmz);
    float fUzz = m_fInvDzDz*(m_fUzzp - 2.0f*m_fUzzz + m_fUzzm);

    m_aaafDst[iZ][iY][iX] = m_fUzzz + m_fTimeStep*(fUxx + fUyy + fUzz);
}
//----------------------------------------------------------------------------
