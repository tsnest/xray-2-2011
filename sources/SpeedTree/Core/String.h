///////////////////////////////////////////////////////////////////////  
//  String.h
//
//  This file is part of IDV's lightweight STL-like container classes.
//  Like STL, these containers are templated and portable, but are thin
//  enough not to have some of the portability issues of STL nor will
//  it conflict with an application's similar libraries.  These containers
//  also contain some mechanisms for mitigating the inordinate number of
//  of heap allocations associated with the standard STL.
//
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization and may
//  not be copied or disclosed except in accordance with the terms of
//  that agreement
//
//      Copyright (c) 2003-2010 IDV, Inc.
//      All Rights Reserved.
//
//      IDV, Inc.
//      http://www.idvinc.com
//
//  *** Release version 5.2.0 ***


/////////////////////////////////////////////////////////////////////
// Preprocessor

#pragma once
#include <speedtree/core/ExportBegin.h>
#include <speedtree/core/Array.h>
#include <speedtree/core/Types.h>
#include <speedtree/core/FixedString.h>
#include <cstring>
#include <cstdarg>

#ifdef __GNUC__
    #include <stdio.h>
#else
    #include <cstdio>
#endif

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    //////////////////////////////////////////////////////////////////////
    // class CBasicString

    template <bool bUseCustomAllocator = true>
    class ST_STORAGE_CLASS CBasicString : public CArray<char, bUseCustomAllocator>
    {
    typedef CArray<char, bUseCustomAllocator> super;
    public:
                            CBasicString(void);
                            CBasicString(const CBasicString& cCopy);
                            CBasicString(const char* pchData);
    virtual                 ~CBasicString(void);

            // typical functions
            void            clear(void);
            void            resize(size_t uiSize);
            const char*     c_str(void) const;
            void            clip(void);
            size_t          length(void) const;
            CBasicString    substr(size_t uiStart, size_t uiCount = npos) const;
            size_t          find(char chFind, size_t uiStart = 0) const;

            // operator overloads
            void            operator += (const char* pchData);
            CBasicString    operator + (const char* pchData) const;
            void            operator += (const CBasicString& strRight);
            CBasicString    operator + (const CBasicString& strRight) const;
            void            operator += (const char& chRight);
            CBasicString    operator + (const char& chRight) const;
            CBasicString&   operator = (const CBasicString& strRight);
            CBasicString&   operator = (const char* pchData);
            bool            operator == (const CBasicString& strRight) const;
            bool            operator != (const CBasicString& strRight) const;
            bool            operator < (const CBasicString& strRight) const;
            bool            operator > (const CBasicString& strRight) const;

            // formatting
    static  CBasicString    Format(const char* pchFormat, ...);
            CBasicString    Extension(char chExtensionChar = '.') const;
            CBasicString    NoExtension(char chExtensionChar = '.') const;
            CBasicString    Path(CBasicString strDelimiters = "/\\") const;
            CBasicString    NoPath(CBasicString strDelimiters = "/\\") const;

            enum { npos = (st_uint32)-1 };
    };

    typedef CBasicString<true> CString;

    // include inline functions
    #include <speedtree/core/String.inl>

} // end namespace SpeedTree


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif

#include <speedtree/core/ExportEnd.h>

