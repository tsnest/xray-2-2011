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

#include "Wm4Dx9RendererPCH.h"
#include "Wm4Dx9Renderer.h"
using namespace Wm4;

DWORD Dx9Renderer::ms_adwCullType[CullState::FT_QUANTITY] = 
{
    D3DCULL_CCW,  // CullState::FT_CCW (front CCW -> cull backface CCW in DX)
    D3DCULL_CW,   // CullState::FT_CW  (front CW -> cull backface CW in DX)
};

//----------------------------------------------------------------------------
void Dx9Renderer::SetCullState (CullState* pkState)
{
    Renderer::SetCullState(pkState);

    if (pkState->Enabled)
    {
        if (m_bReverseCullFace)
        {
            if (ms_adwCullType[pkState->CullFace] == D3DCULL_CW)
            {
                ms_hResult = m_pqDevice->SetRenderState(D3DRS_CULLMODE,
                    D3DCULL_CCW);
                assert(SUCCEEDED(ms_hResult));
            }
            else
            {
                ms_hResult = m_pqDevice->SetRenderState(D3DRS_CULLMODE,
                    D3DCULL_CW);
                assert(SUCCEEDED(ms_hResult));
            }
        } 
        else
        {
            ms_hResult = m_pqDevice->SetRenderState(D3DRS_CULLMODE,
                ms_adwCullType[pkState->CullFace]);
            assert(SUCCEEDED(ms_hResult));
        }
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
        assert(SUCCEEDED(ms_hResult));
    }
}
//----------------------------------------------------------------------------
