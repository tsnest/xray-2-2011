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
// Version: 4.0.1 (2006/09/21)

#ifndef WM4BINARY3D_H
#define WM4BINARY3D_H

#include "Wm4ImagicsLIB.h"
#include "Wm4Images.h"

namespace Wm4
{

class Binary3D : public ImageInt3D
{
public:
    // Construction and destruction.  Binary3D accepts responsibility for
    // deleting the input data array.
    WM4_IMAGICS_ITEM Binary3D (int iXBound, int iYBound, int iZBound,
        Eint* atData = 0);
    WM4_IMAGICS_ITEM Binary3D (const Binary3D& rkImage);
    WM4_IMAGICS_ITEM Binary3D (const char* acFilename);

    // Compute the connected components of a binary image.  The components in
    // the returned image are labeled with positive integer values.  If the
    // image is identically zero, then the components image is identically
    // zero and the returned quantity is zero.
    WM4_IMAGICS_ITEM void GetComponents (int& riQuantity,
        ImageInt3D& rkComponents) const;

protected:
    // helper for component labeling
    void AddToAssociative (int i0, int i1, int* aiAssoc) const;
};

}

#endif
