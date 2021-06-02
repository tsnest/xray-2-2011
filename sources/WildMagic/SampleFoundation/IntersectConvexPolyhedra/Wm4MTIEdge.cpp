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

#include "Wm4MTIEdge.h"
using namespace Wm4;

//----------------------------------------------------------------------------
MTIEdge::MTIEdge (int iLabel0, int iLabel1)
{
    if (iLabel0 < iLabel1)
    {
        // L0 is minimum
        m_aiLabel[0] = iLabel0;
        m_aiLabel[1] = iLabel1;
    }
    else
    {
        // L1 is minimum
        m_aiLabel[0] = iLabel1;
        m_aiLabel[1] = iLabel0;
    }
}
//----------------------------------------------------------------------------
int MTIEdge::GetLabel (int i) const
{
    assert(0 <= i && i < 2);
    return m_aiLabel[i];
}
//----------------------------------------------------------------------------
bool MTIEdge::operator< (const MTIEdge& rkE) const
{
    if (m_aiLabel[1] < rkE.m_aiLabel[1])
    {
        return true;
    }

    if (m_aiLabel[1] == rkE.m_aiLabel[1])
    {
        return m_aiLabel[0] < rkE.m_aiLabel[0];
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTIEdge::operator== (const MTIEdge& rkE) const
{
    return m_aiLabel[0] == rkE.m_aiLabel[0]
        && m_aiLabel[1] == rkE.m_aiLabel[1];
}
//----------------------------------------------------------------------------
bool MTIEdge::operator!= (const MTIEdge& rkE) const
{
    return !operator==(rkE);
}
//----------------------------------------------------------------------------
