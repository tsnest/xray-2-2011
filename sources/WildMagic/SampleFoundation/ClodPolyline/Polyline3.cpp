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
// Version: 4.0.0 (2006/06/28)

#include "Polyline3.h"
#include "VertexCollapse.h"

//----------------------------------------------------------------------------
Polyline3::Polyline3 (int iVQuantity, Vector3f* akVertex, bool bClosed)
{
    assert(akVertex  && (bClosed ? iVQuantity >= 3 : iVQuantity >= 2));

    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_bClosed = bClosed;

    // compute the vertex collapses
    VertexCollapse(m_iVQuantity,m_akVertex,m_bClosed,m_aiMap,m_iEQuantity,
        m_aiEdge);

    // polyline initially at full level of detail
    m_iVMin = (m_bClosed ? 3 : 2);
    m_iVMax = m_iVQuantity;
}
//----------------------------------------------------------------------------
Polyline3::~Polyline3 ()
{
    WM4_DELETE[] m_akVertex;
    WM4_DELETE[] m_aiEdge;
    WM4_DELETE[] m_aiMap;
}
//----------------------------------------------------------------------------
void Polyline3::SetLevelOfDetail (int iVQuantity)
{
    if (iVQuantity < m_iVMin || iVQuantity > m_iVMax)
    {
        return;
    }

    // decrease level of detail
    while (m_iVQuantity > iVQuantity)
    {
        m_iVQuantity--;
        m_aiEdge[m_aiMap[m_iVQuantity]] = m_aiEdge[2*m_iEQuantity-1];
        m_iEQuantity--;
    }

    // increase level of detail
    while (m_iVQuantity < iVQuantity)
    {
        m_iEQuantity++;
        m_aiEdge[m_aiMap[m_iVQuantity]] = m_iVQuantity;
        m_iVQuantity++;
    }
}
//----------------------------------------------------------------------------
