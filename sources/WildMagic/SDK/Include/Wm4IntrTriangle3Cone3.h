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

#ifndef WM4INTRTRIANGLE3CONE3_H
#define WM4INTRTRIANGLE3CONE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Triangle3.h"
#include "Wm4Cone3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrTriangle3Cone3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Cone3 (const Triangle3<Real>& rkTriangle,
        const Cone3<Real>& rkCone);

    // object access
    const Triangle3<Real>& GetTriangle () const;
    const Cone3<Real>& GetCone () const;

    // static query
    virtual bool Test ();

private:
    // the objects to intersect
    const Triangle3<Real>* m_pkTriangle;
    const Cone3<Real>* m_pkCone;
};

typedef IntrTriangle3Cone3<float> IntrTriangle3Cone3f;
typedef IntrTriangle3Cone3<double> IntrTriangle3Cone3d;

}

#endif
