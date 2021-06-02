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
#include "Wm4SurfaceMesh.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,SurfaceMesh,TriMesh);
WM4_IMPLEMENT_STREAM(SurfaceMesh);

//----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer,
    Vector2fArray* pkParams, SurfacePatchPtr* aspkPatch,
    bool bAllowDynamicChange)
    :
    TriMesh(pkVBuffer,pkIBuffer)
{
    assert(pkParams && pkParams->GetData());
    assert(aspkPatch);

    m_spkOrigVBuffer = pkVBuffer;
    m_spkOrigIBuffer = pkIBuffer;
    m_spkOrigParams = pkParams;
    m_aspkPatch = aspkPatch;
    m_iLevel = 0;
    m_iFullVQuantity = m_spkOrigVBuffer->GetVertexQuantity();
    m_iPatchQuantity = m_spkOrigIBuffer->GetIndexQuantity()/3;

    m_bAllowDynamicChange = bAllowDynamicChange;
    if (m_bAllowDynamicChange)
    {
        m_akSInfo = WM4_NEW SurfaceInfo[m_iFullVQuantity];
        InitializeSurfaceInfo();
    }
    else
    {
        m_akSInfo = 0;
    }
}
//----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh ()
{
    m_aspkPatch = 0;
    m_iLevel = 0;
    m_iFullVQuantity = 0;
    m_iPatchQuantity = 0;
    m_bAllowDynamicChange = false;
    m_akSInfo = 0;
}
//----------------------------------------------------------------------------
SurfaceMesh::~SurfaceMesh ()
{
    WM4_DELETE[] m_aspkPatch;
    WM4_DELETE[] m_akSInfo;
}
//----------------------------------------------------------------------------
void SurfaceMesh::SetLevel (int iLevel)
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
    int iVQTotal, iEQTotal, iTQTotal;
    THashSet<Edge>* pkESet;
    Triangle* akTriangle;
    Allocate(iVQTotal,iEQTotal,pkESet,iTQTotal,akTriangle);

    // subdivide the mesh
    int iVQ = m_spkOrigVBuffer->GetVertexQuantity();
    int iEQ = pkESet->GetQuantity();
    int iTQ = m_spkOrigIBuffer->GetIndexQuantity()/3;
    int i;
    for (i = 0; i < m_iLevel; i++)
    {
        int iVQNext = iVQ + iEQ;
        int iEQNext = 2*iEQ + 3*iTQ;
        int iTQNext = 4*iTQ;
        Subdivide(iVQ,iEQ,pkESet,iTQ,akTriangle);
        assert(iVQ == iVQNext && iEQ == iEQNext && iTQ == iTQNext);
        iVQ = iVQNext;
        iEQ = iEQNext;
        iTQ = iTQNext;
    }

    // check to make sure the implementation is correct
    assert(iVQ == iVQTotal && iEQ == iEQTotal && iTQ == iTQTotal);
    m_iFullVQuantity = iVQTotal;

    UpdateModelBound();

    int iIQTotal = 3*iTQTotal;
    IBuffer = WM4_NEW IndexBuffer(iIQTotal);
    int* piIndex = IBuffer->GetData();
    for (i = 0; i < iTQTotal; i++)
    {
        Triangle& rkT = akTriangle[i];
        for (int j = 0; j < 3; j++)
        {
            *piIndex++ = rkT.V[j];
        }
    }

    WM4_DELETE pkESet;
    WM4_DELETE[] akTriangle;

    IBuffer->Release();
    OnDynamicChange();
}
//----------------------------------------------------------------------------
void SurfaceMesh::Allocate (int& riVQTotal, int& riEQTotal,
    THashSet<Edge>*& rpkESet, int& riTQTotal, Triangle*& rakTriangle)
{
    // the number of original vertices
    int iVQOrig = m_spkOrigVBuffer->GetVertexQuantity();

    // the number of original indices and triangles
    int iIQOrig = m_spkOrigIBuffer->GetIndexQuantity();
    int iTQOrig = iIQOrig/3;

    // count the number of unique edges in the mesh
    const int* piIndex = m_spkOrigIBuffer->GetData();
    rpkESet = WM4_NEW THashSet<Edge>(iIQOrig);
    rpkESet->UserHashFunction = &Edge::HashFunction;
    int i, iV0, iV1, iV2;
    for (i = 0; i < iTQOrig; i++)
    {
        // get the vertex indices for the triangle
        iV0 = *piIndex++;
        iV1 = *piIndex++;
        iV2 = *piIndex++;

        // store the edges with the vertex indices
        rpkESet->Insert(Edge(iV0,iV1));
        rpkESet->Insert(Edge(iV1,iV2));
        rpkESet->Insert(Edge(iV2,iV0));
    }
    int iEQOrig = rpkESet->GetQuantity();
    WM4_DELETE rpkESet;

    // determine the various quantities needed for subdivision
    riVQTotal = iVQOrig;
    riEQTotal = iEQOrig;
    riTQTotal = iTQOrig;
    for (i = 0; i < m_iLevel; i++)
    {
        riVQTotal = riVQTotal + riEQTotal;
        riEQTotal = 2*riEQTotal + 3*riTQTotal;
        riTQTotal = 4*riTQTotal;
    }

    // allocate storage for the subdivision
    VBuffer = WM4_NEW VertexBuffer(m_spkOrigVBuffer->GetAttributes(),
        riVQTotal);
    rpkESet = WM4_NEW THashSet<Edge>(riEQTotal);
    rpkESet->UserHashFunction = &Edge::HashFunction;
    rakTriangle = WM4_NEW Triangle[riTQTotal];
    if (m_bAllowDynamicChange)
    {
        WM4_DELETE[] m_akSInfo;
        m_akSInfo = WM4_NEW SurfaceInfo[riVQTotal];
        InitializeSurfaceInfo();
    }

    // copy the original vertices to the storage
    size_t uiSize = m_spkOrigVBuffer->GetChannelQuantity()*sizeof(float);
    System::Memcpy(VBuffer->GetData(),uiSize,m_spkOrigVBuffer->GetData(),
        uiSize);

    // initialize the edge and triangle data structures
    const Vector2f* pkOrigParam = m_spkOrigParams->GetData();
    piIndex = m_spkOrigIBuffer->GetData();
    for (i = 0; i < iTQOrig; i++)
    {
        Triangle& rkT = rakTriangle[i];

        // surface affecting the triangle
        rkT.Patch = m_aspkPatch[i];

        // get the vertex indices for the triangle
        rkT.V[0] = iV0 = *piIndex++;
        rkT.V[1] = iV1 = *piIndex++;
        rkT.V[2] = iV2 = *piIndex++;

        // get the parameters associated with the vertices
        const Vector2f& rkP0 = *pkOrigParam++;
        const Vector2f& rkP1 = *pkOrigParam++;
        const Vector2f& rkP2 = *pkOrigParam++;

        // store the edges with the surface and parameter values
        InsertInto(rpkESet,rkT.Patch,iV0,iV1,rkP0,rkP1,1);
        InsertInto(rpkESet,rkT.Patch,iV1,iV2,rkP1,rkP2,1);
        InsertInto(rpkESet,rkT.Patch,iV2,iV0,rkP2,rkP0,1);
    }

    assert(rpkESet->GetQuantity() == iEQOrig);
}
//----------------------------------------------------------------------------
void SurfaceMesh::Subdivide (int& riVQ, int& riEQ, THashSet<Edge>* pkESet,
    int& riTQ, Triangle* akTriangle)
{
    const Attributes& rkAttr = VBuffer->GetAttributes();
    Edge* pkE = pkESet->GetFirst();
    while (pkE)
    {
        // Compute the vertex at the average of the parameter values.
        // The index and parameter values of the new vertex will be used
        // when subdividing the triangles.
        pkE->ParamMid = 0.5f*(pkE->Param[0] + pkE->Param[1]);
        VBuffer->Position3(riVQ) = pkE->Patch->P(pkE->ParamMid.X(),
            pkE->ParamMid.Y());

        // Compute the average of vertex attributes.
        if (rkAttr.HasNormal())
        {
            // Use the curve normal.
            Vector3f kPos, kTan0, kTan1, kNormal;
            pkE->Patch->GetFrame(pkE->ParamMid.X(),pkE->ParamMid.Y(),kPos,
                kTan0,kTan1,kNormal);
            VBuffer->Normal3(riVQ) = kNormal;
        }

        int iV0 = pkE->V[0];
        int iV1 = pkE->V[1];
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
            m_akSInfo[riVQ].Patch = pkE->Patch;
            m_akSInfo[riVQ].Param = pkE->ParamMid;
        }

        pkE->VMid = riVQ++;

        pkE = pkESet->GetNext();
    }

    int i, iMax = riTQ;  // allows riTQ to change inside the loop
    for (i = 0; i < iMax; i++)
    {
        Triangle& rkT0 = akTriangle[i];

        // get the vertices
        int iV0 = rkT0.V[0];
        int iV1 = rkT0.V[1];
        int iV2 = rkT0.V[2];

        // get the edge information
        Edge* pkE01 = pkESet->Get(Edge(iV0,iV1));
        Edge* pkE12 = pkESet->Get(Edge(iV1,iV2));
        Edge* pkE20 = pkESet->Get(Edge(iV2,iV0));
        Vector2f kP0 = pkE01->Param[pkE01->V[0] == iV0 ? 0 : 1];
        Vector2f kP1 = pkE12->Param[pkE12->V[0] == iV1 ? 0 : 1];
        Vector2f kP2 = pkE20->Param[pkE20->V[0] == iV2 ? 0 : 1];

        // get the midpoint information
        int iV01 = pkE01->VMid;
        Vector2f kP01 = pkE01->ParamMid;
        int iV12 = pkE12->VMid;
        Vector2f kP12 = pkE12->ParamMid;
        int iV20 = pkE20->VMid;
        Vector2f kP20 = pkE20->ParamMid;

        // if done with edges, remove them
        if (--pkE01->References == 0)
        {
            pkESet->Remove(*pkE01);
        }
        if (--pkE12->References == 0)
        {
            pkESet->Remove(*pkE12);
        }
        if (--pkE20->References == 0)
        {
            pkESet->Remove(*pkE20);
        }

        // insert the six new boundary edges
        InsertInto(pkESet,rkT0.Patch,iV0,iV01,kP0,kP01,1);
        InsertInto(pkESet,rkT0.Patch,iV01,iV1,kP01,kP1,1);
        InsertInto(pkESet,rkT0.Patch,iV1,iV12,kP1,kP12,1);
        InsertInto(pkESet,rkT0.Patch,iV12,iV2,kP12,kP2,1);
        InsertInto(pkESet,rkT0.Patch,iV2,iV20,kP2,kP20,1);
        InsertInto(pkESet,rkT0.Patch,iV20,iV0,kP20,kP0,1);

        // insert the three new interior edges
        InsertInto(pkESet,rkT0.Patch,iV01,iV12,kP01,kP12,2);
        InsertInto(pkESet,rkT0.Patch,iV12,iV20,kP12,kP20,2);
        InsertInto(pkESet,rkT0.Patch,iV20,iV01,kP20,kP01,2);

        // the old triangle is replaced by the new interior triangle
        rkT0.V[0] = iV01;
        rkT0.V[1] = iV12;
        rkT0.V[2] = iV20;

        // insert the remaining triangles in new slots
        Triangle& rkT1 = akTriangle[riTQ++];
        rkT1.Patch = rkT0.Patch;
        rkT1.V[0] = iV0;
        rkT1.V[1] = iV01;
        rkT1.V[2] = iV20;

        Triangle& rkT2 = akTriangle[riTQ++];
        rkT2.Patch = rkT0.Patch;
        rkT2.V[0] = iV01;
        rkT2.V[1] = iV1;
        rkT2.V[2] = iV12;

        Triangle& rkT3 = akTriangle[riTQ++];
        rkT3.Patch = rkT0.Patch;
        rkT3.V[0] = iV20;
        rkT3.V[1] = iV12;
        rkT3.V[2] = iV2;
    }

    riEQ = pkESet->GetQuantity();
}
//----------------------------------------------------------------------------
void SurfaceMesh::InsertInto (THashSet<Edge>* pkESet,
    SurfacePatchPtr spkPatch, int iV0, int iV1, const Vector2f& rkParam0,
    const Vector2f& rkParam1, int iNewReferences)
{
    Edge* pkE = pkESet->Insert(Edge(iV0,iV1));
    if (pkE->References == 0)
    {
        pkE->Patch = spkPatch;
        pkE->Param[0] = rkParam0;
        pkE->Param[1] = rkParam1;
    }
    pkE->References += iNewReferences;
}
//----------------------------------------------------------------------------
void SurfaceMesh::OnDynamicChange ()
{
    if (m_bAllowDynamicChange)
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            SurfaceInfo& rkSI = m_akSInfo[i];
            VBuffer->Position3(i) = rkSI.Patch->P(rkSI.Param.X(),
                rkSI.Param.Y());
        }
        VBuffer->Release();
        UpdateMS();
    }
}
//----------------------------------------------------------------------------
void SurfaceMesh::Lock ()
{
    if (m_spkOrigVBuffer)
    {
        m_spkOrigVBuffer = 0;
        m_spkOrigIBuffer = 0;
        m_spkOrigParams = 0;
        WM4_DELETE[] m_aspkPatch;
        m_aspkPatch = 0;
    }

    // The array m_akSInfo is not deleted.  This allows dynamic updates of
    // the vertices of the mesh, even though the mesh is "locked".
}
//----------------------------------------------------------------------------
void SurfaceMesh::InitializeSurfaceInfo ()
{
    // Because vertices are shared by triangles, the last visited triangle
    // for a vertex donates its patch and parameter values to that vertex.
    const int* piIndex = m_spkOrigIBuffer->GetData();
    const Vector2f* pkOrigParam = m_spkOrigParams->GetData();
    for (int i = 0; i < m_iPatchQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;
        m_akSInfo[iV0].Patch = m_aspkPatch[i];
        m_akSInfo[iV0].Param = *pkOrigParam++;
        m_akSInfo[iV1].Patch = m_aspkPatch[i];
        m_akSInfo[iV1].Param = *pkOrigParam++;
        m_akSInfo[iV2].Patch = m_aspkPatch[i];
        m_akSInfo[iV2].Param = *pkOrigParam++;
    }
}
//----------------------------------------------------------------------------
SurfaceMesh::Edge::Edge (int iV0, int iV1)
{
    Patch = 0;
    V[0] = iV0;
    V[1] = iV1;
    Param[0] = Vector2f::ZERO;
    Param[1] = Vector2f::ZERO;
    VMid = -1;
    ParamMid = Vector2f::ZERO;
    References = 0;
}
//----------------------------------------------------------------------------
bool SurfaceMesh::Edge::operator== (const Edge& rkEdge) const
{
    return (V[0] == rkEdge.V[0] && V[1] == rkEdge.V[1])
        || (V[1] == rkEdge.V[0] && V[0] == rkEdge.V[1]);
}
//----------------------------------------------------------------------------
bool SurfaceMesh::Edge::operator!= (const Edge& rkEdge) const
{
    return !(operator==(rkEdge));
}
//----------------------------------------------------------------------------
int SurfaceMesh::Edge::HashFunction (const Edge& rkEdge)
{
    return rkEdge.V[0] ^ rkEdge.V[1];
}
//----------------------------------------------------------------------------
SurfaceMesh::Triangle::Triangle ()
{
    Patch = 0;
    for (int i = 0; i < 3; i++)
    {
        V[i] = -1;
    }
}
//----------------------------------------------------------------------------
SurfaceMesh::SurfaceInfo::SurfaceInfo ()
{
    Patch = 0;
    Param = Vector2f::ZERO;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* SurfaceMesh::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = TriMesh::GetObjectByName(rkName);
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

    if (m_spkOrigIBuffer)
    {
        pkFound = m_spkOrigIBuffer->GetObjectByName(rkName);
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

    if (m_aspkPatch)
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            if (m_aspkPatch[i])
            {
                pkFound = m_aspkPatch[i]->GetObjectByName(rkName);
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
void SurfaceMesh::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    TriMesh::GetAllObjectsByName(rkName,rkObjects);

    if (m_spkOrigVBuffer)
    {
        m_spkOrigVBuffer->GetAllObjectsByName(rkName,rkObjects);
    }

    if (m_spkOrigIBuffer)
    {
        m_spkOrigIBuffer->GetAllObjectsByName(rkName,rkObjects);
    }

    if (m_spkOrigParams)
    {
        m_spkOrigParams->GetAllObjectsByName(rkName,rkObjects);
    }

    if (m_aspkPatch)
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            if (m_aspkPatch[i])
            {
                m_aspkPatch[i]->GetAllObjectsByName(rkName,rkObjects);
            }
        }
    }
}
//----------------------------------------------------------------------------
Object* SurfaceMesh::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = TriMesh::GetObjectByID(uiID);
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

    if (m_spkOrigIBuffer)
    {
        pkFound = m_spkOrigIBuffer->GetObjectByID(uiID);
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

    if (m_aspkPatch)
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            if (m_aspkPatch[i])
            {
                pkFound = m_aspkPatch[i]->GetObjectByID(uiID);
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
void SurfaceMesh::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iLevel);
    rkStream.Read(m_iFullVQuantity);
    rkStream.Read(m_iPatchQuantity);
    rkStream.Read(m_bAllowDynamicChange);

    // link data
    Object* pkObject;

    rkStream.Read(pkObject);  // m_spkOrigVBuffer
    pkLink->Add(pkObject);
    bool bLocked = (pkObject == 0);

    rkStream.Read(pkObject);  // m_spkOrigIBuffer
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // m_spkOrigParams
    pkLink->Add(pkObject);

    if (!bLocked)
    {
        m_aspkPatch = WM4_NEW SurfacePatchPtr[m_iPatchQuantity];
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_aspkPatch[i]
            pkLink->Add(pkObject);
        }
    }

    if (m_bAllowDynamicChange)
    {
        m_akSInfo = WM4_NEW SurfaceInfo[m_iFullVQuantity];
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            rkStream.Read(pkObject);
            pkLink->Add(pkObject);
            rkStream.Read(m_akSInfo[i].Param);
        }
    }

    WM4_END_DEBUG_STREAM_LOAD(SurfaceMesh);
}
//----------------------------------------------------------------------------
void SurfaceMesh::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkOrigVBuffer = (VertexBuffer*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    m_spkOrigIBuffer = (IndexBuffer*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    m_spkOrigParams = (Vector2fArray*)rkStream.GetFromMap(pkLinkID);

    if (m_aspkPatch)
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_aspkPatch[i] = (SurfacePatch*)rkStream.GetFromMap(pkLinkID);
        }
    }

    if (m_akSInfo)
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_akSInfo[i].Patch = (SurfacePatch*)rkStream.GetFromMap(pkLinkID);
        }
    }
}
//----------------------------------------------------------------------------
bool SurfaceMesh::Register (Stream& rkStream) const
{
    if (!TriMesh::Register(rkStream))
    {
        return false;
    }

    if (m_spkOrigVBuffer)
    {
        m_spkOrigVBuffer->Register(rkStream);
    }

    if (m_spkOrigIBuffer)
    {
        m_spkOrigIBuffer->Register(rkStream);
    }

    if (m_spkOrigParams)
    {
        m_spkOrigParams->Register(rkStream);
    }

    if (m_aspkPatch)
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            if (m_aspkPatch[i])
            {
                m_aspkPatch[i]->Register(rkStream);
            }
        }
    }

    if (m_akSInfo)
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            if (m_akSInfo[i].Patch)
            {
                m_akSInfo[i].Patch->Register(rkStream);
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void SurfaceMesh::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iLevel);
    rkStream.Write(m_iFullVQuantity);
    rkStream.Write(m_iPatchQuantity);
    rkStream.Write(m_bAllowDynamicChange);

    // link data
    rkStream.Write(m_spkOrigVBuffer);
    rkStream.Write(m_spkOrigIBuffer);
    rkStream.Write(m_spkOrigParams);

    if (m_aspkPatch)
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            rkStream.Write(m_aspkPatch[i]);
        }
    }

    if (m_akSInfo)
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            rkStream.Write(m_akSInfo[i].Patch);
            rkStream.Write(m_akSInfo[i].Param);
        }
    }

    WM4_END_DEBUG_STREAM_SAVE(SurfaceMesh);
}
//----------------------------------------------------------------------------
int SurfaceMesh::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = TriMesh::GetDiskUsed(rkVersion) +
        sizeof(m_iLevel) +
        sizeof(m_iFullVQuantity) +
        sizeof(m_iPatchQuantity) +
        sizeof(char) +  // m_bAllowDynamicChange
        WM4_PTRSIZE(m_spkOrigVBuffer) +
        WM4_PTRSIZE(m_spkOrigIBuffer) +
        WM4_PTRSIZE(m_spkOrigParams);

    if (m_aspkPatch)
    {
        iSize += m_iPatchQuantity*WM4_PTRSIZE(m_aspkPatch[0]);
    }

    if (m_akSInfo)
    {
        iSize += m_iFullVQuantity*(WM4_PTRSIZE(m_akSInfo[0].Patch) +
            sizeof(Vector2f));
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* SurfaceMesh::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("level =",m_iLevel));
    pkTree->Append(Format("locked =",IsLocked()));

    // children
    pkTree->Append(TriMesh::SaveStrings());

    if (m_spkOrigVBuffer)
    {
        pkTree->Append(m_spkOrigVBuffer->SaveStrings("original vbuffer"));
    }

    if (m_spkOrigIBuffer)
    {
        pkTree->Append(m_spkOrigIBuffer->SaveStrings("original ibuffer"));
    }

    if (m_spkOrigParams)
    {
        pkTree->Append(m_spkOrigParams->SaveStrings("original parameters"));
    }

    for (int i = 0; i < m_iPatchQuantity; i++)
    {
        if (m_aspkPatch[i])
        {
            pkTree->Append(m_aspkPatch[i]->SaveStrings());
        }
    }

    return pkTree;
}
//----------------------------------------------------------------------------
