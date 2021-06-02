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
#include "Wm4TerrainPage.h"
#include "Wm4StandardMesh.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,TerrainPage,TriMesh);
WM4_IMPLEMENT_STREAM(TerrainPage);
WM4_IMPLEMENT_DEFAULT_NAME_ID(TerrainPage,TriMesh);

//----------------------------------------------------------------------------
TerrainPage::TerrainPage (const Attributes& rkAttr, int iSize,
    unsigned short* ausHeight, const Vector2f& rkOrigin, float fMinElevation,
    float fMaxElevation, float fSpacing, float fUVBias)
    :
    m_kOrigin(rkOrigin)
{
    // iSize = 2^p + 1, p <= 7
    assert(iSize ==  3 || iSize ==  5 || iSize ==   9 || iSize == 17
        || iSize == 33 || iSize == 65 || iSize == 129);

    // native data
    m_iSize = iSize;
    m_ausHeight = ausHeight;
    m_fMinElevation = fMinElevation;
    m_fMaxElevation = fMaxElevation;
    m_fSpacing = fSpacing;

    InitializeDerivedData();

    // create mesh
    StandardMesh kSM(rkAttr);

    float fExtent = m_fSpacing*m_iSizeM1;
    TriMesh* pkMesh = kSM.Rectangle(m_iSize,m_iSize,fExtent,fExtent);
    VBuffer = pkMesh->VBuffer;
    IBuffer = pkMesh->IBuffer;

    m_fUVBias = fUVBias;
    WM4_DELETE pkMesh;

    // modify the vertices to use the terrain data
    int iVQuantity = VBuffer->GetVertexQuantity();
    for (int i = 0; i < iVQuantity; i++)
    {
        int iX = i % m_iSize;
        int iY = i / m_iSize;
        VBuffer->Position3(i) = Vector3f(GetX(iX),GetY(iY),GetHeight(i));
    }

    UpdateMS();
}
//----------------------------------------------------------------------------
TerrainPage::TerrainPage ()
    :
    m_kOrigin(Vector2f::ZERO)
{
    m_iSize = 0;
    m_ausHeight = 0;
    m_fMinElevation = 0.0f;
    m_fMaxElevation = 0.0f;
    m_fSpacing = 0.0f;
    m_iSizeM1 = 0;
    m_fInvSpacing = 0.0f;
    m_fTextureSpacing = 0.0f;
    m_fMultiplier = 0.0f;
}
//----------------------------------------------------------------------------
TerrainPage::~TerrainPage ()
{
    WM4_DELETE[] m_ausHeight;
}
//----------------------------------------------------------------------------
void TerrainPage::InitializeDerivedData ()
{
    m_iSizeM1 = m_iSize - 1;
    m_fInvSpacing = 1.0f/m_fSpacing;
    m_fTextureSpacing = 1.0f/(float)m_iSizeM1;
    m_fMultiplier = (m_fMaxElevation - m_fMinElevation)/65535.0f;
}
//----------------------------------------------------------------------------
float TerrainPage::GetHeight (float fX, float fY) const
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
        fH00 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex];
        fH11 = m_fMinElevation + m_fMultiplier *
            m_ausHeight[iIndex+1+m_iSize];
        if (fDiff > 0.0f)
        {
            fH10 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex+1];
            fHeight = (1.0f-fDiff-fDy)*fH00+fDiff*fH10+fDy*fH11;
        }
        else
        {
            fH01 = m_fMinElevation + m_fMultiplier *
                m_ausHeight[iIndex+m_iSize];
            fHeight = (1.0f+fDiff-fDx)*fH00-fDiff*fH01+fDx*fH11;
        }
    }
    else
    {
        float fSum = fDx + fDy;
        fH10 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex+1];
        fH01 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex+m_iSize];
        if (fSum <= 1.0f)
        {
            fH00 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex];
            fHeight = (1.0f-fSum)*fH00+fDx*fH10+fDy*fH01;
        }
        else
        {
            fH11 = m_fMinElevation + m_fMultiplier *
                m_ausHeight[iIndex+1+m_iSize];
            fHeight = (fSum-1.0f)*fH11+(1.0f-fDy)*fH10+(1.0f-fDx)*fH01;
        }
    }

    return fHeight;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void TerrainPage::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iSize);
    int iVQuantity = m_iSize*m_iSize;
    m_ausHeight = WM4_NEW unsigned short[iVQuantity];
    rkStream.Read(iVQuantity,m_ausHeight);
    rkStream.Read(m_kOrigin);
    rkStream.Read(m_fMinElevation);
    rkStream.Read(m_fMaxElevation);
    rkStream.Read(m_fSpacing);

    InitializeDerivedData();

    WM4_END_DEBUG_STREAM_LOAD(TerrainPage);
}
//----------------------------------------------------------------------------
void TerrainPage::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool TerrainPage::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void TerrainPage::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iSize);
    int iVQuantity = VBuffer->GetVertexQuantity();
    rkStream.Write(iVQuantity,m_ausHeight);
    rkStream.Write(m_kOrigin);
    rkStream.Write(m_fMinElevation);
    rkStream.Write(m_fMaxElevation);
    rkStream.Write(m_fSpacing);

    WM4_END_DEBUG_STREAM_SAVE(TerrainPage);
}
//----------------------------------------------------------------------------
int TerrainPage::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return TriMesh::GetDiskUsed(rkVersion) +
        sizeof(m_iSize) +
        m_iSize*m_iSize*sizeof(m_ausHeight[0]) +
        sizeof(m_kOrigin) +
        sizeof(m_fMinElevation) +
        sizeof(m_fMaxElevation) +
        sizeof(m_fSpacing);
}
//----------------------------------------------------------------------------
StringTree* TerrainPage::SaveStrings (const char*)
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

    // children
    pkTree->Append(TriMesh::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
