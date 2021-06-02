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

#ifndef WM4INTRBOX2BOX2_H
#define WM4INTRBOX2BOX2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Box2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrBox2Box2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrBox2Box2 (const Box2<Real>& rkBox0, const Box2<Real>& rkBox1);

    // object access
    const Box2<Real>& GetBox0 () const;
    const Box2<Real>& GetBox1 () const;

    // static test-intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Box2<Real>* m_pkBox0;
    const Box2<Real>* m_pkBox1;
};

typedef IntrBox2Box2<float> IntrBox2Box2f;
typedef IntrBox2Box2<double> IntrBox2Box2d;

}

#endif
