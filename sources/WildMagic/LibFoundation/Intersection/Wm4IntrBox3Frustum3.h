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

#ifndef WM4INTRBOX3FRUSTUM3_H
#define WM4INTRBOX3FRUSTUM3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Box3.h"
#include "Wm4Frustum3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrBox3Frustum3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrBox3Frustum3 (const Box3<Real>& rkBox,
        const Frustum3<Real>& rkFrustum);

    // object access
    const Box3<Real>& GetBox () const;
    const Frustum3<Real>& GetFrustum () const;

    // test-intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Box3<Real>* m_pkBox;
    const Frustum3<Real>* m_pkFrustum;
};

typedef IntrBox3Frustum3<float> IntrBox3Frustum3f;
typedef IntrBox3Frustum3<double> IntrBox3Frustum3d;

}

#endif
