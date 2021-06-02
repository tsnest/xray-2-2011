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
// Version: 4.0.6 (2008/01/20)

#include "Wm4GraphicsPCH.h"
#include "Wm4Geometry.h"
#include "Wm4Culler.h"
#include "Wm4Light.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Geometry,Spatial);
WM4_IMPLEMENT_ABSTRACT_STREAM(Geometry);

//----------------------------------------------------------------------------
Geometry::Geometry ()
    :
    ModelBound(BoundingVolume::Create())
{
    memset(States,0,GlobalState::MAX_STATE_TYPE*sizeof(GlobalState*));
}
//----------------------------------------------------------------------------
Geometry::Geometry (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer)
    :
    VBuffer(pkVBuffer),
    IBuffer(pkIBuffer),
    ModelBound(BoundingVolume::Create())
{
    memset(States,0,GlobalState::MAX_STATE_TYPE*sizeof(GlobalState*));
    UpdateModelBound();
}
//----------------------------------------------------------------------------
Geometry::~Geometry ()
{
}
//----------------------------------------------------------------------------
void Geometry::UpdateMS (bool bUpdateNormals, int iTangentSpaceType)
{
    UpdateModelBound();

    if (bUpdateNormals)
    {
        UpdateModelNormals();
    }

    if (iTangentSpaceType > (int)GU_TANGENT_SPACE_NONE)
    {
        UpdateModelTangentSpace(iTangentSpaceType);
    }
}
//----------------------------------------------------------------------------
void Geometry::UpdateModelBound ()
{
    ModelBound->ComputeFromData(VBuffer);
}
//----------------------------------------------------------------------------
void Geometry::UpdateWorldBound ()
{
    ModelBound->TransformBy(World,WorldBound);
}
//----------------------------------------------------------------------------
void Geometry::UpdateState (std::vector<GlobalState*>* akGStack,
    std::vector<Light*>* pkLStack)
{
    // update global state
    int i;
    for (i = 0; i < GlobalState::MAX_STATE_TYPE; i++)
    {
        GlobalState* pkGState = 0;
        pkGState = akGStack[i].back();
        States[i] = pkGState;
    }

    // update lights
    int iLQuantity = (int)pkLStack->size();
    if (iLQuantity > 0)
    {
        if (m_spkLEffect)
        {
            m_spkLEffect->DetachAllLights();
        }
        else
        {
            m_spkLEffect = WM4_NEW LightingEffect;
            m_kEffects.insert(m_kEffects.begin(),
                StaticCast<Effect>(m_spkLEffect));
        }

        for (i = 0; i < iLQuantity; i++)
        {
            m_spkLEffect->AttachLight((*pkLStack)[i]);
        }

        m_spkLEffect->Configure();
    }
    else
    {
        if (m_spkLEffect)
        {
            if (m_kEffects.size() > 0)
            {
                // The first effect is m_spkLEffect.
                m_kEffects.erase(m_kEffects.begin());
            }
            m_spkLEffect = 0;
        }
    }
}
//----------------------------------------------------------------------------
void Geometry::GetVisibleSet (Culler& rkCuller, bool)
{
    rkCuller.Insert(this,0);
}
//----------------------------------------------------------------------------
void Geometry::UpdateWorldData (double dAppTime)
{
    Spatial::UpdateWorldData(dAppTime);
    World.GetHomogeneous(HWorld);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Geometry::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Spatial::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (ModelBound)
    {
        pkFound = ModelBound->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (VBuffer)
    {
        pkFound = VBuffer->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (IBuffer)
    {
        pkFound = IBuffer->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void Geometry::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Spatial::GetAllObjectsByName(rkName,rkObjects);

    if (ModelBound)
    {
        ModelBound->GetAllObjectsByName(rkName,rkObjects);
    }

    if (VBuffer)
    {
        VBuffer->GetAllObjectsByName(rkName,rkObjects);
    }

    if (IBuffer)
    {
        IBuffer->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* Geometry::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Spatial::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (ModelBound)
    {
        pkFound = ModelBound->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (VBuffer)
    {
        pkFound = VBuffer->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    if (IBuffer)
    {
        pkFound = IBuffer->GetObjectByID(uiID);
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
void Geometry::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Spatial::Load(rkStream,pkLink);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // ModelBound
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // VBuffer
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // IBuffer
    pkLink->Add(pkObject);

    WM4_END_DEBUG_STREAM_LOAD(Geometry);
}
//----------------------------------------------------------------------------
void Geometry::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Spatial::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    ModelBound = (BoundingVolume*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    VBuffer = (VertexBuffer*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    IBuffer = (IndexBuffer*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool Geometry::Register (Stream& rkStream) const
{
    // m_spkLEffect is created by an UpdateRS call and represents lights
    // on the path from scene graph root to geometry leaf.  Do not allow
    // Spatial to save it to disk.
    bool removedLightEffect = (m_spkLEffect && m_kEffects.size() > 0);
    if (removedLightEffect)
    {
        // The first effect is m_spkLEffect.
        m_kEffects.erase(m_kEffects.begin());
    }

    bool bFirstTime = Spatial::Register(rkStream);

    if (removedLightEffect)
    {
        // Reinsert m_spkLEffect into the front of the effects list.
        m_kEffects.insert(m_kEffects.begin(),
            StaticCast<Effect>(m_spkLEffect));
    }

    if (!bFirstTime)
    {
        return false;
    }

    if (ModelBound)
    {
        ModelBound->Register(rkStream);
    }

    if (VBuffer)
    {
        VBuffer->Register(rkStream);
    }

    if (IBuffer)
    {
        IBuffer->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void Geometry::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    // m_spkLEffect is created by an UpdateRS call and represents lights
    // on the path from scene graph root to geometry leaf.  Do not allow
    // Spatial to save it to disk.
    bool removedLightEffect = (m_spkLEffect && m_kEffects.size() > 0);
    if (removedLightEffect)
    {
        // The first effect is m_spkLEffect.
        m_kEffects.erase(m_kEffects.begin());
    }

    Spatial::Save(rkStream);

    if (removedLightEffect)
    {
        // Reinsert m_spkLEffect into the front of the effects list.
        m_kEffects.insert(m_kEffects.begin(),
            StaticCast<Effect>(m_spkLEffect));
    }

    // link data
    rkStream.Write(ModelBound);
    rkStream.Write(VBuffer);
    rkStream.Write(IBuffer);

    WM4_END_DEBUG_STREAM_SAVE(Geometry);
}
//----------------------------------------------------------------------------
int Geometry::GetDiskUsed (const StreamVersion& rkVersion) const
{
    // m_spkLEffect is created by an UpdateRS call and represents lights
    // on the path from scene graph root to geometry leaf.  Do not allow
    // Spatial to count it as disk space.
    bool removedLightEffect = (m_spkLEffect && m_kEffects.size() > 0);
    if (removedLightEffect)
    {
        // The first effect is m_spkLEffect.
        m_kEffects.erase(m_kEffects.begin());
    }

    int iSize = Spatial::GetDiskUsed(rkVersion) +
        WM4_PTRSIZE(ModelBound) +
        WM4_PTRSIZE(VBuffer) +
        WM4_PTRSIZE(IBuffer);

    if (removedLightEffect)
    {
        // Reinsert m_spkLEffect into the front of the effects list.
        m_kEffects.insert(m_kEffects.begin(),
            StaticCast<Effect>(m_spkLEffect));
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* Geometry::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));

    // children
    pkTree->Append(Spatial::SaveStrings());

    if (ModelBound)
    {
        pkTree->Append(ModelBound->SaveStrings());
    }

    if (VBuffer)
    {
        pkTree->Append(VBuffer->SaveStrings("vertices"));
    }

    if (IBuffer)
    {
        pkTree->Append(IBuffer->SaveStrings("indices"));
    }

    return pkTree;
}
//----------------------------------------------------------------------------
