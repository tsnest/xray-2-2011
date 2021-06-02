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
#include "Wm4IKController.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,IKController,Controller);
WM4_IMPLEMENT_STREAM(IKController);

//----------------------------------------------------------------------------
IKController::IKController (int iJointQuantity, IKJointPtr* aspkJoint,
    int iGoalQuantity, IKGoalPtr* aspkGoal)
{
#ifdef _DEBUG
    assert(iJointQuantity > 0 && aspkJoint);
    assert(iGoalQuantity > 0 && aspkGoal);
    int i;
    for (i = 0; i < iJointQuantity; i++)
    {
        assert(aspkJoint[i]);
    }
    for (i = 0; i < iGoalQuantity; i++)
    {
        assert(aspkGoal[i]);
    }
#endif

    m_iJointQuantity = iJointQuantity;
    m_aspkJoint = aspkJoint;
    m_iGoalQuantity = iGoalQuantity;
    m_aspkGoal = aspkGoal;
    Iterations = 128;
    OrderEndToRoot = true;
}
//----------------------------------------------------------------------------
IKController::IKController ()
{
    m_iJointQuantity = 0;
    m_aspkJoint = 0;
    m_iGoalQuantity = 0;
    m_aspkGoal = 0;
    Iterations = 0;
    OrderEndToRoot = false;
}
//----------------------------------------------------------------------------
IKController::~IKController ()
{
    WM4_DELETE[] m_aspkJoint;
    WM4_DELETE[] m_aspkGoal;
}
//----------------------------------------------------------------------------
bool IKController::Update (double dAppTime)
{
    if (!Controller::Update(dAppTime))
    {
        return false;
    }

    // Make sure effectors are all current in world space.  It is assumed
    // that the joints form a chain, so the world transforms of joint I
    // are the parent transforms for the joint I+1.
    int iJoint;
    for (iJoint = 0; iJoint < m_iJointQuantity; iJoint++)
    {
        m_aspkJoint[iJoint]->UpdateWorldSRT();
    }

    // Update joints one-at-a-time to meet goals.  As each joint is updated,
    // the nodes occurring in the chain after that joint must be made current
    // in world space.
    int iIter, i, j;
    IKJoint* pkJoint;
    if (OrderEndToRoot)
    {
        for (iIter = 0; iIter < Iterations; iIter++)
        {
            for (iJoint = 0; iJoint < m_iJointQuantity; iJoint++)
            {
                int iRJoint = m_iJointQuantity - 1 - iJoint;
                pkJoint = m_aspkJoint[iRJoint];

                for (i = 0; i < 3; i++)
                {
                    if (pkJoint->AllowTranslation[i])
                    {
                        if (pkJoint->UpdateLocalT(i))
                        {
                            for (j = iRJoint; j < m_iJointQuantity; j++)
                            {
                                m_aspkJoint[j]->UpdateWorldRT();
                            }
                        }
                    }
                }

                for (i = 0; i < 3; i++)
                {
                    if (pkJoint->AllowRotation[i])
                    {
                        if (pkJoint->UpdateLocalR(i))
                        {
                            for (j = iRJoint; j < m_iJointQuantity; j++)
                            {
                                m_aspkJoint[j]->UpdateWorldRT();
                            }
                        }
                    }
                }
            }
        }
    }
    else  // m_eOrder == PO_ROOT_TO_END
    {
        for (iIter = 0; iIter < Iterations; iIter++)
        {
            for (iJoint = 0; iJoint < m_iJointQuantity; iJoint++)
            {
                pkJoint = m_aspkJoint[iJoint];

                for (i = 0; i < 3; i++)
                {
                    if (pkJoint->AllowTranslation[i])
                    {
                        if (pkJoint->UpdateLocalT(i))
                        {
                            for (j = iJoint; j < m_iJointQuantity; j++)
                            {
                                m_aspkJoint[j]->UpdateWorldRT();
                            }
                        }
                    }
                }

                for (i = 0; i < 3; i++)
                {
                    if (pkJoint->AllowRotation[i])
                    {
                        if (pkJoint->UpdateLocalR(i))
                        {
                            for (j = iJoint; j < m_iJointQuantity; j++)
                            {
                                m_aspkJoint[j]->UpdateWorldRT();
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* IKController::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Controller::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        pkFound = m_aspkJoint[i]->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    for (i = 0; i < m_iGoalQuantity; i++)
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
void IKController::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Controller::GetAllObjectsByName(rkName,rkObjects);

    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        m_aspkJoint[i]->GetAllObjectsByName(rkName,rkObjects);
    }

    for (i = 0; i < m_iGoalQuantity; i++)
    {
        m_aspkGoal[i]->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* IKController::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Controller::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        pkFound = m_aspkJoint[i]->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    for (i = 0; i < m_iGoalQuantity; i++)
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
void IKController::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Controller::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iJointQuantity);
    rkStream.Read(m_iGoalQuantity);
    rkStream.Read(Iterations);
    rkStream.Read(OrderEndToRoot);

    // link data
    Object* pkObject;
    int i;
    m_aspkJoint = WM4_NEW IKJointPtr[m_iJointQuantity];
    for (i = 0; i < m_iJointQuantity; i++)
    {
        rkStream.Read(pkObject);  // m_aspkJoint[i]
        pkLink->Add(pkObject);
    }

    m_aspkGoal = WM4_NEW IKGoalPtr[m_iGoalQuantity];
    for (i = 0; i < m_iGoalQuantity; i++)
    {
        rkStream.Read(pkObject);  // m_aspkGoal[i]
        pkLink->Add(pkObject);
    }

    WM4_END_DEBUG_STREAM_LOAD(IKController);
}
//----------------------------------------------------------------------------
void IKController::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Controller::Link(rkStream,pkLink);

    Object* pkLinkID;
    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_aspkJoint[i] = (IKJoint*)rkStream.GetFromMap(pkLinkID);
    }

    for (i = 0; i < m_iGoalQuantity; i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_aspkGoal[i] = (IKGoal*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool IKController::Register (Stream& rkStream) const
{
    if (!Controller::Register(rkStream))
    {
        return false;
    }

    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        m_aspkJoint[i]->Register(rkStream);
    }

    for (i = 0; i < m_iGoalQuantity; i++)
    {
        m_aspkGoal[i]->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void IKController::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Controller::Save(rkStream);

    // native data
    rkStream.Write(m_iJointQuantity);
    rkStream.Write(m_iGoalQuantity);
    rkStream.Write(Iterations);
    rkStream.Write(OrderEndToRoot);

    // link data
    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        rkStream.Write(m_aspkJoint[i]);
    }

    for (i = 0; i < m_iGoalQuantity; i++)
    {
        rkStream.Write(m_aspkGoal[i]);
    }

    WM4_END_DEBUG_STREAM_SAVE(IKController);
}
//----------------------------------------------------------------------------
int IKController::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Controller::GetDiskUsed(rkVersion) +
        sizeof(m_iJointQuantity) +
        sizeof(m_iGoalQuantity) +
        sizeof(Iterations) +
        sizeof(OrderEndToRoot) +
        m_iJointQuantity*WM4_PTRSIZE(m_aspkJoint[0]) +
        m_iGoalQuantity*WM4_PTRSIZE(m_aspkGoal[0]);
}
//----------------------------------------------------------------------------
StringTree* IKController::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("joint quantity =",m_iJointQuantity));
    pkTree->Append(Format("goal quantity =",m_iGoalQuantity));
    pkTree->Append(Format("iterations =",Iterations));
    pkTree->Append(Format("end to root =",OrderEndToRoot));

    // children
    pkTree->Append(Controller::SaveStrings());

    // joints
    StringTree* pkJTree = WM4_NEW StringTree;
    pkJTree->Append(Format("joints"));
    int i;
    for (i = 0; i < m_iJointQuantity; i++)
    {
        pkJTree->Append(m_aspkJoint[i]->SaveStrings());
    }
    pkTree->Append(pkJTree);

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
