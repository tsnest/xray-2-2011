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
#include "Wm4IKJoint.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,IKJoint,Object);
WM4_IMPLEMENT_STREAM(IKJoint);

//----------------------------------------------------------------------------
IKJoint::IKJoint (Spatial* pkObject, int iGoalQuantity, IKGoalPtr* aspkGoal)
{
#ifdef _DEBUG
    assert(iGoalQuantity > 0 && aspkGoal);
    for (int j = 0; j < iGoalQuantity; j++)
    {
        assert(aspkGoal[j]);
    }
#endif

    m_pkObject = pkObject;
    m_iGoalQuantity = iGoalQuantity;
    m_aspkGoal = aspkGoal;

    for (int i = 0; i < 3; i++)
    {
        AllowTranslation[i] = false;
        MinTranslation[i] = -Mathf::MAX_REAL;
        MaxTranslation[i] = Mathf::MAX_REAL;
        AllowRotation[i] = false;
        MinRotation[i] = -Mathf::PI;
        MaxRotation[i] = Mathf::PI;
    }
}
//----------------------------------------------------------------------------
IKJoint::IKJoint ()
{
    m_pkObject = 0;
    m_iGoalQuantity = 0;
    m_aspkGoal = 0;

    for (int i = 0; i < 3; i++)
    {
        AllowTranslation[i] = false;
        MinTranslation[i] = -Mathf::MAX_REAL;
        MaxTranslation[i] = Mathf::MAX_REAL;
        AllowRotation[i] = false;
        MinRotation[i] = -Mathf::PI;
        MaxRotation[i] = Mathf::PI;
    }
}
//----------------------------------------------------------------------------
IKJoint::~IKJoint ()
{
    WM4_DELETE[] m_aspkGoal;
}
//----------------------------------------------------------------------------
void IKJoint::UpdateWorldSRT ()
{
    const Spatial* pkParent = m_pkObject->GetParent();
    if (pkParent)
    {
        m_pkObject->World.Product(pkParent->World,m_pkObject->Local);
    }
    else
    {
        m_pkObject->World = m_pkObject->Local;
    }
}
//----------------------------------------------------------------------------
void IKJoint::UpdateWorldRT ()
{
    const Spatial* pkParent = m_pkObject->GetParent();
    if (pkParent)
    {
        m_pkObject->World.SetRotate(pkParent->World.GetRotate() *
            m_pkObject->Local.GetRotate());
        m_pkObject->World.SetTranslate(pkParent->World.ApplyForward(
            m_pkObject->Local.GetTranslate()));
    }
    else
    {
        m_pkObject->World.SetRotate(m_pkObject->Local.GetRotate());
        m_pkObject->World.SetTranslate(m_pkObject->Local.GetTranslate());
    }
}
//----------------------------------------------------------------------------
Vector3f IKJoint::GetAxis (int i)
{
    const Spatial* pkParent = m_pkObject->GetParent();
    if (pkParent)
    {
        return pkParent->World.GetRotate().GetColumn(i);
    }
    else if (i == 0)
    {
        return Vector3f::UNIT_X;
    }
    else if (i == 1)
    {
        return Vector3f::UNIT_Y;
    }
    else  //  i == 2
    {
        return Vector3f::UNIT_Z;
    }
}
//----------------------------------------------------------------------------
bool IKJoint::UpdateLocalT (int i)
{
    Vector3f kU = GetAxis(i);
    float fNumer = 0.0f;
    float fDenom = 0.0f;

    float fOldNorm = 0.0f;
    IKGoal* pkGoal;
    int iG;
    for (iG = 0; iG < m_iGoalQuantity; iG++)
    {
        pkGoal = m_aspkGoal[iG];
        Vector3f kGmE = pkGoal->GetTargetPosition() -
            pkGoal->GetEffectorPosition();
        fOldNorm += kGmE.SquaredLength();
        fNumer += pkGoal->Weight*kU.Dot(kGmE);
        fDenom += pkGoal->Weight;
    }

    if (Mathf::FAbs(fDenom) <= Mathf::ZERO_TOLERANCE)
    {
        // weights were too small, no translation
        return false;
    }

    // desired distance to translate along axis(i)
    float fT = fNumer/fDenom;

    // clamp to range
    Vector3f kTrn = m_pkObject->Local.GetTranslate();
    float fDesired = kTrn[i] + fT;
    if ( fDesired > MinTranslation[i] )
    {
        if ( fDesired < MaxTranslation[i] )
        {
            kTrn[i] = fDesired;
        }
        else
        {
            fT = MaxTranslation[i] - kTrn[i];
            kTrn[i] = MaxTranslation[i];
        }
    }
    else
    {
        fT = MinTranslation[i] - kTrn[i];
        kTrn[i] = MinTranslation[i];
    }

    // test if step should be taken
    float fNewNorm = 0.0f;
    Vector3f kStep = fT*kU;
    for (iG = 0; iG < m_iGoalQuantity; iG++)
    {
        pkGoal = m_aspkGoal[iG];
        Vector3f kNewE = pkGoal->GetEffectorPosition() + kStep;
        Vector3f kDiff = pkGoal->GetTargetPosition() - kNewE;
        fNewNorm += kDiff.SquaredLength();
    }
    if ( fNewNorm >= fOldNorm )
    {
        // translation does not get effector closer to goal
        return false;
    }

    // update the local translation
    m_pkObject->Local.SetTranslate(kTrn);
    return true;
}
//----------------------------------------------------------------------------
bool IKJoint::UpdateLocalR (int i)
{
    Vector3f kU = GetAxis(i);
    float fNumer = 0.0f;
    float fDenom = 0.0f;

    float fOldNorm = 0.0f;
    IKGoal* pkGoal;
    int iG;
    for (iG = 0; iG < m_iGoalQuantity; iG++)
    {
        pkGoal = m_aspkGoal[iG];
        Vector3f kEmP = pkGoal->GetEffectorPosition() -
            m_pkObject->World.GetTranslate();
        Vector3f kGmP = pkGoal->GetTargetPosition() -
            m_pkObject->World.GetTranslate();
        Vector3f kGmE = pkGoal->GetTargetPosition() -
            pkGoal->GetEffectorPosition();
        fOldNorm += kGmE.SquaredLength();
        Vector3f kUxEmP = kU.Cross(kEmP);
        Vector3f kUxUxEmP = kU.Cross(kUxEmP);
        fNumer += pkGoal->Weight*kGmP.Dot(kUxEmP);
        fDenom -= pkGoal->Weight*kGmP.Dot(kUxUxEmP);
    }

    if (fNumer*fNumer + fDenom*fDenom <= Mathf::ZERO_TOLERANCE)
    {
        // undefined atan2, no rotation
        return false;
    }

    // desired angle to rotate about axis(i)
    float fTheta = Mathf::ATan2(fNumer,fDenom);

    // factor local rotation into Euler angles
    float afEuler[3];
    m_pkObject->Local.GetRotate().ToEulerAnglesZYX(afEuler[2],afEuler[1],
        afEuler[0]);

    // clamp to range
    float fDesired = afEuler[i] + fTheta;
    if (fDesired > MinRotation[i])
    {
        if (fDesired < MaxRotation[i])
        {
            afEuler[i] = fDesired;
        }
        else
        {
            fTheta = MaxRotation[i] - afEuler[i];
            afEuler[i] = MaxRotation[i];
        }
    }
    else
    {
        fTheta = MinRotation[i] - afEuler[i];
        afEuler[i] = MinRotation[i];
    }

    // test if step should be taken
    float fNewNorm = 0.0f;
    Matrix3f kRot(kU,fTheta);
    for (iG = 0; iG < m_iGoalQuantity; iG++)
    {
        pkGoal = m_aspkGoal[iG];
        Vector3f kEmP = pkGoal->GetEffectorPosition() -
            m_pkObject->World.GetTranslate();
        Vector3f kNewE = m_pkObject->World.GetTranslate() + kRot*kEmP;
        Vector3f kGmE = pkGoal->GetTargetPosition() - kNewE;
        fNewNorm += kGmE.SquaredLength();
    }

    if (fNewNorm >= fOldNorm)
    {
        // rotation does not get effector closer to goal
        return false;
    }

    // update the local rotation
    kRot.FromEulerAnglesZYX(afEuler[2],afEuler[1],afEuler[0]);
    m_pkObject->Local.SetRotate(kRot);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* IKJoint::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_pkObject)
    {
        pkFound = m_pkObject->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    for (int i = 0; i < m_iGoalQuantity; i++)
    {
        pkFound = m_aspkGoal[i]->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void IKJoint::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);

    if (m_pkObject)
    {
        m_pkObject->GetAllObjectsByName(rkName,rkObjects);
    }

    for (int i = 0; i < m_iGoalQuantity; i++)
    {
        m_aspkGoal[i]->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* IKJoint::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_pkObject)
    {
        pkFound = m_pkObject->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    for (int i = 0; i < m_iGoalQuantity; i++)
    {
        pkFound = m_aspkGoal[i]->GetObjectByID(uiID);
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
void IKJoint::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(3,AllowTranslation);
    rkStream.Read(3,MinTranslation);
    rkStream.Read(3,MaxTranslation);
    rkStream.Read(3,AllowRotation);
    rkStream.Read(3,MinRotation);
    rkStream.Read(3,MaxRotation);
    rkStream.Read(m_iGoalQuantity);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_pkObject
    pkLink->Add(pkObject);
    m_aspkGoal = WM4_NEW IKGoalPtr[m_iGoalQuantity];
    for (int i = 0; i < m_iGoalQuantity; i++)
    {
        rkStream.Read(pkObject);  // m_aspkGoal[i]
        pkLink->Add(pkObject);
    }

    WM4_END_DEBUG_STREAM_LOAD(IKJoint);
}
//----------------------------------------------------------------------------
void IKJoint::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_pkObject = (Spatial*)rkStream.GetFromMap(pkLinkID);

    for (int i = 0; i < m_iGoalQuantity; i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_aspkGoal[i] = (IKGoal*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool IKJoint::Register (Stream& rkStream) const
{
    if (!Object::Register(rkStream)) 
    {
        return false;
    }

    if (m_pkObject)
    {
        m_pkObject->Register(rkStream);
    }

    for (int i = 0; i < m_iGoalQuantity; i++)
    {
        m_aspkGoal[i]->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void IKJoint::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(3,AllowTranslation);
    rkStream.Write(3,MinTranslation);
    rkStream.Write(3,MaxTranslation);
    rkStream.Write(3,AllowRotation);
    rkStream.Write(3,MinRotation);
    rkStream.Write(3,MaxRotation);
    rkStream.Write(m_iGoalQuantity);

    // link data
    rkStream.Write(m_pkObject);
    for (int i = 0; i < m_iGoalQuantity; i++)
    {
        rkStream.Write(m_aspkGoal[i]);
    }

    WM4_END_DEBUG_STREAM_SAVE(IKJoint);
}
//----------------------------------------------------------------------------
int IKJoint::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion) +
        3*sizeof(char) + // AllowTranslation[0..2]
        3*sizeof(MinTranslation[0]) +
        3*sizeof(MaxTranslation[0]) +
        3*sizeof(char) + // AllowRotation[0..2]
        3*sizeof(MinRotation[0]) +
        3*sizeof(MaxRotation[0]) +
        WM4_PTRSIZE(m_pkObject) +
        sizeof(m_iGoalQuantity) +
        m_iGoalQuantity*WM4_PTRSIZE(m_aspkGoal[0]);
}
//----------------------------------------------------------------------------
StringTree* IKJoint::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));

    const size_t uiSize = 128;
    char acDummy[uiSize];
    int i;
    Vector3f kTmp;

    System::Strcpy(acDummy,uiSize,"( ");
    for (i = 0; i < 3; i++)
    {
        if (AllowTranslation[i])
        {
            System::Strcat(acDummy,uiSize,"true ");
        }
        else
        {
            System::Strcat(acDummy,uiSize,"false ");
        }
    }
    System::Strcat(acDummy,uiSize,")");

    pkTree->Append(Format("allow trn =",acDummy));

    kTmp = Vector3f(MinTranslation[0],MinTranslation[1],MinTranslation[2]);
    pkTree->Append(Format("min trn =",kTmp));
    kTmp = Vector3f(MaxTranslation[0],MaxTranslation[1],MaxTranslation[2]);
    pkTree->Append(Format("max trn =",kTmp));

    System::Strcpy(acDummy,uiSize,"( ");
    for (i = 0; i < 3; i++)
    {
        if (AllowRotation[i])
        {
            System::Strcat(acDummy,uiSize,"true ");
        }
        else
        {
            System::Strcat(acDummy,uiSize,"false ");
        }
    }
    System::Strcat(acDummy,uiSize,")");

    pkTree->Append(Format("allow rot =",acDummy));

    kTmp = Vector3f(MinRotation[0],MinRotation[1],MinRotation[2]);
    pkTree->Append(Format("min rot =",kTmp));
    kTmp = Vector3f(MaxRotation[0],MaxRotation[1],MaxRotation[2]);
    pkTree->Append(Format("max rot =",kTmp));
    pkTree->Append(Format("goal quantity =",m_iGoalQuantity));

    // children
    pkTree->Append(Object::SaveStrings());

    // goals
    StringTree* pkGTree = WM4_NEW StringTree;
    pkGTree->Append(Format("goals"));
    for (i = 0; i < m_iGoalQuantity; i++)
    {
        pkGTree->Append(m_aspkGoal[i]->SaveStrings());
    }
    pkTree->Append(pkGTree);

    return pkTree;
}
//----------------------------------------------------------------------------
