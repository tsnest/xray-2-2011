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
// Version: 4.0.1 (2007/05/06)

#include "Wm4Dx9RendererPCH.h"
#include "Wm4Dx9Renderer.h"
using namespace Wm4;

DWORD Dx9Renderer::ms_adwAlphaSrcBlend[AlphaState::SBF_QUANTITY] =
{
    D3DBLEND_ZERO,          // AlphaState::SBF_ZERO
    D3DBLEND_ONE,           // AlphaState::SBF_ONE
    D3DBLEND_DESTCOLOR,     // AlphaState::SBF_DST_COLOR
    D3DBLEND_INVDESTCOLOR,  // AlphaState::SBF_ONE_MINUS_DST_COLOR
    D3DBLEND_SRCALPHA,      // AlphaState::SBF_SRC_ALPHA
    D3DBLEND_INVSRCALPHA,   // AlphaState::SBF_ONE_MINUS_SRC_ALPHA
    D3DBLEND_DESTALPHA,     // AlphaState::SBF_DST_ALPHA
    D3DBLEND_INVDESTALPHA,  // AlphaState::SBF_ONE_MINUS_DST_ALPHA
    D3DBLEND_SRCALPHASAT,   // AlphaState::SBF_SRC_ALPHA_SATURATE
    0,                      // AlphaState::SBF_CONSTANT_COLOR
    0,                      // AlphaState::SBF_ONE_MINUS_CONSTANT_COLOR
    0,                      // AlphaState::SBF_CONSTANT_ALPHA
    0                       // AlphaState::SBF_ONE_MINUS_CONSTANT_ALPHA
};

DWORD Dx9Renderer::ms_adwAlphaDstBlend[AlphaState::DBF_QUANTITY] =
{
    D3DBLEND_ZERO,          // AlphaState::DBF_ZERO
    D3DBLEND_ONE,           // AlphaState::DBF_ONE
    D3DBLEND_SRCCOLOR,      // AlphaState::DBF_SRC_COLOR
    D3DBLEND_INVSRCCOLOR,   // AlphaState::DBF_ONE_MINUS_SRC_COLOR
    D3DBLEND_SRCALPHA,      // AlphaState::DBF_SRC_ALPHA
    D3DBLEND_INVSRCALPHA,   // AlphaState::DBF_ONE_MINUS_SRC_ALPHA
    D3DBLEND_DESTALPHA,     // AlphaState::DBF_DST_ALPHA
    D3DBLEND_INVDESTALPHA,  // AlphaState::DBF_ONE_MINUS_DST_ALPHA
    0,                      // AlphaState::DBF_CONSTANT_COLOR
    0,                      // AlphaState::DBF_ONE_MINUS_CONSTANT_COLOR
    0,                      // AlphaState::DBF_CONSTANT_ALPHA
    0                       // AlphaState::DBF_ONE_MINUS_CONSTANT_ALPHA
};

DWORD Dx9Renderer::ms_adwAlphaTest[AlphaState::TF_QUANTITY] =
{
    D3DCMP_NEVER,           // AlphaState::TF_NEVER
    D3DCMP_LESS,            // AlphaState::TF_LESS
    D3DCMP_EQUAL,           // AlphaState::TF_EQUAL
    D3DCMP_LESSEQUAL,       // AlphaState::TF_LEQUAL
    D3DCMP_GREATER,         // AlphaState::TF_GREATER
    D3DCMP_NOTEQUAL,        // AlphaState::TF_NOTEQUAL
    D3DCMP_GREATEREQUAL,    // AlphaState::TF_GEQUAL
    D3DCMP_ALWAYS           // AlphaState::TF_ALWAYS
};

