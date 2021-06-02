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
// Version: 4.0.2 (2007/07/11)

#include "Wm4PhysicsPCH.h"
#include "Wm4IntersectingIntervals.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntersectingIntervals<Real>::IntersectingIntervals (
    std::vector<Vector2<Real> >& rkInterval)
    :
    m_pkInterval(&rkInterval)
{
    Initialize();
}
//----------------------------------------------------------------------------
template <class Real>
IntersectingIntervals<Real>::~IntersectingIntervals ()
{
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingIntervals<Real>::Initialize ()
{
    // get the interval end points
    int iISize = (int)m_pkInterval->size(), iESize = 2*iISize;
    m_kEndPoint.resize(iESize);
    int i, j;
    for (i = 0, j = 0; i < iISize; i++)
    {
        EndPoint& rkEMin = m_kEndPoint[j++];
        rkEMin.Type = 0;
        rkEMin.Value = (*m_pkInterval)[i][0];
        rkEMin.Index = i;

        EndPoint& rkEMax = m_kEndPoint[j++];
        rkEMax.Type = 1;
        rkEMax.Value = (*m_pkInterval)[i][1];
        rkEMax.Index = i;
    }

    // sort the interval end points
    std::sort(m_kEndPoint.begin(),m_kEndPoint.end());

    // create the interval-to-endpoint lookup table
    m_kLookup.resize(iESize);
    for (j = 0; j < iESize; j++)
    {
        EndPoint& rkE = m_kEndPoint[j];
        m_kLookup[2*rkE.Index + rkE.Type] = j;
    }

    // active set of intervals (stored by index in array)
    std::set<int> kActive;

    // set of overlapping intervals (stored by pairs of indices in array)
    m_kOverlap.clear();

    // sweep through the end points to determine overlapping intervals
    for (i = 0; i < iESize; i++)
    {
        EndPoint& rkEnd = m_kEndPoint[i];
        int iIndex = rkEnd.Index;
        if (rkEnd.Type == 0)  // an interval 'begin' value
        {
            std::set<int>::iterator pkIter = kActive.begin();
            for (/**/; pkIter != kActive.end(); pkIter++)
            {
                int iAIndex = *pkIter;
                if (iAIndex < iIndex)
                {
                    m_kOverlap.insert(EdgeKey(iAIndex,iIndex));
                }
                else
                {
                    m_kOverlap.insert(EdgeKey(iIndex,iAIndex));
                }
            }
            kActive.insert(iIndex);
        }
        else  // an interval 'end' value
        {
            kActive.erase(iIndex);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingIntervals<Real>::SetInterval (int i, Real fMin, Real fMax)
{
    assert(0 <= i && i < (int)m_pkInterval->size());
    (*m_pkInterval)[i][0] = fMin;
    (*m_pkInterval)[i][1] = fMax;
    m_kEndPoint[m_kLookup[2*i]].Value = fMin;
    m_kEndPoint[m_kLookup[2*i+1]].Value = fMax;
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingIntervals<Real>::GetInterval (int i, Real& rfMin,
    Real& rfMax) const
{
    assert(0 <= i && i < (int)m_pkInterval->size());
    rfMin = (*m_pkInterval)[i][0];
    rfMax = (*m_pkInterval)[i][1];
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingIntervals<Real>::Update ()
{
    // Apply an insertion sort.  Under the assumption that the intervals
    // have not changed much since the last call, the end points are nearly
    // sorted.  The insertion sort should be very fast in this case.
    int iESize = (int)m_kEndPoint.size();
    for (int j = 1; j < iESize; j++)
    {
        EndPoint kKey = m_kEndPoint[j];
        int i = j - 1;
        while (i >= 0 && kKey < m_kEndPoint[i])
        {
            EndPoint kE0 = m_kEndPoint[i];
            EndPoint kE1 = m_kEndPoint[i+1];

            // Update the overlap status.
            if (kE0.Type == 0)
            {
                if (kE1.Type == 1)
                {
                    // The 'b' of interval E0.Index was smaller than the 'e'
                    // of interval E1.Index, and the intervals *might have
                    // been* overlapping.  Now 'b' and 'e' are swapped, and
                    // the intervals cannot overlap.  Remove the pair from
                    // the overlap set.  The removal operation needs to find
                    // the pair and erase it if it exists.  Finding the pair
                    // is the expensive part of the operation, so there is no
                    // time savings in testing for existence first and then
                    // deleting.
                    m_kOverlap.erase(EdgeKey(kE0.Index,kE1.Index));
                }
            }
            else
            {
                if (kE1.Type == 0)
                {
                    // The 'b' of interval E1.index was larger than the 'e'
                    // of interval E0.index, and the intervals were not
                    // overlapping.  Now 'b' and 'e' are swapped, and the
                    // intervals *might be* overlapping.  Determine if they
                    // are overlapping and then insert.
                    const Vector2<Real>& rkI0 = (*m_pkInterval)[kE0.Index];
                    const Vector2<Real>& rkI1 = (*m_pkInterval)[kE1.Index];
                    if (rkI0[0] <= rkI1[1])
                    {
                        m_kOverlap.insert(EdgeKey(kE0.Index,kE1.Index));
                    }
                }
            }

            // Reorder the items to maintain the sorted list.
            m_kEndPoint[i] = kE1;
            m_kEndPoint[i+1] = kE0;
            m_kLookup[2*kE1.Index + kE1.Type] = i;
            m_kLookup[2*kE0.Index + kE0.Type] = i+1;
            i--;
        }
        m_kEndPoint[i+1] = kKey;
        m_kLookup[2*kKey.Index + kKey.Type] = i+1;
    }
}
//----------------------------------------------------------------------------
template <class Real>
const std::set<EdgeKey>& IntersectingIntervals<Real>::GetOverlap () const
{
    return m_kOverlap;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_PHYSICS_ITEM
class IntersectingIntervals<float>;

template WM4_PHYSICS_ITEM
class IntersectingIntervals<double>;
//----------------------------------------------------------------------------
}
