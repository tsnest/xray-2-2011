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
// Version: 4.0.1 (2008/01/20)

#include "Wm4GraphicsPCH.h"
#include "Wm4VisibleSet.h"
using namespace Wm4;

//----------------------------------------------------------------------------
VisibleSet::VisibleSet (int iMaxQuantity, int iGrowBy)
{
    m_akVisible = 0;
    Resize(iMaxQuantity,iGrowBy);
}
//----------------------------------------------------------------------------
VisibleSet::~VisibleSet ()
{
    WM4_DELETE[] m_akVisible;
}
//----------------------------------------------------------------------------
void VisibleSet::Insert (Spatial* pkObject, Effect* pkGlobalEffect)
{
    // Increase the array storage if necessary.
    if (m_iQuantity >= m_iMaxQuantity)
    {
        int iNewMaxQuantity = m_iMaxQuantity + m_iGrowBy;
        VisibleObject* akNewVisible = WM4_NEW VisibleObject[iNewMaxQuantity];
        size_t uiSize = m_iQuantity*sizeof(VisibleObject);
        System::Memcpy(akNewVisible,uiSize,m_akVisible,uiSize);
        WM4_DELETE[] m_akVisible;
        m_akVisible = akNewVisible;
        m_iMaxQuantity = iNewMaxQuantity;
    }

    // Append the potentially visible object to the set.
    int iIndex = m_iQuantity++;
    m_akVisible[iIndex].Object = pkObject;
    m_akVisible[iIndex].GlobalEffect = pkGlobalEffect;
}
//----------------------------------------------------------------------------
void VisibleSet::Resize (int iMaxQuantity, int iGrowBy)
{
    if (iMaxQuantity > 0)
    {
        m_iMaxQuantity = iMaxQuantity;
    }
    else
    {
        m_iMaxQuantity = VS_DEFAULT_MAX_QUANTITY;
    }

    if (iGrowBy > 0)
    {
        m_iGrowBy = iGrowBy;
    }
    else
    {
        m_iGrowBy = VS_DEFAULT_GROWBY;
    }

    WM4_DELETE[] m_akVisible;
    m_iQuantity = 0;
    m_akVisible = WM4_NEW VisibleObject[m_iMaxQuantity];
}
//----------------------------------------------------------------------------
