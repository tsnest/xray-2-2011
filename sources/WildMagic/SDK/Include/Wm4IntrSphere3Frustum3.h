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

#ifndef WM4INTRSPHERE3FRUSTUM3_H
#define WM4INTRSPHERE3FRUSTUM3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Sphere3.h"
#include "Wm4Frustum3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrSphere3Frustum3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSphere3Frustum3 (const Sphere3<Real>& rkSphere,
        const Frustum3<Real>& rkFrustum);

    // object access
    const Sphere3<Real>& GetSphere () const;
    const Frustum3<Real>& GetFrustum () const;

    // static intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Sphere3<Real>* m_pkSphere;
    const Frustum3<Real>* m_pkFrustum;
};

typedef IntrSphere3Frustum3<float> IntrSphere3Frustum3f;
typedef IntrSphere3Frustum3<double> IntrSphere3Frustum3d;

}

#endif
