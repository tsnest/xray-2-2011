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

#ifndef WM4CRMCULLER_H
#define WM4CRMCULLER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Culler.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM CRMCuller : public Culler
{
public:
    CRMCuller (int iMaxQuantity = 0, int iGrowBy = 0,
        const Camera* pkCamera = 0);
    virtual ~CRMCuller ();

    // Two portals leading into the room can be visible to the camera.  The
    // Culler would store visible objects twice.  CRMCuller maintains a list
    // of unique objects.
    virtual void Insert (Spatial* pkObject, Effect* pkGlobalEffect);

protected:
    THashSet<Spatial*> m_kUnique;
};

}

#endif
