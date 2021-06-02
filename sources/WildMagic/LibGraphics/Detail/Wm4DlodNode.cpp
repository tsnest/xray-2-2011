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
#include "Wm4DlodNode.h"
#include "Wm4Camera.h"
#include "Wm4Renderer.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,DlodNode,SwitchNode);
WM4_IMPLEMENT_STREAM(DlodNode);
WM4_IMPLEMENT_DEFAULT_NAME_ID(DlodNode,SwitchNode);

//----------------------------------------------------------------------------
DlodNode::DlodNode ()
{
}
//----------------------------------------------------------------------------
DlodNode::~DlodNode ()
{
}
//----------------------------------------------------------------------------
void DlodNode::SetModelDistance (int i, float fMinDist, float fMaxDist)
{
    assert(i >= 0);

    if (i < (int)m_kModelMinDist.size())
    {
        m_kModelMinDist[i] = fMinDist;
        m_kModelMaxDist[i] = fMaxDist;
        m_kWorldMinDist[i] = fMinDist;
        m_kWorldMaxDist[i] = fMaxDist;
    }
    else
    {
        m_kModelMinDist.push_back(fMinDist);
        m_kModelMaxDist.push_back(fMaxDist);
        m_kWorldMinDist.push_back(fMinDist);
        m_kWorldMaxDist.push_back(fMaxDist);
    }
}
//----------------------------------------------------------------------------
void DlodNode::SelectLevelOfDetail (const Camera* pkCamera)
{
    // ASSERT:  The child array of an DlodNode is compacted--there are no
    // empty slots in the array and the number of children is GetQuantity().
    // Moreover, it is assumed that all model distance values were set for
    // these children.

    // compute world LOD center
    m_kWorldLodCenter = World.ApplyForward(m_kModelLodCenter);

    // compute world squared distance intervals
    int i;
    for (i = 0; i < (int)m_kChild.size(); i++)
    {
        float fValue = m_kModelMinDist[i];
        m_kWorldMinDist[i] = World.GetUniformScale()*fValue;

        fValue = m_kModelMaxDist[i];
        m_kWorldMaxDist[i] = World.GetUniformScale()*fValue;
    }

    // select the LOD child
    SetActiveChild(SN_INVALID_CHILD);
    if (m_kChild.size() > 0)
    {
        Vector3f kDiff = m_kWorldLodCenter - pkCamera->GetLocation();
        float fDist = kDiff.Length();

        for (i = 0; i < (int)m_kChild.size(); i++) 
        {
            if (m_kWorldMinDist[i] <= fDist && fDist < m_kWorldMaxDist[i])
            {
                SetActiveChild(i);
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------
void DlodNode::GetVisibleSet (Culler& rkCuller, bool bNoCull)
{
    SelectLevelOfDetail(rkCuller.GetCamera());
    SwitchNode::GetVisibleSet(rkCuller,bNoCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void DlodNode::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    SwitchNode::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kModelLodCenter);

    int i, iQuantity;
    rkStream.Read(iQuantity);
    m_kModelMinDist.resize(iQuantity);
    m_kWorldMinDist.resize(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        rkStream.Read(m_kModelMinDist[i]);
        rkStream.Read(m_kWorldMinDist[i]);
    }

    rkStream.Read(iQuantity);
    m_kModelMaxDist.resize(iQuantity);
    m_kWorldMaxDist.resize(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        rkStream.Read(m_kModelMaxDist[i]);
        rkStream.Read(m_kWorldMaxDist[i]);
    }

    WM4_END_DEBUG_STREAM_LOAD(DlodNode);
}
//----------------------------------------------------------------------------
void DlodNode::Link (Stream& rkStream, Stream::Link* pkLink)
{
    SwitchNode::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool DlodNode::Register (Stream& rkStream) const
{
    return SwitchNode::Register(rkStream);
}
//----------------------------------------------------------------------------
void DlodNode::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    SwitchNode::Save(rkStream);

    // native data
    rkStream.Write(m_kModelLodCenter);

    int i, iQuantity = (int)m_kModelMinDist.size();
    rkStream.Write(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        rkStream.Write(m_kModelMinDist[i]);
        rkStream.Write(m_kWorldMinDist[i]);
    }

    iQuantity = (int)m_kModelMaxDist.size();
    rkStream.Write(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        rkStream.Write(m_kModelMaxDist[i]);
        rkStream.Write(m_kWorldMaxDist[i]);
    }

    // The world LOD center is computed from the model LOD center in
    // SelectLevelOfDetail.  The world distance extremes are also computed
    // from the model distance extremes in SelectLevelOfDetail.  These
    // world quantities do not need to be saved.

    WM4_END_DEBUG_STREAM_SAVE(DlodNode);
}
//----------------------------------------------------------------------------
int DlodNode::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return SwitchNode::GetDiskUsed(rkVersion) +
        sizeof(m_kModelLodCenter) +
        sizeof(int) + ((int)m_kModelMinDist.size())*sizeof(float) +
        ((int)m_kWorldMinDist.size())*sizeof(float) +
        sizeof(int) + ((int)m_kModelMaxDist.size())*sizeof(float) +
        ((int)m_kWorldMaxDist.size())*sizeof(float);
}
//----------------------------------------------------------------------------
StringTree* DlodNode::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("model lod center =",m_kModelLodCenter));
    pkTree->Append(Format("world lod center =",m_kWorldLodCenter));

    // children
    pkTree->Append(SwitchNode::SaveStrings());
    pkTree->Append(Format("model min dist",(int)m_kModelMinDist.size(),
        &m_kModelMinDist.front()));
    pkTree->Append(Format("model max dist",(int)m_kModelMaxDist.size(),
        &m_kModelMaxDist.front()));
    pkTree->Append(Format("world min dist",(int)m_kWorldMinDist.size(),
        &m_kWorldMinDist.front()));
    pkTree->Append(Format("world max dist",(int)m_kWorldMaxDist.size(),
        &m_kWorldMaxDist.front()));

    return pkTree;
}
//----------------------------------------------------------------------------
