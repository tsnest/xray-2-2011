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

//----------------------------------------------------------------------------
void Dx9Renderer::SetPolygonOffsetState (PolygonOffsetState* pkState)
{
    // The LineEnabled and PointEnabled members are ignored by the DX9
    // renderer since DX9 does not support polygon offset for those
    // primitives.

    Renderer::SetPolygonOffsetState(pkState);

    if (pkState->FillEnabled)
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,
            *((DWORD*)&pkState->Scale));
        assert(SUCCEEDED(ms_hResult));

        // TO DO.  The renderer currently always creates a 24-bit depth
        // buffer.  If the precision changes, the adjustment to depth bias
        // must depend on the bits of precision.  The divisor is 2^n for n
        // bits of precision.
        float fBias = pkState->Bias/16777216.0f;
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_DEPTHBIAS,
            *((DWORD*)&fBias));
        assert(SUCCEEDED(ms_hResult));
    }
    else
    {
        ms_hResult = m_pqDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,0);
        assert(SUCCEEDED(ms_hResult));

        ms_hResult = m_pqDevice->SetRenderState(D3DRS_DEPTHBIAS,0);
        assert(SUCCEEDED(ms_hResult));
    }
}
//----------------------------------------------------------------------------
