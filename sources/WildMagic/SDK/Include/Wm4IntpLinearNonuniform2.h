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
// Version: 4.0.1 (2007/05/06)

#ifndef WM4INTPLINEARNONUNIFORM2_H
#define WM4INTPLINEARNONUNIFORM2_H

// Linear interpolation of a network of triangles whose vertices are of the
// form (x,y,f(x,y)).

#include "Wm4FoundationLIB.h"
#include "Wm4Delaunay2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntpLinearNonuniform2
{
public:
    // Construction and destruction.  If you want IntpLinearNonuniform2 to
    // delete the input array during destruction, set bOwner to 'true'.
    // Otherwise, you own the array and must delete it yourself.
    IntpLinearNonuniform2 (const Delaunay2<Real>& rkDT, Real* afF,
        bool bOwner);

    ~IntpLinearNonuniform2 ();

    // Linear interpolation.  The return value is 'true' if and only if the
    // input point P is in the convex hull of the input vertices, in which
    // case the interpolation is valid.
    bool Evaluate (const Vector2<Real>& rkP, Real& rfF);

private:
    const Delaunay2<Real>* m_pkDT;
    Real* m_afF;
    bool m_bOwner;
};

typedef IntpLinearNonuniform2<float> IntpLinearNonuniform2f;
typedef IntpLinearNonuniform2<double> IntpLinearNonuniform2d;

}

#endif
