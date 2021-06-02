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

#ifndef WM4REALV_H
#define WM4REALV_H

#include "Wm4GraphicsLIB.h"
#include "Wm4RealVBase.h"
#include "Wm4Bool.h"
#include "Wm4Math.h"

namespace Wm4
{

template <typename Real, int N>
class RealV : public RealVBase<Real>
{
public:
    // constructors
    RealV ();  // uninitialized
    RealV (Real fV0, Real fV1 = (Real)0, Real fV2 = (Real)0,
        Real fV3 = (Real)0);
    RealV (const RealV& rkVector);
    RealV (const RealVBase<Real>& rkVector);

    // assignment
    RealV& operator= (const RealV& rkVector);
    RealV& operator= (const RealVBase<Real>& rkVector);

    // comparisons
    Bool<N> operator== (const RealV& rkVector) const;
    Bool<N> operator!= (const RealV& rkVector) const;
    Bool<N> operator<  (const RealV& rkVector) const;
    Bool<N> operator<= (const RealV& rkVector) const;
    Bool<N> operator>  (const RealV& rkVector) const;
    Bool<N> operator>= (const RealV& rkVector) const;

    // arithmetic operations
    RealV operator+ (const RealV& rkVector) const;
    RealV operator- (const RealV& rkVector) const;
    RealV operator* (Real fScalar) const;
    RealV operator/ (Real fScalar) const;
    RealV operator- () const;

    // arithmetic updates
    RealV& operator+= (const RealV& rkVector);
    RealV& operator-= (const RealV& rkVector);
    RealV& operator*= (Real fScalar);
    RealV& operator/= (Real fScalar);

    using RealVBase<Real>::m_afTuple;
};

// arithmetic operations
template <typename Real, int N>
RealV<Real,N> operator* (Real fScalar, const RealV<Real,N>& rkVector);

#include "Wm4RealV.inl"

typedef RealV<float,1> Float1;
typedef RealV<float,2> Float2;
typedef RealV<float,3> Float3;
typedef RealV<float,4> Float4;

typedef RealV<double,1> Double1;
typedef RealV<double,2> Double2;
typedef RealV<double,3> Double3;
typedef RealV<double,4> Double4;

}

#endif
