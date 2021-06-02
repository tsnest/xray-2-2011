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
inline const TriMesh* BoundingVolumeTree::GetMesh () const
{
    return m_pkMesh;
}
//----------------------------------------------------------------------------
inline const BoundingVolume* BoundingVolumeTree::GetWorldBound () const
{
    return m_spkWorldBound;
}
//----------------------------------------------------------------------------
inline int BoundingVolumeTree::GetTriangleQuantity () const
{
    return m_iTriangleQuantity;
}
//----------------------------------------------------------------------------
inline int BoundingVolumeTree::GetTriangle (int i) const
{
    return m_aiTriangle[i];
}
//----------------------------------------------------------------------------
inline const int* BoundingVolumeTree::GetTriangles () const
{
    return m_aiTriangle;
}
//----------------------------------------------------------------------------
inline BoundingVolumeTree* BoundingVolumeTree::GetLChild ()
{
    return m_pkLChild;
}
//----------------------------------------------------------------------------
inline BoundingVolumeTree* BoundingVolumeTree::GetRChild ()
{
    return m_pkRChild;
}
//----------------------------------------------------------------------------
inline bool BoundingVolumeTree::IsInteriorNode () const
{
    return m_pkLChild || m_pkRChild;
}
//----------------------------------------------------------------------------
inline bool BoundingVolumeTree::IsLeafNode () const
{
    return !m_pkLChild && !m_pkRChild;
}
//----------------------------------------------------------------------------
