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
// Version: 4.0.1 (2007/01/19)

#ifndef POLYGON2_H
#define POLYGON2_H

#include "Types2.h"

class BspTree2;

class Polygon2
{
public:
    Polygon2 ();
    Polygon2 (const Polygon2& rkP);
    ~Polygon2 ();

    Polygon2& operator= (const Polygon2& rkP);

    int InsertVertex (const Vector2d& rkV);
    int InsertEdge (const Edge2& rkE);
    void Finalize ();

    int GetVertexQuantity () const;
    bool GetVertex (int i, Vector2d& rkV) const;
    int GetEdgeQuantity () const;
    bool GetEdge (int i, Edge2& rkE) const;

    // negation
    Polygon2 operator~ () const;

    // intersection
    Polygon2 operator& (const Polygon2& rkP) const;

    // union
    Polygon2 operator| (const Polygon2& rkP) const;

    // difference
    Polygon2 operator- (const Polygon2& rkP) const;

    // exclusive or
    Polygon2 operator^ (const Polygon2& rkP) const;

    // point location (-1 inside, 0 on polygon, 1 outside)
    int PointLocation (const Vector2d& rkV) const;

    void Print (const char* acFilename) const;

protected:
    void SplitEdge (int iV0, int iV1, int iVMid);
    void GetInsideEdgesFrom (const Polygon2& rkP, Polygon2& rkInside) const;

    // vertices
    VMap m_kVMap;
    VArray m_kVArray;

    // edges
    EMap m_kEMap;
    EArray m_kEArray;

    friend class BspTree2;
    BspTree2* m_pkTree;

};

#endif
