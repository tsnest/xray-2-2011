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
inline int MTEdge::GetLabel () const
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
inline int& MTEdge::Label ()
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
inline int MTEdge::GetVertex (int i) const
{
    return m_aiVertex[i];
}
//----------------------------------------------------------------------------
inline int& MTEdge::Vertex (int i)
{
    return m_aiVertex[i];
}
//----------------------------------------------------------------------------
inline int MTEdge::GetTriangle (int i) const
{
    return m_aiTriangle[i];
}
//----------------------------------------------------------------------------
inline int& MTEdge::Triangle (int i)
{
    return m_aiTriangle[i];
}
//----------------------------------------------------------------------------
