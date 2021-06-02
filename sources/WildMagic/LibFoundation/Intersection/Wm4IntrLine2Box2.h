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

#ifndef WM4INTRLINE2BOX2_H
#define WM4INTRLINE2BOX2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Line2.h"
#include "Wm4Box2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrLine2Box2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Box2 (const Line2<Real>& rkLine, const Box2<Real>& rkBox);

    // object access
    const Line2<Real>& GetLine () const;
    const Box2<Real>& GetBox () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_SEGMENT;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;

    static bool Clip (Real fDenom, Real fNumer, Real& rfT0, Real& rfT1);

    // the objects to intersect
    const Line2<Real>* m_pkLine;
    const Box2<Real>* m_pkBox;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];

// internal use (shared by IntrRay2Box2 and IntrSegment2Box2)
public:
    static bool DoClipping (Real fT0, Real fT1, const Vector2<Real>& rkOrigin,
        const Vector2<Real>& rkDirection, const Box2<Real>& rkBox,
        bool bSolid, int& riQuantity, Vector2<Real> akPoint[2],
        int& riIntrType);
};

typedef IntrLine2Box2<float> IntrLine2Box2f;
typedef IntrLine2Box2<double> IntrLine2Box2d;

}

#endif
