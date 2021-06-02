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
// Version: 4.0.1 (2007/06/07)

#include "Wm4GraphicsPCH.h"
#include "Wm4SkinController.h"
#include "Wm4Geometry.h"
#include "Wm4Node.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,SkinController,Controller);
WM4_IMPLEMENT_STREAM(SkinController);
WM4_IMPLEMENT_DEFAULT_NAME_ID(SkinController,Controller);

//----------------------------------------------------------------------------
SkinController::SkinController (int iVertexQuantity, int iBoneQuantity,
    Node** apkBones, float** aafWeight, Vector3f** aakOffset)
{
    m_iVertexQuantity = iVertexQuantity;
    m_iBoneQuantity = iBoneQuantity;
    m_apkBones = apkBones;
    m_aafWeight = aafWeight;
    m_aakOffset = aakOffset;
}
//----------------------------------------------------------------------------
SkinController::SkinController ()
{
    m_iVertexQuantity = 0;
    m_iBoneQuantity = 0;
    m_apkBones = 0;
    m_aafWeight = 0;
    m_aakOffset = 0;
}
//----------------------------------------------------------------------------
SkinController::~SkinController ()
{
    WM4_DELETE[] m_apkBones;
    Deallocate<float>(m_aafWeight);
    Deallocate<Vector3f>(m_aakOffset);
}
//----------------------------------------------------------------------------
bool SkinController::Update (double dAppTime)
{
    if (!Controller::Update(dAppTime))
    {
        return false;
    }

    // The skin vertices are calculated in the bone world coordinate system,
    // so the TriMesh world transform must be the identity.
    Geometry* pkGeom = StaticCast<Geometry>(m_pkObject);
    pkGeom->World = Transformation::IDENTITY;
    pkGeom->WorldIsCurrent = true;

    // Compute the skin vertex locations.
    assert(m_iVertexQuantity == pkGeom->VBuffer->GetVertexQuantity());
    for (int i = 0; i < m_iVertexQuantity; i++)
    {
        Vector3f kTmp = m_apkBones[0]->World.ApplyForward(m_aakOffset[i][0]);
        pkGeom->VBuffer->Position3(i) = m_aafWeight[i][0]*kTmp;
        for (int j = 1; j < m_iBoneQuantity; j++)
        {
            if (m_aafWeight[i][j] != 0.0f)
            {
                kTmp = m_apkBones[j]->World.ApplyForward(m_aakOffset[i][j]);
                pkGeom->VBuffer->Position3(i) += m_aafWeight[i][j]*kTmp;
            }
        }
    }

    pkGeom->VBuffer->Release();
    pkGeom->UpdateMS();
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SkinController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iVertexQuantity);
    rkStream.Read(m_iBoneQuantity);

    int i;

    Allocate<float>(m_iBoneQuantity,m_iVertexQuantity,m_aafWeight);
    for (i = 0; i < m_iVertexQuantity; i++)
    {
        rkStream.Read(m_iBoneQuantity,m_aafWeight[i]);
    }

    Allocate<Vector3f>(m_iBoneQuantity,m_iVertexQuantity,m_aakOffset);
    for (i = 0; i < m_iVertexQuantity; i++)
    {
        rkStream.Read(m_iBoneQuantity,m_aakOffset[i]);
    }

    // link data
    for (i = 0; i < m_iBoneQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);  // m_apkBones[i]
        pkLink->Add(pkObject);
    }

    WM4_END_DEBUG_STREAM_LOAD(SkinController);
}
//----------------------------------------------------------------------------
void SkinController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);

    m_apkBones = WM4_NEW Node*[m_iBoneQuantity];
    for (int i = 0; i < m_iBoneQuantity; i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_apkBones[i] = (Node*)rkStream.GetFromMap(pkLinkID);
    }

}
//----------------------------------------------------------------------------
bool SkinController::Register (Stream& rkStream) const
{
    if (!Controller::Register(rkStream))
    {
        return false;
    }

    for (int j = 0; j < m_iBoneQuantity; j++)
    {
        if (m_apkBones[j])
        {
            m_apkBones[j]->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void SkinController::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // native data
    rkStream.Write(m_iVertexQuantity);
    rkStream.Write(m_iBoneQuantity);

    int i;
    for (i = 0; i < m_iVertexQuantity; i++)
    {
        rkStream.Write(m_iBoneQuantity,m_aafWeight[i]);
    }

    for (i = 0; i < m_iVertexQuantity; i++)
    {
        rkStream.Write(m_iBoneQuantity,m_aakOffset[i]);
    }

    // link data
    for (i = 0; i < m_iBoneQuantity; i++)
    {
        rkStream.Write(m_apkBones[i]);
    }

    WM4_END_DEBUG_STREAM_SAVE(SkinController);
}
//----------------------------------------------------------------------------
int SkinController::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Controller::GetDiskUsed(rkVersion) +
        sizeof(m_iVertexQuantity) +
        sizeof(m_iBoneQuantity) +
        m_iVertexQuantity*m_iBoneQuantity*sizeof(m_aafWeight[0][0]) +
        m_iVertexQuantity*m_iBoneQuantity*sizeof(m_aakOffset[0][0]) +
        m_iBoneQuantity*WM4_PTRSIZE(m_apkBones[0]);
}
//----------------------------------------------------------------------------
StringTree* SkinController::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("vertex quantity = ",m_iVertexQuantity));
    pkTree->Append(Format("bone quantity = ",m_iBoneQuantity));

    // children
    pkTree->Append(Controller::SaveStrings());
    pkTree->Append(Format("bones",m_iBoneQuantity,m_apkBones));

    const size_t uiTitleSize = 256;
    char acTitle[uiTitleSize];
    int i;
    for (i = 0; i < m_iVertexQuantity; i++)
    {
        System::Sprintf(acTitle,uiTitleSize,"weights for vertex %d",i);
        pkTree->Append(Format(acTitle,m_iBoneQuantity,m_aafWeight[i]));
    }

    for (i = 0; i < m_iVertexQuantity; i++)
    {
        System::Sprintf(acTitle,uiTitleSize,"offsets for vertex %d",i);
        pkTree->Append(Format(acTitle,m_iBoneQuantity,m_aakOffset[i]));
    }

    return pkTree;
}
//----------------------------------------------------------------------------
