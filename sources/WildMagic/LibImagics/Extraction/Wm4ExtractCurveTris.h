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
// Version: 4.0.1 (2006/09/21)

#ifndef WM4EXTRACTCURVETRIS_H
#define WM4EXTRACTCURVETRIS_H

#include "Wm4ImagicsLIB.h"
#include "Wm4Vector2.h"
#include "Wm4EdgeKey.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM ExtractCurveTris
{
public:
    // The input is a 2D image with lexicographically ordered pixels (x,y)
    // stored in a linear array.  Pixel (x,y) is stored in the array at
    // location x+xbound*y.  The caller is responsible for deleting aiData if
    // it was dynamically allocated.
    ExtractCurveTris (int iXBound, int iYBound, int* aiData);

    // Extract a level curve of the specified level value.
    void ExtractContour (int iLevel, std::vector<Vector2f>& rkVA,
        std::vector<EdgeKey>& rkEA);

    // The extraction has duplicate vertices on edges shared by pixels.  This
    // function will eliminate the duplication.
    void MakeUnique (std::vector<Vector2f>& rkVA, std::vector<EdgeKey>& rkEA);

protected:
    // Since the level value is an integer, the vertices of the extracted
    // level curve are stored internally as rational numbers.
    class WM4_IMAGICS_ITEM Vertex
    {
    public:
        Vertex (int iXNumer = 0, int iXDenom = 0, int iYNumer = 0,
            int iYDenom = 0);

        int XNumer, XDenom, YNumer, YDenom;
    };

    void AddVertex (std::vector<Vertex>& rkVA, int iXNumer, int iXDenom,
        int iYNumer, int iYDenom);

    void AddEdge (std::vector<Vertex>& rkVA, std::vector<EdgeKey>& rkEA,
        int iXNumer0, int iXDenom0, int iYNumer0, int iYDenom0, int iXNumer1,
        int iXDenom1, int iYNumer1, int iYDenom1);

    void ProcessTriangle (std::vector<Vertex>& rkVA,
        std::vector<EdgeKey>& rkEA, int iX0, int iY0, int iF0, int iX1,
        int iY1, int iF1, int iX2, int iY2, int iF2);

    int m_iXBound, m_iYBound, m_iQuantity;
    int* m_aiData;
};

}

#endif
