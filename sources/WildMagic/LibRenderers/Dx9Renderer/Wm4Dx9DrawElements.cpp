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
// Version: 4.0.5 (2008/10/11)

#include "Wm4Dx9RendererPCH.h"
#include "Wm4Dx9Renderer.h"
#include "Wm4Polyline.h"
#include "Wm4Polypoint.h"
#include "Wm4TriStrip.h"
#include "Wm4TriFan.h"
using namespace Wm4;

D3DPRIMITIVETYPE Dx9Renderer::ms_aeObjectType[Geometry::GT_MAX_QUANTITY] =
{
    D3DPT_POINTLIST,      // GT_POLYPOINT
    D3DPT_LINELIST,       // GT_POLYLINE_SEGMENTS
    D3DPT_LINESTRIP,      // GT_POLYLINE_OPEN
    D3DPT_LINESTRIP,      // GT_POLYLINE_CLOSED
    D3DPT_TRIANGLELIST,   // GT_TRIMESH
    D3DPT_TRIANGLESTRIP,  // GT_TRISTRIP
    D3DPT_TRIANGLEFAN     // GT_TRIFAN
};

//----------------------------------------------------------------------------
void Dx9Renderer::DrawElements ()
{
    // Get the correct active and primitive quantities.
    int iPQuantity, iAQuantity;
    switch (m_pkGeometry->Type)
    {
    case Geometry::GT_TRIMESH:
        iAQuantity = m_pkGeometry->VBuffer->GetVertexQuantity();
        iPQuantity = m_pkGeometry->IBuffer->GetIndexQuantity()/3;
        break;
    case Geometry::GT_TRISTRIP:
        iAQuantity = m_pkGeometry->VBuffer->GetVertexQuantity();
        iPQuantity =
            StaticCast<TriStrip>(m_pkGeometry)->GetTriangleQuantity();
        break;
    case Geometry::GT_TRIFAN:
        iAQuantity = m_pkGeometry->VBuffer->GetVertexQuantity();
        iPQuantity = StaticCast<TriFan>(m_pkGeometry)->GetTriangleQuantity();
        break;
    case Geometry::GT_POLYLINE_OPEN:
        iAQuantity = StaticCast<Polyline>(m_pkGeometry)->GetActiveQuantity();
        iPQuantity = iAQuantity - 1;
        break;
    case Geometry::GT_POLYLINE_CLOSED:
        iAQuantity = StaticCast<Polyline>(m_pkGeometry)->GetActiveQuantity();
        iPQuantity = iAQuantity;
        break;
    case Geometry::GT_POLYLINE_SEGMENTS:
        iAQuantity = StaticCast<Polyline>(m_pkGeometry)->GetActiveQuantity();
        iPQuantity = iAQuantity / 2;
        break;
    case Geometry::GT_POLYPOINT:
        iPQuantity = StaticCast<Polypoint>(m_pkGeometry)->GetActiveQuantity();
        ms_hResult = m_pqDevice->DrawPrimitive(D3DPT_POINTLIST,0,iPQuantity);
        assert(SUCCEEDED(ms_hResult));
        return;
    default:
        assert(false);
        return;
    }

    int iOffset = m_pkGeometry->IBuffer->GetOffset();
    ms_hResult = m_pqDevice->DrawIndexedPrimitive(
        ms_aeObjectType[m_pkGeometry->Type],0,0,iAQuantity,iOffset,
        iPQuantity);
    assert(SUCCEEDED(ms_hResult));
}
//----------------------------------------------------------------------------
