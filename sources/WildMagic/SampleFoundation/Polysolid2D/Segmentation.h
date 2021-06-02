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

#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "RPoint2.h"
#include <list>

class Polysolid2;
class SegmentEdge;

enum { OTAG, ITAG, MTAG, PTAG };

class Segmentation
{
public:
    Segmentation (const RPoint2& rkNormal, const Rational& rkConstant);
    ~Segmentation ();

    bool HasPoints () const;
    void AddPoint (const RPoint2& rkPosition, int eTag);
    void SegmentBy (const Polysolid2& rkPoly);
    void IntersectWith (const RPoint2& rkPosition0,
        const RPoint2& rkPosition1);
    void ConvertToEdges (SegmentEdge akTEdge[4]);

protected:
    class TaggedPoint
    {
    public:
        TaggedPoint (const Rational& rkOrder, const RPoint2& rkPosition,
            int eTag)
            :
            Order(rkOrder),
            Position(rkPosition),
            Tag(eTag)
        {
        }

        Rational Order;
        RPoint2 Position;
        int Tag;
    };

    RPoint2 m_kNormal;
    Rational m_kConstant;
    std::list<TaggedPoint> m_kList;

    static int ms_aiKlein4[4][4];
};

#endif
