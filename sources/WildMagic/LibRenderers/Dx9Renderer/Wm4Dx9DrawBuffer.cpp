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
void Dx9Renderer::Draw (const unsigned char* aucBuffer)
{
    if (!aucBuffer)
    {
        return;
    }

    IDirect3DSurface9* pkBackBuffer = 0;
    ms_hResult = m_pqDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,
        &pkBackBuffer);
    assert( pkBackBuffer );
    assert(SUCCEEDED(ms_hResult));
    if (FAILED(ms_hResult) || !pkBackBuffer)
    {
        return;
    }

    RECT kSrcRect = { 0, 0, m_iWidth-1, m_iHeight-1 };
    ms_hResult = D3DXLoadSurfaceFromMemory(pkBackBuffer,0,0,aucBuffer,
        D3DFMT_R8G8B8,3*m_iWidth,0,&kSrcRect,D3DX_FILTER_NONE,0);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
