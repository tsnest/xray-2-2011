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
#include "Wm4CurveSegment.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,CurveSegment,Object);
WM4_IMPLEMENT_ABSTRACT_STREAM(CurveSegment);
WM4_IMPLEMENT_DEFAULT_NAME_ID(CurveSegment,Object);

//----------------------------------------------------------------------------
CurveSegment::CurveSegment (float fUMin, float fUMax)
{
    assert(fUMin < fUMax);

    m_fUMin = fUMin;
    m_fUMax = fUMax;
}
//----------------------------------------------------------------------------
CurveSegment::CurveSegment ()
{
    m_fUMin = 0.0f;
    m_fUMax = 0.0f;
}
//----------------------------------------------------------------------------
CurveSegment::~CurveSegment ()
{
}
//----------------------------------------------------------------------------
Vector3f CurveSegment::Tangent (float fU) const
{
    Vector3f kVelocity = PU(fU);
    kVelocity.Normalize();
    return kVelocity;
}
//----------------------------------------------------------------------------
Vector3f CurveSegment::Normal (float fU) const
{
    Vector3f kVelocity = PU(fU);
    Vector3f kAcceleration = PUU(fU);
    float fVDotV = kVelocity.Dot(kVelocity);
    float fVDotA = kVelocity.Dot(kAcceleration);
    Vector3f kNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    kNormal.Normalize();
    return kNormal;
}
//----------------------------------------------------------------------------
Vector3f CurveSegment::Binormal (float fU) const
{
    Vector3f kVelocity = PU(fU);
    Vector3f kAcceleration = PUU(fU);
    float fVDotV = kVelocity.Dot(kVelocity);
    float fVDotA = kVelocity.Dot(kAcceleration);
    Vector3f kNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    kNormal.Normalize();
    kVelocity.Normalize();
    Vector3f kBinormal = kVelocity.Cross(kNormal);
    return kBinormal;
}
//----------------------------------------------------------------------------
void CurveSegment::GetFrame (float fU, Vector3f& rkPosition,
    Vector3f& rkTangent, Vector3f& rkNormal, Vector3f& rkBinormal) const
{
    rkPosition = P(fU);
    Vector3f kVelocity = PU(fU);
    Vector3f kAcceleration = PUU(fU);
    float fVDotV = kVelocity.Dot(kVelocity);
    float fVDotA = kVelocity.Dot(kAcceleration);
    rkNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    rkNormal.Normalize();
    rkTangent = kVelocity;
    rkTangent.Normalize();
    rkBinormal = rkTangent.Cross(rkNormal);
}
//----------------------------------------------------------------------------
float CurveSegment::Curvature (float fU) const
{
    Vector3f kVelocity = PU(fU);
    float fSpeedSqr = kVelocity.SquaredLength();

    if (fSpeedSqr >= Mathf::ZERO_TOLERANCE)
    {
        Vector3f kAcceleration = PUU(fU);
        Vector3f kCross = kVelocity.Cross(kAcceleration);
        float fNumer = kCross.Length();
        float fDenom = Mathf::Pow(fSpeedSqr,1.5f);
        return fNumer/fDenom;
    }
    else
    {
        // curvature is indeterminate, just return 0
        return 0.0f;
    }
}
//----------------------------------------------------------------------------
float CurveSegment::Torsion (float fU) const
{
    Vector3f kVelocity = PU(fU);
    Vector3f kAcceleration = PUU(fU);
    Vector3f kCross = kVelocity.Cross(kAcceleration);
    float fDenom = kCross.SquaredLength();

    if (fDenom >= Mathf::ZERO_TOLERANCE)
    {
        Vector3f kJerk = PUUU(fU);
        float fNumer = kCross.Dot(kJerk);
        return fNumer/fDenom;
    }
    else
    {
        // torsion is indeterminate, just return 0
        return 0.0f;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void CurveSegment::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_fUMin);
    rkStream.Read(m_fUMax);

    WM4_END_DEBUG_STREAM_LOAD(CurveSegment);
}
//----------------------------------------------------------------------------
void CurveSegment::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool CurveSegment::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void CurveSegment::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(m_fUMin);
    rkStream.Write(m_fUMax);

    WM4_END_DEBUG_STREAM_SAVE(CurveSegment);
}
//----------------------------------------------------------------------------
int CurveSegment::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion) +
        sizeof(m_fUMin) +
        sizeof(m_fUMax);
}
//----------------------------------------------------------------------------
StringTree* CurveSegment::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("umin =",m_fUMin));
    pkTree->Append(Format("umax =",m_fUMax));

    // children
    pkTree->Append(Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
