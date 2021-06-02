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

//----------------------------------------------------------------------------
inline bool BasicMesh::IsValid () const
{
    return m_bIsValid;
}
//----------------------------------------------------------------------------
inline int BasicMesh::GetVQuantity () const
{
    return m_iVQuantity;
}
//----------------------------------------------------------------------------
inline int BasicMesh::GetEQuantity () const
{
    return m_iEQuantity;
}
//----------------------------------------------------------------------------
inline int BasicMesh::GetTQuantity () const
{
    return m_iTQuantity;
}
//----------------------------------------------------------------------------
inline const void* BasicMesh::GetPoints () const
{
    return m_akPoint;
}
//----------------------------------------------------------------------------
inline const int* BasicMesh::GetIndices () const
{
    return m_aiIndex;
}
//----------------------------------------------------------------------------
inline const BasicMesh::Vertex* BasicMesh::GetVertices () const
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
inline const BasicMesh::Edge* BasicMesh::GetEdges () const
{
    return m_akEdge;
}
//----------------------------------------------------------------------------
inline const BasicMesh::Triangle* BasicMesh::GetTriangles () const
{
    return m_akTriangle;
}
//----------------------------------------------------------------------------
