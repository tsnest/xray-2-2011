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

#include "Wm4GraphicsPCH.h"
#include "Wm4ClodTerrainPage.h"
#include "Wm4ClodTerrainBlock.h"
#include "Wm4ClodTerrainVertex.h"
#include "Wm4Camera.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,ClodTerrainPage,TriMesh);
WM4_IMPLEMENT_STREAM(ClodTerrainPage);
WM4_IMPLEMENT_DEFAULT_NAME_ID(ClodTerrainPage,TriMesh);

//----------------------------------------------------------------------------
ClodTerrainPage::ClodTerrainPage (const Attributes& rkAttr, int iSize,
    unsigned short* ausHeight, const Vector2f& rkOrigin, float fMinElevation,
    float fMaxElevation, float fSpacing, float fUVBias)
    :
    m_kOrigin(rkOrigin)
{
    // iSize = 2^p + 1, p <= 7
    assert(iSize ==  3 || iSize ==  5 || iSize ==   9 || iSize == 17
        || iSize == 33 || iSize == 65 || iSize == 129);

    // generate space for all the vertices
    m_iTotalVQuantity = iSize*iSize;
    VBuffer = WM4_NEW VertexBuffer(rkAttr,m_iTotalVQuantity);

    // generate space for all the indices
    m_iTotalTQuantity = 2*(iSize-1)*(iSize-1);
    m_iTotalIQuantity = 3*m_iTotalTQuantity;
    IBuffer = WM4_NEW IndexBuffer(m_iTotalIQuantity);

    // native data
    m_iSize = iSize;
    m_ausHeight = ausHeight;
    m_fMinElevation = fMinElevation;
    m_fMaxElevation = fMaxElevation;
    m_fSpacing = fSpacing;
    m_fUVBias = fUVBias;

    InitializeDerivedData();
}
//----------------------------------------------------------------------------
ClodTerrainPage::ClodTerrainPage ()
    :
    m_kOrigin(Vector2f::ZERO)
{
    // native data
    m_iTotalVQuantity = 0;
    m_iTotalIQuantity = 0;
    m_iSize = 0;
    m_ausHeight = 0;
    m_fMinElevation = 0.0f;
    m_fMaxElevation = 0.0f;
    m_fSpacing = 0.0f;
    m_fUVBias = 0.0f;

    // derived data
    m_iSizeM1 = 0;
    m_fPixelTolerance = 0.0f;
    m_fWorldTolerance = 0.0f;
    m_fInvSpacing = 0.0f;
    m_fTextureSpacing = 0.0f;
    m_fMultiplier = 0.0f;
    m_bNeedsTessellation = false;
    m_aiLookup = 0;
    m_iConnectLength = 0;
    m_akTVertex = 0;
    m_iBlockQuantity = 0;
    m_akBlock = 0;
    m_usQueueQuantity = 0;
    m_ausQueue = 0;
    m_usFront = 0;
    m_usRear = 0;
    m_usItemsInQueue = 0;
}
//----------------------------------------------------------------------------
ClodTerrainPage::~ClodTerrainPage ()
{
    WM4_DELETE[] m_ausHeight;
    WM4_DELETE[] m_akTVertex;
    WM4_DELETE[] m_akBlock;
    WM4_DELETE[] m_ausQueue;
    WM4_DELETE[] m_aiLookup;
}
//----------------------------------------------------------------------------
void ClodTerrainPage::InitializeDerivedData ()
{
    m_iSizeM1 = m_iSize - 1;
    m_fPixelTolerance = 1.0f;
    m_fWorldTolerance = -1.0f;
    m_fInvSpacing = 1.0f/m_fSpacing;
    m_fTextureSpacing = 1.0f/float(m_iSizeM1);
    m_fMultiplier = (m_fMaxElevation - m_fMinElevation)/65535.0f;
    m_bNeedsTessellation = true;

    // for tessellation (mapping of indices)
    m_aiLookup = WM4_NEW int[m_iTotalVQuantity];

    // initialize vertex information array
    m_akTVertex = WM4_NEW ClodTerrainVertex[m_iTotalVQuantity];
    memset(m_akTVertex,0,m_iTotalVQuantity*sizeof(ClodTerrainVertex));

    // allocate quadtree
    m_iBlockQuantity = m_iSize*(m_iSize-2)/3;
    m_akBlock = WM4_NEW ClodTerrainBlock[m_iBlockQuantity];

    // initialize quadtree
    int iStride = (m_iSize-1)/2;
    m_akBlock[0].Initialize(this,m_akBlock,0,0,0,iStride,true);
    m_akBlock[0].UpdateBoundingBox(this,m_akBlock,0,iStride);

    // model bounding sphere contains the top level block's bounding box
    Vector3f kCenter = 0.5f*(m_akBlock[0].GetMax() + m_akBlock[0].GetMin());
    Vector3f kDiag = 0.5f*(m_akBlock[0].GetMax() - m_akBlock[0].GetMin());
    ModelBound->SetCenter(kCenter);
    ModelBound->SetRadius(kDiag.Length());

    // allocate queue
    m_usQueueQuantity =
        (unsigned short)(m_iBlockQuantity - m_iBlockQuantity/4);
    m_ausQueue = WM4_NEW unsigned short[m_usQueueQuantity];

    // root of quadtree is initially active
    m_ausQueue[0] = 0;
    m_usFront = 0;
    m_usRear = 1;
    m_usItemsInQueue = 1;
}
//----------------------------------------------------------------------------
void ClodTerrainPage::SetPixelTolerance (const Renderer* pkRenderer,
    float fTolerance)
{
    float fWidth = (float)pkRenderer->GetWidth();
    CameraPtr spkCamera = pkRenderer->GetCamera();
    float fRMax = spkCamera->GetRMax();
    float fDMin = spkCamera->GetDMin();

    m_fPixelTolerance = fTolerance;
    m_fWorldTolerance = 2.0f*fRMax*m_fPixelTolerance/(fDMin*fWidth);
    m_fWorldTolerance *= m_fWorldTolerance;
}
//----------------------------------------------------------------------------
float ClodTerrainPage::GetHeight (float fX, float fY) const
{
    float fXGrid = (fX - m_kOrigin.X())*m_fInvSpacing;
    if (fXGrid < 0.0f || fXGrid >= (float)m_iSizeM1)
    {
        // location not in page
        return Mathf::MAX_REAL;
    }

    float fYGrid = (fY - m_kOrigin.Y())*m_fInvSpacing;
    if (fYGrid < 0.0f || fYGrid >= (float)m_iSizeM1)
    {
        // location not in page
        return Mathf::MAX_REAL;
    }

    float fCol = Mathf::Floor(fXGrid);
    int iCol = (int)fCol;
    float fRow = Mathf::Floor(fYGrid);
    int iRow = (int)fRow;

    int iIndex = iCol + m_iSize*iRow;
    float fDx = fXGrid - fCol;
    float fDy = fYGrid - fRow;
    float fH00, fH10, fH01, fH11, fHeight;

    if ((iCol & 1) == (iRow & 1))
    {
        float fDiff = fDx - fDy;
        fH00 = m_fMinElevation+m_fMultiplier*m_ausHeight[iIndex];
        fH11 = m_fMinElevation+m_fMultiplier*m_ausHeight[iIndex+1+m_iSize];
        if (fDiff > 0.0f)
        {
            fH10 = m_fMinElevation+m_fMultiplier*m_ausHeight[iIndex+1];
            fHeight = (1.0f-fDiff-fDy)*fH00+fDiff*fH10+fDy*fH11;
        }
        else
        {
            fH01 = m_fMinElevation+m_fMultiplier*m_ausHeight[iIndex+m_iSize];
            fHeight = (1.0f+fDiff-fDx)*fH00-fDiff*fH01+fDx*fH11;
        }
    }
    else
    {
        float fSum = fDx + fDy;
        fH10 = m_fMinElevation+m_fMultiplier*m_ausHeight[iIndex+1];
        fH01 = m_fMinElevation+m_fMultiplier*m_ausHeight[iIndex+m_iSize];
        if (fSum <= 1.0f)
        {
            fH00 = m_fMinElevation+m_fMultiplier*m_ausHeight[iIndex];
            fHeight = (1.0f-fSum)*fH00+fDx*fH10+fDy*fH01;
        }
        else
        {
            fH11 = m_fMinElevation+m_fMultiplier*
                m_ausHeight[iIndex+1+m_iSize];
            fHeight = (fSum-1.0f)*fH11+(1.0f-fDy)*fH10+(1.0f-fDx)*fH01;
        }
    }

    return fHeight;
}
//----------------------------------------------------------------------------
bool ClodTerrainPage::IntersectFrustum (const Camera* pkCamera)
{
    // check if terrain page itself is inside frustum
    m_akBlock[0].TestIntersectFrustum(this,pkCamera);
    bool bIntersect = m_akBlock[0].GetVisible();
    m_akBlock[0].ClearBits();
    return bIntersect;
}
//----------------------------------------------------------------------------
void ClodTerrainPage::AddToQueue (unsigned short usBlock)
{
    m_ausQueue[m_usRear] = usBlock;
    if (++m_usRear == m_usQueueQuantity)
    {
        m_usRear = 0;
    }
    m_usItemsInQueue++;
}
//----------------------------------------------------------------------------
unsigned short ClodTerrainPage::RemoveFromQueue ()
{
    unsigned short usBlock = m_ausQueue[m_usFront];
    if (++m_usFront == m_usQueueQuantity)
    {
        m_usFront = 0;
    }
    m_usItemsInQueue--;
    return usBlock;
}
//----------------------------------------------------------------------------
unsigned short ClodTerrainPage::ReadFromQueue (unsigned short usIndex)
{
    usIndex = usIndex + m_usFront;
    if (usIndex < m_usQueueQuantity)
    {
        return m_ausQueue[usIndex];
    }
    else
    {
        return m_ausQueue[usIndex - m_usQueueQuantity];
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::ResetBlocks ()
{
    unsigned short usQueue, usBlock;
    if (m_usFront < m_usRear)
    {
        m_usNumUnprocessed = m_usRear - m_usFront;
        for (usQueue = m_usFront; usQueue < m_usRear; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if (m_akBlock[usBlock].BitsSet())
            {
                m_akBlock[usBlock].Disable(this);
                m_akBlock[usBlock].ClearBits();
            }
        }
    }
    else
    {
        m_usNumUnprocessed = m_usQueueQuantity - m_usFront + m_usRear;
        for (usQueue = m_usFront; usQueue < m_usQueueQuantity; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if (m_akBlock[usBlock].BitsSet())
            {
                m_akBlock[usBlock].Disable(this);
                m_akBlock[usBlock].ClearBits();
            }
        }
        for (usQueue = 0; usQueue < m_usRear; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if (m_akBlock[usBlock].BitsSet())
            {
                m_akBlock[usBlock].Disable(this);
                m_akBlock[usBlock].ClearBits();
            }
        }
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::SimplifyBlocks (const Camera* pkCamera,
    const Vector3f& rkModelEye, const Vector3f& rkModelDir,
    bool bCloseAssumption)
{
    while (m_usNumUnprocessed > 0)
    {
        // process the block in the front of queue
        unsigned short usBlock = RemoveFromQueue();
        ClodTerrainBlock* pkBlock = &m_akBlock[usBlock];

        if (!pkBlock->GetProcessed())
        {
            m_usNumUnprocessed--;

            unsigned short usChild;
            ClodTerrainBlock* pkChild;
            Vector2f kCLoc;
            int i;

            if (pkBlock->IsFirstChild(usBlock))
            {
                // first child has responsibility for replacing by parent
                if (pkBlock->IsSibling(usBlock,ReadFromQueue(2)))
                {
                    pkChild = pkBlock;
                    if (bCloseAssumption)
                    {
                        for (i = 0; i < 4; i++, pkChild++)
                        {
                            kCLoc.X() = pkChild->GetX()*m_fSpacing +
                                m_kOrigin.X();
                            kCLoc.Y() = pkChild->GetY()*m_fSpacing +
                                m_kOrigin.Y();
                            pkChild->ComputeInterval(rkModelEye,rkModelDir,
                                m_fWorldTolerance,kCLoc,m_fSpacing);
                            if (pkChild->GetDeltaMax() > pkChild->GetDeltaL())
                            {
                                break;
                            }
                        }
                    }
                    else // distant assumption
                    {
                        for (i = 0; i < 4; i++, pkChild++)
                        {
                            pkChild->ComputeInterval(rkModelEye,
                                m_fWorldTolerance);
                            if (pkChild->GetDeltaMax() > pkChild->GetDeltaL())
                            {
                                break;
                            }
                        }
                    }

                    if (i == 4)
                    {
                        // remove child blocks (first child already removed)
                        for (i = 0; i < 3; i++)
                        {
                            usChild = RemoveFromQueue();
                            if (!m_akBlock[usChild].GetProcessed())
                            {
                                m_usNumUnprocessed--;
                            }
                            m_akBlock[usChild].ClearBits();
                        }

                        // add parent block
                        unsigned short usParent =
                            (unsigned short)pkBlock->GetParentIndex(usBlock);
                        AddToQueue(usParent);
                        assert(!m_akBlock[usParent].GetProcessed());
                        m_usNumUnprocessed++;
                        continue;
                    }
                }
            }

            if (!pkBlock->GetVisibilityTested())
            {
                pkBlock->TestIntersectFrustum(this,pkCamera);
            }

            if (pkBlock->GetStride() > 1)
            {
                // subdivide only if bounding box of block intersects frustum
                if (pkBlock->GetVisible())
                {
                    usChild = (unsigned short)pkBlock->GetChildIndex(usBlock,1);
                    pkChild = &m_akBlock[usChild];
                    if (bCloseAssumption) 
                    {
                        for (i = 0; i < 4; i++, pkChild++)
                        {
                            kCLoc.X() = pkChild->GetX()*m_fSpacing +
                                m_kOrigin.X();
                            kCLoc.Y() = pkChild->GetY()*m_fSpacing +
                                m_kOrigin.Y();
                            pkChild->ComputeInterval(rkModelEye,rkModelDir,
                                m_fWorldTolerance,kCLoc,m_fSpacing);
                            if (pkChild->GetDeltaMax() > pkChild->GetDeltaL())
                            {
                                break;
                            }
                        }
                    }
                    else // distant assumption
                    {
                        for (i = 0; i < 4; i++, pkChild++)
                        {
                            pkChild->ComputeInterval(rkModelEye,
                                m_fWorldTolerance);
                            if (pkChild->GetDeltaMax() > pkChild->GetDeltaL())
                            {
                                break;
                            }
                        }
                    }

                    // subdivide only if children all agree it should happen
                    if (i < 4)
                    {
                        // add child blocks (parent already removed)
                        for (i = 0; i < 4; i++, usChild++)
                        {
                            // add child block
                            AddToQueue(usChild);
                            assert(!m_akBlock[usChild].GetProcessed());
                            m_usNumUnprocessed++;
                        }
                        continue;
                    }
                }
            }

            // tag block as processed
            pkBlock->SetProcessed(true);
        }

        // put processed block at rear of queue
        AddToQueue(usBlock);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::SimplifyVertices (const Vector3f& rkModelEye,
    const Vector3f& rkModelDir, bool bCloseAssumption)
{
    unsigned int usQueue, usBlock;

    if (m_usFront < m_usRear)
    {
        for (usQueue = m_usFront; usQueue < m_usRear; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if (m_akBlock[usBlock].GetVisible())
            {
                m_akBlock[usBlock].SimplifyVertices(this,rkModelEye,
                    rkModelDir,m_fWorldTolerance,bCloseAssumption);
            }
        }
    }
    else
    {
        for (usQueue = m_usFront; usQueue < m_usQueueQuantity; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if (m_akBlock[usBlock].GetVisible())
            {
                m_akBlock[usBlock].SimplifyVertices(this,rkModelEye,
                    rkModelDir,m_fWorldTolerance,bCloseAssumption);
            }
        }
        for (usQueue = 0; usQueue < m_usRear; usQueue++)
        {
            usBlock = m_ausQueue[usQueue];
            if (m_akBlock[usBlock].GetVisible())
            {
                m_akBlock[usBlock].SimplifyVertices(this,rkModelEye,
                    rkModelDir,m_fWorldTolerance,bCloseAssumption);
            }
        }
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Simplify (const Renderer* pkRenderer,
    const Vector3f& rkModelEye, const Vector3f& rkModelDir,
    bool bCloseAssumption)
{
    if (m_fWorldTolerance == -1.0f)
    {
        SetPixelTolerance(pkRenderer,m_fPixelTolerance);
    }

    SimplifyBlocks(pkRenderer->GetCamera(),rkModelEye,rkModelDir,
        bCloseAssumption);
    SimplifyVertices(rkModelEye,rkModelDir,bCloseAssumption);

    m_bNeedsTessellation = true;
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Render (ClodTerrainBlock& rkBlock)
{
    int iOrigin = rkBlock.GetX() + m_iSize*rkBlock.GetY();
    int iStride = rkBlock.GetStride();
    int iTwoStride = 2*iStride;
    int iTwoStrideTimesSize = iTwoStride*m_iSize;
    int aiIndex[5] =
    {
        iOrigin,
        iOrigin + iTwoStride,
        iOrigin + iStride*(m_iSize + 1),
        iOrigin + iTwoStrideTimesSize,
        iOrigin + iTwoStrideTimesSize + iTwoStride
    };

    if (rkBlock.GetEven())
    {
        RenderTriangle(aiIndex[0],aiIndex[3],aiIndex[1]);
        RenderTriangle(aiIndex[4],aiIndex[1],aiIndex[3]);
    }
    else
    {
        RenderTriangle(aiIndex[1],aiIndex[0],aiIndex[4]);
        RenderTriangle(aiIndex[3],aiIndex[4],aiIndex[0]);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::RenderTriangle (int iT, int iL, int iR)
{
    // determine if triangle is leaf or interior
    bool bInterior;
    if (iR > iT)
    {
        if (iL > iT)
        {
            bInterior = (iR - iT > 1);
        }
        else
        {
            bInterior = (iT - iL > 1);
        }
    }
    else
    {
        if (iL > iT)
        {
            bInterior = (iL - iT > 1);
        }
        else
        {
            bInterior = (iT - iR > 1);
        }
    }

    if (bInterior)
    {
        // Triangle is part of internal block and can be subdivided.  M is
        // the midpoint of edge <L,R>.
        int iM = ((iL + iR) >> 1);
        if (m_akTVertex[iM].GetEnabled())
        {
            RenderTriangle(iM,iT,iL);
            RenderTriangle(iM,iR,iT);
            return;
        }
    }

    // pack the vertex data
    const Attributes& rkAttr = VBuffer->GetAttributes();
    int iVQuantity = VBuffer->GetVertexQuantity();
    int iX, iY, iUnit;
    Vector2f kTCoord;
    if (m_aiLookup[iT] == -1)
    {
        iX = iT % m_iSize;
        iY = iT / m_iSize;

        VBuffer->Position3(iVQuantity) =
            Vector3f(GetX(iX),GetY(iY),GetHeight(iT));

        kTCoord = Vector2f(GetTextureCoordinate(iX),GetTextureCoordinate(iY));
        for (iUnit = 0; iUnit < rkAttr.GetMaxTCoords(); iUnit++)
        {
            if (rkAttr.HasTCoord(iUnit))
            {
                VBuffer->TCoord2(iUnit,iVQuantity) = kTCoord;
            }
        }
        m_aiLookup[iT] = iVQuantity++;
        VBuffer->SetVertexQuantity(iVQuantity);
    }

    if (m_aiLookup[iR] == -1)
    {
        iX = iR % m_iSize;
        iY = iR / m_iSize;

        VBuffer->Position3(iVQuantity) =
            Vector3f(GetX(iX),GetY(iY),GetHeight(iR));

        kTCoord = Vector2f(GetTextureCoordinate(iX),GetTextureCoordinate(iY));
        for (iUnit = 0; iUnit < rkAttr.GetMaxTCoords(); iUnit++)
        {
            if (rkAttr.HasTCoord(iUnit))
            {
                VBuffer->TCoord2(iUnit,iVQuantity) = kTCoord;
            }
        }
        m_aiLookup[iR] = iVQuantity++;
        VBuffer->SetVertexQuantity(iVQuantity);
    }

    if (m_aiLookup[iL] == -1)
    {
        iX = iL % m_iSize;
        iY = iL / m_iSize;

        VBuffer->Position3(iVQuantity) =
            Vector3f(GetX(iX),GetY(iY),GetHeight(iL));

        kTCoord = Vector2f(GetTextureCoordinate(iX),GetTextureCoordinate(iY));
        for (iUnit = 0; iUnit < rkAttr.GetMaxTCoords(); iUnit++)
        {
            if (rkAttr.HasTCoord(iUnit))
            {
                VBuffer->TCoord2(iUnit,iVQuantity) = kTCoord;
            }
        }
        m_aiLookup[iL] = iVQuantity++;
        VBuffer->SetVertexQuantity(iVQuantity);
    }

    assert(iVQuantity < m_iTotalVQuantity);

    // add triangle to connectivity array
    int iIQuantity = IBuffer->GetIndexQuantity();
    int* aiIndex = IBuffer->GetData();
    aiIndex[iIQuantity++] = m_aiLookup[iT];
    aiIndex[iIQuantity++] = m_aiLookup[iR];
    aiIndex[iIQuantity++] = m_aiLookup[iL];
    IBuffer->SetIndexQuantity(iIQuantity);

    assert(iIQuantity < m_iTotalIQuantity);
}
//----------------------------------------------------------------------------
void ClodTerrainPage::RenderBlocks ()
{
    // reset dynamic quantities
    memset(m_aiLookup,0xFF,m_iSize*m_iSize*sizeof(int));
    VBuffer->SetVertexQuantity(0);
    IBuffer->SetIndexQuantity(0);

    unsigned short usQueue;
    if (m_usFront < m_usRear)
    {
        for (usQueue = m_usFront; usQueue < m_usRear; usQueue++)
        {
            ClodTerrainBlock& rkBlock = m_akBlock[m_ausQueue[usQueue]];
            if (rkBlock.GetVisible())
            {
                Render(rkBlock);
            }
        }
    }
    else
    {
        for (usQueue = m_usFront; usQueue < m_usQueueQuantity; usQueue++)
        {
            ClodTerrainBlock& rkBlock = m_akBlock[m_ausQueue[usQueue]];
            if (rkBlock.GetVisible())
            {
                Render(rkBlock);
            }
        }
        for (usQueue = 0; usQueue < m_usRear; usQueue++)
        {
            ClodTerrainBlock& rkBlock = m_akBlock[m_ausQueue[usQueue]];
            if (rkBlock.GetVisible())
            {
                Render(rkBlock);
            }
        }
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::GetVisibleSet (Culler& rkCuller, bool bNoCull)
{
    if (m_bNeedsTessellation)
    {
        m_bNeedsTessellation = false;
        RenderBlocks();
    }

    // It is possible (but not likely) that blocks are not culled, but the
    // camera is positioned so that no triangles are visible.  For example,
    // this can occur if you are below the terrain mesh and looking down.
    if (IBuffer->GetIndexQuantity() > 0)
    {
        TriMesh::GetVisibleSet(rkCuller,bNoCull);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::StitchNextCol (ClodTerrainPage* pkNextCol)
{
    // 'this' is page (r,c), 'pkNextCol' is page (r,c+1)
    assert(pkNextCol->m_iSize == m_iSize);

    int iMax = m_iSize - 2;
    int iXThis = m_iSize - 1;
    int iXNext = 0;
    for (int iY = 1; iY <= iMax; iY++)
    {
        int iIThis = iXThis + m_iSize*iY;
        int iINext = iXNext + m_iSize*iY;
        ClodTerrainVertex* pkVThis = &m_akTVertex[iIThis];
        ClodTerrainVertex* pkVNext = &pkNextCol->m_akTVertex[iINext];
        pkVNext->SetDependent(0,pkVThis);
        pkVThis->SetDependent(1,pkVNext);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::UnstitchNextCol (ClodTerrainPage* pkNextCol)
{
    // 'this' is page (r,c), 'pkNextCol' is page (r,c+1)
    assert(pkNextCol->m_iSize == m_iSize);

    int iMax = m_iSize - 2;
    int iXThis = m_iSize - 1;
    int iXNext = 0;
    for (int iY = 1; iY <= iMax; iY++)
    {
        int iIThis = iXThis + m_iSize*iY;
        int iINext = iXNext + m_iSize*iY;
        ClodTerrainVertex* pkVThis = &m_akTVertex[iIThis];
        ClodTerrainVertex* pkVNext = &pkNextCol->m_akTVertex[iINext];
        pkVNext->SetDependent(0,0);
        pkVThis->SetDependent(1,0);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::StitchNextRow (ClodTerrainPage* pkNextRow)
{
    // 'this' is page (r,c), 'pkNextRow' is page (r+1,c)
    assert(pkNextRow->m_iSize == m_iSize);

    int iMax = m_iSize - 2;
    int iYThis = m_iSize - 1;
    int iYNext = 0;
    for (int iX = 1; iX <= iMax; iX++)
    {
        int iIThis = iX + m_iSize*iYThis;
        int iINext = iX + m_iSize*iYNext;
        ClodTerrainVertex* pkVThis = &m_akTVertex[iIThis];
        ClodTerrainVertex* pkVNext = &pkNextRow->m_akTVertex[iINext];
        pkVNext->SetDependent(1,pkVThis);
        pkVThis->SetDependent(0,pkVNext);
    }
}
//----------------------------------------------------------------------------
void ClodTerrainPage::UnstitchNextRow (ClodTerrainPage* pkNextRow)
{
    // 'this' is page (r,c), 'pkNextRow' is page (r+1,c)
    assert(pkNextRow->m_iSize == m_iSize);

    int iMax = m_iSize - 2;
    int iYThis = m_iSize - 1;
    int iYNext = 0;
    for (int iX = 1; iX <= iMax; iX++)
    {
        int iIThis = iX + m_iSize*iYThis;
        int iINext = iX + m_iSize*iYNext;
        ClodTerrainVertex* pkVThis = &m_akTVertex[iIThis];
        ClodTerrainVertex* pkVNext = &pkNextRow->m_akTVertex[iINext];
        pkVNext->SetDependent(1,0);
        pkVThis->SetDependent(0,0);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ClodTerrainPage::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iSize);
    m_iTotalVQuantity = m_iSize*m_iSize;
    m_ausHeight = WM4_NEW unsigned short[m_iTotalVQuantity];
    rkStream.Read(m_iTotalVQuantity,m_ausHeight);
    rkStream.Read(m_kOrigin);
    rkStream.Read(m_fMinElevation);
    rkStream.Read(m_fMaxElevation);
    rkStream.Read(m_fSpacing);

    InitializeDerivedData();

    WM4_END_DEBUG_STREAM_LOAD(ClodTerrainPage);
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ClodTerrainPage::Register (Stream& rkStream) const
{
    // The vertex and triangle quantities are dynamically varied during the
    // simplification.  Write the maximum quantities to disk.  A Simplify()
    // call must be made after loading the file containing the terrain pages.
    VBuffer->SetVertexQuantity(m_iTotalVQuantity);
    IBuffer->SetIndexQuantity(m_iTotalIQuantity);
    m_bNeedsTessellation = true;

    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void ClodTerrainPage::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iSize);
    rkStream.Write(m_iTotalVQuantity,m_ausHeight);
    rkStream.Write(m_kOrigin);
    rkStream.Write(m_fMinElevation);
    rkStream.Write(m_fMaxElevation);
    rkStream.Write(m_fSpacing);

    WM4_END_DEBUG_STREAM_SAVE(ClodTerrainPage);
}
//----------------------------------------------------------------------------
int ClodTerrainPage::GetDiskUsed (const StreamVersion& rkVersion) const
{
    // The vertex and triangle quantities are dynamically varied during the
    // simplification.  Count the maximum quantities, then reset to the
    // current dynamic values.  The typecast to integer references is to
    // circumvent the 'const' of this function.  In effect, however, the
    // function is 'const' since the quantities are restored.
    int iSaveVQ = 0;
    if (VBuffer)
    {
        iSaveVQ = VBuffer->GetVertexQuantity();
        VBuffer->SetVertexQuantity(m_iTotalVQuantity);
    }

    int iSaveIQ = 0;
    if (IBuffer)
    {
        iSaveIQ = IBuffer->GetIndexQuantity();
        IBuffer->SetIndexQuantity(m_iTotalIQuantity);
    }

    int iDiskUsed = TriMesh::GetDiskUsed(rkVersion) +
        sizeof(m_iSize) +
        m_iTotalVQuantity*sizeof(m_ausHeight[0]) +
        sizeof(m_kOrigin) +
        sizeof(m_fMinElevation) +
        sizeof(m_fMaxElevation) +
        sizeof(m_fSpacing);

    if (VBuffer)
    {
        VBuffer->SetVertexQuantity(iSaveVQ);
    }

    if (IBuffer)
    {
        IBuffer->SetIndexQuantity(iSaveIQ);
    }

    return iDiskUsed;
}
//----------------------------------------------------------------------------
StringTree* ClodTerrainPage::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("size =",m_iSize));
    pkTree->Append(Format("origin =",m_kOrigin));
    pkTree->Append(Format("min elevation =",m_fMinElevation));
    pkTree->Append(Format("max elevation =",m_fMaxElevation));
    pkTree->Append(Format("spacing =",m_fSpacing));
    pkTree->Append(Format("uv bias =",m_fUVBias));
    pkTree->Append(Format("pixel tolerance =",m_fPixelTolerance));
    pkTree->Append(Format("world tolerance =",m_fWorldTolerance));

    // children
    pkTree->Append(TriMesh::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
