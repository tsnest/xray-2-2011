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
#include "Wm4BspNode.h"
#include "Wm4Camera.h"
#include "Wm4Renderer.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,BspNode,Node);
WM4_IMPLEMENT_STREAM(BspNode);
WM4_IMPLEMENT_DEFAULT_NAME_ID(BspNode,Node);

//----------------------------------------------------------------------------
BspNode::BspNode ()
    :
    m_kModelPlane(Vector3f::ZERO,0.0f)
{
    m_kChild.push_back(0);  // left child
    m_kChild.push_back(0);  // middle child
    m_kChild.push_back(0);  // right child
}
//----------------------------------------------------------------------------
BspNode::BspNode (const Plane3f& rkModelPlane)
    :
    m_kModelPlane(rkModelPlane)
{
    m_kChild.push_back(0);  // left child
    m_kChild.push_back(0);  // middle child
    m_kChild.push_back(0);  // right child
}
//----------------------------------------------------------------------------
BspNode::~BspNode ()
{
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::AttachPositiveChild (Spatial* pkChild)
{
    return SetChild(0,pkChild);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::AttachCoplanarChild (Spatial* pkChild)
{
    return SetChild(1,pkChild);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::AttachNegativeChild (Spatial* pkChild)
{
    return SetChild(2,pkChild);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::DetachPositiveChild ()
{
    return DetachChildAt(0);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::DetachCoplanarChild ()
{
    return DetachChildAt(1);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::DetachNegativeChild ()
{
    return DetachChildAt(2);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::GetPositiveChild ()
{
    return GetChild(0);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::GetCoplanarChild ()
{
    return GetChild(1);
}
//----------------------------------------------------------------------------
SpatialPtr BspNode::GetNegativeChild ()
{
    return GetChild(2);
}
//----------------------------------------------------------------------------
void BspNode::UpdateWorldData (double dAppTime)
{
    Node::UpdateWorldData(dAppTime);
    m_kWorldPlane = World.ApplyForward(m_kModelPlane);
}
//----------------------------------------------------------------------------
void BspNode::GetVisibleSet (Culler& rkCuller, bool bNoCull)
{
    int i;
    for (i = 0; i < (int)m_kEffects.size(); i++)
    {
        // This is a global effect.  Place a 'begin' marker in the visible
        // set to indicate the effect is active.
        rkCuller.Insert(this,m_kEffects[i]);
    }

    // Get visible Geometry in back-to-front order.  If a global effect is
    // active, the Geometry objects in the subtree will be drawn using it.
    SpatialPtr spkPChild = GetPositiveChild();
    SpatialPtr spkCChild = GetCoplanarChild();
    SpatialPtr spkNChild = GetNegativeChild();

    const Camera* pkCamera = rkCuller.GetCamera();
    int iLocSide = m_kWorldPlane.WhichSide(pkCamera->GetLocation());
    int iFruSide = rkCuller.WhichSide(m_kWorldPlane);

    if (iLocSide > 0)
    {
        // camera origin on positive side of plane

        if (iFruSide <= 0)
        {
            // The frustum is on the negative side of the plane or straddles
            // the plane.  In either case, the negative child is potentially
            // visible.
            if (spkNChild)
            {
                spkNChild->OnGetVisibleSet(rkCuller,bNoCull);
            }
        }

        if (iFruSide == 0)
        {
            // The frustum straddles the plane.  The coplanar child is
            // potentially visible.
            if (spkCChild)
            {
                spkCChild->OnGetVisibleSet(rkCuller,bNoCull);
            }
        }

        if (iFruSide >= 0)
        {
            // The frustum is on the positive side of the plane or straddles
            // the plane.  In either case, the positive child is potentially
            // visible.
            if (spkPChild)
            {
                spkPChild->OnGetVisibleSet(rkCuller,bNoCull);
            }
        }
    }
    else if (iLocSide < 0)
    {
        // camera origin on negative side of plane

        if (iFruSide >= 0)
        {
            // The frustum is on the positive side of the plane or straddles
            // the plane.  In either case, the positive child is potentially
            // visible.
            if (spkPChild)
            {
                spkPChild->OnGetVisibleSet(rkCuller,bNoCull);
            }
        }

        if (iFruSide == 0)
        {
            // The frustum straddles the plane.  The coplanar child is
            // potentially visible.
            if (spkCChild)
            {
                spkCChild->OnGetVisibleSet(rkCuller,bNoCull);
            }
        }

        if (iFruSide <= 0)
        {
            // The frustum is on the negative side of the plane or straddles
            // the plane.  In either case, the negative child is potentially
            // visible.
            if (spkNChild)
            {
                spkNChild->OnGetVisibleSet(rkCuller,bNoCull);
            }
        }
    }
    else
    {
        // Camera origin on plane itself.  Both sides of the plane are
        // potentially visible as well as the plane itself.  Select the
        // first-to-be-drawn half space to be the one to which the camera
        // direction points.
        float fNdD = m_kWorldPlane.Normal.Dot(pkCamera->GetDVector());
        if (fNdD >= 0.0f)
        {
            if (spkPChild)
            {
                spkPChild->OnGetVisibleSet(rkCuller,bNoCull);
            }

            if (spkCChild)
            {
                spkCChild->OnGetVisibleSet(rkCuller,bNoCull);
            }

            if (spkNChild)
            {
                spkNChild->OnGetVisibleSet(rkCuller,bNoCull);
            }
        }
        else
        {
            if (spkNChild)
            {
                spkNChild->OnGetVisibleSet(rkCuller,bNoCull);
            }

            if (spkCChild)
            {
                spkCChild->OnGetVisibleSet(rkCuller,bNoCull);
            }

            if (spkPChild)
            {
                spkPChild->OnGetVisibleSet(rkCuller,bNoCull);
            }
        }
    }

    for (i = 0; i < (int)m_kEffects.size(); i++)
    {
        // Place an 'end' marker in the visible set to indicate that the
        // global effect is inactive.
        rkCuller.Insert(0,0);
    }
}
//----------------------------------------------------------------------------
Spatial* BspNode::GetContainingNode (const Vector3f& rkPoint)
{
    SpatialPtr spkPChild = GetPositiveChild();
    SpatialPtr spkNChild = GetNegativeChild();

    if (spkPChild || spkNChild)
    {
        BspNode* pkBspChild;

        if (m_kWorldPlane.WhichSide(rkPoint) < 0)
        {
            pkBspChild = DynamicCast<BspNode>(spkNChild);
            if (pkBspChild)
            {
                return pkBspChild->GetContainingNode(rkPoint);
            }
            else
            {
                return spkNChild;
            }
        }
        else
        {
            pkBspChild = DynamicCast<BspNode>(spkPChild);
            if (pkBspChild)
            {
                return pkBspChild->GetContainingNode(rkPoint);
            }
            else
            {
                return spkPChild;
            }
        }
    }

    return this;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BspNode::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kModelPlane);

    WM4_END_DEBUG_STREAM_LOAD(BspNode);
}
//----------------------------------------------------------------------------
void BspNode::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BspNode::Register (Stream& rkStream) const
{
    return Node::Register(rkStream);
}
//----------------------------------------------------------------------------
void BspNode::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // native data
    rkStream.Write(m_kModelPlane);

    // world plane is computed from model plane in update, no need to save
    WM4_END_DEBUG_STREAM_SAVE(BspNode);
}
//----------------------------------------------------------------------------
int BspNode::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Node::GetDiskUsed(rkVersion) + sizeof(m_kModelPlane);
}
//----------------------------------------------------------------------------
StringTree* BspNode::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("model plane =",m_kModelPlane));

    // children
    pkTree->Append(Node::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
