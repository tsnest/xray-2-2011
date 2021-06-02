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

#include "Wm4MTVertex.h"
using namespace Wm4;

//----------------------------------------------------------------------------
MTVertex::MTVertex (int iLabel, int iEGrow, int iTGrow)
    :
    m_kESet(iEGrow,iEGrow),
    m_kTSet(iTGrow,iTGrow)
{
    m_iLabel = iLabel;
}
//----------------------------------------------------------------------------
MTVertex::MTVertex (const MTVertex& rkV)
    :
    m_kESet(rkV.m_kESet),
    m_kTSet(rkV.m_kTSet)
{
    m_iLabel = rkV.m_iLabel;
}
//----------------------------------------------------------------------------
MTVertex::~MTVertex ()
{
}
//----------------------------------------------------------------------------
MTVertex& MTVertex::operator= (const MTVertex& rkV)
{
    m_kESet = rkV.m_kESet;
    m_kTSet = rkV.m_kTSet;
    m_iLabel = rkV.m_iLabel;
    return *this;
}
//----------------------------------------------------------------------------
bool MTVertex::InsertEdge (int iE)
{
    return m_kESet.Insert(iE);
}
//----------------------------------------------------------------------------
bool MTVertex::RemoveEdge (int iE)
{
    return m_kESet.Remove(iE);
}
//----------------------------------------------------------------------------
bool MTVertex::ReplaceEdge (int iEOld, int iENew)
{
    for (int i = 0; i < m_kESet.GetQuantity(); i++)
    {
        if (m_kESet[i] == iEOld)
        {
            m_kESet[i] = iENew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTVertex::InsertTriangle (int iT)
{
    return m_kTSet.Insert(iT);
}
//----------------------------------------------------------------------------
bool MTVertex::RemoveTriangle (int iT)
{
    return m_kTSet.Remove(iT);
}
//----------------------------------------------------------------------------
bool MTVertex::ReplaceTriangle (int iTOld, int iTNew)
{
    for (int i = 0; i < m_kTSet.GetQuantity(); i++)
    {
        if (m_kTSet[i] == iTOld)
        {
            m_kTSet[i] = iTNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTVertex::operator== (const MTVertex& rkV) const
{
    return m_iLabel == rkV.m_iLabel;
}
//----------------------------------------------------------------------------
