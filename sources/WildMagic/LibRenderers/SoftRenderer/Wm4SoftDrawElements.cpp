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
// Version: 4.0.4 (2007/07/25)

#include "Wm4SoftRendererPCH.h"
#include "Wm4SoftRenderer.h"
#include "Wm4SoftResources.h"
#include "Wm4Geometry.h"
#include "Wm4Camera.h"
#include "Wm4VertexShader.h"
#include "Wm4PixelShader.h"
#include "Wm4RVector3.h"
using namespace Wm4;

//----------------------------------------------------------------------------
void SoftRenderer::PerspectiveInterpolate (const float* afAttr0,
    const float* afAttr1, int i0, int i1, float (*aafInterp)[MAX_CHANNELS])
    const
{
    float afCoeffA[MAX_CHANNELS], afCoeffB[MAX_CHANNELS];
    float fW0 = afAttr0[3], fW1 = afAttr1[3];
    float fInv;
    int i, j;

    // Yes, I intend this to be an exact inequality comparison between
    // floating-point numbers.  An orthographic camera will produce w values
    // of exactly 1, so the inequality works as expected in this case.
    if (fW0 != fW1)
    {

        // Use perspective interpolation.
        float fCoeffC = fW0 - fW1;
        float fCoeffD = fW1*i1 - fW0*i0;
        for (j = 0; j < m_iNumOutputChannels; j++)
        {
            float fW0Attr1 = fW0*afAttr1[j];
            float fW1Attr0 = fW1*afAttr0[j];
            afCoeffA[j] = fW0Attr1 - fW1Attr0;
            afCoeffB[j] = fW1Attr0*i1 - fW0Attr1*i0;
        }

        // Interpolate strictly between the end points.
        for (i = i0 + 1; i < i1; i++)
        {
            fInv = 1.0f/(fCoeffC*i + fCoeffD);
            for (j = 0; j < m_iNumOutputChannels; j++)
            {
                aafInterp[i][j] = (afCoeffA[j]*i + afCoeffB[j])*fInv;
            }
        }
    }
    else
    {
        // Use linear interpolation.
        fInv = 1.0f/(i1 - i0);
        for (j = 0; j < m_iNumOutputChannels; j++)
        {
            afCoeffA[j] = (afAttr1[j] - afAttr0[j])*fInv;
            afCoeffB[j] = (afAttr0[j]*i1 - afAttr1[j]*i0)*fInv;
        }

        // Interpolate strictly between the end points.
        for (i = i0 + 1; i < i1; i++)
        {
            for (j = 0; j < m_iNumOutputChannels; j++)
            {
                aafInterp[i][j] = afCoeffA[j]*i + afCoeffB[j];
            }
        }
    }
}
//----------------------------------------------------------------------------
float SoftRenderer::ComputePolygonOffset (int* aiX, int* aiY, float* afDepth)
{
    if (GetPolygonOffsetState()->FillEnabled)
    {
        int iX1mX0 = aiX[1] - aiX[0];
        int iY1mY0 = aiY[1] - aiY[0];
        int iX2mX0 = aiX[2] - aiX[0];
        int iY2mY0 = aiY[2] - aiY[0];
        int iTwoArea = iX1mX0*iY2mY0 - iX2mX0*iY1mY0;
        if (iTwoArea != 0)
        {
            float fZ1mZ0 = afDepth[1] - afDepth[0];
            float fZ2mZ0 = afDepth[2] - afDepth[0];
            float fADzDx = Mathf::FAbs(iY1mY0*fZ2mZ0 - iY2mY0*fZ1mZ0);
            float fADzDy = Mathf::FAbs(iX1mX0*fZ2mZ0 - iX2mX0*fZ1mZ0);
            float fMultiplier;
            if (fADzDx >= fADzDy)
            {
                fMultiplier = fADzDx/Mathf::FAbs((float)iTwoArea);
            }
            else
            {
                fMultiplier = fADzDy/Mathf::FAbs((float)iTwoArea);
            }

            float fDepthOffset = fMultiplier*m_fPolygonOffsetScale +
                m_fPolygonOffsetBias;

            return fDepthOffset;
        }
    }
    return 0.0f;
}
//----------------------------------------------------------------------------
void SoftRenderer::RasterizePoint (int iV0)
{
    // Disable mipmapping for points.
    for (int i = 0; i < (int)m_kPSampler.size(); i++)
    {
        m_kPSampler[i]->DisallowMipmapping();
    }

    // Get the output vertex from the vertex shader.
    const float* afVertex0 = GetOutputVertex(iV0);

    // Test for containment in the view frustum.

    // constraint:  W - 1 >= 0  (W >= 1)
    float fW = afVertex0[3];
    if (fW - 1.0f < 0.0f)
    {
        return;
    }

    // constraint:  F - N*W >= 0 (W <= F/N)
    float fDMin = m_pkCamera->GetDMin();
    float fDMax = m_pkCamera->GetDMax();
    if (fDMax - fDMin*fW < 0.0f)
    {
        return;
    }

    // constraint:  W + X >= 0
    float fX = afVertex0[0];
    if (fW + fX < 0.0f)
    {
        return;
    }

    // constraint:  W - X >= 0
    if (fW - fX < 0.0f)
    {
        return;
    }

    // constraint:  W + Y >= 0
    float fY = afVertex0[1];
    if (fW + fY < 0.0f)
    {
        return;
    }

    // constraint:  W - Y >= 0
    if (fW - fY < 0.0f)
    {
        return;
    }

    // Get the screen-space coordinates for the vertex.
    float fDepth, fInverseW;
    ClipToWindow(afVertex0,fX,fY,fDepth,fInverseW);
    int iX0 = WM4_FLOAT_TO_INT(fX);
    int iY0 = WM4_FLOAT_TO_INT(fY);
    if (iX0 >= m_iWidth)
    {
        iX0 = m_iWidth-1;
    }
    if (iY0 >= m_iHeight)
    {
        iY0 = m_iHeight-1;
    }

    // Draw the pixel.
    ApplyPixelShader(iX0,iY0,fDepth,fInverseW,&afVertex0[4]);
}
//----------------------------------------------------------------------------
void SoftRenderer::RasterizeEdge (int iV0, int iV1)
{
    // Disable mipmapping for polylines.
    for (int i = 0; i < (int)m_kPSampler.size(); i++)
    {
        m_kPSampler[i]->DisallowMipmapping();
    }

    // Get the output vertices from the vertex shader.
    const float* afVertex0 = GetOutputVertex(iV0);
    const float* afVertex1 = GetOutputVertex(iV1);

    // Get the screen-space coordinates for the vertices.
    float fX, fY, fDepth, fInverseW;
    ClipToWindow(afVertex0,fX,fY,fDepth,fInverseW);
    int iX0 = WM4_FLOAT_TO_INT(fX);
    int iY0 = WM4_FLOAT_TO_INT(fY);
    if (iX0 >= m_iWidth)
    {
        iX0 = m_iWidth-1;
    }
    if (iY0 >= m_iHeight)
    {
        iY0 = m_iHeight-1;
    }
    ClipToWindow(afVertex1,fX,fY,fDepth,fInverseW);
    int iX1 = WM4_FLOAT_TO_INT(fX);
    int iY1 = WM4_FLOAT_TO_INT(fY);
    if (iX1 >= m_iWidth)
    {
        iX1 = m_iWidth-1;
    }
    if (iY1 >= m_iHeight)
    {
        iY1 = m_iHeight-1;
    }

    int iX, iY, iSave;
    const float* afSave;

    int iDx = iX1 - iX0, iDy = iY1 - iY0;
    if (abs(iDx) >= abs(iDy))
    {
        // Iterate in the x direction.
        if (iDx < 0)
        {
            iSave = iX0;
            iX0 = iX1;
            iX1 = iSave;
            iSave = iY0;
            iY0 = iY1;
            iY1 = iSave;
            afSave = afVertex0;
            afVertex0 = afVertex1;
            afVertex1 = afSave;
            iDx = -iDx;
        }

        // Draw the starting pixel.
        ClipToWindow(afVertex0,fX,fY,fDepth,fInverseW);
        ApplyPixelShader(iX0,iY0,fDepth,fInverseW,&afVertex0[4]);

        if (iDx >= 2)
        {
            // Interpolate and draw the inter pixels on the edge.
            PerspectiveInterpolate(afVertex0,afVertex1,iX0,iX1,m_aafScanAttr);
            for (iX = iX0 + 1; iX < iX1; iX++)
            {
                ClipToWindow(m_aafScanAttr[iX],fX,fY,fDepth,fInverseW);
                iY = WM4_FLOAT_TO_INT(fY);
                ApplyPixelShader(iX,iY,fDepth,fInverseW,
                    &m_aafScanAttr[iX][4]);
            }
        }

        // Draw the ending pixel.
        if (iDx >= 1)
        {
            ClipToWindow(afVertex0,fX,fY,fDepth,fInverseW);
            ApplyPixelShader(iX1,iY1,fDepth,fInverseW,&afVertex0[4]);
        }
    }
    else
    {
        // Iterate in the y direction.
        if (iDy < 0)
        {
            iSave = iX0;
            iX0 = iX1;
            iX1 = iSave;
            iSave = iY0;
            iY0 = iY1;
            iY1 = iSave;
            afSave = afVertex0;
            afVertex0 = afVertex1;
            afVertex1 = afSave;
            iDy = -iDy;
        }

        // Draw the starting pixel.
        ClipToWindow(afVertex0,fX,fY,fDepth,fInverseW);
        ApplyPixelShader(iX0,iY0,fDepth,fInverseW,&afVertex0[4]);

        if (iDy >= 2)
        {
            // Interpolate and draw the interior pixels on the edge.
            PerspectiveInterpolate(afVertex0,afVertex1,iY0,iY1,m_aafScanAttr);
            for (iY = iY0 + 1; iY < iY1; iY++)
            {
                ClipToWindow(m_aafScanAttr[iY],fX,fY,fDepth,fInverseW);
                iX = WM4_FLOAT_TO_INT(fX);
                ApplyPixelShader(iX,iY,fDepth,fInverseW,
                    &m_aafScanAttr[iY][4]);
            }
        }

        if (iDy >= 1)
        {
            // Draw the starting pixel.
            ClipToWindow(afVertex0,fX,fY,fDepth,fInverseW);
            ApplyPixelShader(iX1,iY1,fDepth,fInverseW,&afVertex0[4]);
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::RasterizeTriangle (int iV0, int iV1, int iV2)
{
    // Get the output vertices from the vertex shader.
    const float* aafAttr[3];
    aafAttr[0] = GetOutputVertex(iV0);
    aafAttr[1] = GetOutputVertex(iV1);
    aafAttr[2] = GetOutputVertex(iV2);

    // Get the screen-space coordinates for the vertices.
    float afX[3], afY[3], afDepth[3], fInverseW;
    int aiX[3], aiY[3], iYMin, iYMax;
    ClipToWindow(aafAttr[0],afX[0],afY[0],afDepth[0],fInverseW);
    aiX[0] = WM4_FLOAT_TO_INT(afX[0]);
    aiY[0] = WM4_FLOAT_TO_INT(afY[0]);
    ClipToWindow(aafAttr[1],afX[1],afY[1],afDepth[1],fInverseW);
    aiX[1] = WM4_FLOAT_TO_INT(afX[1]);
    aiY[1] = WM4_FLOAT_TO_INT(afY[1]);
    ClipToWindow(aafAttr[2],afX[2],afY[2],afDepth[2],fInverseW);
    aiX[2] = WM4_FLOAT_TO_INT(afX[2]);
    aiY[2] = WM4_FLOAT_TO_INT(afY[2]);

    if (!ComputeEdgeBuffers(aiX,aiY,aafAttr,iYMin,iYMax))
    {
        // The triangle is degenerate.
        return;
    }

    // Adjust the depths when polygon offset state is active.
    float fDepthOffset = ComputePolygonOffset(aiX,aiY,afDepth);

    // The samplers need to compute texture-coordinate derivatives when
    // mipmapping is enabled.  The derivatives depend on the triangle
    // vertices and attributes.
    for (int i = 0; i < (int)m_kPSampler.size(); i++)
    {
        int iUnit = m_apkActiveSamplers[i]->GetTextureUnit();
        m_kPSampler[i]->ComputeMipmapParameters(aiX,aiY,aafAttr,
            m_pkVBResource->OAttr,iUnit);
    }

    for (int iY = iYMin; iY < iYMax; iY++)
    {
        // Get the edge buffer information for this scan line.
        const float* afXMinAttr = m_aafXMinAttr[iY];
        const float* afXMaxAttr = m_aafXMaxAttr[iY];
        int iXMin = m_aiXMin[iY];
        int iXMax = m_aiXMax[iY];
        int iDX = iXMax - iXMin;
        if (iDX == 0)
        {
            continue;
        }

        // Project the point at (iXMin,iY) and draw the pixel.
        assert(iXMin < m_iWidth);
        float fX, fY, fDepth;
        ClipToWindow(afXMinAttr,fX,fY,fDepth,fInverseW);
        if (fDepthOffset != 0.0f)
        {
            fDepth += fDepthOffset;
            if (fDepth < 0.0f)
            {
                fDepth = 0.0f;
            }
            else if (fDepth > 1.0f)
            {
                fDepth = 1.0f;
            }
        }
        ApplyPixelShader(iXMin,iY,fDepth,fInverseW,&afXMinAttr[4]);

        if (iDX >= 2)
        {
            // Draw the interior pixels on the scan line.
            PerspectiveInterpolate(afXMinAttr,afXMaxAttr,iXMin,iXMax,
                m_aafScanAttr);
            for (int iX = iXMin + 1; iX < iXMax; iX++)
            {
                assert(iX < m_iWidth);
                ClipToWindow(m_aafScanAttr[iX],fX,fY,fDepth,fInverseW);
                if (fDepthOffset != 0.0f)
                {
                    fDepth += fDepthOffset;
                    if (fDepth < 0.0f)
                    {
                        fDepth = 0.0f;
                    }
                    else if (fDepth > 1.0f)
                    {
                        fDepth = 1.0f;
                    }
                }
                ApplyPixelShader(iX,iY,fDepth,fInverseW,
                    &m_aafScanAttr[iX][4]);
            }
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::DrawElements ()
{
    ApplyVertexShader();
    (this->*m_aoDrawFunction[m_pkGeometry->Type])();
}
//----------------------------------------------------------------------------
void SoftRenderer::DrawPolypoint ()
{
    int iIQuantity = m_pkIBResource->IBuffer->GetIndexQuantity();
    int iOffset = m_pkIBResource->IBuffer->GetOffset();
    const int* piIndex = m_pkIBResource->IBuffer->GetData() + iOffset;
    for (int i = 0; i < iIQuantity; i++)
    {
        RasterizePoint(*piIndex++);
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::DrawPolylineDisjoint ()
{
    int iSQuantity = m_pkIBResource->IBuffer->GetIndexQuantity()/2;
    int iOffset = m_pkIBResource->IBuffer->GetOffset();
    const int* piIndex = m_pkIBResource->IBuffer->GetData() + iOffset;
    int iQuantity, aiEnd[2];
    for (int i = 0; i < iSQuantity; i++)
    {
        iQuantity = 2;
        aiEnd[0] = *piIndex++;
        aiEnd[1] = *piIndex++;
        ClipSegment(iQuantity,aiEnd);
        if (iQuantity == 2)
        {
            RasterizeEdge(aiEnd[0],aiEnd[1]);
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::DrawPolylineContiguous ()
{
    int iSQuantity = m_pkIBResource->IBuffer->GetIndexQuantity() - 1;
    int iOffset = m_pkIBResource->IBuffer->GetOffset();
    const int* aiIndex = m_pkIBResource->IBuffer->GetData() + iOffset;
    int iQuantity, aiEnd[2];
    for (int i = 0; i < iSQuantity; i++)
    {
        iQuantity = 2;
        aiEnd[0] = aiIndex[i];
        aiEnd[1] = aiIndex[i+1];
        ClipSegment(iQuantity,aiEnd);
        if (iQuantity == 2)
        {
            RasterizeEdge(aiEnd[0],aiEnd[1]);
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::DrawTriMesh ()
{
    // Process the triangles to discard back-facing ones, clipping and
    // drawing the front-facing ones.
    int iTQuantity = m_pkIBResource->IBuffer->GetIndexQuantity()/3;
    int iOffset = m_pkIBResource->IBuffer->GetOffset();
    const int* piIndex = m_pkIBResource->IBuffer->GetData() + iOffset;
    for (int i = 0; i < iTQuantity; i++)
    {
        // Get the triangle vertices.
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;

        DrawTriangle(iV0,iV1,iV2);
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::DrawTriStrip ()
{
    // Process the triangles to discard back-facing ones, clipping and
    // drawing the front-facing ones.
    int iTQuantity = m_pkIBResource->IBuffer->GetIndexQuantity() - 2;
    int iStart = m_pkIBResource->IBuffer->GetOffset();
    int iParity = ((iStart & 1) == 0 ? 1 : 0);
    const int* aiIndex = m_pkIBResource->IBuffer->GetData();
    for (int i = iStart; i < iTQuantity; i++)
    {
        // Get the triangle vertices.
        int iV0 = aiIndex[i];
        int iV1 = aiIndex[i+1];
        int iV2 = aiIndex[i+2];

        if ((i & iParity) == 0)
        {
            DrawTriangle(iV0,iV1,iV2);
        }
        else
        {
            DrawTriangle(iV0,iV2,iV1);
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::DrawTriFan ()
{
    // Process the triangles to discard back-facing ones, clipping and
    // drawing the front-facing ones.
    int iTQuantity = m_pkIBResource->IBuffer->GetIndexQuantity() - 2;
    int iStart = m_pkIBResource->IBuffer->GetOffset();
    const int* aiIndex = m_pkIBResource->IBuffer->GetData();
    int iV0 = aiIndex[iStart];
    for (int i = iStart; i < iTQuantity; i++)
    {
        // Get the triangle vertices.
        int iV1 = aiIndex[i+1];
        int iV2 = aiIndex[i+2];

        DrawTriangle(iV0,iV1,iV2);
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::DrawTriangle (int iV0, int iV1, int iV2)
{
    CullState* pkCullState = GetCullState();
    WireframeState* pkWireframeState = GetWireframeState();

    // Cull the triangles based on culling state.
    if (pkCullState->Enabled)
    {
        // Get the clip-space coordinates (r'[i],u'[i],d'[i],w'[i]).
        const float* afV0 = GetOutputVertex(iV0);
        const float* afV1 = GetOutputVertex(iV1);
        const float* afV2 = GetOutputVertex(iV2);

        // The visibility test is applied to clip-space coordinates
        // (r'[i],u'[i],w'[i]).  The d'[i] term cannot be used as the
        // third component since it contains a translation from the
        // projection matrix that maps view coordinates to clip
        // coordinates.  The normal vector is fine to compute with the
        // d'[i] term, but the dot product of the normal with one of the
        // vertices is not
        Vector3f kE1(afV1[0]-afV0[0],afV1[1]-afV0[1],afV1[3]-afV0[3]);
        Vector3f kE2(afV2[0]-afV0[0],afV2[1]-afV0[1],afV2[3]-afV0[3]);
        Vector3f kN = kE1.Cross(kE2);

        // NOTE:  The camera coordinates are E+r*R+u*U+d*D, but {R,U,D}
        // is a left-handed system.  The visibility test must be computed
        // accordingly.  The handedness is absorbed into the computation
        // of m_iCullSignAdjust.  The default sign adjustment is +1 for
        // counterclockwise triangles and back face culling.
        float fVisibility = m_iCullSignAdjust*(kN[0]*afV0[0] +
            kN[1]*afV0[1] + kN[2]*afV0[3]);
        if (fVisibility <= 0.0f)
        {
            // The triangle is potentially back facing, but an incorrect
            // classification can occur due to round-off errors.  Try a
            // more detailed test to decide if the triangle really should
            // be rejected.

            // The visibility test is applied to the window coordinates of
            // the points.  If any of the w-components of the points are
            // negative, the points are behind the viewer.  Triangles
            // with any such vertices are sent to the clipper because
            // parts of the triangle are potentially visible.
            if (afV0[3] > 0.0f && afV1[3] > 0.0f && afV2[3] > 0.0f)
            {
                int aiX[3], aiY[3];
                ClipToWindowXY(afV0,aiX[0],aiY[0]);
                ClipToWindowXY(afV1,aiX[1],aiY[1]);
                ClipToWindowXY(afV2,aiX[2],aiY[2]);
                int iVisibility = m_iCullSignAdjust*(
                    (aiX[2] - aiX[0])*(aiY[1] - aiY[0]) -
                    (aiX[1] - aiX[0])*(aiY[2] - aiY[0]));

                if (iVisibility <= 0)
                {
                    // The triangle is back facing.
                    return;
                }
            }
        }
    }

    // The triangle is visible.  Clip it against the view frustum.
    int aiIndex[SOFT_MAX_CLIP_INDICES];
    int aiEdge[SOFT_MAX_CLIP_INDICES];
    int iQuantity = 3;
    aiIndex[0] = iV0;
    aiIndex[1] = iV1;
    aiIndex[2] = iV2;
    aiEdge[0] = 0;
    aiEdge[1] = 1;
    aiEdge[2] = 2;
    ClipPolygon(iQuantity,aiIndex,aiEdge);
    if (iQuantity < 3)
    {
        // The entire triangle was outside the view frustum.
        return;
    }

    // Rasterize the triangles and call the pixel shaders.
    int iVQuantity = m_pkVBResource->VQuantity;
    if (pkWireframeState->Enabled)
    {
        // Draw only the polygon boundary edges, but not any diagonals
        // generated by clipping or edges whose end points are both clip
        // vertices.
        for (int j0 = iQuantity - 1, j1 = 0; j1 < iQuantity; j0 = j1++)
        {
            if (aiIndex[j0] < iVQuantity
            ||  aiIndex[j1] < iVQuantity
            ||  aiEdge[j0] == aiEdge[j1])
            {
                RasterizeEdge(aiIndex[j0],aiIndex[j1]);
            }
        }
    }
    else
    {
        int iNumTriangles = iQuantity - 2;
        for (int j = 1; j <= iNumTriangles; j++)
        {
            RasterizeTriangle(aiIndex[0],aiIndex[j],aiIndex[j+1]);
        }
    }
}
//----------------------------------------------------------------------------
