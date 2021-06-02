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
// Version: 4.0.1 (2006/08/07)

#include "Wm4GraphicsPCH.h"
#include "Wm4SphereBV.h"
#include "Wm4ContSphere3.h"
#include "Wm4IntrLine3Sphere3.h"
#include "Wm4IntrRay3Sphere3.h"
#include "Wm4IntrSegment3Sphere3.h"
#include "Wm4IntrSphere3Sphere3.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,SphereBV,BoundingVolume);
WM4_IMPLEMENT_STREAM(SphereBV);
WM4_IMPLEMENT_DEFAULT_NAME_ID(SphereBV,BoundingVolume);

//----------------------------------------------------------------------------
// The factory function of BoundingVolume creates SphereBV objects.  You may
// delete this and allow another bounding volume class to be the default.
//----------------------------------------------------------------------------
BoundingVolume* BoundingVolume::Create ()
{
    return WM4_NEW SphereBV;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
SphereBV::SphereBV ()
    :
    m_kSphere(Vector3f::ZERO,0.0f)
{
}
//----------------------------------------------------------------------------
SphereBV::SphereBV (const Sphere3f& rkSphere)
    :
    m_kSphere(rkSphere)
{
}
//----------------------------------------------------------------------------
void SphereBV::ComputeFromData (const Vector3fArray* pkVertices)
{
    if (pkVertices)
    {
        int iVQuantity = pkVertices->GetQuantity();
        const Vector3f* akVertex = pkVertices->GetData();
        m_kSphere = ContSphereAverage(iVQuantity,akVertex);
    }
}
//----------------------------------------------------------------------------
void SphereBV::ComputeFromData (const VertexBuffer* pkVBuffer)
{
    // TO DO.  This is a hack for now.  What to do if positions are (x,y,z,w)
    // with w not equal to 1?
    if (pkVBuffer)
    {
        int iPQuantity = pkVBuffer->GetVertexQuantity();

        m_kSphere.Center = Vector3f::ZERO;
        m_kSphere.Radius = 0.0f;
        int i;
        for (i = 0; i < iPQuantity; i++)
        {
            m_kSphere.Center += pkVBuffer->Position3(i);
        }
        m_kSphere.Center /= (float)iPQuantity;

        for (i = 0; i < iPQuantity; i++)
        {
            Vector3f kDiff = pkVBuffer->Position3(i) - m_kSphere.Center;
            float fRadiusSqr = kDiff.SquaredLength();
            if (fRadiusSqr > m_kSphere.Radius)
            {
                m_kSphere.Radius = fRadiusSqr;
            }
        }

        m_kSphere.Radius = Mathf::Sqrt(m_kSphere.Radius);
    }
}
//----------------------------------------------------------------------------
void SphereBV::TransformBy (const Transformation& rkTransform,
    BoundingVolume* pkResult)
{
    Sphere3f& rkTarget = ((SphereBV*)pkResult)->m_kSphere;
    rkTarget.Center = rkTransform.ApplyForward(m_kSphere.Center);
    rkTarget.Radius = rkTransform.GetNorm()*m_kSphere.Radius;
}
//----------------------------------------------------------------------------
int SphereBV::WhichSide (const Plane3f& rkPlane) const
{
    float fDistance = rkPlane.DistanceTo(m_kSphere.Center);

    if (fDistance <= -m_kSphere.Radius)
    {
        return -1;
    }

    if (fDistance >= m_kSphere.Radius)
    {
        return +1;
    }

    return 0;
}
//----------------------------------------------------------------------------
bool SphereBV::TestIntersection (const Vector3f& rkOrigin,
    const Vector3f& rkDirection, float fTMin, float fTMax) const
{
    if (fTMin == -Mathf::MAX_REAL)
    {
        Line3f kLine(rkOrigin,rkDirection);
        return IntrLine3Sphere3f(kLine,m_kSphere).Test();
    }

    assert(fTMin == 0.0f);
    if (fTMax == Mathf::MAX_REAL)
    {
        Ray3f kRay(rkOrigin,rkDirection);
        return IntrRay3Sphere3f(kRay,m_kSphere).Test();
    }

    assert(fTMax > fTMin);
    Segment3f kSegment;
    kSegment.Extent = 0.5f*fTMax;
    kSegment.Origin = rkOrigin + kSegment.Extent*rkDirection;
    kSegment.Direction = rkDirection;
    return IntrSegment3Sphere3f(kSegment,m_kSphere).Test();
}
//----------------------------------------------------------------------------
bool SphereBV::TestIntersection (const BoundingVolume* pkInput) const
{
    return IntrSphere3Sphere3<float>(m_kSphere,
        ((SphereBV*)pkInput)->m_kSphere).Test();
}
//----------------------------------------------------------------------------
void SphereBV::CopyFrom (const BoundingVolume* pkInput)
{
    m_kSphere = ((SphereBV*)pkInput)->m_kSphere;
}
//----------------------------------------------------------------------------
void SphereBV::GrowToContain (const BoundingVolume* pkInput)
{
    m_kSphere = MergeSpheres(m_kSphere,((SphereBV*)pkInput)->m_kSphere);
}
//----------------------------------------------------------------------------
bool SphereBV::Contains (const Vector3f& rkPoint) const
{
    return InSphere(rkPoint,m_kSphere);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SphereBV::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    BoundingVolume::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kSphere.Center);
    rkStream.Read(m_kSphere.Radius);

    WM4_END_DEBUG_STREAM_LOAD(SphereBV);
}
//----------------------------------------------------------------------------
void SphereBV::Link (Stream& rkStream, Stream::Link* pkLink)
{
    BoundingVolume::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool SphereBV::Register (Stream& rkStream) const
{
    return BoundingVolume::Register(rkStream);
}
//----------------------------------------------------------------------------
void SphereBV::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    BoundingVolume::Save(rkStream);

    // native data
    rkStream.Write(m_kSphere.Center);
    rkStream.Write(m_kSphere.Radius);

    WM4_END_DEBUG_STREAM_SAVE(SphereBV);
}
//----------------------------------------------------------------------------
int SphereBV::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return BoundingVolume::GetDiskUsed(rkVersion) +
        sizeof(m_kSphere);
}
//----------------------------------------------------------------------------
StringTree* SphereBV::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("center =",m_kSphere.Center));
    pkTree->Append(Format("radius =",m_kSphere.Radius));

    // children
    pkTree->Append(BoundingVolume::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
