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
#include "Wm4ClodTerrainVertex.h"
using namespace Wm4;

//----------------------------------------------------------------------------
ClodTerrainVertex::ClodTerrainVertex ()
{
    m_akDependent[0] = 0;
    m_akDependent[1] = 0;
    m_bEnabled = false;
}
//----------------------------------------------------------------------------
void ClodTerrainVertex::Enable ()
{
    m_bEnabled = true;

    if (m_akDependent[0])
    {
        if (!m_akDependent[0]->m_bEnabled)
        {
            m_akDependent[0]->Enable();
        }
    }
    
    if (m_akDependent[1])
    {
        if (!m_akDependent[1]->m_bEnabled)
        {
            m_akDependent[1]->Enable();
        }
    }
}
//----------------------------------------------------------------------------
void ClodTerrainVertex::Disable ()
{
    m_bEnabled = false;
    
    if (m_akDependent[0])
    {
        if (m_akDependent[0]->m_bEnabled)
        {
            m_akDependent[0]->Disable();
        }
    }

    if (m_akDependent[1])
    {
        if (m_akDependent[1]->m_bEnabled)
        {
            m_akDependent[1]->Disable();
        }
    }
}
//----------------------------------------------------------------------------
