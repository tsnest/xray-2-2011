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

//----------------------------------------------------------------------------
inline int PdeFilter2::GetXBound () const
{
    return m_iXBound;
}
//----------------------------------------------------------------------------
inline int PdeFilter2::GetYBound () const
{
    return m_iYBound;
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetXSpacing () const
{
    return m_fXSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetYSpacing () const
{
    return m_fYSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetU (int iX, int iY) const
{
    return m_aafSrc[iY+1][iX+1];
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUx (int iX, int iY) const
{
    int iYp1 = iY + 1;
    return m_fHalfInvDx*(m_aafSrc[iYp1][iX+2] - m_aafSrc[iYp1][iX]);
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUy (int iX, int iY) const
{
    int iXp1 = iX + 1;
    return m_fHalfInvDy*(m_aafSrc[iY+2][iXp1] - m_aafSrc[iY][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUxx (int iX, int iY) const
{
    int iYp1 = iY + 1;
    return m_fInvDxDx*(m_aafSrc[iYp1][iX+2] - 2.0f*m_aafSrc[iYp1][iX+1] +
        m_aafSrc[iYp1][iX]);
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUxy (int iX, int iY) const
{
    int iXp2 = iX + 2, iYp2 = iY + 2;
    return m_fFourthInvDxDy*(m_aafSrc[iY][iX] - m_aafSrc[iY][iXp2] +
        m_aafSrc[iYp2][iXp2] - m_aafSrc[iYp2][iX]);
}
//----------------------------------------------------------------------------
inline float PdeFilter2::GetUyy (int iX, int iY) const
{
    int iXp1 = iX + 1;
    return m_fInvDyDy*(m_aafSrc[iY+2][iXp1] - 2.0f*m_aafSrc[iY+1][iXp1] +
        m_aafSrc[iY][iXp1]);
}
//----------------------------------------------------------------------------
inline bool PdeFilter2::GetMask (int iX, int iY) const
{
    return (m_aabMask ? m_aabMask[iY+1][iX+1] : true);
}
//----------------------------------------------------------------------------
