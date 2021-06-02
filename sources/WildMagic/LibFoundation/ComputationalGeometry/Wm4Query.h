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
// Version: 4.0.1 (2009/01/01)

#ifndef WM4QUERY_H
#define WM4QUERY_H

#include "Wm4FoundationLIB.h"

namespace Wm4
{

class Query
{
public:
    // abstract base class
    virtual ~Query ();

    // run-time type information
    enum Type
    {
        QT_INT64,
        QT_INTEGER,
        QT_RATIONAL,
        QT_REAL,
        QT_FILTERED
    };

    virtual Type GetType () const = 0;

    // Support for ordering a set of unique indices into the vertex pool.  On
    // output it is guaranteed that:  v0 < v1 < v2.  This is used to guarantee
    // consistent queries when the vertex ordering of a primitive is permuted,
    // a necessity when using floating-point arithmetic that suffers from
    // numerical round-off errors.  The input indices are considered the
    // positive ordering.  The output indices are either positively ordered
    // (an even number of transpositions occurs during sorting) or negatively
    // ordered (an odd number of transpositions occurs during sorting).  The
    // functions return 'true' for a positive ordering and 'false' for a
    // negative ordering.
    static bool Sort (int& iV0, int& iV1);
    static bool Sort (int& iV0, int& iV1, int& iV2);
    static bool Sort (int& iV0, int& iV1, int& iV2, int& iV3);

protected:
    Query ();
};

}

#endif
