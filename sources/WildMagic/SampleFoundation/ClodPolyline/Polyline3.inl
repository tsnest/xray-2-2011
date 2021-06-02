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
inline int Polyline3::GetVertexQuantity () const
{
    return m_iVQuantity;
}
//----------------------------------------------------------------------------
inline const Vector3f* Polyline3::GetVertices () const
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
inline bool Polyline3::GetClosed () const
{
    return m_bClosed;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetEdgeQuantity () const
{
    return m_iEQuantity;
}
//----------------------------------------------------------------------------
inline const int* Polyline3::GetEdges () const
{
    return m_aiEdge;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetMinLevelOfDetail () const
{
    return m_iVMin;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetMaxLevelOfDetail () const
{
    return m_iVMax;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetLevelOfDetail () const
{
    return m_iVQuantity;
}
//----------------------------------------------------------------------------
