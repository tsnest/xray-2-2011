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

//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::TSharedArray (int iQuantity, T* atArray)
{
    m_iQuantity = iQuantity;
    m_atArray = atArray;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::TSharedArray (const TSharedArray& rkShared)
{
    m_atArray = 0;
    *this = rkShared;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::~TSharedArray ()
{
    WM4_DELETE[] m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>& TSharedArray<T>::operator= (const TSharedArray& rkShared)
{
    WM4_DELETE[] m_atArray;
    m_iQuantity = rkShared.m_iQuantity;
    if (rkShared.m_atArray)
    {
        m_atArray = WM4_NEW T[m_iQuantity];
        for (int i = 0; i < m_iQuantity; i++)
        {
            m_atArray[i] = rkShared.m_atArray[i];
        }
    }
    else
    {
        m_atArray = 0;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <class T>
int TSharedArray<T>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class T>
T* TSharedArray<T>::GetData () const
{
    return m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::operator const T* () const
{
    return m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
TSharedArray<T>::operator T* ()
{
    return m_atArray;
}
//----------------------------------------------------------------------------
template <class T>
const T& TSharedArray<T>::operator[] (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_atArray[i];
}
//----------------------------------------------------------------------------
template <class T>
T& TSharedArray<T>::operator[] (int i)
{
    assert(0 <= i && i < m_iQuantity);
    return m_atArray[i];
}
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::SetActiveQuantity (int iActiveQuantity)
{
    assert(iActiveQuantity >= 0);
    m_iQuantity = iActiveQuantity;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
template <class T> 
Object* TSharedArray<T>::Factory (Stream& rkStream) 
{ 
    TSharedArray<T>* pkObject = WM4_NEW TSharedArray<T>; 
    Stream::Link* pkLink = WM4_NEW Stream::Link(pkObject); 
    pkObject->Load(rkStream,pkLink); 
    return pkObject; 
} 
//----------------------------------------------------------------------------
template <class T> 
void TSharedArray<T>::InitializeFactory () 
{ 
    if (!ms_pkFactory)
    { 
        ms_pkFactory = WM4_NEW TStringHashTable<FactoryFunction>(
            FACTORY_MAP_SIZE);
    } 
    ms_pkFactory->Insert(TYPE.GetName(),(FactoryFunction)Factory); 
} 
//----------------------------------------------------------------------------
template <class T> 
bool TSharedArray<T>::RegisterFactory () 
{ 
    if (!ms_bStreamRegistered) 
    { 
        Main::AddInitializer(TSharedArray<T>::InitializeFactory); 
        ms_bStreamRegistered = true; 
    } 
    return ms_bStreamRegistered; 
}
//----------------------------------------------------------------------------
template <class T>
Object* TSharedArray<T>::GetObjectByName (const std::string& rkName)
{
    return Object::GetObjectByName(rkName);
}
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Object::GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
template <class T>
Object* TSharedArray<T>::GetObjectByID (unsigned int uiID)
{
    return Object::GetObjectByID(uiID);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);
    rkStream.Read(m_iQuantity);
    m_atArray = WM4_NEW T[m_iQuantity];
    rkStream.Read(m_iQuantity,m_atArray);

    WM4_END_DEBUG_STREAM_LOAD(TSharedArray<T>);
}
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
template <class T>
bool TSharedArray<T>::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
template <class T>
void TSharedArray<T>::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);
    rkStream.Write(m_iQuantity);
    rkStream.Write(m_iQuantity,m_atArray);

    WM4_END_DEBUG_STREAM_SAVE(TSharedArray<T>);
}
//----------------------------------------------------------------------------
template <class T>
int TSharedArray<T>::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion) +
        sizeof(m_iQuantity) +
        m_iQuantity*sizeof(T);
}
//----------------------------------------------------------------------------
template <class T>
StringTree* TSharedArray<T>::SaveStrings (const char* acTitle)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));

    // children
    pkTree->Append(Object::SaveStrings());
    pkTree->Append(Format(acTitle,m_iQuantity,m_atArray));

    return pkTree;
}
//----------------------------------------------------------------------------
