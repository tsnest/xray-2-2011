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
// Version: 4.7.0 (2008/09/15)

#include "Wm4MTITriangle.h"
using namespace Wm4;

//----------------------------------------------------------------------------
MTITriangle::MTITriangle (int iLabel0, int iLabel1, int iLabel2)
{
    if (iLabel0 < iLabel1)
    {
        if (iLabel0 < iLabel2)
        {
            // L0 is minimum
            m_aiLabel[0] = iLabel0;
            m_aiLabel[1] = iLabel1;
            m_aiLabel[2] = iLabel2;
        }
        else
        {
            // L2 is minimum
            m_aiLabel[0] = iLabel2;
            m_aiLabel[1] = iLabel0;
            m_aiLabel[2] = iLabel1;
        }
    }
    else
    {
        if (iLabel1 < iLabel2)
        {
            // L1 is minimum
            m_aiLabel[0] = iLabel1;
            m_aiLabel[1] = iLabel2;
            m_aiLabel[2] = iLabel0;
        }
        else
        {
            // L2 is minimum
            m_aiLabel[0] = iLabel2;
            m_aiLabel[1] = iLabel0;
            m_aiLabel[2] = iLabel1;
        }
    }
}
//----------------------------------------------------------------------------
inline int MTITriangle::GetLabel (int i) const
{
    assert(0 <= i && i < 3);
    return m_aiLabel[i];
}
//----------------------------------------------------------------------------
bool MTITriangle::operator< (const MTITriangle& rkT) const
{
    if (m_aiLabel[2] < rkT.m_aiLabel[2])
        return true;

    if (m_aiLabel[2] == rkT.m_aiLabel[2])
    {
        if (m_aiLabel[1] < rkT.m_aiLabel[1])
        {
            return true;
        }

        if (m_aiLabel[1] == rkT.m_aiLabel[1])
        {
            return m_aiLabel[0] < rkT.m_aiLabel[0];
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTITriangle::operator== (const MTITriangle& rkT) const
{
    return (m_aiLabel[0] == rkT.m_aiLabel[0]) &&
    ((m_aiLabel[1] == rkT.m_aiLabel[1] && m_aiLabel[2] == rkT.m_aiLabel[2])
    ||
    (m_aiLabel[1] == rkT.m_aiLabel[2] && m_aiLabel[2] == rkT.m_aiLabel[1]));
}
//----------------------------------------------------------------------------
bool MTITriangle::operator!= (const MTITriangle& rkT) const
{
    return !operator==(rkT);
}
//----------------------------------------------------------------------------
