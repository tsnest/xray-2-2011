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
// Version: 4.0.3 (2007/09/24)

#include "Wm4GraphicsPCH.h"
#include "Wm4CurveMesh.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,CurveMesh,Polyline);
WM4_IMPLEMENT_STREAM(CurveMesh);

//----------------------------------------------------------------------------
CurveMesh::CurveMesh (VertexBuffer* pkVBuffer, CurveSegmentPtr* aspkSegment,
    FloatArray* pkParams, bool bAllowDynamicChange)
    :
    Polyline(pkVBuffer,false,true)
{
    assert(pkParams && pkParams->GetData());
    assert(aspkSegment);

    m_spkOrigVBuffer = pkVBuffer;
    m_spkOrigParams = pkParams;
    m_aspkSegment = aspkSegment;
    m_iLevel = 0;
    m_iFullVQuantity = pkVBuffer->GetVertexQuantity();
    m_iSegmentQuantity = pkVBuffer->GetVertexQuantity() - 1;

    m_bAllowDynamicChange = bAllowDynamicChange;
    if (m_bAllowDynamicChange)
    {
        m_akCInfo = WM4_NEW CurveInfo[m_iFullVQuantity];
        InitializeCurveInfo();
    }
    else
    {
        m_akCInfo = 0;
    }
}
//----------------------------------------------------------------------------
CurveMesh::CurveMesh ()
{
    m_aspkSegment = 0;
    m_iLevel = 0;
    m_iFullVQuantity = 0;
    m_iSegmentQuantity = 0;
    m_bAllowDynamicChange = false;
    m_akCInfo = 0;
}
//----------------------------------------------------------------------------
CurveMesh::~CurveMesh ()
{
    WM4_DELETE[] m_aspkSegment;
    WM4_DELETE[] m_akCInfo;
}
//----------------------------------------------------------------------------
void CurveMesh::SetLevel (int iLevel)
{
    if (!m_spkOrigVBuffer)
    {
        // the mesh is locked, no subdivision allowed
        return;
    }

    assert(iLevel >= 0);
    if (iLevel == m_iLevel)
    {
        return;
    }
    m_iLevel = iLevel;

    // allocate the storage for subdivision
    int iVQTotal, iEQTotal;
    Edge* akEdge;
    Allocate(iVQTotal,iEQTotal,akEdge);

    // subdivide the mesh
    int iVQ = m_iSegmentQuantity + 1;
    int iEQ = m_iSegmentQuantity;
    int i;
    for (i = 0; i < m_iLevel; i++)
    {
        int iVQNext = iVQ + iEQ;
        int iEQNext = 2*iEQ;
        Subdivide(iVQ,iEQ,akEdge);
        assert(iVQ == iVQNext && iEQ == iEQNext);
        iVQ = iVQNext;
        iEQ = iEQNext;
    }

    // check to make sure the implementation is correct
    assert(iVQ == iVQTotal && iEQ == iEQTotal);
    m_iActiveQuantity = iVQTotal;
    m_iFullVQuantity = iVQTotal;

    UpdateModelBound();

    IBuffer = WM4_NEW IndexBuffer(iVQTotal);
    int* piIndex = IBuffer->GetData();
    for (i = 0; i < iEQTotal; i++)
    {
        *piIndex++ = akEdge[i].V[0];
    }
    *piIndex++ = akEdge[iEQTotal-1].V[1];

    WM4_DELETE[] akEdge;

    IBuffer->Release();
    OnDynamicChange();
}
//----------------------------------------------------------------------------
void CurveMesh::Allocate (int& riVQTotal, int& riEQTotal, Edge*& rakEdge)
{
    // the number of original vertices
    int iVQOrig = m_iSegmentQuantity + 1;

    // the number of original edges
    int iEQOrig = m_iSegmentQuantity;

    // determine the various quantities needed for subdivision
    riVQTotal = iVQOrig;
    riEQTotal = iEQOrig;
    int i;
    for (i = 0; i < m_iLevel; i++)
    {
        riVQTotal = riVQTotal + riEQTotal;
        riEQTotal = 2*riEQTotal;
    }

    // allocate storage for the subdivision
    VBuffer = WM4_NEW VertexBuffer(m_spkOrigVBuffer->GetAttributes(),
        riVQTotal);
    rakEdge = WM4_NEW Edge[riEQTotal];
    if (m_bAllowDynamicChange)
    {
        WM4_DELETE[] m_akCInfo;
        m_akCInfo = WM4_NEW CurveInfo[riVQTotal];
        InitializeCurveInfo();
    }

    // compute the end point vertices
    // copy the original vertices to the storage
    size_t uiSize = m_spkOrigVBuffer->GetChannelQuantity()*sizeof(float);
    System::Memcpy(VBuffer->GetData(),uiSize,m_spkOrigVBuffer->GetData(),
        uiSize);

    // initialize the edge data structures
    const float* pfOrigParam = m_spkOrigParams->GetData();
    int iIndex = 0;
    for (i = 0; i < iEQOrig; i++)
    {
        Edge& rkE = rakEdge[i];

        // curve affecting the edge
        rkE.Segment = m_aspkSegment[i];

        // get the vertex indices for the edge
        rkE.V[0] = iIndex++;
        rkE.V[1] = iIndex;

        // get the parameters associated with the vertices
        rkE.Param[0] = *pfOrigParam++;
        rkE.Param[1] = *pfOrigParam++;
    }
}
//----------------------------------------------------------------------------
void CurveMesh::Subdivide (int& riVQ, int& riEQ, Edge* akEdge)
{
    // The subdivision is computed in-place, so you need to fill in the new
    // edge slots starting from the end of the array and working towards the
    // beginning.  This avoid overwriting old edges not yet subdivided.
    const Attributes& rkAttr = VBuffer->GetAttributes();
    for (int iOld = riEQ-1, iCurr = 2*riEQ-1; iOld >= 0; iOld--)
    {
        Edge& rkE = akEdge[iOld];

        // compute the vertex at the average of the parameter values
        float fParamMid = 0.5f*(rkE.Param[0] + rkE.Param[1]);
        VBuffer->Position3(riVQ) = rkE.Segment->P(fParamMid);

        // Compute the average of vertex attributes.
        if (rkAttr.HasNormal())
        {
            // Use the curve normal.
            VBuffer->Normal3(riVQ) = rkE.Segment->Normal(fParamMid);
        }

        int iV0 = rkE.V[0];
        int iV1 = rkE.V[1];
        int iUnit, i;
        float* afData0;
        float* afData1;
        float* afData;
        for (iUnit = 0; iUnit < rkAttr.GetMaxColors(); iUnit++)
        {
            if (rkAttr.HasColor(iUnit))
            {
                afData0 = VBuffer->ColorTuple(iUnit,iV0);
                afData1 = VBuffer->ColorTuple(iUnit,iV1);
                afData = VBuffer->ColorTuple(iUnit,riVQ);
                for (i = 0; i < rkAttr.GetColorChannels(iUnit); i++)
                {
                    afData[i] = 0.5f*(afData0[i] + afData1[i]);
                }
            }
        }

        for (iUnit = 0; iUnit < rkAttr.GetMaxTCoords(); iUnit++)
        {
            if (rkAttr.HasTCoord(iUnit))
            {
                afData0 = VBuffer->TCoordTuple(iUnit,iV0);
                afData1 = VBuffer->TCoordTuple(iUnit,iV1);
                afData = VBuffer->TCoordTuple(iUnit,riVQ);
                for (i = 0; i < rkAttr.GetTCoordChannels(iUnit); i++)
                {
                    afData[i] = 0.5f*(afData0[i] + afData1[i]);
                }
            }
        }

        if (m_bAllowDynamicChange)
        {
            m_akCInfo[riVQ].Segment = rkE.Segment;
            m_akCInfo[riVQ].Param = fParamMid;
        }

        // subdivide the edge
        Edge& rkE1 = akEdge[iCurr--];
        rkE1.Segment = rkE.Segment;
        rkE1.V[0] = riVQ;
        rkE1.V[1] = rkE.V[1];
        rkE1.Param[0] = fParamMid;
        rkE1.Param[1] = rkE.Param[1];

        Edge& rkE0 = akEdge[iCurr--];
        rkE0.Segment = rkE.Segment;
        rkE0.V[0] = rkE.V[0];
        rkE0.V[1] = riVQ;
        rkE0.Param[0] = rkE.Param[0];
        rkE0.Param[1] = fParamMid;

        riVQ++;
    }

    riEQ = 2*riEQ;
}
//----------------------------------------------------------------------------
void CurveMesh::OnDynamicChange ()
{
    if (m_bAllowDynamicChange)
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            CurveInfo& rkCI = m_akCInfo[i];
            VBuffer->Position3(i) = rkCI.Segment->P(rkCI.Param);
        }
        VBuffer->Release();
        UpdateMS();
    }
}
//----------------------------------------------------------------------------
void CurveMesh::Lock ()
{
    if (m_spkOrigVBuffer)
    {
        m_spkOrigVBuffer = 0;
        m_spkOrigParams = 0;
        WM4_DELETE[] m_aspkSegment;
        m_aspkSegment = 0;
    }

    // The array m_akCInfo is not deleted.  This allows dynamic updates of
    // the vertices of the mesh, even though the mesh is "locked".
}
//----------------------------------------------------------------------------
void CurveMesh::InitializeCurveInfo ()
{
    // Because vertices are shared by edges, the last visited edge for a
    // vertex donates its segment and parameter values to that vertex.
    const float* afParam = m_spkOrigParams->GetData();
    int iIndex = 0;
    for (int i = 0; i < m_iSegmentQuantity; i++)
    {
        int iV0 = iIndex++;
        int iV1 = iIndex;
        m_akCInfo[iV0].Segment = m_aspkSegment[i];
        m_akCInfo[iV0].Param = *afParam++;
        m_akCInfo[iV1].Segment = m_aspkSegment[i];
        m_akCInfo[iV1].Param = *afParam++;
    }
}
//----------------------------------------------------------------------------
CurveMesh::Edge::Edge ()
{
    Segment = 0;
    V[0] = -1;
    V[1] = -1;
    Param[0] = 0.0f;
    Param[1] = 0.0f;
}
//----------------------------------------------------------------------------
CurveMesh::CurveInfo::CurveInfo ()
{
    Segment = 0;
    Param = 0.0f;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* CurveMesh::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Polyline::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkOrigVBuffer)
    {
        pkFound = m_spkOrigVBuffer->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (m_spkOrigParams)
    {
        pkFound = m_spkOrigParams->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (m_aspkSegment)
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if (m_aspkSegment[i])
            {
                pkFound = m_aspkSegment[i]->GetObjectByName(rkName);
                if (pkFound)
                {
                    return pkFound;
                }
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void CurveMesh::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Polyline::GetAllObjectsByName(rkName,rkObjects);

    if (m_spkOrigVBuffer)
    {
        m_spkOrigVBuffer->GetAllObjectsByName(rkName,rkObjects);
    }

    if (m_spkOrigParams)
    {
        m_spkOrigParams->GetAllObjectsByName(rkName,rkObjects);
    }

    if (m_aspkSegment)
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if (m_aspkSegment[i])
            {
                m_aspkSegment[i]->GetAllObjectsByName(rkName,rkObjects);
            }
        }
    }
}
//----------------------------------------------------------------------------
Object* CurveMesh::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Polyline::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkOrigVBuffer)
    {
        pkFound = m_spkOrigVBuffer->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (m_spkOrigParams)
    {
        pkFound = m_spkOrigParams->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (m_aspkSegment)
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if (m_aspkSegment[i])
            {
                pkFound = m_aspkSegment[i]->GetObjectByID(uiID);
                if (pkFound)
                {
                    return pkFound;
                }
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void CurveMesh::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Polyline::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iSegmentQuantity);
    rkStream.Read(m_iLevel);
    rkStream.Read(m_iFullVQuantity);
    rkStream.Read(m_bAllowDynamicChange);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkOrigVBuffer
    pkLink->Add(pkObject);
    rkStream.Read(pkObject);  // m_spkOrigParams
    pkLink->Add(pkObject);
    bool bLocked = (pkObject == 0);

    if (!bLocked)
    {
        m_aspkSegment = WM4_NEW CurveSegmentPtr[m_iSegmentQuantity];
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_aspkSegment[i]
            pkLink->Add(pkObject);
        }
    }

    if (m_bAllowDynamicChange)
    {
        m_akCInfo = WM4_NEW CurveInfo[m_iFullVQuantity];
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_akInfo[i].Segment
            pkLink->Add(pkObject);
            rkStream.Read(m_akCInfo[i].Param);
        }
    }

    WM4_END_DEBUG_STREAM_LOAD(CurveMesh);
}
//----------------------------------------------------------------------------
void CurveMesh::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Polyline::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkOrigVBuffer = (VertexBuffer*)rkStream.GetFromMap(pkLinkID);
    pkLinkID = pkLink->GetLinkID();
    m_spkOrigParams = (FloatArray*)rkStream.GetFromMap(pkLinkID);

    if (m_aspkSegment)
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_aspkSegment[i] = (CurveSegment*)rkStream.GetFromMap(pkLinkID);
        }
    }

    if (m_akCInfo)
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_akCInfo[i].Segment =
                (CurveSegment*)rkStream.GetFromMap(pkLinkID);
        }
    }
}
//----------------------------------------------------------------------------
bool CurveMesh::Register (Stream& rkStream) const
{
    if (!Polyline::Register(rkStream))
    {
        return false;
    }

    if (m_spkOrigVBuffer)
    {
        m_spkOrigVBuffer->Register(rkStream);
    }

    if (m_spkOrigParams)
    {
        m_spkOrigParams->Register(rkStream);
    }

    if (m_aspkSegment)
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if (m_aspkSegment[i])
            {
                m_aspkSegment[i]->Register(rkStream);
            }
        }
    }

    if (m_akCInfo)
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            if (m_akCInfo[i].Segment)
            {
                m_akCInfo[i].Segment->Register(rkStream);
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void CurveMesh::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Polyline::Save(rkStream);

    // native data
    rkStream.Write(m_iSegmentQuantity);
    rkStream.Write(m_iLevel);
    rkStream.Write(m_iFullVQuantity);
    rkStream.Write(m_bAllowDynamicChange);

    // link data
    rkStream.Write(m_spkOrigVBuffer);
    rkStream.Write(m_spkOrigParams);

    if (m_aspkSegment)
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            rkStream.Write(m_aspkSegment[i]);
        }
    }


    if (m_akCInfo)
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            rkStream.Write(m_akCInfo[i].Segment);
            rkStream.Write(m_akCInfo[i].Param);
        }
    }

    WM4_END_DEBUG_STREAM_SAVE(CurveMesh);
}
//----------------------------------------------------------------------------
int CurveMesh::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Polyline::GetDiskUsed(rkVersion) +
        sizeof(m_iSegmentQuantity) +
        sizeof(m_iLevel) +
        sizeof(m_iFullVQuantity) +
        sizeof(char) +  // m_bAllowDynamicChange
        WM4_PTRSIZE(m_spkOrigVBuffer) +
        WM4_PTRSIZE(m_spkOrigParams);

    if (m_aspkSegment)
    {
        iSize += m_iSegmentQuantity*WM4_PTRSIZE(m_aspkSegment[0]);
    }

    if (m_akCInfo)
    {
        iSize += m_iFullVQuantity*(WM4_PTRSIZE(m_akCInfo[0].Segment) +
            sizeof(float));
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* CurveMesh::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("level =",m_iLevel));
    pkTree->Append(Format("locked =",IsLocked()));

    // children
    pkTree->Append(Polyline::SaveStrings());

    if (m_spkOrigVBuffer)
    {
        pkTree->Append(m_spkOrigVBuffer->SaveStrings("original vbuffer"));
    }

    if (m_spkOrigParams)
    {
        pkTree->Append(m_spkOrigParams->SaveStrings("original parameters"));
    }

    if (m_aspkSegment)
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if (m_aspkSegment[i])
            {
                pkTree->Append(m_aspkSegment[i]->SaveStrings());
            }
        }
    }

    return pkTree;
}
//----------------------------------------------------------------------------