//----------------------------------------------------------------------------
void Dx9Renderer::SetAlphaState (AlphaState* pkState)
{
    Renderer::SetAlphaState(pkState);

    if (pkState->BlendEnabled)
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
        assert(SUCCEEDED(ms_hResult));

        DWORD dwSrcBlend = ms_adwAlphaSrcBlend[pkState->SrcBlend];
        DWORD dwSrcConstantColor = 0;
        if (dwSrcBlend > 0)
        {
            dwSrcConstantColor = 0;
        }
        else  // CONSTANT format
        {
            switch (pkState->SrcBlend)
            {
            case AlphaState::SBF_CONSTANT_COLOR:
                dwSrcBlend = D3DBLEND_BLENDFACTOR;
                dwSrcConstantColor = D3DCOLOR_COLORVALUE(
                    pkState->ConstantColor.R(),pkState->ConstantColor.G(),
                    pkState->ConstantColor.B(),pkState->ConstantColor.A());
                break;
            case AlphaState::SBF_ONE_MINUS_CONSTANT_COLOR:
                dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
                dwSrcConstantColor = D3DCOLOR_COLORVALUE(
                    pkState->ConstantColor.R(),pkState->ConstantColor.G(),
                    pkState->ConstantColor.B(),pkState->ConstantColor.A());
                break;
            case AlphaState::SBF_CONSTANT_ALPHA:
                dwSrcBlend = D3DBLEND_BLENDFACTOR;
                dwSrcConstantColor = D3DCOLOR_COLORVALUE(
                    pkState->ConstantColor.A(),pkState->ConstantColor.A(),
                    pkState->ConstantColor.A(),pkState->ConstantColor.A());
                break;
            case AlphaState::SBF_ONE_MINUS_CONSTANT_ALPHA:
                dwSrcBlend = D3DBLEND_INVBLENDFACTOR;
                dwSrcConstantColor = D3DCOLOR_COLORVALUE(
                    pkState->ConstantColor.A(),pkState->ConstantColor.A(),
                    pkState->ConstantColor.A(),pkState->ConstantColor.A());
                break;
            }
        }

        DWORD dwDstBlend = ms_adwAlphaDstBlend[pkState->DstBlend];
        DWORD dwDstConstantColor = 0;
        if (dwDstBlend > 0)
        {
            dwDstConstantColor = 0;
        }
        else  // CONSTANT format
        {
            switch (pkState->DstBlend)
            {
            case AlphaState::DBF_CONSTANT_COLOR:
                dwDstBlend = D3DBLEND_BLENDFACTOR;
                dwDstConstantColor = D3DCOLOR_COLORVALUE(
                    pkState->ConstantColor.R(),pkState->ConstantColor.G(),
                    pkState->ConstantColor.B(),pkState->ConstantColor.A());
                break;
            case AlphaState::DBF_ONE_MINUS_CONSTANT_COLOR:
                dwDstBlend = D3DBLEND_INVBLENDFACTOR;
                dwDstConstantColor = D3DCOLOR_COLORVALUE(
                    pkState->ConstantColor.R(),pkState->ConstantColor.G(),
                    pkState->ConstantColor.B(),pkState->ConstantColor.A());
                break;
            case AlphaState::DBF_CONSTANT_ALPHA:
                dwDstBlend = D3DBLEND_BLENDFACTOR;
                dwDstConstantColor = D3DCOLOR_COLORVALUE(
                    pkState->ConstantColor.A(),pkState->ConstantColor.A(),
                    pkState->ConstantColor.A(),pkState->ConstantColor.A());
                break;
            case AlphaState::DBF_ONE_MINUS_CONSTANT_ALPHA:
                dwDstBlend = D3DBLEND_INVBLENDFACTOR;
                dwDstConstantColor = D3DCOLOR_COLORVALUE(
                    pkState->ConstantColor.A(),pkState->ConstantColor.A(),
                    pkState->ConstantColor.A(),pkState->ConstantColor.A());
                break;
            }

        }

        // DirectX appears not to support the OpenGL modes GL_CONSTANT_ALPHA
        // and GL_ONE_MINUS_CONSTANT_ALPHA.  In these cases, the constant
        // color is being set explicitly to the alpha values.  If the source
        // uses constant color (constant alpha) and the destination uses
        // constant alpha (constant color), then the blending results will be
        // incorrect.
#ifdef _DEBUG
        if (dwSrcBlend == D3DBLEND_BLENDFACTOR
        ||  dwSrcBlend == D3DBLEND_INVBLENDFACTOR)
        {
            if (dwDstBlend == D3DBLEND_BLENDFACTOR
            ||  dwDstBlend == D3DBLEND_INVBLENDFACTOR)
            {
                assert(dwSrcConstantColor == dwDstConstantColor);
            }
        }
#endif

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_SRCBLEND,dwSrcBlend);
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_DESTBLEND,dwDstBlend);
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_BLENDFACTOR,
            dwSrcConstantColor);
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
        assert(SUCCEEDED(ms_hResult));
    }

    if (pkState->TestEnabled)
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHAFUNC,
            ms_adwAlphaTest[pkState->Test]);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHAREF,
            (DWORD)(255.0f*pkState->Reference));
        assert(SUCCEEDED(ms_hResult));
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
        assert(SUCCEEDED(ms_hResult));
    }
}
//----------------------------------------------------------------------------
