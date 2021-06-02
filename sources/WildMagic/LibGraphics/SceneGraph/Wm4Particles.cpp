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
// Version: 4.0.2 (2007/09/24)

#include "Wm4GraphicsPCH.h"
#include "Wm4Particles.h"
#include "Wm4Camera.h"
#include "Wm4Culler.h"
#include "Wm4Light.h"
#include "Wm4Vector2Array.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Particles,TriMesh);
WM4_IMPLEMENT_STREAM(Particles);

//----------------------------------------------------------------------------
Particles::Particles (const Attributes& rkAttr, Vector3fArray* pkLocations,
    FloatArray* pkSizes)
    :
    Locations(pkLocations),
    Sizes(pkSizes)
{
    // Create storage for model space quad vertices.
    int iLQuantity = Locations->GetQuantity();
    int iVQuantity = 4*iLQuantity;
    VBuffer = WM4_NEW VertexBuffer(rkAttr,iVQuantity);

    // Set any 2D texture coordinates to the standard ones.
    int i, j;
    for (int iUnit = 0; iUnit < rkAttr.GetMaxTCoords(); iUnit++)
    {
        if (rkAttr.HasTCoord(iUnit) && rkAttr.GetTCoordChannels(iUnit) == 2)
        {
            for (i = 0, j = 0; i < iLQuantity; i++)
            {
                VBuffer->TCoord2(iUnit,j++) = Vector2f(0.0f,0.0f);
                VBuffer->TCoord2(iUnit,j++) = Vector2f(1.0f,0.0f);
                VBuffer->TCoord2(iUnit,j++) = Vector2f(1.0f,1.0f);
                VBuffer->TCoord2(iUnit,j++) = Vector2f(0.0f,1.0f);
            }
        }
    }

    // Generate the connectivity for the quads.
    int iIQuantity = 6*iLQuantity;
    IBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int* aiIndex = IBuffer->GetData();
    for (i = 0, j = 0; i < iLQuantity; i++)
    {
        int iFI = 4*i, iFIp1 = iFI+1, iFIp2 = iFI+2, iFIp3 = iFI+3;
        aiIndex[j++] = iFI;
        aiIndex[j++] = iFIp1;
        aiIndex[j++] = iFIp2;
        aiIndex[j++] = iFI;
        aiIndex[j++] = iFIp2;
        aiIndex[j++] = iFIp3;
    }

    // Compute a bounding sphere based only on the particle locations.
    ModelBound->ComputeFromData(Locations);

    SizeAdjust = 1.0f;
    m_iActiveQuantity = iLQuantity;
}
//----------------------------------------------------------------------------
Particles::Particles ()
{
    SizeAdjust = 0.0f;
    m_iActiveQuantity = 0;
}
//----------------------------------------------------------------------------
Particles::~Particles ()
{
}
//----------------------------------------------------------------------------
void Particles::SetActiveQuantity (int iActiveQuantity)
{
    int iLQuantity = Locations->GetQuantity();
    if (0 <= iActiveQuantity && iActiveQuantity <= iLQuantity)
    {
        m_iActiveQuantity = iActiveQuantity;
    }
    else
    {
        m_iActiveQuantity = iLQuantity;
    }

    IBuffer->SetIndexQuantity(6*m_iActiveQuantity);
    IBuffer->Release();

    int iFourActiveQuantity = 4*m_iActiveQuantity;
    VBuffer->SetVertexQuantity(iFourActiveQuantity);
    VBuffer->Release();
}
//----------------------------------------------------------------------------
void Particles::GenerateParticles (const Camera* pkCamera)
{
    int i, j;

    // Get camera axis directions in model space of particles.
    Vector3f kUpR = (pkCamera->GetUVector() +
        pkCamera->GetRVector())*World.GetRotate();
    Vector3f kUmR = (pkCamera->GetUVector() -
        pkCamera->GetRVector())*World.GetRotate();

    // Normal vectors are towards the viewer.
    if (VBuffer->GetAttributes().HasNormal())
    {
        Vector3f kDir = -pkCamera->GetDVector()*World.GetRotate();
        for (j = 0; j < 4*m_iActiveQuantity; j++)
        {
            VBuffer->Normal3(j) = kDir;
        }
    }

    // Generate particle quadrilaterals as pairs of triangles.
    Vector3f* akLocation = Locations->GetData();
    float* afSize = Sizes->GetData();
    for (i = 0, j = 0; i < m_iActiveQuantity; i++)
    {
        Vector3f& rkCenter = akLocation[i];

        float fTrueSize = SizeAdjust*afSize[i];
        Vector3f kScaledUpR = fTrueSize*kUpR;
        Vector3f kScaledUmR = fTrueSize*kUmR;

        VBuffer->Position3(j++) = rkCenter - kScaledUpR;
        VBuffer->Position3(j++) = rkCenter - kScaledUmR;
        VBuffer->Position3(j++) = rkCenter + kScaledUpR;
        VBuffer->Position3(j++) = rkCenter + kScaledUmR;
    }

    UpdateMS(true);
}
//----------------------------------------------------------------------------
void Particles::GetVisibleSet (Culler& rkCuller, bool bNoCull)
{
    GenerateParticles(rkCuller.GetCamera());
    TriMesh::GetVisibleSet(rkCuller,bNoCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Particles::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = TriMesh::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (Locations)
    {
        pkFound = Locations->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (Sizes)
    {
        pkFound = Sizes->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void Particles::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    TriMesh::GetAllObjectsByName(rkName,rkObjects);

    if (Locations)
    {
        Locations->GetAllObjectsByName(rkName,rkObjects);
    }

    if (Sizes)
    {
        Sizes->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* Particles::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = TriMesh::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (Locations)
    {
        pkFound = Locations->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (Sizes)
    {
        pkFound = Sizes->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Particles::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(SizeAdjust);
    rkStream.Read(m_iActiveQuantity);

    // link data
    Object* pkObject;

    rkStream.Read(pkObject);  // Locations
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // Sizes
    pkLink->Add(pkObject);

    WM4_END_DEBUG_STREAM_LOAD(Particles);
}
//----------------------------------------------------------------------------
void Particles::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    Locations = (Vector3fArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    Sizes = (FloatArray*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Particles::Register (Stream& rkStream) const
{
    if (!TriMesh::Register(rkStream))
    {
        return false;
    }

    if (Locations)
    {
        Locations->Register(rkStream);
    }

    if (Sizes)
    {
        Sizes->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void Particles::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(SizeAdjust);
    rkStream.Write(m_iActiveQuantity);

    // link data
    rkStream.Write(Locations);
    rkStream.Write(Sizes);

    WM4_END_DEBUG_STREAM_SAVE(Particles);
}
//----------------------------------------------------------------------------
int Particles::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return TriMesh::GetDiskUsed(rkVersion) +
        sizeof(SizeAdjust) +
        sizeof(m_iActiveQuantity) +
        WM4_PTRSIZE(Locations) +
        WM4_PTRSIZE(Sizes);
}
//----------------------------------------------------------------------------
StringTree* Particles::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("size adjust =",SizeAdjust));
    pkTree->Append(Format("active quantity =",m_iActiveQuantity));

    // children
    pkTree->Append(TriMesh::SaveStrings());
    pkTree->Append(Locations->SaveStrings());
    pkTree->Append(Sizes->SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
