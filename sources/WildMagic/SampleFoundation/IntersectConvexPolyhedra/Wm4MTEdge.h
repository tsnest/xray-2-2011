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

#ifndef WM4MTEDGE_H
#define WM4MTEDGE_H

#include "Wm4System.h"

namespace Wm4
{

class MTEdge
{
public:
    MTEdge (int iLabel = -1);
    MTEdge (const MTEdge& rkE);
    virtual ~MTEdge ();

    MTEdge& operator= (const MTEdge& rkE);

    int GetLabel () const;
    int& Label ();

    int GetVertex (int i) const;
    int& Vertex (int i);
    bool ReplaceVertex (int iVOld, int iVNew);

    int GetTriangle (int i) const;
    int& Triangle (int i);
    bool ReplaceTriangle (int iTOld, int iTNew);

    bool operator== (const MTEdge& rkE) const;

protected:
    int m_iLabel;
    int m_aiVertex[2];
    int m_aiTriangle[2];
};

#include "Wm4MTEdge.inl"

}

#endif
