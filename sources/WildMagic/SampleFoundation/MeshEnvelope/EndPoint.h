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

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "Rational.h"

class EndPoint
{
public:
    Rational Value;  // x-value for a segment endpoint
    int Type;        // '0' if interval min, '1' if interval max
    int Index;       // index of interval containing this endpoint

    // support for sorting
    bool operator< (const EndPoint& rkPoint) const
    {
        if (Value < rkPoint.Value)
        {
            return true;
        }
        if (Value > rkPoint.Value)
        {
            return false;
        }
        return Type < rkPoint.Type;
    }
};

#endif
