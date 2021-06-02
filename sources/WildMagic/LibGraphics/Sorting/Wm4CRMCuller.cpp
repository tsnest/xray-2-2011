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

#include "Wm4GraphicsPCH.h"
#include "Wm4CRMCuller.h"
using namespace Wm4;

//----------------------------------------------------------------------------
CRMCuller::CRMCuller (int iMaxQuantity, int iGrowBy, const Camera* pkCamera)
    :
    Culler(iMaxQuantity,iGrowBy,pkCamera),
    m_kUnique(iMaxQuantity > 0 ? iMaxQuantity : 1024)
{
}
//----------------------------------------------------------------------------
CRMCuller::~CRMCuller ()
{
}
//----------------------------------------------------------------------------
void CRMCuller::Insert (Spatial* pkObject, Effect* pkGlobalEffect)
{
    if (pkObject)
    {
        if (&pkObject != m_kUnique.Insert(pkObject))
        {
            // The object was not in the list of unique objects.
            Culler::Insert(pkObject,pkGlobalEffect);
        }
    }
    else
    {
        // This is an end-sentinel for a global effect.  There can be
        // multiple copies of these.
        Culler::Insert(0,0);
    }
}
//----------------------------------------------------------------------------
