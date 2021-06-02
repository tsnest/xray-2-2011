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

#ifndef WM4LINCOMP3_H
#define WM4LINCOMP3_H

#include "Wm4FoundationLIB.h"
#include "Wm4LinComp.h"
#include "Wm4Vector3.h"

namespace Wm4
{

template <class Real>
class LinComp3 : public LinComp<Real>
{
public:
    // P is set to (0,0,0), D is set to (0,0,0), and [min,max] is set to
    // [MAX_REAL,-MAX_REAL].  The type is CT_EMPTY.
    LinComp3 ();

    // P and D are specified.  D must be unit length.
    LinComp3 (const Vector3<Real>& rkOrigin, const Vector3<Real>& rkDirection,
        Real fMin, Real fMax);

    // copy constructor
    LinComp3 (const LinComp3& rkComponent);

    // assignment
    LinComp3& operator= (const LinComp3& rkComponent);

    virtual void MakeCanonical ();

    Vector3<Real> Origin, Direction;

protected:
    using LinComp<Real>::CT_EMPTY;
    using LinComp<Real>::CT_POINT;
    using LinComp<Real>::CT_SEGMENT;
    using LinComp<Real>::CT_RAY;
    using LinComp<Real>::CT_LINE;
    using LinComp<Real>::m_iType;
    using LinComp<Real>::m_fMin;
    using LinComp<Real>::m_fMax;
};

#include "Wm4LinComp3.inl"

typedef LinComp3<float> LinComp3f;
typedef LinComp3<double> LinComp3d;

}

#endif
