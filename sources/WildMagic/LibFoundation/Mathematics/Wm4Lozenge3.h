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

#ifndef WM4LOZENGE3_H
#define WM4LOZENGE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Rectangle3.h"

namespace Wm4
{

template <class Real>
class Lozenge3
{
public:
    // construction
    Lozenge3 ();  // uninitialized
    Lozenge3 (const Rectangle3<Real>& rkRectangle, Real fRadius);

    Rectangle3<Real> Rectangle;
    Real Radius;
};

#include "Wm4Lozenge3.inl"

typedef Lozenge3<float> Lozenge3f;
typedef Lozenge3<double> Lozenge3d;

}

#endif
