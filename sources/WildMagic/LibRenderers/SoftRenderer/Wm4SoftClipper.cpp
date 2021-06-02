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

#include "Wm4SoftRendererPCH.h"
#include "Wm4SoftRenderer.h"
#include "Wm4Camera.h"
using namespace Wm4;

//----------------------------------------------------------------------------
int SoftRenderer::ClipInterpolate (int aiIndex[SOFT_MAX_CLIP_INDICES],
    float afTest[SOFT_MAX_CLIP_INDICES], int iPrev, int iCurr)
{
    // The interpolated vertex is stored in the array locations m_iNextOutput
    // through m_iNextOutput+m_iNumOutputChannels-1.  If this range causes
    // array overflow, we need to resize the array.
    int iCurrentIndex = m_iNextVertexIndex++;
    int iRequestedChannels = m_iNumOutputChannels*m_iNextVertexIndex;
    m_kOutputVertex.EnsureCapacity(iRequestedChannels);

    const float* pfVPrev = GetOutputVertex(aiIndex[iPrev]);
    const float* pfVCurr = GetOutputVertex(aiIndex[iCurr]);
    float* pfVIntp = GetOutputVertex(iCurrentIndex);
    float fT = afTest[iPrev]/(afTest[iPrev] - afTest[iCurr]);

    for (int i = 0; i < m_iNumOutputChannels; i++)
    {
        *pfVIntp = (*pfVPrev) + fT*((*pfVCurr) - (*pfVPrev));
        pfVPrev++;
        pfVCurr++;
        pfVIntp++;
    }

    return iCurrentIndex;
}
//----------------------------------------------------------------------------
void SoftRenderer::ClipPolygonAgainstPlane (int& riQuantity,
    int aiIndex[SOFT_MAX_CLIP_INDICES], int iFirstPosIndex,
    float afTest[SOFT_MAX_CLIP_INDICES], int aiEdge[SOFT_MAX_CLIP_INDICES])
{
    // The current polygon transversely intersects the clipping plane.
    int iCQuantity = 0, iCurr, iPrev;
    int aiCIndex[SOFT_MAX_CLIP_INDICES];
    int aiEIndex[SOFT_MAX_CLIP_INDICES];

    if (iFirstPosIndex > 0)
    {
        // Compute the intersection of the plane and the first polygon edge to
        // cross the plane.  The intersection point is
        //   P = V[prev]+(sd[prev]/(sd[prev]-sd[curr])*(V[curr]-V[prev])
        // where sd[prev] = Dot(plane,V[prev]) and sd[curr] =
        // Dot(plane,V[curr]).
        iCurr = iFirstPosIndex;
        iPrev = iCurr - 1;
        aiCIndex[iCQuantity] = ClipInterpolate(aiIndex,afTest,iPrev,iCurr);
        aiEIndex[iCQuantity] = aiEdge[iPrev];
        iCQuantity++;

        // The vertices on the positive side of the plane are not clipped, so
        // just include them in the polygon.
        while (iCurr < riQuantity && afTest[iCurr] >= 0.0f)
        {
            aiCIndex[iCQuantity] = aiIndex[iCurr];
            aiEIndex[iCQuantity] = aiEdge[iCurr];
            iCQuantity++;
            iCurr++;
        }

        // Compute the intersection of the plane and the last polygon edge to
        // cross the plane.
        if (iCurr < riQuantity)
        {
            iPrev = iCurr - 1;
        }
        else
        {
            iCurr = 0;
            iPrev = riQuantity - 1;
        }
        aiCIndex[iCQuantity] = ClipInterpolate(aiIndex,afTest,iPrev,iCurr);
        aiEIndex[iCQuantity] = aiEdge[iPrev];
        iCQuantity++;
    }
    else  // iFirstPosIndex is 0
    {
        // The vertices on the positive side of the plane are not clipped, so
        // just include them in the polygon.
        iCurr = 0;
        while (iCurr < riQuantity && afTest[iCurr] >= 0.0f)
        {
            aiCIndex[iCQuantity] = aiIndex[iCurr];
            aiEIndex[iCQuantity] = aiEdge[iCurr];
            iCQuantity++;
            iCurr++;
        }

        // Compute the intersection of the plane and the first polygon edge to
        // cross the plane.
        iPrev = iCurr - 1;
        aiCIndex[iCQuantity] = ClipInterpolate(aiIndex,afTest,iPrev,iCurr);
        aiEIndex[iCQuantity] = aiEdge[iPrev];
        iCQuantity++;

        // Skip the vertices on the negative side of the plane.
        while (iCurr < riQuantity && afTest[iCurr] < 0.0f)
        {
            iCurr++;
        }

        // Compute the intersection of the plane and the last polygon edge to
        // cross the plane.
        if (iCurr < riQuantity)
        {
            iPrev = iCurr - 1;
            aiCIndex[iCQuantity] = ClipInterpolate(aiIndex,afTest,iPrev,
                iCurr);
            aiEIndex[iCQuantity] = aiEdge[iPrev];
            iCQuantity++;

            // The vertices on the positive side of the plane are not clipped,
            // so just include them in the polygon.
            while (iCurr < riQuantity && afTest[iCurr] >= 0.0f)
            {
                aiCIndex[iCQuantity] = aiIndex[iCurr];
                aiEIndex[iCQuantity] = aiEdge[iCurr];
                iCQuantity++;
                iCurr++;
            }
        }
        else
        {
            iCurr = 0;
            iPrev = riQuantity - 1;
            aiCIndex[iCQuantity] = ClipInterpolate(aiIndex,afTest,iPrev,
                iCurr);
            aiEIndex[iCQuantity] = aiEdge[iPrev];
            iCQuantity++;
        }
    }

    riQuantity = iCQuantity;
    memcpy(aiIndex,aiCIndex,iCQuantity*sizeof(int));
    memcpy(aiEdge,aiEIndex,iCQuantity*sizeof(int));
}
//----------------------------------------------------------------------------
void SoftRenderer::ClipPolygon (const Vector4f& rkPlane, int& riQuantity,
    int aiIndex[SOFT_MAX_CLIP_INDICES], int aiEdge[SOFT_MAX_CLIP_INDICES])
{
    // Test on which side of the plane the vertices lie.
    int iPositive = 0, iNegative = 0, iFirstPosIndex = -1;
    float afTest[SOFT_MAX_CLIP_INDICES];
    for (int i = 0; i < riQuantity; i++)
    {
        // constraint:  Dot(plane.normal,vertex) - planeconstant >= 0
        const Vector4f& rkVertex =
            *(const Vector4f*)GetOutputVertex(aiIndex[i]);
        afTest[i] = rkVertex.Dot(rkPlane);
        if (afTest[i] > 0.0f)
        {
            iPositive++;
            if (iFirstPosIndex < 0)
            {
                iFirstPosIndex = i;
            }
        }
        else if (afTest[i] < 0.0f)
        {
            iNegative++;
        }
    }

    if (iPositive == 0 && iNegative > 0)
    {
        // The polygon is entirely clipped (i.e. culled).
        riQuantity = 0;
        return;
    }

    if (iPositive >= 0 && iNegative == 0)
    {
        // The polygon is entirely on the frustum side of the plane, so
        // there is no clipping necessary.
        return;
    }

    ClipPolygonAgainstPlane(riQuantity,aiIndex,iFirstPosIndex,afTest,aiEdge);
}
//----------------------------------------------------------------------------
void SoftRenderer::ClipPolygon (int& riQuantity,
    int aiIndex[SOFT_MAX_CLIP_INDICES], int aiEdge[SOFT_MAX_CLIP_INDICES])
{
    // Clip against the frustum planes.
    int i;
    for (i = 0; i < 6; i++)
    {
        ClipPolygon(m_akFrustumClipPlane[i],riQuantity,aiIndex,aiEdge);
        if (riQuantity < 3)
        {
            return;
        }
    }

    // Clip against user-defined planes.
    if (m_bSomeUserClipPlaneActive)
    {
        for (i = 0; i < SOFT_MAX_USER_CLIP_PLANES; i++)
        {
            if (m_abUserClipPlaneActive[i])
            {
                ClipPolygon(m_akUserClipPlane[i],riQuantity,aiIndex,aiEdge);
                if (riQuantity < 3)
                {
                    return;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClipSegment (const Vector4f& rkPlane, int& riQuantity,
    int aiEnd[2])
{
    // Test on which side of the plane the vertices lie.
    const Vector4f& rkVertex0 = *(const Vector4f*)GetOutputVertex(aiEnd[0]);
    const Vector4f& rkVertex1 = *(const Vector4f*)GetOutputVertex(aiEnd[1]);

    float afTest[2] = { rkVertex0.Dot(rkPlane), rkVertex1.Dot(rkPlane) };

    if (afTest[0] <= 0.0f && afTest[1] <= 0.0f)
    {
        // The segment is outside the frustum or coincident with a face.
        riQuantity = 0;
        return;
    }

    if (afTest[0]*afTest[1] >= 0.0f)
    {
        // The segment is entirely inside the frustum, so no clipping is
        // necessary.
        return;
    }

    int iClipIndex = ClipInterpolate(aiEnd,afTest,0,1);
    if (afTest[0] < 0.0f)
    {
        aiEnd[0] = iClipIndex;
    }
    else
    {
        aiEnd[1] = iClipIndex;
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClipSegment (int& riQuantity, int aiEnd[2])
{
    // Clip against the frustum planes.
    int i;
    for (i = 0; i < 6; i++)
    {
        ClipSegment(m_akFrustumClipPlane[i],riQuantity,aiEnd);
        if (riQuantity == 0)
        {
            return;
        }
    }

    // Clip against user-defined planes.
    if (m_bSomeUserClipPlaneActive)
    {
        for (i = 0; i < SOFT_MAX_USER_CLIP_PLANES; i++)
        {
            if (m_abUserClipPlaneActive[i])
            {
                ClipSegment(m_akUserClipPlane[i],riQuantity,aiEnd);
                if (riQuantity == 0)
                {
                    return;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClipToWindow (const float* afVertex, float& rfXWindow,
    float& rfYWindow, float& rfDepth, float& rfInverseW)
{
    // The values m_fXCoeff0 and m_fYCoeff0 already include the 1/2 term to
    // support the rounding to the nearest integer.
    rfInverseW = 1.0f/afVertex[3];
    rfXWindow = m_fXCoeff0 + m_fXCoeff1*(afVertex[0]*rfInverseW);
    rfYWindow = m_fYCoeff0 + m_fYCoeff1*(afVertex[1]*rfInverseW);

    // Clamp to valid ranges.
    if (rfXWindow < 0.0f)
    {
        rfXWindow = 0.0f;
    }
    else if (rfXWindow > (float)m_iWidth)
    {
        rfXWindow = (float)m_iWidth;
    }

    if (rfYWindow < 0.0f)
    {
        rfYWindow = 0.0f;
    }
    else if (rfYWindow > (float)m_iHeight)
    {
        rfYWindow = (float)m_iHeight;
    }

    rfDepth = m_fRangeDepth*afVertex[2]*rfInverseW + m_fMinDepth;
    if (rfDepth < 0.0f)
    {
        rfDepth = 0.0f;
    }
    else if (rfDepth > 1.0f)
    {
        rfDepth = 1.0f;
    }
}
//----------------------------------------------------------------------------
void SoftRenderer::ClipToWindowXY (const float* afVertex, int& riX, int& riY)
{
    // The values m_fXCoeff0 and m_fYCoeff0 already include the 1/2 term to
    // support the rounding to the nearest integer.
    float fInverseW = 1.0f/afVertex[3];
    float fXWindow = m_fXCoeff0 + m_fXCoeff1*(afVertex[0]*fInverseW);
    float fYWindow = m_fYCoeff0 + m_fYCoeff1*(afVertex[1]*fInverseW);
    riX = WM4_FLOAT_TO_INT(fXWindow);
    riY = WM4_FLOAT_TO_INT(fYWindow);
}
//----------------------------------------------------------------------------
