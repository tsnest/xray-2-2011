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
// Version: 4.0.1 (2007/09/08)

#include "Wm4GraphicsPCH.h"
#include "Wm4Bindable.h"
#include "Wm4Renderer.h"
using namespace Wm4;

//----------------------------------------------------------------------------
Bindable::Bindable ()
{
}
//----------------------------------------------------------------------------
Bindable::~Bindable ()
{
}
//----------------------------------------------------------------------------
int Bindable::GetInfoQuantity () const
{
    return (int)m_kInfoArray.size();
}
//----------------------------------------------------------------------------
int Bindable::GetInfoQuantity (Renderer* pkUser) const
{
    int iQuantity = 0;
    for (int i = 0; i < (int)m_kInfoArray.size(); i++)
    {
        const Info& rkInfo = m_kInfoArray[i];
        if (rkInfo.User == pkUser)
        {
            iQuantity++;
        }
    }
    return iQuantity;
}
//----------------------------------------------------------------------------
ResourceIdentifier* Bindable::GetIdentifier (Renderer* pkUser) const
{
    for (int i = 0; i < (int)m_kInfoArray.size(); i++)
    {
        const Info& rkInfo = m_kInfoArray[i];
        if (rkInfo.User == pkUser)
        {
            return rkInfo.ID;
        }
    }

    // The resource is not yet bound to the renderer.
    return 0;
}
//----------------------------------------------------------------------------
ResourceIdentifier* Bindable::GetIdentifier (int i, Renderer* pkUser) const
{
    if (0 <= i && i < (int)m_kInfoArray.size())
    {
        const Info& rkInfo = m_kInfoArray[i];
        if (rkInfo.User == pkUser)
        {
            return rkInfo.ID;
        }
    }

    // The resource is not yet bound to the renderer.
    return 0;
}
//----------------------------------------------------------------------------
void Bindable::Release ()
{
    while (m_kInfoArray.size() > 0)
    {
        Info& rkInfo = m_kInfoArray[0];
        (rkInfo.User->*rkInfo.Release)(this);
    }
}
//----------------------------------------------------------------------------
void Bindable::OnLoad (Renderer* pkUser, Renderer::ReleaseFunction oRelease,
    ResourceIdentifier* pkID)
{
    Info kInfo;
    kInfo.User = pkUser;
    kInfo.Release = oRelease;
    kInfo.ID = pkID;
    m_kInfoArray.push_back(kInfo);
}
//----------------------------------------------------------------------------
void Bindable::OnRelease (Renderer* pkUser, ResourceIdentifier* pkID)
{
    int iQuantity = (int)m_kInfoArray.size();
    for (int i = 0; i < iQuantity; i++)
    {
        const Info& rkInfo = m_kInfoArray[i];
        if (rkInfo.User == pkUser && rkInfo.ID == pkID)
        {
            // Move the last array element to the current slot, if necessary.
            if (i < --iQuantity)
            {
                m_kInfoArray[i] = m_kInfoArray[iQuantity];
            }

            // Remove the last array element.
            m_kInfoArray.pop_back();
            return;
        }
    }
}
//----------------------------------------------------------------------------
