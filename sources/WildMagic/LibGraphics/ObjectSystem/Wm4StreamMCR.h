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
// Version: 4.0.3 (2008/02/18)

#ifndef WM4STREAMMCR_H
#define WM4STREAMMCR_H

//----------------------------------------------------------------------------
#define WM4_DECLARE_STREAM \
public: \
    static bool RegisterFactory (); \
    static void InitializeFactory (); \
    static Object* Factory (Stream& rkStream); \
    virtual void Load (Stream& rkStream, Stream::Link* pkLink); \
    virtual void Link (Stream& rkStream, Stream::Link* pkLink); \
    virtual bool PostLink () { return true; } \
    virtual bool Register (Stream& rkStream) const; \
    virtual void Save (Stream& rkStream) const; \
    virtual int GetDiskUsed (const StreamVersion& rkVersion) const; \
    virtual StringTree* SaveStrings (const char* acTitle = 0); \
private: \
    static bool ms_bStreamRegistered
//----------------------------------------------------------------------------
#define WM4_DECLARE_STREAM_WITH_POSTLINK \
public: \
    static bool RegisterFactory (); \
    static void InitializeFactory (); \
    static Object* Factory (Stream& rkStream); \
    virtual void Load (Stream& rkStream, Stream::Link* pkLink); \
    virtual void Link (Stream& rkStream, Stream::Link* pkLink); \
    virtual bool PostLink (); \
    virtual bool Register (Stream& rkStream) const; \
    virtual void Save (Stream& rkStream) const; \
    virtual int GetDiskUsed (const StreamVersion& rkVersion) const; \
    virtual StringTree* SaveStrings (const char* acTitle = 0); \
private: \
    static bool ms_bStreamRegistered
//----------------------------------------------------------------------------
#define WM4_DECLARE_TEMPLATE_STREAM \
public: \
    static bool RegisterFactory (); \
    static void InitializeFactory (); \
    static Object* Factory (Stream& rkStream); \
    virtual void Load (Stream& rkStream, Stream::Link* pkLink); \
    virtual void Link (Stream& rkStream, Stream::Link* pkLink); \
    virtual bool PostLink () { return true; } \
    virtual bool Register (Stream& rkStream) const; \
    virtual void Save (Stream& rkStream) const; \
    virtual int GetDiskUsed (const StreamVersion& rkVersion) const; \
    virtual StringTree* SaveStrings (const char* acTitle = 0); \
private: \
    WM4_GRAPHICS_ITEM static bool ms_bStreamRegistered
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_STREAM(classname) \
bool classname::ms_bStreamRegistered = false; \
\
bool classname::RegisterFactory () \
{ \
    if (!ms_bStreamRegistered) \
    { \
        Main::AddInitializer(classname::InitializeFactory); \
        ms_bStreamRegistered = true; \
    } \
    return ms_bStreamRegistered; \
} \
\
void classname::InitializeFactory () \
{ \
    if (!ms_pkFactory) \
    { \
        ms_pkFactory = WM4_NEW TStringHashTable<FactoryFunction>( \
            FACTORY_MAP_SIZE); \
    } \
    ms_pkFactory->Insert(TYPE.GetName(),(FactoryFunction)Factory); \
} \
\
Object* classname::Factory (Stream& rkStream) \
{ \
    classname* pkObject = WM4_NEW classname; \
    Stream::Link* pkLink = WM4_NEW Stream::Link(pkObject); \
    pkObject->Load(rkStream,pkLink); \
    return pkObject; \
}
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_ABSTRACT_STREAM(classname) \
bool classname::ms_bStreamRegistered = false; \
\
bool classname::RegisterFactory () \
{ \
    if (!ms_bStreamRegistered) \
    { \
        Main::AddInitializer(classname::InitializeFactory); \
        ms_bStreamRegistered = true; \
    } \
    return ms_bStreamRegistered; \
} \
\
void classname::InitializeFactory () \
{ \
    if (!ms_pkFactory) \
    { \
        ms_pkFactory = WM4_NEW TStringHashTable<FactoryFunction>( \
            FACTORY_MAP_SIZE); \
    } \
    ms_pkFactory->Insert(TYPE.GetName(),(FactoryFunction)Factory); \
} \
\
Object* classname::Factory (Stream&) \
{ \
    assert(false); \
    return 0; \
}
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_TEMPLATE_STREAM(classname) \
template <> \
bool classname::ms_bStreamRegistered = false;
//----------------------------------------------------------------------------
#define WM4_REGISTER_STREAM(classname) \
static bool gs_bStreamRegistered_##classname = \
    classname::RegisterFactory ()
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define WM4_BEGIN_DEBUG_STREAM_LOAD \
    int iBeginLoad_ = rkStream.GetBufferNext()
#else
#define WM4_BEGIN_DEBUG_STREAM_LOAD
#endif
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define WM4_END_DEBUG_STREAM_LOAD(classname) \
    int iEndLoad_ = rkStream.GetBufferNext(); \
    int iUsedLoad_ = iEndLoad_ - iBeginLoad_ + GetType().GetDiskUsed(); \
    int iUsedReported_ = classname::GetDiskUsed(rkStream.GetVersion()); \
    assert(iUsedLoad_ == iUsedReported_)
#else
#define WM4_END_DEBUG_STREAM_LOAD(classname)
#endif
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define WM4_BEGIN_DEBUG_STREAM_SAVE \
    int iBeginSave_ = rkStream.GetBufferNext()
#else
#define WM4_BEGIN_DEBUG_STREAM_SAVE
#endif
//----------------------------------------------------------------------------
#ifdef _DEBUG
#define WM4_END_DEBUG_STREAM_SAVE(classname) \
    int iEndSave_ = rkStream.GetBufferNext(); \
    int iUsedSave_ = iEndSave_ - iBeginSave_; \
    int iUsedReported_ = classname::GetDiskUsed( \
        StreamVersion(StreamVersion::MAJOR,StreamVersion::MINOR)); \
    assert(iUsedSave_ == iUsedReported_)
#else
#define WM4_END_DEBUG_STREAM_SAVE(classname)
#endif
//----------------------------------------------------------------------------
// The WM4_PTRSIZE macro is used only by GetDiskUsed.  A pointer is written
// to 8 bytes on disk, whether running on a 32-bit or 64-bit system.  This
// change was required to support 64-bit platforms, where pointers are
// potentially 8 bytes in size.
#define WM4_PTRSIZE(value) 8
//----------------------------------------------------------------------------

#endif
