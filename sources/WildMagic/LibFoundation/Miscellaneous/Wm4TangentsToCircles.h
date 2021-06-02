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

#ifndef WM4TANGENTSTOCIRCLES_H
#define WM4TANGENTSTOCIRCLES_H

// Given two circles |X-C0|^2 = R0^2 and |X-C1|^2 = R1^2, compute the
// lines that are tangent to both circles.  Assumptions:  The two circles
// do not intersect and one does not contain the other.  This condition
// is guaranteed when |C1-C0| > R0+R1.  Without loss of generality, assume
// that R1 >= R0.
//
// A line in parametric form is X(t) = P+t*D where D is a unit length
// vector.  To intersect the first circle,
//
//   R0^2 = |X(t)-C0|^2 = t^2 + 2*Dot(D,P-C0)*t + |P-C0|^2
//
// To be tangent at that intersect, we need
//
//   0 = Dot(D,X(t)-C0) = t + Dot(D,P-C0).
//
// Solve for t in the second equation and plug into the first to get
//
//   R0^2 = |P-C0|^2 - [Dot(D,P-C0)]^2.
//
// Similarly for the second circle,
//
//   R1^2 = |P-C1|^2 - [Dot(D,P-C1)]^2.
//
// It should be intuitive from the geometry that the point P can be
// chosen on the line containing the two centers C0 and C1.  Let
// P = (1-s)*C0+s*C1 for unknown s.  Define W = C1-C0.  Then
// P-C0 = s*W and P-C1 = (s-1)*W.  Plug these into the previous two
// displayed equations to get
//
//   R0^2 = s^2*{|W|^2 - [Dot(D,W)]^2}
//   R1^2 = (s-1)^2*{|W|^2 - [Dot(D,W)]^2}
//
// Thus, R0^2/s^2 = R1^2/(s-1)^2 or
//
//   (R1^2-R0^2)*s^2 + 2*R0^2*s - R0^2 = 0.
//
// If R1 = R0, then s = 1/2.  The point P is the midpoint of the line
// segment connecting the circle centers (expected from the geometric
// symmetry).  Moreover, [Dot(D,W)]^2 = |W|^2 - 4*R0^2 = A^2 > 0, so
// Dot(D,W) = A (using -A just leads to a direction vector with
// opposite sign).  If D = (d0,d1), Dot(D,W) = A is the equation for
// a line.  The constraint |D|^2 = 1 corresponds to a circle.  The two
// together represent intersection of line with circle.  Either solution
// will do.  Let W = (w0,w1).  Then w0*d0+w1*d1 = A and d0^2+d1^2 = 1.
// If |w0| >= |w1|, then d0 = (A-w1*d1)/w0 and
//
//   (w0^2+w1^2)*d1^2 - 2*A*w1*d1 + A^2-w0^2 = 0
//
// If |w1| >= |w0|, then d1 = (A-w0*d0)/w1 and
//
//   (w0^2+w1^2)*d0^2 - 2*A*w0*d0 + A^2-w1^2 = 0.
//
// In either case, the two roots lead to two direction vectors for
// the tangent lines.
//
// If R1 > R0, the quadratic in s has two real-valued solutions.  From
// the geometry, one of the value must satisfy 0 < s < 1 and produces
// the two tangents that intersect each other between the two circles.
// The other root cannot be s = 0 (otherwise P would be at a circle
// center, not possible).
//
// For each root s, the same idea as in the case R1 = R0 works.  The
// quadratic to solve is [Dot(D,W)]^2 = |W|^2 - R0^2/s^2 = A^2 > 0.
// Also, [Dot(D,W)]^2 = |W|^2 - R^2/(s-1)^2 = A^2.  The first equation
// should be used when s^2 >= (s-1)^2, otherwise use the second one.
// The same quadratics may be set up for d0 or d1 (A has a different
// value, though) and solved.

#include "Wm4FoundationLIB.h"
#include "Wm4Circle2.h"
#include "Wm4Line2.h"

namespace Wm4
{

// The function returns 'true' if all four tangent lines have been computed
// successfully.  The return value is 'false' if the circles are
// intersecting or nested, in which case the output array of lines is
// invalid.

template <class Real> WM4_FOUNDATION_ITEM
bool GetTangentsToCircles (const Circle2<Real>& rkCircle0,
    const Circle2<Real>& rkCircle1, Line2<Real> akLine[4]);

}

#endif
