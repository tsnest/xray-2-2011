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
// Version: 4.0.1 (2007/08/11)

#include "Wm4GraphicsPCH.h"
#include "Wm4Camera.h"
#include "Wm4ConvexRegionManager.h"
#include "Wm4ConvexRegion.h"
#include "Wm4Renderer.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,ConvexRegionManager,BspNode);
WM4_IMPLEMENT_STREAM(ConvexRegionManager);
WM4_IMPLEMENT_DEFAULT_NAME_ID(ConvexRegionManager,BspNode);

//----------------------------------------------------------------------------
ConvexRegionManager::ConvexRegionManager ()
{
}
//----------------------------------------------------------------------------
ConvexRegionManager::~ConvexRegionManager ()
{
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::AttachOutside (Spatial* pkOutside)
{
    return SetChild(1,pkOutside);
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::DetachOutside ()
{
    return DetachChildAt(1);
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::GetOutside ()
{
    return GetChild(1);
}
//----------------------------------------------------------------------------
ConvexRegion* ConvexRegionManager::GetContainingRegion (
    const Vector3f& rkPoint)
{
    return DynamicCast<ConvexRegion>(GetContainingNode(rkPoint));
}
//----------------------------------------------------------------------------
void ConvexRegionManager::GetVisibleSet (Culler& rkCuller, bool bNoCull)
{
    ConvexRegion* pkRegion = GetContainingRegion(
        rkCuller.GetCamera()->GetLocation());

    if (pkRegion)
    {
        // Accumulate visible objects starting in the region containing the
        // camera.  Use the CRMCuller to maintain a list of unique objects.
        //pkRegion->GetVisibleSet(m_kCuller,bNoCull);
        m_kCuller.SetCamera(rkCuller.GetCamera());
        m_kCuller.SetFrustum(rkCuller.GetFrustum());
        m_kCuller.ComputeVisibleSet(pkRegion);

        // Copy the unique list to the scene culler.
        VisibleSet& rkVisible = m_kCuller.GetVisibleSet();
        const int iQuantity = rkVisible.GetQuantity();
        for (int i = 0; i < iQuantity; i++)
        {
            VisibleObject& rkVObj = rkVisible.GetVisible(i);
            rkCuller.Insert(rkVObj.Object,rkVObj.GlobalEffect);
        }
    }
    else
    {
        // The camera is outside the set of regions.  Accumulate visible
        // objects for the outside scene (if it exists).
        if (GetOutside())
        {
            GetOutside()->GetVisibleSet(rkCuller,bNoCull);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ConvexRegionManager::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    BspNode::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(ConvexRegionManager);
}
//----------------------------------------------------------------------------
void ConvexRegionManager::Link (Stream& rkStream, Stream::Link* pkLink)
{
    BspNode::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ConvexRegionManager::Register (Stream& rkStream) const
{
    return BspNode::Register(rkStream);
}
//----------------------------------------------------------------------------
void ConvexRegionManager::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    BspNode::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(ConvexRegionManager);
}
//----------------------------------------------------------------------------
int ConvexRegionManager::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return BspNode::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* ConvexRegionManager::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(BspNode::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
