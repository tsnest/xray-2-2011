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
#include "Wm4LightNode.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,LightNode,Node);
WM4_IMPLEMENT_STREAM(LightNode);

//----------------------------------------------------------------------------
LightNode::LightNode (Light* pkLight)
    :
    m_spkLight(pkLight)
{
    if (m_spkLight)
    {
        Local.SetTranslate(m_spkLight->Position);
        Local.SetRotate(Matrix3f(m_spkLight->DVector,m_spkLight->UVector,
            m_spkLight->RVector,true));
    }
}
//----------------------------------------------------------------------------
LightNode::~LightNode ()
{
}
//----------------------------------------------------------------------------
void LightNode::SetLight (Light* pkLight)
{
    m_spkLight = pkLight;

    if (m_spkLight)
    {
        Local.SetTranslate(m_spkLight->Position);
        Local.SetRotate(Matrix3f(m_spkLight->DVector,m_spkLight->UVector,
            m_spkLight->RVector,true));
        UpdateGS();
    }
}
//----------------------------------------------------------------------------
void LightNode::UpdateWorldData (double dAppTime)
{
    Node::UpdateWorldData(dAppTime);

    if (m_spkLight)
    {
        m_spkLight->Position = World.GetTranslate();
        m_spkLight->DVector = World.GetRotate().GetColumn(0);
        m_spkLight->UVector = World.GetRotate().GetColumn(1);
        m_spkLight->RVector = World.GetRotate().GetColumn(2);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* LightNode::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Node::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkLight)
    {
        pkFound = m_spkLight->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void LightNode::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Node::GetAllObjectsByName(rkName,rkObjects);

    if (m_spkLight)
    {
        m_spkLight->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* LightNode::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Node::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkLight)
    {
        pkFound = m_spkLight->GetObjectByID(uiID);
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
void LightNode::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkLight
    pkLink->Add(pkObject);

    WM4_END_DEBUG_STREAM_LOAD(LightNode);
}
//----------------------------------------------------------------------------
void LightNode::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkLight = (Light*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool LightNode::Register (Stream& rkStream) const
{
    if (!Node::Register(rkStream))
    {
        return false;
    }

    if (m_spkLight)
    {
        m_spkLight->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void LightNode::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // link data
    rkStream.Write(m_spkLight);

    WM4_END_DEBUG_STREAM_SAVE(LightNode);
}
//----------------------------------------------------------------------------
int LightNode::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Node::GetDiskUsed(rkVersion) + WM4_PTRSIZE(m_spkLight);
}
//----------------------------------------------------------------------------
StringTree* LightNode::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));

    // children
    pkTree->Append(Node::SaveStrings());
    if (m_spkLight)
    {
        pkTree->Append(m_spkLight->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
