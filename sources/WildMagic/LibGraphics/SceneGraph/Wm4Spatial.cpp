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
// Version: 4.0.3 (2007/06/07)

#include "Wm4GraphicsPCH.h"
#include "Wm4Spatial.h"
#include "Wm4Camera.h"
#include "Wm4Culler.h"
#include "Wm4Light.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Spatial,Object);
WM4_IMPLEMENT_ABSTRACT_STREAM(Spatial);

const char* Spatial::ms_aacCullingMode[MAX_CULLING_MODE] =
{
    "CULL_DYNAMIC",
    "CULL_ALWAYS",
    "CULL_NEVER"
};

//----------------------------------------------------------------------------
Spatial::Spatial ()
    :
    WorldBound(BoundingVolume::Create())
{
    Culling = CULL_DYNAMIC;
    WorldIsCurrent = false;
    WorldBoundIsCurrent = false;
    m_pkParent = 0;
    m_iStartEffect = 0;
}
//----------------------------------------------------------------------------
Spatial::~Spatial ()
{
    DetachAllGlobalStates();
    DetachAllLights();
    DetachAllEffects();
}
//----------------------------------------------------------------------------
void Spatial::UpdateGS (double dAppTime, bool bInitiator)
{
    UpdateWorldData(dAppTime);
    UpdateWorldBound();
    if (bInitiator)
    {
        PropagateBoundToRoot();
    }
}
//----------------------------------------------------------------------------
void Spatial::UpdateBS ()
{
    UpdateWorldBound();
    PropagateBoundToRoot();
}
//----------------------------------------------------------------------------
void Spatial::UpdateWorldData (double dAppTime)
{
    // update any controllers associated with this object
    UpdateControllers(dAppTime);

    int i;
    for (i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        m_kGlobalStates[i]->UpdateControllers(dAppTime);
    }

    for (i = 0; i < (int)m_kLights.size(); i++)
    {
        m_kLights[i]->UpdateControllers(dAppTime);
    }

    // update world transforms
    if (!WorldIsCurrent)
    {
        if (m_pkParent)
        {
            World.Product(m_pkParent->World,Local);
        }
        else
        {
            World = Local;
        }
    }
}
//----------------------------------------------------------------------------
void Spatial::PropagateBoundToRoot ()
{
    if (m_pkParent)
    {
        m_pkParent->UpdateWorldBound();
        m_pkParent->PropagateBoundToRoot();
    }
}
//----------------------------------------------------------------------------
GlobalState* Spatial::GetGlobalState (GlobalState::StateType eType) const
{
    // check if type of state already exists
    for (int i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        if (m_kGlobalStates[i]->GetStateType() == eType)
        {
            // type of state exists, return it
            return m_kGlobalStates[i];
        }
    }
    return 0;
}
//----------------------------------------------------------------------------
void Spatial::AttachGlobalState (GlobalState* pkState)
{
    assert(pkState);

    // Check if this type of state is already in the list.
    for (int i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        if (m_kGlobalStates[i]->GetStateType() == pkState->GetStateType())
        {
            // This type of state already exists, so replace it.
            m_kGlobalStates[i] = pkState;
            return;
        }
    }

    // This type of state is not in the current list, so add it.
    m_kGlobalStates.push_back(pkState);
}
//----------------------------------------------------------------------------
void Spatial::DetachGlobalState (GlobalState::StateType eType)
{
    std::vector<GlobalStatePtr>::iterator pkIter = m_kGlobalStates.begin();
    for (/**/; pkIter != m_kGlobalStates.end(); pkIter++)
    {
        GlobalState* pkState = *pkIter;
        if (pkState->GetStateType() == eType)
        {
            m_kGlobalStates.erase(pkIter);
            return;
        }
    }
}
//----------------------------------------------------------------------------
void Spatial::AttachLight (Light* pkLight)
{
    assert(pkLight);

    // Check if the light is already in the list.
    for (int i = 0; i < (int)m_kLights.size(); i++)
    {
        if (m_kLights[i] == pkLight)
        {
            // The light already exists, so do nothing.
            return;
        }
    }

    // The light is not in the current list, so add it.
    m_kLights.push_back(pkLight);
}
//----------------------------------------------------------------------------
void Spatial::DetachLight (Light* pkLight)
{
    std::vector<ObjectPtr>::iterator pkIter = m_kLights.begin();
    for (/**/; pkIter != m_kLights.end(); pkIter++)
    {
        if (pkLight == *pkIter)
        {
            m_kLights.erase(pkIter);
            return;
        }
    }
}
//----------------------------------------------------------------------------
void Spatial::AttachEffect (Effect* pkEffect)
{
    assert(pkEffect);

    // Check if the effect is already in the list.
    for (int i = 0; i < (int)m_kEffects.size(); i++)
    {
        if (m_kEffects[i] == pkEffect)
        {
            // The effect already exists, so do nothing.
            return;
        }
    }

    // The effect is not in the current list, so add it.
    m_kEffects.push_back(pkEffect);
}
//----------------------------------------------------------------------------
void Spatial::DetachEffect (Effect* pkEffect)
{
    std::vector<EffectPtr>::iterator pkIter = m_kEffects.begin();
    for (/**/; pkIter != m_kEffects.end(); pkIter++)
    {
        if (pkEffect == *pkIter)
        {
            m_kEffects.erase(pkIter);
            return;
        }
    }
}
//----------------------------------------------------------------------------
void Spatial::UpdateRS (std::vector<GlobalState*>* akGStack,
    std::vector<Light*>* pkLStack)
{
    bool bInitiator = (akGStack == 0);

    if (bInitiator)
    {
        // The order of preference is
        //   (1) Default global states are used.
        //   (2) Geometry can override them, but if global state FOOBAR
        //       has not been pushed to the Geometry leaf node, then
        //       the current FOOBAR remains in effect (rather than the
        //       default FOOBAR being used).
        //   (3) Effect can override default or Geometry render states.
        akGStack = WM4_NEW std::vector<GlobalState*>[
            GlobalState::MAX_STATE_TYPE];
        for (int i = 0; i < GlobalState::MAX_STATE_TYPE; i++)
        {
            akGStack[i].push_back(0);
        }

        // stack has no lights initially
        pkLStack = WM4_NEW std::vector<Light*>;

        // traverse to root and push states from root to this node
        PropagateStateFromRoot(akGStack,pkLStack);
    }
    else
    {
        // push states at this node
        PushState(akGStack,pkLStack);
    }

    // propagate the new state to the subtree rooted here
    UpdateState(akGStack,pkLStack);

    if (bInitiator)
    {
        WM4_DELETE[] akGStack;
        WM4_DELETE pkLStack;
    }
    else
    {
        // pop states at this node
        PopState(akGStack,pkLStack);
    }
}
//----------------------------------------------------------------------------
void Spatial::PropagateStateFromRoot (std::vector<GlobalState*>* akGStack,
    std::vector<Light*>* pkLStack)
{
    // traverse to root to allow downward state propagation
    if (m_pkParent)
    {
        m_pkParent->PropagateStateFromRoot(akGStack,pkLStack);
    }

    // push states onto current render state stack
    PushState(akGStack,pkLStack);
}
//----------------------------------------------------------------------------
void Spatial::PushState (std::vector<GlobalState*>* akGStack,
    std::vector<Light*>* pkLStack)
{
    int i;
    for (i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        int eType = m_kGlobalStates[i]->GetStateType();
        akGStack[eType].push_back(m_kGlobalStates[i]);
    }

    for (i = 0; i < (int)m_kLights.size(); i++)
    {
        Light* pkLight = StaticCast<Light>(m_kLights[i]);
        pkLStack->push_back(pkLight);
    }
}
//----------------------------------------------------------------------------
void Spatial::PopState (std::vector<GlobalState*>* akGStack,
    std::vector<Light*>* pkLStack)
{
    int i;
    for (i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        int eType = m_kGlobalStates[i]->GetStateType();
        akGStack[eType].pop_back();
    }

    for (i = 0; i < (int)m_kLights.size(); i++)
    {
        pkLStack->pop_back();
    }
}
//----------------------------------------------------------------------------
void Spatial::OnGetVisibleSet (Culler& rkCuller, bool bNoCull)
{
    if (Culling == CULL_ALWAYS)
    {
        return;
    }

    if (Culling == CULL_NEVER)
    {
        bNoCull = true;
    }

    unsigned int uiSavePlaneState = rkCuller.GetPlaneState();
    if (bNoCull || rkCuller.IsVisible(WorldBound))
    {
        GetVisibleSet(rkCuller,bNoCull);
    }
    rkCuller.SetPlaneState(uiSavePlaneState);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Spatial::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Object::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (WorldBound)
    {
        pkFound = WorldBound->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    int i;
    for (i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        if (m_kGlobalStates[i])
        {
            pkFound = m_kGlobalStates[i]->GetObjectByName(rkName);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    for (i = 0; i < (int)m_kLights.size(); i++)
    {
        if (m_kLights[i])
        {
            pkFound = m_kLights[i]->GetObjectByName(rkName);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    for (i = 0; i < (int)m_kEffects.size(); i++)
    {
        if (m_kEffects[i])
        {
            pkFound = m_kEffects[i]->GetObjectByName(rkName);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    // To avoid cycles in a recursive search of a scene graph for an object
    // with the specified name, the member m_pkParent is not checked.
    return 0;
}
//----------------------------------------------------------------------------
void Spatial::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);

    if (WorldBound)
    {
        WorldBound->GetAllObjectsByName(rkName,rkObjects);
    }

    int i;
    for (i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        if (m_kGlobalStates[i])
        {
            m_kGlobalStates[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }

    for (i = 0; i < (int)m_kLights.size(); i++)
    {
        if (m_kLights[i])
        {
            m_kLights[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }

    for (i = 0; i < (int)m_kEffects.size(); i++)
    {
        if (m_kEffects[i])
        {
            m_kEffects[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }

    // To avoid cycles in a recursive search of a scene graph for an object
    // with the specified name, the member m_pkParent is not checked.
}
//----------------------------------------------------------------------------
Object* Spatial::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Object::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (WorldBound)
    {
        pkFound = WorldBound->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    int i;
    for (i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        if (m_kGlobalStates[i])
        {
            pkFound = m_kGlobalStates[i]->GetObjectByID(uiID);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    for (i = 0; i < (int)m_kLights.size(); i++)
    {
        if (m_kLights[i])
        {
            pkFound = m_kLights[i]->GetObjectByID(uiID);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    for (i = 0; i < (int)m_kEffects.size(); i++)
    {
        if (m_kEffects[i])
        {
            pkFound = m_kEffects[i]->GetObjectByID(uiID);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    // To avoid cycles in a recursive search of a scene graph for an object
    // with the specified id, the member m_pkParent is not checked.
    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Spatial::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(Local);
    rkStream.Read(World);
    int iTmp;
    rkStream.Read(iTmp);
    Culling = (CullingMode)iTmp;

    rkStream.Read(WorldIsCurrent);
    rkStream.Read(WorldBoundIsCurrent);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkWorldBound
    pkLink->Add(pkObject);

    int i, iQuantity;
    rkStream.Read(iQuantity);
    m_kGlobalStates.resize(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);  // m_kGlobalStates[i]
        pkLink->Add(pkObject);
    }

    rkStream.Read(iQuantity);
    m_kLights.resize(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);  // m_kLights[i]
        pkLink->Add(pkObject);
    }

    rkStream.Read(iQuantity);
    m_kEffects.resize(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);  // m_kEffects[i]
        pkLink->Add(pkObject);
    }

    WM4_END_DEBUG_STREAM_LOAD(Spatial);
}
//----------------------------------------------------------------------------
void Spatial::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    WorldBound = (BoundingVolume*)rkStream.GetFromMap(pkLinkID);

    int i;
    for (i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_kGlobalStates[i] = (GlobalState*)rkStream.GetFromMap(pkLinkID);
    }

    for (i = 0; i < (int)m_kLights.size(); i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_kLights[i] = (Light*)rkStream.GetFromMap(pkLinkID);
    }

    for (i = 0; i < (int)m_kEffects.size(); i++)
    {
        pkLinkID = pkLink->GetLinkID();
        m_kEffects[i] = (Effect*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool Spatial::Register (Stream& rkStream) const
{
    if (!Object::Register(rkStream))
    {
        return false;
    }

    if (WorldBound)
    {
        WorldBound->Register(rkStream);
    }

    int i;
    for (i = 0; i < (int)m_kGlobalStates.size(); i++)
    {
        if (m_kGlobalStates[i])
        {
            m_kGlobalStates[i]->Register(rkStream);
        }
    }

    for (i = 0; i < (int)m_kLights.size(); i++)
    {
        if (m_kLights[i])
        {
            m_kLights[i]->Register(rkStream);
        }
    }

    for (i = 0; i < (int)m_kEffects.size(); i++)
    {
        if (m_kEffects[i])
        {
            m_kEffects[i]->Register(rkStream);
        }
    }

    // m_pkParent need not be registered since the parent itself must have
    // initiated the Register call to its children, 'this' being one of them.
    return true;
}
//----------------------------------------------------------------------------
void Spatial::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(Local);
    rkStream.Write(World);
    rkStream.Write((int)Culling);
    rkStream.Write(WorldIsCurrent);
    rkStream.Write(WorldBoundIsCurrent);

    // link data
    rkStream.Write(WorldBound);

    int iQuantity = (int)m_kGlobalStates.size();
    rkStream.Write(iQuantity);
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        rkStream.Write(m_kGlobalStates[i]);
    }

    iQuantity = (int)m_kLights.size();
    rkStream.Write(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        rkStream.Write(m_kLights[i]);
    }

    iQuantity = (int)m_kEffects.size();
    rkStream.Write(iQuantity);
    for (i = 0; i < iQuantity; i++)
    {
        rkStream.Write(m_kEffects[i]);
    }

    // m_pkParent need not be saved since 'this' will be attached as a
    // child in Node::Link.

    WM4_END_DEBUG_STREAM_SAVE(Spatial);
}
//----------------------------------------------------------------------------
int Spatial::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion) +
        Transformation::DISK_USED +  // Local
        Transformation::DISK_USED +  // World
        sizeof(char) + // WorldIsCurrent
        sizeof(char) + // WorldBoundIsCurrent
        WM4_PTRSIZE(WorldBound) +
        sizeof(int) + // Culling
        sizeof(int) +
            ((int)m_kGlobalStates.size())*WM4_PTRSIZE(GlobalStatePtr) +
        sizeof(int) + ((int)m_kLights.size())*WM4_PTRSIZE(LightPtr) +
        sizeof(int) + ((int)m_kEffects.size())*WM4_PTRSIZE(EffectPtr);
}
//----------------------------------------------------------------------------
StringTree* Spatial::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));

    if (Local.IsRSMatrix())
    {
        pkTree->Append(Format("local rotate =",Local.GetRotate()));
    }
    else
    {
        pkTree->Append(Format("local matrix =",Local.GetMatrix()));
    }

    pkTree->Append(Format("local trans =",Local.GetTranslate()));

    if (Local.IsRSMatrix())
    {
        pkTree->Append(Format("local scale =",Local.GetScale()));
    }
    else
    {
        pkTree->Append(Format("local scale part of matrix"));
    }

    if (World.IsRSMatrix())
    {
        pkTree->Append(Format("world rotate =",World.GetRotate()));
    }
    else
    {
        pkTree->Append(Format("world matrix =",World.GetMatrix()));
    }

    pkTree->Append(Format("world trans =",World.GetTranslate()));

    if (World.IsRSMatrix())
    {
        pkTree->Append(Format("world scale =",World.GetScale()));
    }
    else
    {
        pkTree->Append(Format("world scale part of matrix"));
    }

    pkTree->Append(Format("world bound =",WorldBound));
    pkTree->Append(Format("culling =",ms_aacCullingMode[Culling]));
    pkTree->Append(Format("start effect =",m_iStartEffect));

    // children
    pkTree->Append(Object::SaveStrings());

    int iQuantity = (int)m_kGlobalStates.size();
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        pkTree->Append(m_kGlobalStates[i]->SaveStrings());
    }

    iQuantity = (int)m_kLights.size();
    for (i = 0; i < iQuantity; i++)
    {
        pkTree->Append(m_kLights[i]->SaveStrings());
    }

    iQuantity = (int)m_kEffects.size();
    for (i = 0; i < iQuantity; i++)
    {
        pkTree->Append(m_kEffects[i]->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
