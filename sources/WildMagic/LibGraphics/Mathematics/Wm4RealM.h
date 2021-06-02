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
// Version: 4.0.0 (2006/09/06)

#ifndef WM4REALM_H
#define WM4REALM_H

#include "Wm4GraphicsLIB.h"
#include "Wm4RealMBase.h"

namespace Wm4
{

template <typename Real, int Rows, int Cols>
class RealM : public RealMBase<Real>
{
public:
    // constructors
    RealM ();  // uninitialized
    RealM (bool bZero);  // zero if 'true', Math<Real>::MAX_REAL if 'false'
    RealM (const RealM& rkMatrix);
    RealM (const RealMBase<Real>& rkMatrix);

    // assignment
    RealM& operator= (const RealM& rkMatrix);
    RealM& operator= (const RealMBase<Real>& rkMatrix);

    // arithmetic operations
    RealM operator+ (const RealM& rkMatrix) const;
    RealM operator- (const RealM& rkMatrix) const;
    RealM operator* (Real fScalar) const;
    RealM operator/ (Real fScalar) const;
    RealM operator- () const;

    // arithmetic updates
    RealM& operator+= (const RealM& rkMatrix);
    RealM& operator-= (const RealM& rkMatrix);
    RealM& operator*= (Real fScalar);
    RealM& operator/= (Real fScalar);

    enum 
    {
        N = Rows*Cols,
        INCR = 5 - Cols
    };

    using RealMBase<Real>::m_afEntry;
};

// arithmetic operations
template <typename Real, int Rows, int Cols>
RealM<Real,Rows,Cols> operator* (Real fScalar,
    const RealM<Real,Rows,Cols>& rkMatrix);

#include "Wm4RealM.inl"

typedef RealM<float,1,1> Float1x1;
typedef RealM<float,1,2> Float1x2;
typedef RealM<float,1,3> Float1x3;
typedef RealM<float,1,4> Float1x4;
typedef RealM<float,2,1> Float2x1;
typedef RealM<float,2,2> Float2x2;
typedef RealM<float,2,3> Float2x3;
typedef RealM<float,2,4> Float2x4;
typedef RealM<float,3,1> Float3x1;
typedef RealM<float,3,2> Float3x2;
typedef RealM<float,3,3> Float3x3;
typedef RealM<float,3,4> Float3x4;
typedef RealM<float,4,1> Float4x1;
typedef RealM<float,4,2> Float4x2;
typedef RealM<float,4,3> Float4x3;
typedef RealM<float,4,4> Float4x4;

typedef RealM<double,1,1> Double1x1;
typedef RealM<double,1,2> Double1x2;
typedef RealM<double,1,3> Double1x3;
typedef RealM<double,1,4> Double1x4;
typedef RealM<double,2,1> Double2x1;
typedef RealM<double,2,2> Double2x2;
typedef RealM<double,2,3> Double2x3;
typedef RealM<double,2,4> Double2x4;
typedef RealM<double,3,1> Double3x1;
typedef RealM<double,3,2> Double3x2;
typedef RealM<double,3,3> Double3x3;
typedef RealM<double,3,4> Double3x4;
typedef RealM<double,4,1> Double4x1;
typedef RealM<double,4,2> Double4x2;
typedef RealM<double,4,3> Double4x3;
typedef RealM<double,4,4> Double4x4;

}

#endif
