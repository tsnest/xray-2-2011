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

//----------------------------------------------------------------------------
template <class Scalar>
RectangleSet<Scalar>::RectangleSet ()
    :
    m_iRectangleQuantity(0)
{
}
//----------------------------------------------------------------------------
template <class Scalar>
RectangleSet<Scalar>::RectangleSet (Scalar kXMin, Scalar kXMax, Scalar kYMin,
    Scalar kYMax)
{
    if (kXMin < kXMax && kYMin < kYMax)
    {
        m_iRectangleQuantity = 1;
        m_kStrips.push_back(Strip(kYMin,kYMax,
            new IntervalSet<Scalar>(kXMin,kXMax)));
    }
    else
    {
        m_iRectangleQuantity = 0;
    }
}
//----------------------------------------------------------------------------
template <class Scalar>
RectangleSet<Scalar>::~RectangleSet ()
{
}
//----------------------------------------------------------------------------
template <class Scalar>
int RectangleSet<Scalar>::GetRectangleQuantity () const
{
    return m_iRectangleQuantity;
}
//----------------------------------------------------------------------------
template <class Scalar>
bool RectangleSet<Scalar>::GetRectangle (int i, Scalar& rkXMin,
    Scalar& rkXMax, Scalar& rkYMin, Scalar& rkYMax) const
{
    int iYQuantity = (int)m_kStrips.size();
    for (int j = 0, iTotalQuantity = 0; j < iYQuantity; j++)
    {
        const IntervalSet<Scalar>& rkIntervals = *m_kStrips[j].Intervals;
        int iXQuantity = rkIntervals.GetIntervalQuantity();
        int iNextTotalQuantity = iTotalQuantity + iXQuantity;
        if (i < iNextTotalQuantity)
        {
            i -= iTotalQuantity;
            rkIntervals.GetInterval(i,rkXMin,rkXMax);
            return true;
        }
        iTotalQuantity = iNextTotalQuantity;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class Scalar>
void RectangleSet<Scalar>::Clear ()
{
    m_iRectangleQuantity = 0;
    m_kStrips.clear();
}
//----------------------------------------------------------------------------
template <class Scalar>
int RectangleSet<Scalar>::GetStripQuantity() const
{
    return (int)m_kStrips.size();
}
//----------------------------------------------------------------------------
template <class Scalar>
bool RectangleSet<Scalar>::GetStrip (int i, Scalar& rkYMin, Scalar& rkYMax,
    const IntervalSet<Scalar>* pkXIntervals) const
{
    if (0 <= i && i < GetStripQuantity())
    {
        const Strip& rkStrip = m_kStrips[i];
        rkYMin = rkStrip.Min;
        rkYMax = rkStrip.Max;
        if (pkXIntervals)
        {
            pkXIntervals = rkStrip.Intervals;
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class Scalar>
bool RectangleSet<Scalar>::Insert (Scalar kXMin, Scalar kXMax, Scalar kYMin,
    Scalar kYMax)
{
    if (kXMin < kXMax && kYMin < kYMax)
    {
        RectangleSet kInput(kXMin,kXMax,kYMin,kYMax), kOutput;
        Union(*this,kInput,kOutput);
        *this = kOutput;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class Scalar>
bool RectangleSet<Scalar>::Remove (Scalar kXMin, Scalar kXMax, Scalar kYMin,
    Scalar kYMax)
{
    if (kXMin < kXMax && kYMin < kYMax)
    {
        RectangleSet kInput(kXMin,kXMax,kYMin,kYMax), kOutput;
        Difference(*this,kInput,kOutput);
        *this = kOutput;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class Scalar>
void RectangleSet<Scalar>::Union (const RectangleSet& rkInput0,
    const RectangleSet& rkInput1, RectangleSet& rkOutput)
{
    Operate(&IntervalSet<Scalar>::Union,rkInput0,rkInput1,rkOutput);
}
//----------------------------------------------------------------------------
template <class Scalar>
void RectangleSet<Scalar>::Intersection (const RectangleSet& rkInput0,
    const RectangleSet& rkInput1, RectangleSet& rkOutput)
{
    Operate(&IntervalSet<Scalar>::Intersection,rkInput0,rkInput1,rkOutput);
}
//----------------------------------------------------------------------------
template <class Scalar>
void RectangleSet<Scalar>::Difference (const RectangleSet& rkInput0,
    const RectangleSet& rkInput1, RectangleSet& rkOutput)
{
    Operate(&IntervalSet<Scalar>::Difference,rkInput0,rkInput1,rkOutput);
}
//----------------------------------------------------------------------------
template <class Scalar>
void RectangleSet<Scalar>::ExclusiveOr (const RectangleSet& rkInput0,
    const RectangleSet& rkInput1, RectangleSet& rkOutput)
{
    Operate(&IntervalSet<Scalar>::ExclusiveOr,rkInput0,rkInput1,rkOutput);
}
//----------------------------------------------------------------------------
template <class Scalar>
void RectangleSet<Scalar>::ComputeRectangleQuantity ()
{
    const int iYQuantity = (int)m_kStrips.size();
    m_iRectangleQuantity = 0;
    for (int i = 0; i < iYQuantity; i++)
    {
        m_iRectangleQuantity += m_kStrips[i].Intervals->GetIntervalQuantity();
    }
}
//----------------------------------------------------------------------------
template <class Scalar>
void RectangleSet<Scalar>::Operate (
    typename IntervalSet<Scalar>::Operator oOperator,
    const RectangleSet& rkInput0, const RectangleSet& rkInput1,
    RectangleSet& rkOutput)
{
    rkOutput.Clear();

    bool bUnionExclusiveOr = (oOperator == &IntervalSet<Scalar>::Union ||
        oOperator == &IntervalSet<Scalar>::ExclusiveOr);

    bool bUnionExclusiveOrDifference = (bUnionExclusiveOr ||
        oOperator == &IntervalSet<Scalar>::Difference);

    const int iStripQuantity0 = rkInput0.GetStripQuantity();
    const int iStripQuantity1 = rkInput1.GetStripQuantity();
    int i0 = 0, i1 = 0;
    bool bGetOriginal0 = true, bGetOriginal1 = true;
    Scalar tMin0, tMax0, tMin1, tMax1;
    IntervalSet<Scalar>* pkResult;

    while (i0 < iStripQuantity0 && i1 < iStripQuantity1)
    {
        IntervalSet<Scalar>& rkIntr0 = *rkInput0.m_kStrips[i0].Intervals;
        if (bGetOriginal0)
        {
            tMin0 = rkInput0.m_kStrips[i0].Min;
            tMax0 = rkInput0.m_kStrips[i0].Max;
        }

        IntervalSet<Scalar>& rkIntr1 = *rkInput1.m_kStrips[i1].Intervals;
        if (bGetOriginal1)
        {
            tMin1 = rkInput1.m_kStrips[i1].Min;
            tMax1 = rkInput1.m_kStrips[i1].Max;
        }

        // Case 1.
        if (tMax1 <= tMin0)
        {
            // operator(empty,strip1)
            if (bUnionExclusiveOr)
            {
                rkOutput.m_kStrips.push_back(Strip(tMin1,tMax1,
                    new IntervalSet<Scalar>(rkIntr1)));
            }

            i1++;
            bGetOriginal0 = false;
            bGetOriginal1 = true;
            continue;  // using next tMin1/tMax1
        }

        // Case 11.
        if (tMin1 >= tMax0)
        {
            // operator(strip0,empty)
            if (bUnionExclusiveOrDifference)
            {
                rkOutput.m_kStrips.push_back(Strip(tMin0,tMax0,
                    new IntervalSet<Scalar>(rkIntr0)));
            }

            i0++;
            bGetOriginal0 = true;
            bGetOriginal1 = false;
            continue;  // using next tMin0/tMax0
        }

        // Reduce cases 2, 3, 4 to cases 5, 6, 7.
        if (tMin1 < tMin0)
        {
            // operator(empty,[tMin1,tMin0))
            if (bUnionExclusiveOr)
            {
                rkOutput.m_kStrips.push_back(Strip(tMin1,tMin0,
                    new IntervalSet<Scalar>(rkIntr1)));
            }

            tMin1 = tMin0;
            bGetOriginal1 = false;
        }

        // Reduce cases 8, 9, 10 to cases 5, 6, 7.
        if (tMin1 > tMin0)
        {
            // operator([tMin0,tMin1),empty)
            if (bUnionExclusiveOrDifference)
            {
                rkOutput.m_kStrips.push_back(Strip(tMin0,tMin1,
                    new IntervalSet<Scalar>(rkIntr0)));
            }

            tMin0 = tMin1;
            bGetOriginal0 = false;
        }

        // Case 5.
        if (tMax1 < tMax0)
        {
            // operator(strip0,[tMin1,tMax1))
            pkResult = new IntervalSet<Scalar>;
            oOperator(rkIntr0,rkIntr1,*pkResult);
            rkOutput.m_kStrips.push_back(Strip(tMin1,tMax1,pkResult));

            tMin0 = tMax1;
            i1++;
            bGetOriginal0 = false;
            bGetOriginal1 = true;
            continue;  // using next tMin1/tMax1
        }

        // Case 6.
        if (tMax1 == tMax0)
        {
            // operator(strip0,[tMin1,tMax1))
            pkResult = new IntervalSet<Scalar>;
            oOperator(rkIntr0,rkIntr1,*pkResult);
            rkOutput.m_kStrips.push_back(Strip(tMin1,tMax1,pkResult));

            i0++;
            i1++;
            bGetOriginal0 = true;
            bGetOriginal1 = true;
            continue;  // using next tMin0/tMax0 and tMin1/tMax1
        }

        // Case 7.
        if (tMax1 > tMax0)
        {
            // operator(strip0,[tMin1,tMax0))
            pkResult = new IntervalSet<Scalar>;
            oOperator(rkIntr0,rkIntr1,*pkResult);
            rkOutput.m_kStrips.push_back(Strip(tMin1,tMax0,pkResult));

            tMin1 = tMax0;
            i0++;
            bGetOriginal0 = true;
            bGetOriginal1 = false;
            // continue;  using current tMin1/tMax1
        }
    }

    if (bUnionExclusiveOrDifference)
    {
        while (i0 < iStripQuantity0)
        {
            if (bGetOriginal0)
            {
                tMin0 = rkInput0.m_kStrips[i0].Min;
                tMax0 = rkInput0.m_kStrips[i0].Max;
            }
            else
            {
                bGetOriginal0 = true;
            }

            // operator(strip0,empty)
            rkOutput.m_kStrips.push_back(Strip(tMin0,tMax0,
                new IntervalSet<Scalar>(*rkInput0.m_kStrips[i0].Intervals)));

            i0++;
        }
    }

    if (bUnionExclusiveOr)
    {
        while (i1 < iStripQuantity1)
        {
            if (bGetOriginal1)
            {
                tMin1 = rkInput1.m_kStrips[i1].Min;
                tMax1 = rkInput1.m_kStrips[i1].Max;
            }
            else
            {
                bGetOriginal1 = true;
            }

            // operator(empty,strip1)
            rkOutput.m_kStrips.push_back(Strip(tMin1,tMax1,
                new IntervalSet<Scalar>(*rkInput1.m_kStrips[i1].Intervals)));

            i1++;
        }
    }

    rkOutput.ComputeRectangleQuantity();
}
//----------------------------------------------------------------------------
