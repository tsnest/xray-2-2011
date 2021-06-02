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
#include "Wm4IntersectingBoxes.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntersectingBoxes<Real>::IntersectingBoxes (
    std::vector<AxisAlignedBox3<Real> >& rkBoxes)
    :
    m_pkBoxes(&rkBoxes)
{
    Initialize();
}
//----------------------------------------------------------------------------
template <class Real>
IntersectingBoxes<Real>::~IntersectingBoxes ()
{
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingBoxes<Real>::Initialize ()
{
    // get the rectangle end points
    int iISize = (int)m_pkBoxes->size(), iESize = 2*iISize;
    m_kXEndPoint.resize(iESize);
    m_kYEndPoint.resize(iESize);
    m_kZEndPoint.resize(iESize);
    int i, j;
    for (i = 0, j = 0; i < iISize; i++)
    {
        m_kXEndPoint[j].Type = 0;
        m_kXEndPoint[j].Value = (*m_pkBoxes)[i].Min[0];
        m_kXEndPoint[j].Index = i;
        m_kYEndPoint[j].Type = 0;
        m_kYEndPoint[j].Value = (*m_pkBoxes)[i].Min[1];
        m_kYEndPoint[j].Index = i;
        m_kZEndPoint[j].Type = 0;
        m_kZEndPoint[j].Value = (*m_pkBoxes)[i].Min[2];
        m_kZEndPoint[j].Index = i;
        j++;

        m_kXEndPoint[j].Type = 1;
        m_kXEndPoint[j].Value = (*m_pkBoxes)[i].Max[0];
        m_kXEndPoint[j].Index = i;
        m_kYEndPoint[j].Type = 1;
        m_kYEndPoint[j].Value = (*m_pkBoxes)[i].Max[1];
        m_kYEndPoint[j].Index = i;
        m_kZEndPoint[j].Type = 1;
        m_kZEndPoint[j].Value = (*m_pkBoxes)[i].Max[2];
        m_kZEndPoint[j].Index = i;
        j++;
    }

    // sort the rectangle end points
    std::sort(m_kXEndPoint.begin(),m_kXEndPoint.end());
    std::sort(m_kYEndPoint.begin(),m_kYEndPoint.end());
    std::sort(m_kZEndPoint.begin(),m_kZEndPoint.end());

    // create the interval-to-endpoint lookup tables
    m_kXLookup.resize(iESize);
    m_kYLookup.resize(iESize);
    m_kZLookup.resize(iESize);
    for (j = 0; j < iESize; j++)
    {
        m_kXLookup[2*m_kXEndPoint[j].Index + m_kXEndPoint[j].Type] = j;
        m_kYLookup[2*m_kYEndPoint[j].Index + m_kYEndPoint[j].Type] = j;
        m_kZLookup[2*m_kZEndPoint[j].Index + m_kZEndPoint[j].Type] = j;
    }

    // active set of rectangles (stored by index in array)
    std::set<int> kActive;

    // set of overlapping rectangles (stored by pairs of indices in array)
    m_kOverlap.clear();

    // sweep through the end points to determine overlapping x-intervals
    for (i = 0; i < iESize; i++)
    {
        EndPoint& rkEnd = m_kXEndPoint[i];
        int iIndex = rkEnd.Index;
        if (rkEnd.Type == 0)  // an interval 'begin' value
        {
            // In the 1D problem, the current interval overlaps with all the
            // active intervals.  In 3D this we also need to check for
            // y-overlap and z-overlap.
            std::set<int>::iterator pkIter = kActive.begin();
            for (/**/; pkIter != kActive.end(); pkIter++)
            {
                // Rectangles iAIndex and iIndex overlap in the x-dimension.
                // Test for overlap in the y-dimension and z-dimension.
                int iAIndex = *pkIter;
                const AxisAlignedBox3<Real>& rkB0 = (*m_pkBoxes)[iAIndex];
                const AxisAlignedBox3<Real>& rkB1 = (*m_pkBoxes)[iIndex];
                if ( rkB0.HasYOverlap(rkB1) && rkB0.HasZOverlap(rkB1) )
                {
                    if (iAIndex < iIndex)
                    {
                        m_kOverlap.insert(EdgeKey(iAIndex,iIndex));
                    }
                    else
                    {
                        m_kOverlap.insert(EdgeKey(iIndex,iAIndex));
                    }
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
void IntersectingBoxes<Real>::SetBox (int i,
    const AxisAlignedBox3<Real>& rkBox)
{
    assert(0 <= i && i < (int)m_pkBoxes->size());
    (*m_pkBoxes)[i] = rkBox;
    m_kXEndPoint[m_kXLookup[2*i]].Value = rkBox.Min[0];
    m_kXEndPoint[m_kXLookup[2*i+1]].Value = rkBox.Max[0];
    m_kYEndPoint[m_kYLookup[2*i]].Value = rkBox.Min[1];
    m_kYEndPoint[m_kYLookup[2*i+1]].Value = rkBox.Max[1];
    m_kZEndPoint[m_kZLookup[2*i]].Value = rkBox.Min[2];
    m_kZEndPoint[m_kZLookup[2*i+1]].Value = rkBox.Max[2];
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingBoxes<Real>::GetBox (int i, AxisAlignedBox3<Real>& rkBox)
    const
{
    assert(0 <= i && i < (int)m_pkBoxes->size());
    rkBox = (*m_pkBoxes)[i];
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingBoxes<Real>::InsertionSort (
    std::vector<EndPoint>& rkEndPoint, std::vector<int>& rkLookup)
{
    // Apply an insertion sort.  Under the assumption that the rectangles
    // have not changed much since the last call, the end points are nearly
    // sorted.  The insertion sort should be very fast in this case.

    int iESize = (int)rkEndPoint.size();
    for (int j = 1; j < iESize; j++)
    {
        EndPoint kKey = rkEndPoint[j];
        int i = j - 1;
        while (i >= 0 && kKey < rkEndPoint[i])
        {
            EndPoint kE0 = rkEndPoint[i];
            EndPoint kE1 = rkEndPoint[i+1];

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
                    // real time savings in testing for existence first, then
                    // deleting if it does.
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
                    const AxisAlignedBox3<Real>& rkB0 = (*m_pkBoxes)[kE0.Index];
                    const AxisAlignedBox3<Real>& rkB1 = (*m_pkBoxes)[kE1.Index];
                    if (rkB0.TestIntersection(rkB1))
                    {
                        m_kOverlap.insert(EdgeKey(kE0.Index,kE1.Index));
                    }
                }
            }

            // Reorder the items to maintain the sorted list.
            rkEndPoint[i] = kE1;
            rkEndPoint[i+1] = kE0;
            rkLookup[2*kE1.Index + kE1.Type] = i;
            rkLookup[2*kE0.Index + kE0.Type] = i+1;
            i--;
        }
        rkEndPoint[i+1] = kKey;
        rkLookup[2*kKey.Index + kKey.Type] = i+1;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntersectingBoxes<Real>::Update ()
{
    InsertionSort(m_kXEndPoint,m_kXLookup);
    InsertionSort(m_kYEndPoint,m_kYLookup);
    InsertionSort(m_kZEndPoint,m_kZLookup);
}
//----------------------------------------------------------------------------
template <class Real>
const std::set<EdgeKey>& IntersectingBoxes<Real>::GetOverlap () const
{
    return m_kOverlap;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_PHYSICS_ITEM
class IntersectingBoxes<float>;

template WM4_PHYSICS_ITEM
class IntersectingBoxes<double>;
//----------------------------------------------------------------------------
}
