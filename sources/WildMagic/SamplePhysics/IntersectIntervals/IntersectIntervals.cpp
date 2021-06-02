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

#include "IntersectIntervals.h"
using namespace Wm4;

WM4_CONSOLE_APPLICATION(IntersectIntervals);

//----------------------------------------------------------------------------
IntersectIntervals::IntersectIntervals ()
{
}
//----------------------------------------------------------------------------
void IntersectIntervals::BookExample ()
{
    // The initial intervals that lead to an ordering as shown in Figure 5.29
    // of the Game Physics book.
    std::vector<Vector2f> kInterval(5);
    kInterval[0][0] = 2.0f;  kInterval[0][1] = 7.0f;
    kInterval[1][0] = 3.0f;  kInterval[1][1] = 13.0f;
    kInterval[2][0] = 1.0f;  kInterval[2][1] = 5.0f;
    kInterval[3][0] = 10.0f; kInterval[3][1] = 14.0f;
    kInterval[4][0] = 8.0f;  kInterval[4][1] = 11.0f;

    // Compute all pairs of overlapping intervals.  In the implementation,
    // the intervals are labeled using zero-based indexing.  The cout
    // commands add one to each index to obtain the one-based indexing that
    // is used in the book.
    //
    // S = {(1,2), (1,3), (2,3), (2,4), (2,5), (4,5) }

    IntersectingIntervalsf kIntr(kInterval);
    const std::set<EdgeKey>& rkOverlap = kIntr.GetOverlap();
    std::set<EdgeKey>::const_iterator pkIter;
    for (pkIter = rkOverlap.begin(); pkIter != rkOverlap.end(); pkIter++)
    {
        std::cout
            << "interval " 
            << 1+pkIter->V[0]
            << " overlaps with interval "
            << 1+pkIter->V[1]
            << std::endl;
    }
    std::cout << std::endl;

    // Move intervals 1 and 4 (using the one-based book indexing) and
    // update the set of overlapping intervals.  The new ordering is now
    // of the form shown in Figure 5.30 of the Game Physics book.
    //
    // S = {(1,2), (1,3), (1,5), (2,3), (2,4), (2,5) }

    kIntr.SetInterval(0,3.5f,8.5f);
    kIntr.SetInterval(4,6.0f,9.0f);
    kIntr.Update();
    for (pkIter = rkOverlap.begin(); pkIter != rkOverlap.end(); pkIter++)
    {
        std::cout
            << "interval "
            << 1+pkIter->V[0]
            << " overlaps with interval "
            << 1+pkIter->V[1]
            << std::endl;
    }
    std::cout << std::endl;
}
//----------------------------------------------------------------------------
int IntersectIntervals::Main (int, char**)
{
    BookExample();
    return 0;
}
//----------------------------------------------------------------------------
