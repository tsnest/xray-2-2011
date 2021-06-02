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

#ifndef WM4IVECTOR2_H
#define WM4IVECTOR2_H

#include "Wm4FoundationLIB.h"
#include "Wm4TIVector.h"

namespace Wm4
{

class IVector2 : public TIVector<2>
{
public:
    // construction
    IVector2 ();
    IVector2 (const IVector2& rkV);
    IVector2 (const TIVector<2>& rkV);
    IVector2 (const Integer64& riX, const Integer64& riY);

    // member access
    Integer64 X () const;
    Integer64& X ();
    Integer64 Y () const;
    Integer64& Y ();

    // assignment
    IVector2& operator= (const IVector2& rkV);
    IVector2& operator= (const TIVector<2>& rkV);

    // returns Dot(this,V)
    Integer64 Dot (const IVector2& rkV) const;

    // returns (y,-x)
    IVector2 Perp () const;

    // returns Cross((x,y,0),(V.x,V.y,0)) = x*V.y - y*V.x
    Integer64 DotPerp (const IVector2& rkV) const;

protected:
    using TIVector<2>::m_aiTuple;
};

#include "Wm4IVector2.inl"

}

#endif
