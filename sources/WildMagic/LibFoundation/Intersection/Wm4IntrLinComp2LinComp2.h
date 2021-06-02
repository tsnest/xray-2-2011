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

#ifndef WM4INTRLINCOMP2LINCOMP2_H
#define WM4INTRLINCOMP2LINCOMP2_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Intersector1.h"
#include "Wm4LinComp2.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrLinComp2LinComp2
    : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLinComp2LinComp2 (const LinComp2<Real>& rkComponent0,
        const LinComp2<Real>& rkComponent1);

    // object access
    const LinComp2<Real>& GetComponent0 () const;
    const LinComp2<Real>& GetComponent1 () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // Information about the intersection set.  GetIntersectionType() returns
    // IT_EMPTY, IT_POINT, IT_LINE, IT_RAY, or IT_SEGMENT.
    const LinComp2<Real>& GetIntersectionComponent() const;

    // If the intersection is a single point, the parameters of the input
    // components at the point of intersection may be accessed using these
    // functions.
    Real GetParameter0 () const;
    Real GetParameter1 () const;

protected:
    using Intersector<Real,Vector2<Real> >::IT_EMPTY;
    using Intersector<Real,Vector2<Real> >::IT_POINT;
    using Intersector<Real,Vector2<Real> >::IT_LINE;
    using Intersector<Real,Vector2<Real> >::IT_OTHER;
    using Intersector<Real,Vector2<Real> >::m_iIntersectionType;

    // The linear components are P0 + s0*D0 and P1 + s1*D1.  Determine if
    // the lines intersect in a single point, are the same line, or are
    // parallel and disjoint.  If you want to know the parameters s0 and s1,
    // pass a nonnull array to afS; it must have two elements.  If you want
    // to know P1-P0, pass a nonnull pointer to pkDiff.  If you want to know
    // (P1-P0)/|P1-P0|, pass a nonnull pointer to pkDiffN.  The return value
    // is one of the IT_* enumerations.  The afS values are valid only when
    // the return is IT_POINT.  The pkDiff value is valid always.  The pkDiffN
    // value is valid only when the return value is IT_EMPTY or IT_LINE.
    static int Classify (const LinComp2<Real>& rkComponent0,
        const LinComp2<Real>& rkComponent1, Real* afS = 0,
        Vector2<Real>* pkDiff = 0, Vector2<Real>* pkDiffN = 0);

    // the objects to intersect
    const LinComp2<Real>* m_pkComponent0;
    const LinComp2<Real>* m_pkComponent1;

    // information about the intersection set
    LinComp2<Real> m_kIntrComponent;
    Real m_afParameter[2];
};

typedef IntrLinComp2LinComp2<float> IntrLinComp2LinComp2f;
typedef IntrLinComp2LinComp2<double> IntrLinComp2LinComp2d;

}

#endif
