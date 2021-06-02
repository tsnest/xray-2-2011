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

#ifndef WM4TIVECTOR_H
#define WM4TIVECTOR_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <int VSIZE>
class TIVector
{
public:
    // construction
    TIVector ();
    TIVector (const TIVector& rkV);

    // coordinate access
    operator const Integer64* () const;
    operator Integer64* ();
    Integer64 operator[] (int i) const;
    Integer64& operator[] (int i);

    // assignment
    TIVector& operator= (const TIVector& rkV);

    // comparison
    bool operator== (const TIVector& rkV) const;
    bool operator!= (const TIVector& rkV) const;
    bool operator<  (const TIVector& rkV) const;
    bool operator<= (const TIVector& rkV) const;
    bool operator>  (const TIVector& rkV) const;
    bool operator>= (const TIVector& rkV) const;

    // arithmetic operations
    TIVector operator+ (const TIVector& rkV) const;
    TIVector operator- (const TIVector& rkV) const;
    TIVector operator* (const Integer64& riI) const;
    TIVector operator/ (const Integer64& riI) const;
    TIVector operator- () const;

    // arithmetic updates
    TIVector& operator+= (const TIVector& rkV);
    TIVector& operator-= (const TIVector& rkV);
    TIVector& operator*= (const Integer64& riI);
    TIVector& operator/= (const Integer64& riI);

    // vector operations
    Integer64 SquaredLength () const;
    Integer64 Dot (const TIVector& rkV) const;

protected:
    // support for comparisons
    int CompareArrays (const TIVector& rkV) const;

    Integer64 m_aiTuple[VSIZE];
};

template <int VSIZE>
TIVector<VSIZE> operator* (const Integer64& riI, const TIVector<VSIZE>& rkV);

#include "Wm4TIVector.inl"

}

#endif
