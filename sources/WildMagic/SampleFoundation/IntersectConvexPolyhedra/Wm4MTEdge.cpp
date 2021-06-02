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

#include "Wm4MTEdge.h"
using namespace Wm4;

//----------------------------------------------------------------------------
MTEdge::MTEdge (int iLabel)
{
    m_iLabel = iLabel;

    for (int i = 0; i < 2; i++)
    {
        m_aiVertex[i] = -1;
        m_aiTriangle[i] = -1;
    }
}
//----------------------------------------------------------------------------
MTEdge::MTEdge (const MTEdge& rkE)
{
    m_iLabel = rkE.m_iLabel;

    for (int i = 0; i < 2; i++)
    {
        m_aiVertex[i] = rkE.m_aiVertex[i];
        m_aiTriangle[i] = rkE.m_aiTriangle[i];
    }
}
//----------------------------------------------------------------------------
MTEdge::~MTEdge ()
{
}
//----------------------------------------------------------------------------
MTEdge& MTEdge::operator= (const MTEdge& rkE)
{
    m_iLabel = rkE.m_iLabel;

    for (int i = 0; i < 2; i++)
    {
        m_aiVertex[i] = rkE.m_aiVertex[i];
        m_aiTriangle[i] = rkE.m_aiTriangle[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
bool MTEdge::ReplaceVertex (int iVOld, int iVNew)
{
    for (int i = 0; i < 2; i++)
    {
        if (m_aiVertex[i] == iVOld)
        {
            m_aiVertex[i] = iVNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTEdge::ReplaceTriangle (int iTOld, int iTNew)
{
    for (int i = 0; i < 2; i++)
    {
        if (m_aiTriangle[i] == iTOld)
        {
            m_aiTriangle[i] = iTNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTEdge::operator== (const MTEdge& rkE) const
{
    return
        (m_aiVertex[0] == rkE.m_aiVertex[0] &&
         m_aiVertex[1] == rkE.m_aiVertex[1]) ||
        (m_aiVertex[0] == rkE.m_aiVertex[1] &&
         m_aiVertex[1] == rkE.m_aiVertex[0]);
}
//----------------------------------------------------------------------------
