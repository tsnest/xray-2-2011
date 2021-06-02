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

#ifndef RECTANGLESET_H
#define RECTANGLESET_H

#include "IntervalSet.h"

// Disjoint sets of half-open rectangles of the form [xmin,xmax)x[ymin,ymax)
// with xmin < xmax and ymin < ymax.
template <class Scalar>
class RectangleSet
{
public:
    // Construction and destruction.  The non-default constructor requires
    // that kXMin < kXMax and kYMin < kYMax.
    RectangleSet ();
    RectangleSet (Scalar kXMin, Scalar kXMax, Scalar kYMin, Scalar kYMax);
    ~RectangleSet ();

    // The rectangle set consists of y-strips of interval sets.
    class Strip
    {
    public:
        Strip (Scalar kMin = (Scalar)0, Scalar kMax = (Scalar)0,
            IntervalSet<Scalar>* pkIntervals = 0)
            :
            Min(kMin),
            Max(kMax),
            Intervals(pkIntervals)
        {
        }

        Scalar Min, Max;
        IntervalSet<Scalar>* Intervals;
    };

    // The number of rectangles in the set.
    int GetRectangleQuantity () const;

    // The i-th rectangle is [rkXMin,rkXMax)x[rkYMin,rkYMax).  The values
    // rkXMin, rkXMax, rkYMin, and rkYMax are valid only when
    // 0 <= i < GetQuantity().
    bool GetRectangle (int i, Scalar& rkXMin, Scalar& rkXMax, Scalar& rkYMin,
        Scalar& rkYMax) const;

    // Make this set empty.
    void Clear ();

    // The number of y-strips in the set.
    int GetStripQuantity () const;

    // The i-th strip.  The returned values are valid only when
    // 0 <= i < GetStripQuantity().
    bool GetStrip (int i, Scalar& rkYMin, Scalar& ckYMax,
        const IntervalSet<Scalar>* pkXIntervals = 0) const;

    // Insert [kXMin,kXMax)x[kYMin,kYMax) into the set.  This is a Boolean
    // union operation.  The operation is successful only when kXMin < kXMax
    // and kYMin < kYMax.
    bool Insert (Scalar kXMin, Scalar kXMax, Scalar kYMin, Scalar kYMax);

    // Remove [kXMin,kXMax)x[kYMin,kYMax) from the set.  This is a Boolean
    // difference operation.  The operation is successful only when
    // kXMin < kXMax and kYMin < kYMax.
    bool Remove (Scalar kXMin, Scalar kXMax, Scalar kYMin, Scalar kYMax);

    // Boolean operations of sets.
    static void Union (const RectangleSet& rkInput0,
        const RectangleSet& rkInput1, RectangleSet& rkOutput);

    static void Intersection (const RectangleSet& rkInput0,
        const RectangleSet& rkInput1, RectangleSet& rkOutput);

    static void Difference (const RectangleSet& rkInput0,
        const RectangleSet& rkInput1, RectangleSet& rkOutput);

    static void ExclusiveOr (const RectangleSet& rkInput0,
        const RectangleSet& rkInput1, RectangleSet& rkOutput);

private:
    static void Operate (typename IntervalSet<Scalar>::Operator oOperator,
        const RectangleSet& rkInput0, const RectangleSet& rkInput1,
        RectangleSet& rkOutput);

    void ComputeRectangleQuantity ();

    // The number of rectangles in the set.
    int m_iRectangleQuantity;

    // The y-strips of the set, each containing an x-interval set.
    std::vector<Strip> m_kStrips;
};

#include "RectangleSet.inl"

#endif
