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
inline int PdeFilter3::GetXBound () const
{
    return m_iXBound;
}
//----------------------------------------------------------------------------
inline int PdeFilter3::GetYBound () const
{
    return m_iYBound;
}
//----------------------------------------------------------------------------
inline int PdeFilter3::GetZBound () const
{
    return m_iZBound;
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetXSpacing () const
{
    return m_fXSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetYSpacing () const
{
    return m_fYSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetZSpacing () const
{
    return m_fZSpacing;
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetU (int iX, int iY, int iZ) const
{
    return m_aaafSrc[iZ+1][iY+1][iX+1];
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUx (int iX, int iY, int iZ) const
{
    int iYp1 = iY + 1, iZp1 = iZ + 1;
    return m_fHalfInvDx*(m_aaafSrc[iZp1][iYp1][iX+2] -
        m_aaafSrc[iZp1][iYp1][iX]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUy (int iX, int iY, int iZ) const
{
    int iXp1 = iX + 1, iZp1 = iZ + 1;
    return m_fHalfInvDy*(m_aaafSrc[iZp1][iY+2][iXp1] -
        m_aaafSrc[iZp1][iY][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUz (int iX, int iY, int iZ) const
{
    int iXp1 = iX + 1, iYp1 = iY + 1;
    return m_fHalfInvDz*(m_aaafSrc[iZ+2][iYp1][iXp1] -
        m_aaafSrc[iZ][iYp1][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUxx (int iX, int iY, int iZ) const
{
    int iYp1 = iY + 1, iZp1 = iZ + 1;
    return m_fInvDxDx*(m_aaafSrc[iZp1][iYp1][iX+2] -
        2.0f*m_aaafSrc[iZp1][iYp1][iX+1] + m_aaafSrc[iZp1][iYp1][iX]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUxy (int iX, int iY, int iZ) const
{
    int iXp2 = iX + 2, iYp2 = iY + 2, iZp1 = iZ + 1;
    return m_fFourthInvDxDy*(m_aaafSrc[iZp1][iY][iX] -
        m_aaafSrc[iZp1][iY][iXp2] + m_aaafSrc[iZp1][iYp2][iXp2] -
        m_aaafSrc[iZp1][iYp2][iX]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUxz (int iX, int iY, int iZ) const
{
    int iXp2 = iX + 2, iYp1 = iY + 1, iZp2 = iZ + 2;
    return m_fFourthInvDxDz*(m_aaafSrc[iZ][iYp1][iX] -
        m_aaafSrc[iZ][iYp1][iXp2] + m_aaafSrc[iZp2][iYp1][iXp2] -
        m_aaafSrc[iZp2][iYp1][iX]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUyy (int iX, int iY, int iZ) const
{
    int iXp1 = iX + 1, iZp1 = iZ + 1;
    return m_fInvDyDy*(m_aaafSrc[iZp1][iY+2][iXp1] -
        2.0f*m_aaafSrc[iZp1][iY+1][iXp1] + m_aaafSrc[iZp1][iY][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUyz (int iX, int iY, int iZ) const
{
    int iXp1 = iX + 1, iYp2 = iY + 2, iZp2 = iZ + 2;
    return m_fFourthInvDyDz*(m_aaafSrc[iZ][iY][iXp1] -
        m_aaafSrc[iZ][iYp2][iXp1] + m_aaafSrc[iZp2][iYp2][iXp1] -
        m_aaafSrc[iZp2][iY][iXp1]);
}
//----------------------------------------------------------------------------
inline float PdeFilter3::GetUzz (int iX, int iY, int iZ) const
{
    int iXp1 = iX + 1, iYp1 = iY + 1;
    return m_fInvDzDz*(m_aaafSrc[iZ+2][iYp1][iXp1] -
        2.0f*m_aaafSrc[iZ+1][iYp1][iXp1] + m_aaafSrc[iZ][iYp1][iXp1]);
}
//----------------------------------------------------------------------------
inline bool PdeFilter3::GetMask (int iX, int iY, int iZ) const
{
    return (m_aaabMask ? m_aaabMask[iZ+1][iY+1][iX+1] : true);
}
//----------------------------------------------------------------------------
