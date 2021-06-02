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

//----------------------------------------------------------------------------
inline int MTMesh::GetVQuantity () const
{
    return m_akVertex.GetQuantity();
}
//----------------------------------------------------------------------------
inline int MTMesh::V (int iLabel) const
{
    VCIter pkV = m_kVMap.find(MTIVertex(iLabel));
    return (pkV != m_kVMap.end() ? pkV->second : -1);
}
//----------------------------------------------------------------------------
inline const MTVertex& MTMesh::GetVertex (int iVIndex) const
{
    assert(0 <= iVIndex && iVIndex < m_akVertex.GetQuantity());
    return m_akVertex.Get(iVIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::GetVLabel (int iVIndex) const
{
    assert(0 <= iVIndex && iVIndex < m_akVertex.GetQuantity());
    return m_akVertex.Get(iVIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int MTMesh::GetEQuantity () const
{
    return m_akEdge.GetQuantity();
}
//----------------------------------------------------------------------------
inline int MTMesh::E (int iLabel0, int iLabel1) const
{
    ECIter pkE = m_kEMap.find(MTIEdge(iLabel0,iLabel1));
    return (pkE != m_kEMap.end() ? pkE->second : -1);
}
//----------------------------------------------------------------------------
inline const MTEdge& MTMesh::GetEdge (int iEIndex) const
{
    assert(0 <= iEIndex && iEIndex < m_akEdge.GetQuantity());
    return m_akEdge.Get(iEIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::GetELabel (int iEIndex) const
{
    assert(0 <= iEIndex && iEIndex < m_akEdge.GetQuantity());
    return m_akEdge.Get(iEIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int& MTMesh::ELabel (int iEIndex)
{
    assert(0 <= iEIndex && iEIndex < m_akEdge.GetQuantity());
    return m_akEdge[iEIndex].Label();
}
//----------------------------------------------------------------------------
inline int MTMesh::GetTQuantity () const
{
    return m_akTriangle.GetQuantity();
}
//----------------------------------------------------------------------------
inline const MTTriangle& MTMesh::GetTriangle (int iTIndex) const
{
    assert(0 <= iTIndex && iTIndex < m_akTriangle.GetQuantity());
    return m_akTriangle.Get(iTIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::T (int iLabel0, int iLabel1, int iLabel2) const
{
    TCIter pkT = m_kTMap.find(MTITriangle(iLabel0,iLabel1,iLabel2));
    return (pkT != m_kTMap.end() ? pkT->second : -1);
}
//----------------------------------------------------------------------------
inline int MTMesh::GetTLabel (int iTIndex) const
{
    assert(0 <= iTIndex && iTIndex < m_akTriangle.GetQuantity());
    return m_akTriangle.Get(iTIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int& MTMesh::TLabel (int iTIndex)
{
    assert(0 <= iTIndex && iTIndex < m_akTriangle.GetQuantity());
    return m_akTriangle[iTIndex].Label();
}
//----------------------------------------------------------------------------
inline int& MTMesh::InitialELabel ()
{
    return m_iInitialELabel;
}
//----------------------------------------------------------------------------
inline int& MTMesh::InitialTLabel ()
{
    return m_iInitialTLabel;
}
//----------------------------------------------------------------------------
