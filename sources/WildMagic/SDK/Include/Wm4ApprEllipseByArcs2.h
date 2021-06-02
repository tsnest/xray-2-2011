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

#ifndef WM4APPRELLIPSEBYARCS2_H
#define WM4APPRELLIPSEBYARCS2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Vector2.h"

namespace Wm4
{

// The ellipse is (x/a)^2 + (y/b)^2 = 1, but only the portion in the first
// quadrant (x >= 0 and y >= 0) is approximated.  Generate iNumArcs >= 2 arcs
// by constructing points corresponding to the weighted averages of the
// curvatures at the ellipse points (a,0) and (0,b).  The returned input point
// array has iNumArcs+1 elements and the returned input center and radius
// arrays each have iNumArc elements.  The arc associated with rakPoint[i] and
// rakPoint[i+1] has center rakCenter[i] and radius rafRadius[i].

template <class Real> WM4_FOUNDATION_ITEM
void ApproximateEllipseByArcs (Real fA, Real fB, int iNumArcs,
    Vector2<Real>*& rakPoint, Vector2<Real>*& rakCenter, Real*& rafRadius);

}

#endif
