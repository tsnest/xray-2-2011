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
// Version: 4.7.0 (2008/07/28)

#ifndef INTERVALSET_H
#define INTERVALSET_H

#include <vector>

// Disjoint sets of half-open intervals of the form [min,max) with min < max.
template <class Scalar>
class IntervalSet
{
public:
    // Construction and destruction.  The non-default constructor requires
    // that kMin < kMax.
    IntervalSet ();
    IntervalSet (const IntervalSet& rkISet);
    IntervalSet (Scalar kMin, Scalar kMax);
    ~IntervalSet ();

    // The number of intervals in the set.
    int GetIntervalQuantity () const;

    // The i-th interval is [rkMin,rkMax).  The values rkMin and rkMax are
    // valid only when 0 <= i < GetQuantity().
    bool GetInterval (int i, Scalar& rkMin, Scalar& rkMax) const;

    // Make this set empty.
    void Clear ();

    // Insert [kMin,kMax) into the set.  This is a Boolean 'union'
    // operation.  The operation is successful only when kMin < kMax.
    bool Insert (Scalar kMin, Scalar kMax);

    // Remove [kMin,kMax) from the set.  This is a Boolean 'difference'
    // operation.  The operation is successful only when kMin < kMax.
    bool Remove (Scalar kMin, Scalar kMax);

    // Boolean operations of sets.  If you want to know the indices of the
    // input intervals that contributed to an output interval, pass an
    // array in the last component to store the pairs of indices.  It is
    // possible that a pair has a -1 component.  In this case, only the
    // nonnegative component contributed.
    static void Union (const IntervalSet& rkInput,
        const IntervalSet& rkInput1, IntervalSet& rkOutput);

    static void Intersection (const IntervalSet& rkInput0,
        const IntervalSet& rkInput1, IntervalSet& rkOutput);

    static void Difference (const IntervalSet& rkInput0,
        const IntervalSet& rkInput1, IntervalSet& rkOutput);

    static void ExclusiveOr (const IntervalSet& rkInput0,
        const IntervalSet& rkInput1, IntervalSet& rkOutput);

    // For use by RectangleSet<Scalar>.
    typedef void (*Operator)(const IntervalSet&, const IntervalSet&,
        IntervalSet&);

private:
    // The array of endpoints has an even number of elements.  The i-th
    // interval is [m_kEndPoints[2*i],m_kEndPoints[2*i+1]).
    std::vector<Scalar> m_kEndpoints;
};

#include "IntervalSet.inl"

#endif
