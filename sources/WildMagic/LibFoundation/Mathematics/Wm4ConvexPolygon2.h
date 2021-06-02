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
// Version: 4.6.0 (2007/08/20)

#ifndef WM4CONVEXPOLYGON2_H
#define WM4CONVEXPOLYGON2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM ConvexPolygon2
{
public:
    typedef typename std::vector<Vector2<Real> > VArray;

    // Line representation Dot(N,X) = c.  The 'first' of the pair is a
    // unit-length normal N to the line.  The 'second' of the pair is the line
    // constant c.  X is any point on the line.
    typedef typename std::pair<Vector2<Real>, Real> NCLine;
    typedef typename std::vector<NCLine> LArray;

    // Construction and destruction.
    ConvexPolygon2 ();
    ConvexPolygon2 (const VArray& rakPoint);
    ConvexPolygon2 (const VArray& rakPoint, const LArray& rakLine);
    ConvexPolygon2 (const ConvexPolygon2& rkPoly);
    ~ConvexPolygon2 ();

    // Deferred creation.
    void Create (const VArray& rakPoint);
    void Create (const VArray& rakPoint, const LArray& rakLine);

    // Assignment.
    ConvexPolygon2& operator= (const ConvexPolygon2& rkPoly);

    // Read points and lines.
    const VArray& GetPoints () const;
    const Vector2<Real>& GetPoint (int iV) const;
    const LArray& GetLines () const;
    const NCLine& GetLine (int iT) const;

    // Allow vertex modification.  The caller is responsible for preserving
    // the convexity.  After modifying the vertices, call UpdateLines to
    // recompute the lines of the polygon edges.
    int AddPoint (const Vector2<Real>& rkPoint);
    VArray& Points ();
    Vector2<Real>& Point (int iV);
    void UpdateLines ();

    // Test for convexity:  This function will iterate over the edges of the
    // polygon and verify for each that the polygon vertices are all on the
    // nonnegative side of the line.  The threshold is the value that the line
    // distance d is compared to, d < 0.  In theory the distances should all
    // be nonegative.  Floating point round-off errors can cause some small
    // distances, so you might set fThreshold to a small negative number.
    bool ValidateHalfSpaceProperty (Real fThreshold = (Real)0.0) const;
    void ComputeCentroid ();
    const Vector2<Real>& GetCentroid () const;

    // Discard the portion of the mesh on the negative side of the line.
    bool Clip (const NCLine& rkLine, ConvexPolygon2& rkIntr) const;

    // Compute the polygon of intersection.
    bool FindIntersection (const ConvexPolygon2& rkPoly,
        ConvexPolygon2& rkIntr) const;

    static void FindAllIntersections (int iQuantity, ConvexPolygon2* akPoly,
        int& riCombos, ConvexPolygon2**& rapkIntr);

    Real GetPerimeterLength () const;
    Real GetArea () const;
    bool ContainsPoint (const Vector2<Real>& rkP) const;

    // Create an egg-shaped object that is axis-aligned and centered at
    // (xc,yc).  The input bounds are all positive and represent the
    // distances from the center to the four extreme points on the egg.
    static void CreateEggShape (const Vector2<Real>& rkCenter, Real fX0,
        Real fX1, Real fY0, Real fY1, int iMaxSteps, ConvexPolygon2& rkEgg);

    // Debugging support.
	//void Print (std::ofstream& rkOStr) const;
    //bool Print (const char* acFilename) const;

protected:
    // Support for intersection testing.
    static ConvexPolygon2* FindSolidIntersection (
        const ConvexPolygon2& rkPoly0, const ConvexPolygon2& rkPoly1);
    static int GetHighBit (int i);

    VArray m_akPoint;
    LArray m_akLine;
    Vector2<Real> m_kCentroid;
};

typedef ConvexPolygon2<float> ConvexPolygon2f;
typedef ConvexPolygon2<double> ConvexPolygon2d;

}

#endif
