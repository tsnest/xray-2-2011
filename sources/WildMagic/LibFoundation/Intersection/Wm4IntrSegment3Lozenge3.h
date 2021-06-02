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

#ifndef WM4INTRSEGMENT3LOZENGE3_H
#define WM4INTRSEGMENT3LOZENGE3_H

#include "Wm4FoundationLIB.h"
#include "Wm4Intersector.h"
#include "Wm4Segment3.h"
#include "Wm4Lozenge3.h"

namespace Wm4
{

template <class Real>
class WM4_FOUNDATION_ITEM IntrSegment3Lozenge3
    : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Lozenge3 (const Segment3<Real>& rkSegment,
        const Lozenge3<Real>& rkLozenge);

    // object access
    const Segment3<Real>& GetSegment () const;
    const Lozenge3<Real>& GetLozenge () const;

    // static intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Segment3<Real>* m_pkSegment;
    const Lozenge3<Real>* m_pkLozenge;
};

typedef IntrSegment3Lozenge3<float> IntrSegment3Lozenge3f;
typedef IntrSegment3Lozenge3<double> IntrSegment3Lozenge3d;

}

#endif
