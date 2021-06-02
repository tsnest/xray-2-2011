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

#include "Edge2.h"

//----------------------------------------------------------------------------
Edge2::Edge2 ()
{
    i0 = -1;
    i1 = -1;
}
//----------------------------------------------------------------------------
Edge2::Edge2 (int j0, int j1)
{
    i0 = j0;
    i1 = j1;
}
//----------------------------------------------------------------------------
bool Edge2::operator< (const Edge2& rkE) const
{
    if (i1 < rkE.i1)
    {
        return true;
    }

    if (i1 == rkE.i1)
    {
        return i0 < rkE.i0;
    }

    return false;
}
//----------------------------------------------------------------------------
