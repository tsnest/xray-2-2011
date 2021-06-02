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
#include "Wm4Portal.h"
#include "Wm4ConvexRegion.h"
#include "Wm4Culler.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Portal,Object);
WM4_IMPLEMENT_STREAM(Portal);

//----------------------------------------------------------------------------
Portal::Portal (int iVQuantity, Vector3f* akModelVertex,
    const Plane3f& rkModelPlane, ConvexRegion* pkAdjacentRegion, bool bOpen)
    :
    m_kModelPlane(rkModelPlane)
{
    assert(iVQuantity >= 3);

    m_iVQuantity = iVQuantity;
    m_akModelVertex = akModelVertex;
    m_akWorldVertex = WM4_NEW Vector3f[m_iVQuantity];
    m_pkAdjacentRegion = pkAdjacentRegion;
    m_bOpen = bOpen;
}
//----------------------------------------------------------------------------
Portal::Portal ()
{
    m_iVQuantity = 0;
    m_akModelVertex = 0;
    m_akWorldVertex = 0;
    m_pkAdjacentRegion = 0;
    m_bOpen = false;
}
//----------------------------------------------------------------------------
Portal::~Portal ()
{
    WM4_DELETE[] m_akModelVertex;
    WM4_DELETE[] m_akWorldVertex;
}
//----------------------------------------------------------------------------
void Portal::UpdateWorldData (const Transformation& rkWorld)
{
    rkWorld.ApplyForward(m_iVQuantity,m_akModelVertex,m_akWorldVertex);
    m_kWorldPlane = rkWorld.ApplyForward(m_kModelPlane);
}
//----------------------------------------------------------------------------
bool Portal::ReducedFrustum (const Culler& rkCuller,
    float afReducedFrustum[6])
{
    // The portal polygon is transformed into the camera coordinate system
    // and projected onto the near plane.  An axis-aligned bounding rectangle
    // is computed for the projected points and clipped against the left,
    // right, bottom, and top frustum planes.  The result is itself an
    // axis-aligned bounding rectangle that is used to define a "reduced
    // frustum" to be used for drawing what is visible through the portal
    // polygon.
    //
    // The algorithm must handle the situation when portal polygon vertices
    // are behind the observer.  Imagine standing in a room with a doorway
    // immediately to your left.  Part of the doorway frame is in front of
    // you (and visible) and part of it is behind you (and not visible).
    // A portal point is represented by P = E + d*D + u*U + r*R, where E is
    // the world location for the eye point, D is the camera's world direction
    // vector, U is the camera's world up vector, and R is the camera's world
    // right vector.  The camera coordinates for the portal point are (d,u,r).
    // If d > 0, P is in front of the eye point and has a projection onto the
    // near plane d = n.  If d < 0, P is behind the eye point and does not
    // have a projection onto the near plane.  If d = 0, P projects to
    // "infinity" on the near plane, a problematic case to deal with.
    //
    // To avoid dealing with d = 0, choose a small value e such that
    // 0 < e < n.  The portal polygon is clipped against the plane d = e,
    // keeping only that portion whose points satisfy d >= e.  The clipped
    // polygon always has a projection onto the near plane.  The axis-aligned
    // bounding box for this projection is computed; clipped against the
    // left, right, bottom, and top of the frustum; and the result used to
    // define the reduced frustum.  All this is designed for an inexact
    // culling of the objects in the adjacent room, so it is useful to avoid
    // preserving the topology of the portal polygon as it is clipped.
    // Instead, the portal polygon vertices with d > e are projected and
    // the intersection points of portal polygon edges with d = e are
    // computed and projected.  The axis-aligned bounding box is computed for
    // the projections, a process that does not require knowing the polygon
    // topology.  The algorithm is essentially the one used for clipping a
    // convex polygon against the view frustum in the software renderer.  The
    // polygon vertices are traversed in-order and the signs of the d values
    // are updated accordingly.  This avoids computing d-signs twice per
    // vertex.

    const Camera* pkCamera = rkCuller.GetCamera();
    const float* afFrustum = rkCuller.GetFrustum();
    float fRMin = +Mathf::MAX_REAL;  // left
    float fRMax = -Mathf::MAX_REAL;  // right
    float fUMin = +Mathf::MAX_REAL;  // bottom
    float fUMax = -Mathf::MAX_REAL;  // top

    Vector3f kDiff, kVertexCam;
    int i;

    if (pkCamera->Perspective)
    {
        const float fEpsilon = 1e-6f, fInvEpsilon = 1e+6f;
        int iFirstSign = 0, iLastSign = 0;  // in {-1,0,1}
        bool bSignChange = false;
        Vector3f kFirstVertex(0.0f,0.0f,0.0f);
        Vector3f kLastVertex(0.0f,0.0f,0.0f);
        float fNdD, fUdD, fRdD, fT;

        for (i = 0; i < m_iVQuantity; i++)
        {
            kDiff = m_akWorldVertex[i] - pkCamera->GetLocation();
            kVertexCam[0] = kDiff.Dot(pkCamera->GetDVector());
            kVertexCam[1] = kDiff.Dot(pkCamera->GetUVector());
            kVertexCam[2] = kDiff.Dot(pkCamera->GetRVector());

            if (kVertexCam[0] > fEpsilon)
            {
                if (iFirstSign == 0)
                {
                    iFirstSign = 1;
                    kFirstVertex = kVertexCam;
                }

                fNdD = afFrustum[Camera::VF_DMIN]/kVertexCam[0];
                fUdD = kVertexCam[1]*fNdD;
                fRdD = kVertexCam[2]*fNdD;

                if (fUdD < fUMin)
                {
                    fUMin = fUdD;
                }
                if (fUdD > fUMax)
                {
                    fUMax = fUdD;
                }

                if (fRdD < fRMin)
                {
                    fRMin = fRdD;
                }
                if (fRdD > fRMax)
                {
                    fRMax = fRdD;
                }

                if (iLastSign < 0)
                {
                    bSignChange = true;
                }

                iLastSign = 1;
            }
            else
            {
                if (iFirstSign == 0)
                {
                    iFirstSign = -1;
                    kFirstVertex = kVertexCam;
                }

                if (iLastSign > 0)
                {
                    bSignChange = true;
                }

                iLastSign = -1;
            }

            if (bSignChange)
            {
                kDiff = kVertexCam - kLastVertex;
                fT = (fEpsilon - kLastVertex[0])/kDiff[0];
                fNdD = afFrustum[Camera::VF_DMIN]*fInvEpsilon;
                fUdD = (kLastVertex[1] + fT*kDiff[1])*fNdD;
                fRdD = (kLastVertex[2] + fT*kDiff[2])*fNdD;

                if (fUdD < fUMin)
                {
                    fUMin = fUdD;
                }
                if (fUdD > fUMax)
                {
                    fUMax = fUdD;
                }

                if (fRdD < fRMin)
                {
                    fRMin = fRdD;
                }
                if (fRdD > fRMax)
                {
                    fRMax = fRdD;
                }

                bSignChange = false;
            }

            kLastVertex = kVertexCam;
        }

        if (iFirstSign*iLastSign < 0)
        {
            // Process the last polygon edge.
            kDiff = kFirstVertex - kLastVertex;
            fT = (fEpsilon - kLastVertex[0])/kDiff[0];
            fUdD = (kLastVertex[1] + fT*kDiff[1])*fInvEpsilon;
            fRdD = (kLastVertex[2] + fT*kDiff[2])*fInvEpsilon;

            if (fUdD < fUMin)
            {
                fUMin = fUdD;
            }
            if (fUdD > fUMax)
            {
                fUMax = fUdD;
            }

            if (fRdD < fRMin)
            {
                fRMin = fRdD;
            }
            if (fRdD > fRMax)
            {
                fRMax = fRdD;
            }
        }
    }
    else
    {
        for (i = 0; i < m_iVQuantity; i++)
        {
            kDiff = m_akWorldVertex[i] - pkCamera->GetLocation();
            kVertexCam[1] = kDiff.Dot(pkCamera->GetUVector());
            kVertexCam[2] = kDiff.Dot(pkCamera->GetRVector());

            if (kVertexCam[1] < fUMin)
            {
                fUMin = kVertexCam[1];
            }
            if (kVertexCam[1] > fUMax)
            {
                fUMax = kVertexCam[1];
            }

            if (kVertexCam[2] < fRMin)
            {
                fRMin = kVertexCam[2];
            }
            if (kVertexCam[2] > fRMax)
            {
                fRMax = kVertexCam[2];
            }
        }
    }

    // Test if the axis-aligned bounding rectangle is outside the current
    // frustum.  If it is, the adjoining room need not be visited.
    if (afFrustum[Camera::VF_RMIN] >= fRMax ||
        afFrustum[Camera::VF_RMAX] <= fRMin ||
        afFrustum[Camera::VF_UMIN] >= fUMax ||
        afFrustum[Camera::VF_UMAX] <= fUMin)
    {
        return false;
    }

    // The axis-aligned bounding rectangle intersects the current frustum.
    // Reduce the frustum for use in drawing the adjoining room.
    for (int j = 0; j < 6; j++)
    {
        afReducedFrustum[j] = afFrustum[j];
    }

    if (afReducedFrustum[Camera::VF_RMIN] < fRMin)
    {
        afReducedFrustum[Camera::VF_RMIN] = fRMin;
    }

    if (afReducedFrustum[Camera::VF_RMAX] > fRMax)
    {
        afReducedFrustum[Camera::VF_RMAX] = fRMax;
    }

    if (afReducedFrustum[Camera::VF_UMIN] < fUMin)
    {
        afReducedFrustum[Camera::VF_UMIN] = fUMin;
    }

    if (afReducedFrustum[Camera::VF_UMAX] > fUMax)
    {
        afReducedFrustum[Camera::VF_UMAX] = fUMax;
    }

    return true;
}
//----------------------------------------------------------------------------
void Portal::GetVisibleSet (Culler& rkCuller, bool bNoCull)
{
    // Visit only the adjacent region if the portal is open.
    if (!m_bOpen)
    {
        return;
    }

    // Traverse only through visible portals.
    if (!rkCuller.IsVisible(m_iVQuantity,m_akWorldVertex,true))
    {
        return;
    }

    // It is possible that this portal is visited along a path of portals
    // from the current room containing the camera.  Such portals might
    // have a back-facing polygon relative to the camera.  It is not possible
    // to see through these, so cull them.
    const Camera* pkCamera = rkCuller.GetCamera();
    if (m_kWorldPlane.WhichSide(pkCamera->GetLocation()) < 0)
    {
        return;
    }

    // Save the current frustum.
    float afSaveFrustum[6];
    const float* afFrustum = rkCuller.GetFrustum();
    for (int j = 0; j < 6; j++)
    {
        afSaveFrustum[j] = afFrustum[j];
    }

    // If the observer can see through the portal, the culler's frustum may
    // be reduced in size based on the portal geometry.
    float afReducedFrustum[6];
    if (ReducedFrustum(rkCuller,afReducedFrustum))
    {
        // Use the reduced frustum for drawing the adjacent region.
        rkCuller.SetFrustum(afReducedFrustum);

        // Visit the adjacent region and any nonculled objects in it.
        m_pkAdjacentRegion->GetVisibleSet(rkCuller,bNoCull);

        // Restore the previous frustum.
        rkCuller.SetFrustum(afSaveFrustum);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Portal::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_pkAdjacentRegion)
    {
        pkFound = m_pkAdjacentRegion->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void Portal::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);

    if (m_pkAdjacentRegion)
    {
         m_pkAdjacentRegion->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* Portal::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_pkAdjacentRegion)
    {
        pkFound = m_pkAdjacentRegion->GetObjectByID(uiID);
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
void Portal::Load (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iVQuantity);
    assert(m_iVQuantity > 0);
    m_akModelVertex = WM4_NEW Vector3f[m_iVQuantity];
    m_akWorldVertex = WM4_NEW Vector3f[m_iVQuantity];
    rkStream.Read(m_iVQuantity,m_akModelVertex);
    rkStream.Read(m_kModelPlane);
    rkStream.Read(m_bOpen);

    // link data
    Object* pkAdjacentRegion;
    rkStream.Read(pkAdjacentRegion);
    pkLink->Add(pkAdjacentRegion);
}
//----------------------------------------------------------------------------
void Portal::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_pkAdjacentRegion = (ConvexRegion*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Portal::Register (Stream& rkStream) const
{
    if (!Object::Register(rkStream))
    {
        return false;
    }

    if (m_pkAdjacentRegion)
    {
        m_pkAdjacentRegion->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void Portal::Save (Stream& rkStream) const
{
    Object::Save(rkStream);

    // Native data (world vertices are computed form model vertices in the
    // update call, no need to save).  TO DO.  Can this just be an array
    // write?
    rkStream.Write(m_iVQuantity);
    rkStream.Write(m_iVQuantity,m_akModelVertex);
    rkStream.Write(m_kModelPlane);
    rkStream.Write(m_bOpen);

    // link data
    rkStream.Write(m_pkAdjacentRegion);
}
//----------------------------------------------------------------------------
int Portal::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion) +
        sizeof(m_iVQuantity) +
        m_iVQuantity*sizeof(m_akModelVertex[0]) +
        sizeof(m_kModelPlane) +
        WM4_PTRSIZE(m_pkAdjacentRegion) +
        sizeof(char); // m_bOpen
}
//----------------------------------------------------------------------------
StringTree* Portal::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("open =",m_bOpen));
    pkTree->Append(Format("adjacent =",m_pkAdjacentRegion));
    pkTree->Append(Format("vertex quantity =",m_iVQuantity));

    const size_t uiSize = 64;
    char acPrefix[uiSize];
    int i;
    for (i = 0; i < m_iVQuantity; i++)
    {
        System::Sprintf(acPrefix,uiSize,"model[%d] =",i);
        pkTree->Append(Format(acPrefix,m_akModelVertex[i]));
    }
    for (i = 0; i < m_iVQuantity; i++)
    {
        System::Sprintf(acPrefix,uiSize,"world[%d] =",i);
        pkTree->Append(Format(acPrefix,m_akWorldVertex[i]));
    }

    // children
    pkTree->Append(Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
