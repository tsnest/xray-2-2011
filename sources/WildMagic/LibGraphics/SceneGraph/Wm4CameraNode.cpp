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
#include "Wm4CameraNode.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,CameraNode,Node);
WM4_IMPLEMENT_STREAM(CameraNode);

//----------------------------------------------------------------------------
CameraNode::CameraNode (Camera* pkCamera)
    :
    m_spkCamera(pkCamera)
{
    if (m_spkCamera)
    {
        Local.SetTranslate(m_spkCamera->GetLocation());
        Local.SetRotate(Matrix3f(m_spkCamera->GetDVector(),
            m_spkCamera->GetUVector(),m_spkCamera->GetRVector(),true));
    }
}
//----------------------------------------------------------------------------
CameraNode::~CameraNode ()
{
}
//----------------------------------------------------------------------------
void CameraNode::SetCamera (Camera* pkCamera)
{
    m_spkCamera = pkCamera;

    if (m_spkCamera)
    {
        Local.SetTranslate(m_spkCamera->GetLocation());
        Local.SetRotate(Matrix3f(m_spkCamera->GetDVector(),
            m_spkCamera->GetUVector(),m_spkCamera->GetRVector(),true));
        UpdateGS();
    }
}
//----------------------------------------------------------------------------
void CameraNode::UpdateWorldData (double dAppTime)
{
    Node::UpdateWorldData(dAppTime);

    if (m_spkCamera)
    {
        m_spkCamera->SetFrame(
            World.GetTranslate(),
            World.GetRotate().GetColumn(0),
            World.GetRotate().GetColumn(1),
            World.GetRotate().GetColumn(2));
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* CameraNode::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Node::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkCamera)
    {
        pkFound = m_spkCamera->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return NULL;
}
//----------------------------------------------------------------------------
void CameraNode::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Node::GetAllObjectsByName(rkName,rkObjects);

    if (m_spkCamera)
    {
        m_spkCamera->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* CameraNode::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Node::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkCamera)
    {
        pkFound = m_spkCamera->GetObjectByID(uiID);
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
void CameraNode::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkCamera
    pkLink->Add(pkObject);

    WM4_END_DEBUG_STREAM_LOAD(CameraNode);
}
//----------------------------------------------------------------------------
void CameraNode::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkCamera = (Camera*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool CameraNode::Register (Stream& rkStream) const
{
    if (!Node::Register(rkStream))
    {
        return false;
    }

    if (m_spkCamera)
    {
        m_spkCamera->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void CameraNode::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // link data
    rkStream.Write(m_spkCamera);

    WM4_END_DEBUG_STREAM_SAVE(CameraNode);
}
//----------------------------------------------------------------------------
int CameraNode::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Node::GetDiskUsed(rkVersion) + WM4_PTRSIZE(m_spkCamera);
}
//----------------------------------------------------------------------------
StringTree* CameraNode::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));

    // children
    pkTree->Append(Node::SaveStrings());
    if (m_spkCamera)
    {
        pkTree->Append(m_spkCamera->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
