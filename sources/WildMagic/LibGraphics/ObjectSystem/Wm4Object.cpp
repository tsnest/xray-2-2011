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
// Version: 4.0.2 (2008/10/11)

#include "Wm4GraphicsPCH.h"
#include "Wm4Object.h"
#include "Wm4Controller.h"
#include "Wm4StringTree.h"
using namespace Wm4;

const Rtti Object::TYPE("Wm4.Object",0);
unsigned int Object::ms_uiNextID = 0;
THashTable<unsigned int,Object*>* Object::InUse = 0;
char Object::NameAppend = '#';
TStringHashTable<FactoryFunction>* Object::ms_pkFactory = 0;
bool Object::ms_bStreamRegistered = false;

//----------------------------------------------------------------------------
Object::Object ()
{
    m_iReferences = 0;
    m_uiID = ms_uiNextID++;

    if (!InUse)
    {
        InUse = WM4_NEW THashTable<unsigned int,Object*>(
            FACTORY_MAP_SIZE);
    }

    InUse->Insert(m_uiID,this);
}
//----------------------------------------------------------------------------
Object::~Object ()
{
    // This assert traps the case when someone tries 'delete spkObject'
    // for a smart-pointer-based object.  Instead, use 'spkObject = 0'.
    assert(m_iReferences == 0);

    DetachAllControllers();
    assert(InUse);
    bool bFound = InUse->Remove(m_uiID);
    assert(bFound);
    (void)bFound;  // avoid compiler warning
}
//----------------------------------------------------------------------------
void Object::DecrementReferences ()
{
    if (--m_iReferences == 0)
    {
        WM4_DELETE this;
    }
}
//----------------------------------------------------------------------------
void Object::PrintInUse (const char* acFilename, const char* acMessage)
{
    FILE* pkFile = System::Fopen(acFilename,"at");
    assert(pkFile);

    System::Fprintf(pkFile,"%s\n",acMessage);

    assert(InUse);
    unsigned int uiID = 0;
    Object** ppkObject = InUse->GetFirst(&uiID);
    while (ppkObject)
    {
        System::Fprintf(pkFile,"id = %6d , type = %s\n",uiID,
            (const char*)(*ppkObject)->GetType().GetName());
        ppkObject = InUse->GetNext(&uiID);
    }

    System::Fprintf(pkFile,"\n\n");
    System::Fclose(pkFile);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// controllers
//----------------------------------------------------------------------------
int Object::GetControllerQuantity () const
{
    return (int)m_kControllers.size();
}
//----------------------------------------------------------------------------
Controller* Object::GetController (int i) const
{
    assert(0 <= i && i < (int)m_kControllers.size());
    return StaticCast<Controller>(m_kControllers[i]);
}
//----------------------------------------------------------------------------
void Object::AttachController (Controller* pkController)
{
    // Controllers may not be controlled.  This avoids arbitrarily complex
    // graphs of Objects.  It is possible to allowed controlled controllers,
    // but modify and proceed at your own risk...
    if (IsDerived(Controller::TYPE))
    {
        assert(false);
        return;
    }

    // controller must exist
    if (!pkController)
    {
        assert(pkController);
        return;
    }

    // check if controller is already in the list
    for (int i = 0; i < (int)m_kControllers.size(); i++)
    {
        if (pkController == m_kControllers[i])
        {
            // controller already exists, nothing to do
            return;
        }
    }

    // Bind the controller to the object.
    pkController->SetObject(this);

    // Controller not in current list, add it.
    m_kControllers.push_back(pkController);
}
//----------------------------------------------------------------------------
void Object::DetachController (Controller* pkController)
{
    std::vector<ObjectPtr>::iterator pkIter = m_kControllers.begin();
    for (/**/; pkIter != m_kControllers.end(); pkIter++)
    {
        if (pkController == *pkIter)
        {
            // Unbind the controller from the object.
            pkController->SetObject(0);

            // Remove the controller from the list.
            m_kControllers.erase(pkIter);

            return;
        }
    }
}
//----------------------------------------------------------------------------
void Object::DetachAllControllers ()
{
    for (int i = 0; i < (int)m_kControllers.size(); i++)
    {
        // Unbind the controller from the object.
        Controller* pkController = StaticCast<Controller>(m_kControllers[i]);
        pkController->SetObject(0);
    }
    m_kControllers.clear();
}
//----------------------------------------------------------------------------
bool Object::UpdateControllers (double dAppTime)
{
    bool bSomeoneUpdated = false;
    for (int i = 0; i < (int)m_kControllers.size(); i++)
    {
        Controller* pkController = StaticCast<Controller>(m_kControllers[i]);
        if (pkController->Update(dAppTime))
        {
            bSomeoneUpdated = true;
        }
    }
    return bSomeoneUpdated;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// copying
//----------------------------------------------------------------------------
ObjectPtr Object::Copy (bool bUniqueNames) const
{
    // save the object to a memory buffer
    Stream kSaveStream;
    kSaveStream.Insert((Object*)this);
    char* acBuffer = 0;
    int iBufferSize = 0;
    bool bSuccessful = kSaveStream.Save(acBuffer,iBufferSize);
    assert(bSuccessful);
    if (!bSuccessful)
    {
        return 0;
    }

    // load the object from the memory buffer
    Stream kLoadStream;
    bSuccessful = kLoadStream.Load(acBuffer,iBufferSize);
    assert(bSuccessful);
    if (!bSuccessful)
    {
        return 0;
    }
    WM4_DELETE[] acBuffer;

    if (bUniqueNames)
    {
        // generate unique names
        for (int i = 0; i < kLoadStream.GetOrderedQuantity(); i++)
        {
            Object* pkObject = kLoadStream.GetOrderedObject(i);
            assert(pkObject);
            const std::string& rkName = pkObject->GetName();
            int iLength = (int)rkName.length();
            if (iLength > 0)
            {
                // object has a name, append a character to make it unique
                const char* acName = rkName.c_str();
                char* acNewName = WM4_NEW char[iLength + 2];
                const size_t uiSize = (size_t)(iLength + 2);
                System::Strcpy(acNewName,uiSize,acName);
                acNewName[iLength] = NameAppend;
                acNewName[iLength+1] = 0;
                pkObject->SetName(std::string(acNewName));
                WM4_DELETE[] acNewName;
            }
        }
    }

    return kLoadStream.GetObjectAt(0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* Object::GetObjectByName (const std::string& rkName)
{
    if (rkName == m_kName)
    {
        return this;
    }

    for (int i = 0; i < (int)m_kControllers.size(); i++)
    {
        if (m_kControllers[i])
        {
            Object* pkFound = m_kControllers[i]->GetObjectByName(rkName);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void Object::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    if (rkName == m_kName)
    {
        rkObjects.push_back(this);
    }

    for (int i = 0; i < (int)m_kControllers.size(); i++)
    {
        if (m_kControllers[i])
        {
            m_kControllers[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }
}
//----------------------------------------------------------------------------
Object* Object::GetObjectByID (unsigned int uiID)
{
    if (uiID == m_uiID)
    {
        return this;
    }

    for (int i = 0; i < (int)m_kControllers.size(); i++)
    {
        if (m_kControllers[i])
        {
            Object* pkFound = m_kControllers[i]->GetObjectByID(uiID);
            if (pkFound)
            {
                return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
bool Object::RegisterFactory ()
{
    if (!ms_bStreamRegistered)
    {
        Main::AddInitializer(Object::InitializeFactory);
        Main::AddTerminator(Object::TerminateFactory);
        ms_bStreamRegistered = true;
    }
    return ms_bStreamRegistered;
}
//----------------------------------------------------------------------------
void Object::InitializeFactory ()
{
    if (!ms_pkFactory)
    {
        ms_pkFactory = WM4_NEW TStringHashTable<FactoryFunction>(
            FACTORY_MAP_SIZE);
    }
    ms_pkFactory->Insert(TYPE.GetName(),(FactoryFunction)Factory);
}
//----------------------------------------------------------------------------
void Object::TerminateFactory ()
{
    WM4_DELETE ms_pkFactory;
    ms_pkFactory = 0;
}
//----------------------------------------------------------------------------
Object* Object::Factory (Stream&)
{
    assert(false);
    return 0;
}
//----------------------------------------------------------------------------
void Object::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    // get old address of object, save it for linking phase
    Object* pkLinkID;
    rkStream.Read(pkLinkID);
    rkStream.InsertInMap(pkLinkID,pkLink);

    // name of object
    rkStream.Read(m_kName);

    // link data
    int iQuantity;
    rkStream.Read(iQuantity);
    m_kControllers.resize(iQuantity);
    for (int i = 0; i < iQuantity; i++)
    {
        Object* pkObject;
        rkStream.Read(pkObject);
        pkLink->Add(pkObject);
    }

    WM4_END_DEBUG_STREAM_LOAD(Object);
}
//----------------------------------------------------------------------------
void Object::Link (Stream& rkStream, Stream::Link* pkLink)
{
    for (int i = 0; i < (int)m_kControllers.size(); i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_kControllers[i] = (Controller*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool Object::Register (Stream& rkStream) const
{
    Object* pkThis = (Object*)this;  // conceptual constness
    if (rkStream.InsertInMap(pkThis,0))
    {
        // Used to ensure the objects are saved in the order corresponding to
        // a depth-first traversal of the scene.
        rkStream.InsertInOrdered(pkThis);

        for (int i = 0; i < (int)m_kControllers.size(); i++)
        {
            if (m_kControllers[i])
            {
                m_kControllers[i]->Register(rkStream);
            }
        }

        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Object::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    // RTTI name for factory lookup on Load
    rkStream.Write(std::string(GetType().GetName()));

    // address of object for unique ID on Load/Link
    rkStream.Write((Object*)this);

    // name of object
    rkStream.Write(m_kName);

    // link data
    int iQuantity = (int)m_kControllers.size();
    rkStream.Write(iQuantity);
    for (int i = 0; i < iQuantity; i++)
    {
        rkStream.Write(m_kControllers[i]);
    }

    WM4_END_DEBUG_STREAM_SAVE(Object);
}
//----------------------------------------------------------------------------
int Object::GetDiskUsed (const StreamVersion&) const
{
    // RTTI name
    int iUsed = GetType().GetDiskUsed();

    // address of object
    iUsed += WM4_PTRSIZE(this);

    // name of object
    iUsed += sizeof(int) + (int)m_kName.length();

    // controllers
    iUsed += sizeof(int) +
        ((int)m_kControllers.size())*WM4_PTRSIZE(ControllerPtr);

    return iUsed;
}
//----------------------------------------------------------------------------
StringTree* Object::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("this =",this));
    pkTree->Append(Format("ID   =",m_uiID));
    pkTree->Append(Format("refs =",m_iReferences));

    // children
    if (m_kControllers.size() > 0)
    {
        StringTree* pkCTree = WM4_NEW StringTree;
        pkCTree->Append(Format("controllers"));
        for (int i = 0; i < (int)m_kControllers.size(); i++)
        {
            if (m_kControllers[i])
            {
                pkCTree->Append(m_kControllers[i]->SaveStrings());
            }
        }
        pkTree->Append(pkCTree);
    }

    return pkTree;
}
//----------------------------------------------------------------------------
