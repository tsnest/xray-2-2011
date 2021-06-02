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
// Version: 4.7.0 (2008/09/15)

#ifndef WM4MTITRIANGLE_H
#define WM4MTITRIANGLE_H

#include "Wm4System.h"

namespace Wm4
{

class MTITriangle
{
public:
    MTITriangle (int iLabel0, int iLabel1, int iLabel2);
    bool operator< (const MTITriangle& rkT) const;
    bool operator== (const MTITriangle& rkT) const;
    bool operator!= (const MTITriangle& rkT) const;

    int GetLabel (int i) const;

protected:
    int m_aiLabel[3];
};

}

#endif
