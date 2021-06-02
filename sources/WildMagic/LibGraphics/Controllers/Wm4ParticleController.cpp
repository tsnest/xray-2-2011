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
#include "Wm4ParticleController.h"
#include "Wm4Particles.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,ParticleController,Controller);
WM4_IMPLEMENT_STREAM(ParticleController);
WM4_IMPLEMENT_DEFAULT_NAME_ID(ParticleController,Controller);

//----------------------------------------------------------------------------
ParticleController::ParticleController ()
    :
    SystemLinearAxis(Vector3f::UNIT_Z),
    SystemAngularAxis(Vector3f::UNIT_Z)
{
    SystemLinearSpeed = 0.0f;
    SystemAngularSpeed = 0.0f;
    m_iLQuantity = 0;
    m_afPointLinearSpeed = 0;
    m_akPointLinearAxis = 0;
    SystemSizeChange = 0.0f;
    m_afPointSizeChange = 0;
}
//----------------------------------------------------------------------------
ParticleController::~ParticleController ()
{
    WM4_DELETE[] m_afPointLinearSpeed;
    WM4_DELETE[] m_akPointLinearAxis;
    WM4_DELETE[] m_afPointSizeChange;
}
//----------------------------------------------------------------------------
void ParticleController::Reallocate (int iLQuantity)
{
    SystemLinearSpeed = 0.0f;
    SystemAngularSpeed = 0.0f;
    SystemLinearAxis = Vector3f::UNIT_Z;
    SystemAngularAxis = Vector3f::UNIT_Z;
    SystemSizeChange = 0.0f;

    WM4_DELETE[] m_afPointLinearSpeed;
    WM4_DELETE[] m_akPointLinearAxis;
    WM4_DELETE[] m_afPointSizeChange;

    m_iLQuantity = iLQuantity;
    if (m_iLQuantity > 0)
    {
        m_afPointLinearSpeed = WM4_NEW float[m_iLQuantity];
        m_akPointLinearAxis = WM4_NEW Vector3f[m_iLQuantity];
        m_afPointSizeChange = WM4_NEW float[m_iLQuantity];

        memset(m_afPointLinearSpeed,0,m_iLQuantity*sizeof(float));
        for (int i = 0; i < m_iLQuantity; i++)
        {
            m_akPointLinearAxis[i] = Vector3f::UNIT_Z;
        }

        memset(m_afPointSizeChange,0,m_iLQuantity*sizeof(float));
    }
    else
    {
        m_afPointLinearSpeed = 0;
        m_akPointLinearAxis = 0;
        m_afPointSizeChange = 0;
    }
}
//----------------------------------------------------------------------------
void ParticleController::SetObject (Object* pkObject)
{
    Controller::SetObject(pkObject);

    if (pkObject)
    {
        assert(pkObject->IsDerived(Particles::TYPE));
        Particles* pkParticles = StaticCast<Particles>(pkObject);
        Reallocate(pkParticles->Locations->GetQuantity());
    }
    else
    {
        Reallocate(0);
    }
}
//----------------------------------------------------------------------------
void ParticleController::UpdateSystemMotion (float fCtrlTime)
{
    Particles* pkParticle = StaticCast<Particles>(m_pkObject);

    float fDSize = fCtrlTime*SystemSizeChange;
    pkParticle->SizeAdjust += fDSize;
    if (pkParticle->SizeAdjust < 0.0f)
    {
        pkParticle->SizeAdjust = 0.0f;
    }

    float fDistance = fCtrlTime*SystemLinearSpeed;
    Vector3f kDTrn = fDistance*SystemLinearAxis;
    pkParticle->Local.SetTranslate(pkParticle->Local.GetTranslate() + kDTrn);

    float fAngle = fCtrlTime*SystemAngularSpeed;
    Matrix3f kDRot(SystemAngularAxis,fAngle);
    pkParticle->Local.SetRotate(kDRot*pkParticle->Local.GetRotate());
}
//----------------------------------------------------------------------------
void ParticleController::UpdatePointMotion (float fCtrlTime)
{
    Particles* pkParticle = StaticCast<Particles>(m_pkObject);
    int i, iAQuantity = pkParticle->GetActiveQuantity();
    float* afSize = pkParticle->Sizes->GetData();

    for (i = 0; i < iAQuantity; i++)
    {
        float fDSize = fCtrlTime*m_afPointSizeChange[i];
        afSize[i] += fDSize;
    }

    Vector3f* akLocation = pkParticle->Locations->GetData();
    for (i = 0; i < iAQuantity; i++)
    {
        float fDistance = fCtrlTime*m_afPointLinearSpeed[i];
        Vector3f kDTrn = fDistance*m_akPointLinearAxis[i];
        akLocation[i] += kDTrn;
    }
}
//----------------------------------------------------------------------------
bool ParticleController::Update (double dAppTime)
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
void ParticleController::Load (Stream& rkStream,
    Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // native data
    rkStream.Read(SystemLinearSpeed);
    rkStream.Read(SystemAngularSpeed);
    rkStream.Read(SystemLinearAxis);
    rkStream.Read(SystemAngularAxis);
    rkStream.Read(SystemSizeChange);

    rkStream.Read(m_iLQuantity);
    if (m_iLQuantity > 0)
    {
        Reallocate(m_iLQuantity);
        rkStream.Read(m_iLQuantity,m_afPointLinearSpeed);
        rkStream.Read(m_iLQuantity,m_akPointLinearAxis);
        rkStream.Read(m_iLQuantity,m_afPointSizeChange);
    }

    WM4_END_DEBUG_STREAM_LOAD(ParticleController);
}
//----------------------------------------------------------------------------
void ParticleController::Link (Stream& rkStream,
    Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ParticleController::Register (Stream& rkStream) const
{
    return Controller::Register(rkStream);
}
//----------------------------------------------------------------------------
void ParticleController::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // native data
    rkStream.Write(SystemLinearSpeed);
    rkStream.Write(SystemAngularSpeed);
    rkStream.Write(SystemLinearAxis);
    rkStream.Write(SystemAngularAxis);
    rkStream.Write(SystemSizeChange);

    // Write this to disk so that Load does not have to wait until the
    // controlled object is loaded and linked in order to allocate the
    // arrays.
    rkStream.Write(m_iLQuantity);
    if (m_iLQuantity > 0)
    {
        rkStream.Write(m_iLQuantity,m_afPointLinearSpeed);
        rkStream.Write(m_iLQuantity,m_akPointLinearAxis);
        rkStream.Write(m_iLQuantity,m_afPointSizeChange);
    }

    WM4_END_DEBUG_STREAM_SAVE(ParticleController);
}
//----------------------------------------------------------------------------
int ParticleController::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Controller::GetDiskUsed(rkVersion) +
        sizeof(SystemLinearSpeed) +
        sizeof(SystemAngularSpeed) +
        sizeof(SystemLinearAxis) +
        sizeof(SystemAngularAxis) +
        sizeof(SystemSizeChange) +
        sizeof(m_iLQuantity);

    if (m_iLQuantity > 0)
    {
        iSize += m_iLQuantity*sizeof(m_afPointLinearSpeed[0]);
        iSize += m_iLQuantity*sizeof(m_akPointLinearAxis[0]);
        iSize += m_iLQuantity*sizeof(m_afPointSizeChange[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* ParticleController::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("sys lin speed =",SystemLinearSpeed));
    pkTree->Append(Format("sys ang speed =",SystemAngularSpeed));
    pkTree->Append(Format("sys lin axis =",SystemLinearAxis));
    pkTree->Append(Format("sys ang axis =",SystemAngularAxis));
    pkTree->Append(Format("sys size change =",SystemSizeChange));

    // children
    pkTree->Append(Controller::SaveStrings());

    pkTree->Append(Format("point lin speed",m_iLQuantity,
        m_afPointLinearSpeed));

    pkTree->Append(Format("point lin axis",m_iLQuantity,
        m_akPointLinearAxis));

    pkTree->Append(Format("point size change",m_iLQuantity,
        m_afPointSizeChange));

    return pkTree;
}
//----------------------------------------------------------------------------
