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

#ifndef WM4MTTRIANGLE_H
#define WM4MTTRIANGLE_H

#include "Wm4System.h"

namespace Wm4
{

class MTTriangle
{
public:
    MTTriangle (int iLabel = -1);
    MTTriangle (const MTTriangle& rkT);
    virtual ~MTTriangle ();

    MTTriangle& operator= (const MTTriangle& rkT);

    int GetLabel () const;
    int& Label ();

    int GetVertex (int i) const;
    int& Vertex (int i);
    bool ReplaceVertex (int iVOld, int iVNew);

    int GetEdge (int i) const;
    int& Edge (int i);
    bool ReplaceEdge (int iEOld, int iENew);

    int GetAdjacent (int i) const;
    int& Adjacent (int i);
    bool ReplaceAdjacent (int iAOld, int iANew);

    bool operator== (const MTTriangle& rkT) const;

protected:
    int m_iLabel;
    int m_aiVertex[3];
    int m_aiEdge[3];
    int m_aiAdjacent[3];
};

#include "Wm4MTTriangle.inl"

}

#endif
