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

DWORD Dx9Renderer::ms_adwStencilCompare[StencilState::CF_QUANTITY] = 
{
    D3DCMP_NEVER,           // StencilState::CF_NEVER
    D3DCMP_LESS,            // StencilState::CF_LESS
    D3DCMP_EQUAL,           // StencilState::CF_EQUAL
    D3DCMP_LESSEQUAL,       // StencilState::CF_LEQUAL
    D3DCMP_GREATER,         // StencilState::CF_GREATER
    D3DCMP_NOTEQUAL,        // StencilState::CF_NOTEQUAL
    D3DCMP_GREATEREQUAL,    // StencilState::CF_GEQUAL
    D3DCMP_ALWAYS,          // StencilState::CF_ALWAYS
};

DWORD Dx9Renderer::ms_adwStencilOperation[StencilState::OT_QUANTITY] =
{
    D3DSTENCILOP_KEEP,      // StencilState::OT_KEEP
    D3DSTENCILOP_ZERO,      // StencilState::OT_ZERO
    D3DSTENCILOP_REPLACE,   // StencilState::OT_REPLACE
    D3DSTENCILOP_INCR,      // StencilState::OT_INCREMENT
    D3DSTENCILOP_DECR,      // StencilState::OT_DECREMENT
    D3DSTENCILOP_INVERT     // StencilState::OT_INVERT
};

//----------------------------------------------------------------------------
void Dx9Renderer::SetStencilState (StencilState* pkState)
{
    Renderer::SetStencilState(pkState);

    if (pkState->Enabled)
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,TRUE);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFUNC,
            ms_adwStencilCompare[pkState->Compare]);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILREF,
            (DWORD)pkState->Reference);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILMASK,
            (DWORD)pkState->Mask);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILWRITEMASK,
            (DWORD)pkState->WriteMask);

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILFAIL,
            ms_adwStencilOperation[pkState->OnFail]);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILZFAIL,
            ms_adwStencilOperation[pkState->OnZFail]);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILPASS,
            ms_adwStencilOperation[pkState->OnZPass]);
        assert(SUCCEEDED(ms_hResult));
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_STENCILENABLE,FALSE);
        assert(SUCCEEDED(ms_hResult));
    }
}
//----------------------------------------------------------------------------
