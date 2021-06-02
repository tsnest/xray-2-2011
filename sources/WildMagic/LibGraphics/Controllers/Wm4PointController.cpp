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
#include "Wm4PointController.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,PointController,Controller);
WM4_IMPLEMENT_STREAM(PointController);
WM4_IMPLEMENT_DEFAULT_NAME_ID(PointController,Controller);

//----------------------------------------------------------------------------
PointController::PointController ()
    :
    SystemLinearAxis(Vector3f::UNIT_Z),
    SystemAngularAxis(Vector3f::UNIT_Z)
{
    SystemLinearSpeed = 0.0f;
    SystemAngularSpeed = 0.0f;
    m_iPQuantity = 0;
    m_afPointLinearSpeed = 0;
    m_afPointAngularSpeed = 0;
    m_akPointLinearAxis = 0;
    m_akPointAngularAxis = 0;
}
//----------------------------------------------------------------------------
PointController::~PointController ()
{
    WM4_DELETE[] m_afPointLinearSpeed;
    WM4_DELETE[] m_afPointAngularSpeed;
    WM4_DELETE[] m_akPointLinearAxis;
    WM4_DELETE[] m_akPointAngularAxis;
}
//----------------------------------------------------------------------------
void PointController::Reallocate (int iPQuantity)
{
    SystemLinearSpeed = 0.0f;
    SystemAngularSpeed = 0.0f;
    SystemLinearAxis = Vector3f::UNIT_Z;
    SystemAngularAxis = Vector3f::UNIT_Z;

    WM4_DELETE[] m_afPointLinearSpeed;
    WM4_DELETE[] m_afPointAngularSpeed;
    WM4_DELETE[] m_akPointLinearAxis;
    WM4_DELETE[] m_akPointAngularAxis;

    m_iPQuantity = iPQuantity;
    if (m_iPQuantity > 0)
    {
        m_afPointLinearSpeed = WM4_NEW float[m_iPQuantity];
        m_afPointAngularSpeed = WM4_NEW float[m_iPQuantity];
        m_akPointLinearAxis = WM4_NEW Vector3f[m_iPQuantity];
        m_akPointAngularAxis = WM4_NEW Vector3f[m_iPQuantity];

        memset(m_afPointLinearSpeed,0,m_iPQuantity*sizeof(float));
        memset(m_afPointAngularSpeed,0,m_iPQuantity*sizeof(float));
        for (int i = 0; i < m_iPQuantity; i++)
        {
            m_akPointLinearAxis[i] = Vector3f::UNIT_Z;
            m_akPointAngularAxis[i] = Vector3f::UNIT_Z;
        }
    }
    else
    {
        m_afPointLinearSpeed = 0;
        m_afPointAngularSpeed = 0;
        m_akPointLinearAxis = 0;
        m_akPointAngularAxis = 0;
    }
}
//----------------------------------------------------------------------------
void PointController::SetObject (Object* pkObject)
{
    Controller::SetObject(pkObject);

    if (pkObject)
    {
        assert(pkObject->IsDerived(Polypoint::TYPE));
        Polypoint* pkPoints = StaticCast<Polypoint>(pkObject);
        Reallocate(pkPoints->VBuffer->GetVertexQuantity());
    }
    else
    {
        Reallocate(0);
    }
}
//----------------------------------------------------------------------------
void PointController::UpdateSystemMotion (float fCtrlTime)
{
    Polypoint* pkPoint = StaticCast<Polypoint>(m_pkObject);

    float fDistance = fCtrlTime*SystemLinearSpeed;
    Vector3f kDTrn = fDistance*SystemLinearAxis;
    pkPoint->Local.SetTranslate(pkPoint->Local.GetTranslate() + kDTrn);

    float fAngle = fCtrlTime*SystemAngularSpeed;
    Matrix3f kDRot(SystemAngularAxis,fAngle);
    pkPoint->Local.SetRotate(kDRot*pkPoint->Local.GetRotate());
}
//----------------------------------------------------------------------------
void PointController::UpdatePointMotion (float fCtrlTime)
{
    Polypoint* pkPoint = StaticCast<Polypoint>(m_pkObject);
    int i, iAQuantity = pkPoint->GetActiveQuantity();

    for (i = 0; i < iAQuantity; i++)
    {
        float fDistance = fCtrlTime*m_afPointLinearSpeed[i];
        Vector3f kDTrn = fDistance*m_akPointLinearAxis[i];
        pkPoint->VBuffer->Position3(i) += kDTrn;
    }

    if (pkPoint->VBuffer->GetAttributes().HasNormal())
    {
        for (i = 0; i < iAQuantity; i++)
        {
            float fAngle = fCtrlTime*m_afPointAngularSpeed[i];
            Matrix3f kDRot(m_akPointAngularAxis[i],fAngle);
            Vector3f kNormal = kDRot*pkPoint->VBuffer->Normal3(i);
            kNormal.Normalize();
            pkPoint->VBuffer->Normal3(i) = kNormal;
        }
    }
}
//----------------------------------------------------------------------------
bool PointController::Update (double dAppTime)
{
    if (!Controller::Update(dAppTime))
    {
        return false;
    }

    float fCtrlTime = (float)GetControlTime(dAppTime);

    UpdateSystemMotion(fCtrlTime);
    UpdatePointMotion(fCtrlTime);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void PointController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // native data
    rkStream.Read(SystemLinearSpeed);
    rkStream.Read(SystemAngularSpeed);
    rkStream.Read(SystemLinearAxis);
    rkStream.Read(SystemAngularAxis);

    int iVertexQuantity;
    rkStream.Read(iVertexQuantity);
    if (iVertexQuantity > 0)
    {
        Reallocate(iVertexQuantity);
        rkStream.Read(iVertexQuantity,m_afPointLinearSpeed);
        rkStream.Read(iVertexQuantity,m_afPointAngularSpeed);
        rkStream.Read(iVertexQuantity,m_akPointLinearAxis);
        rkStream.Read(iVertexQuantity,m_akPointAngularAxis);
    }

    WM4_END_DEBUG_STREAM_LOAD(PointController);
}
//----------------------------------------------------------------------------
void PointController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool PointController::Register (Stream& rkStream) const
{
    return Controller::Register(rkStream);
}
//----------------------------------------------------------------------------
void PointController::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // native data
    rkStream.Write(SystemLinearSpeed);
    rkStream.Write(SystemAngularSpeed);
    rkStream.Write(SystemLinearAxis);
    rkStream.Write(SystemAngularAxis);

    // Write this to disk so that Load does not have to wait until the
    // controlled object is loaded and linked in order to allocate the
    // arrays.
    rkStream.Write(m_iPQuantity);
    if (m_iPQuantity > 0)
    {
        rkStream.Write(m_iPQuantity,m_afPointLinearSpeed);
        rkStream.Write(m_iPQuantity,m_afPointAngularSpeed);
        rkStream.Write(m_iPQuantity,m_akPointLinearAxis);
        rkStream.Write(m_iPQuantity,m_akPointAngularAxis);
    }

    WM4_END_DEBUG_STREAM_SAVE(PointController);
}
//----------------------------------------------------------------------------
int PointController::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Controller::GetDiskUsed(rkVersion) +
        sizeof(SystemLinearSpeed) +
        sizeof(SystemAngularSpeed) +
        sizeof(SystemLinearAxis) +
        sizeof(SystemAngularAxis) +
        sizeof(m_iPQuantity);

    if (m_iPQuantity > 0)
    {
        iSize += m_iPQuantity*sizeof(m_afPointLinearSpeed[0]);
        iSize += m_iPQuantity*sizeof(m_afPointAngularSpeed[0]);
        iSize += m_iPQuantity*sizeof(m_akPointLinearAxis[0]);
        iSize += m_iPQuantity*sizeof(m_akPointAngularAxis[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* PointController::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("sys lin speed =",SystemLinearSpeed));
    pkTree->Append(Format("sys ang speed =",SystemAngularSpeed));
    pkTree->Append(Format("sys lin axis =",SystemLinearAxis));
    pkTree->Append(Format("sys ang axis =",SystemAngularAxis));

    // children
    pkTree->Append(Controller::SaveStrings());

    pkTree->Append(Format("point lin speed",m_iPQuantity,
        m_afPointLinearSpeed));

    pkTree->Append(Format("point ang speed",m_iPQuantity,
        m_afPointAngularSpeed));

    pkTree->Append(Format("point lin axis",m_iPQuantity,
        m_akPointLinearAxis));

    pkTree->Append(Format("point ang axis",m_iPQuantity,
        m_akPointAngularAxis));

    return pkTree;
}
//----------------------------------------------------------------------------
