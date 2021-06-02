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

#ifndef WM4VISIBLESET_H
#define WM4VISIBLESET_H

#include "Wm4GraphicsLIB.h"
#include "Wm4VisibleObject.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM VisibleSet
{
public:
    VisibleSet (int iMaxQuantity = 0, int iGrowBy = 0);
    virtual ~VisibleSet ();

    int GetQuantity () const;
    VisibleObject* GetVisible ();
    VisibleObject& GetVisible (int i);

    // Creates a VisibleObject from the input and appends it to the end of the
    // VisibleObject array.
    void Insert (Spatial* pkObject, Effect* pkGlobalEffect);
    void Clear ();
    void Resize (int iMaxQuantity, int iGrowBy);

private:
    enum
    {
        VS_DEFAULT_MAX_QUANTITY = 32,
        VS_DEFAULT_GROWBY = 32,
    };

    int m_iMaxQuantity, m_iGrowBy, m_iQuantity;
    VisibleObject* m_akVisible;
};

#include "Wm4VisibleSet.inl"

}

#endif
