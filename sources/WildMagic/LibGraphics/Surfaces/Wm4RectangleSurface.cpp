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
// Version: 4.0.1 (2007/06/16)

#include "Wm4GraphicsPCH.h"
#include "Wm4RectangleSurface.h"
#include "Wm4CullState.h"
#include "Wm4StandardMesh.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,RectangleSurface,TriMesh);
WM4_IMPLEMENT_STREAM(RectangleSurface);
WM4_IMPLEMENT_DEFAULT_NAME_ID(RectangleSurface,TriMesh);

//----------------------------------------------------------------------------
RectangleSurface::RectangleSurface (ParametricSurfacef* pkSurface,
    int iUSamples, int iVSamples, const Attributes& rkAttr, bool bDoubleSided,
    const Vector2f* pkTCoordMin, const Vector2f* pkTCoordMax)
{
    assert(pkSurface && pkSurface->IsRectangular());

    m_pkSurface = pkSurface;
    m_iUSamples = iUSamples;
    m_iVSamples = iVSamples;

    float fUMin = m_pkSurface->GetUMin();
    float fURan = m_pkSurface->GetUMax() - fUMin;
    float fUDelta = fURan/(float)(m_iUSamples-1);
    float fVMin = m_pkSurface->GetVMin();
    float fVRan = m_pkSurface->GetVMax() - fVMin;
    float fVDelta = fVRan/(float)(m_iVSamples-1);

    // compute the vertices, normals, and texture coordinates
    int iVQuantity = m_iUSamples*m_iVSamples;
    VBuffer = WM4_NEW VertexBuffer(rkAttr,iVQuantity);

    float fTUDelta = 0.0f, fTVDelta = 0.0f;
    if (pkTCoordMin && pkTCoordMax)
    {
        assert(rkAttr.GetMaxTCoords() > 0);
        fTUDelta = (pkTCoordMax->X() - pkTCoordMin->X())/fURan;
        fTVDelta = (pkTCoordMax->Y() - pkTCoordMin->Y())/fVRan;
    }

    int iU, iV, i;
    for (iU = 0, i = 0; iU < m_iUSamples; iU++)
    {
        float fUIncr = fUDelta*iU;
        float fU = fUMin + fUIncr;
        for (iV = 0; iV < m_iVSamples; iV++)
        {
            float fVIncr = fVDelta*iV;
            float fV = fVMin + fVIncr;
            VBuffer->Position3(i) = m_pkSurface->P(fU,fV);

            if (rkAttr.HasNormal())
            {
                Vector3f kPos, kTan0, kTan1, kNormal;
                m_pkSurface->GetFrame(fU,fV,kPos,kTan0,kTan1,kNormal);
                VBuffer->Normal3(i) = kNormal;
            }

            for (int iUnit = 0; iUnit < rkAttr.GetMaxTCoords(); iUnit++)
            {
                Vector2f kTCoord(
                    pkTCoordMin->X() + fTUDelta*fUIncr,
                    pkTCoordMin->Y() + fTVDelta*fVIncr);
                if (rkAttr.HasTCoord(iUnit))
                {
                    assert(rkAttr.GetTCoordChannels(iUnit) == 2);
                    VBuffer->TCoord2(iUnit,i) = kTCoord;
                }
            }

            i++;
        }
    }

    // compute the surface triangle connectivity
    int iTQuantity = 2*(m_iUSamples-1)*(m_iVSamples-1);
    int iIQuantity = 3*iTQuantity;
    IBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int* aiIndex = IBuffer->GetData();
    int* aiLocalIndex = aiIndex;
    for (iU = 0, i = 0; iU < m_iUSamples-1; iU++)
    {
        int i0 = i;
        int i1 = i0 + 1;
        i += m_iVSamples;
        int i2 = i;
        int i3 = i2 + 1;
        for (iV = 0; iV < m_iVSamples-1; iV++, aiLocalIndex += 6)
        {
            aiLocalIndex[0] = i0;
            aiLocalIndex[1] = i1;
            aiLocalIndex[2] = i2;
            aiLocalIndex[3] = i1;
            aiLocalIndex[4] = i3;
            aiLocalIndex[5] = i2;
            i0++;
            i1++;
            i2++;
            i3++;
        }
    }

    if (bDoubleSided)
    {
        // disable culling
        CullState* pkCS = WM4_NEW CullState;
        pkCS->Enabled = false;
        AttachGlobalState(pkCS);
    }

    UpdateModelBound();
}
//----------------------------------------------------------------------------
RectangleSurface::RectangleSurface ()
{
    m_pkSurface = 0;
}
//----------------------------------------------------------------------------
RectangleSurface::~RectangleSurface ()
{
    WM4_DELETE m_pkSurface;
}
//----------------------------------------------------------------------------
void RectangleSurface::UpdateSurface ()
{
    float fUMin = m_pkSurface->GetUMin();
    float fUDelta = (m_pkSurface->GetUMax() - fUMin)/(float)(m_iUSamples-1);
    float fVMin = m_pkSurface->GetVMin();
    float fVDelta = (m_pkSurface->GetVMax() - fVMin)/(float)(m_iVSamples-1);

    for (int iU = 0, i = 0; iU < m_iUSamples; iU++)
    {
        float fU = fUMin + fUDelta*iU;
        for (int iV = 0; iV < m_iVSamples; iV++)
        {
            float fV = fVMin + fVDelta*iV;
            VBuffer->Position3(i) = m_pkSurface->P(fU,fV);

            if (VBuffer->GetAttributes().HasNormal())
            {
                Vector3f kPos, kTan0, kTan1, kNormal;
                m_pkSurface->GetFrame(fU,fV,kPos,kTan0,kTan1,kNormal);
                VBuffer->Normal3(i) = kNormal;
            }

            i++;
        }
    }

    UpdateModelBound();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void RectangleSurface::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iUSamples);
    rkStream.Read(m_iVSamples);

    // TO DO.  See note in RectangleSurface::Save.
    m_pkSurface = 0;

    WM4_END_DEBUG_STREAM_LOAD(RectangleSurface);
}
//----------------------------------------------------------------------------
void RectangleSurface::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool RectangleSurface::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void RectangleSurface::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iUSamples);
    rkStream.Write(m_iVSamples);

    // TO DO.  The class ParametricSurface3 is abstract and does not know
    // about the data representation for the derived-class object that is
    // passed to the RectangleSurface constructor.  Because of this, any
    // loaded RectangleSurface object will require the application to
    // manually set the surface via the Surface() member function.
    //
    // Streaming support should probably be added to the surface classes.

    WM4_END_DEBUG_STREAM_SAVE(RectangleSurface);
}
//----------------------------------------------------------------------------
int RectangleSurface::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return TriMesh::GetDiskUsed(rkVersion) +
        sizeof(m_iUSamples) +
        sizeof(m_iVSamples);
}
//----------------------------------------------------------------------------
StringTree* RectangleSurface::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("u samples =",m_iUSamples));
    pkTree->Append(Format("v samples =",m_iVSamples));

    // children
    pkTree->Append(TriMesh::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
