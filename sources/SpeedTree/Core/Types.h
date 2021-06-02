///////////////////////////////////////////////////////////////////////  
//  Types.h
//
//  Definitions of the SpeedTree intrinsic types.
//
//
//  *** INTERACTIVE DATA VISUALIZATION (IDV) CONFIDENTIAL AND PROPRIETARY INFORMATION ***
//
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Interactive Data Visualization, Inc. and
//  may not be copied, disclosed, or exploited except in accordance with 
//  the terms of that agreement.
//
//      Copyright (c) 2003-2010 IDV, Inc.
//      All rights reserved in all media.
//
//      IDV, Inc.
//      http://www.idvinc.com
//
//  *** Release version 5.2.0 ***


///////////////////////////////////////////////////////////////////////  
//  Preprocessor

#pragma once
#ifdef _WIN32
    #ifndef _CRT_SECURE_NO_WARNINGS
        #define _CRT_SECURE_NO_WARNINGS 
    #endif
    #ifndef _CRT_SECURE_NO_DEPRECATE
        #define _CRT_SECURE_NO_DEPRECATE
    #endif
#endif

#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(push, 8)
#endif


///////////////////////////////////////////////////////////////////////  
//  All SpeedTree SDK classes and variables are under the namespace "SpeedTree"

namespace SpeedTree
{

    ///////////////////////////////////////////////////////////////////////  
    //  Setup types based on platform, but nothing too complex

    typedef bool            st_bool;
    typedef char            st_int8;
    typedef char            st_char;
    typedef short           st_int16;
    typedef int             st_int32;
    typedef unsigned char   st_uint8;
    typedef unsigned char   st_byte;
    typedef unsigned char   st_uchar;
    typedef unsigned short  st_uint16;
    typedef unsigned int    st_uint32;
    typedef float           st_float32;
    typedef double          st_float64;


    ///////////////////////////////////////////////////////////////////////  
    //  Compile time assertion

    #define ST_ASSERT_ON_COMPILE(expr) extern char AssertOnCompile[(expr) ? 1 : -1]

    ST_ASSERT_ON_COMPILE(sizeof(st_int8) == 1);
    ST_ASSERT_ON_COMPILE(sizeof(st_int16) == 2);
    ST_ASSERT_ON_COMPILE(sizeof(st_int32) == 4);
    ST_ASSERT_ON_COMPILE(sizeof(st_uint8) == 1);
    ST_ASSERT_ON_COMPILE(sizeof(st_uint16) == 2);
    ST_ASSERT_ON_COMPILE(sizeof(st_uint32) == 4);
    ST_ASSERT_ON_COMPILE(sizeof(st_float32) == 4);
    ST_ASSERT_ON_COMPILE(sizeof(st_float64) == 8);

    const st_int32 c_nSizeOfInt = 4;
    const st_int32 c_nSizeOfFloat = 4;
    const st_int32 c_nFixedStringLength = 256;

    // these types are still used internally with the following assumptions
    ST_ASSERT_ON_COMPILE(sizeof(int) == c_nSizeOfInt);
    ST_ASSERT_ON_COMPILE(sizeof(float) == c_nSizeOfFloat);


    ///////////////////////////////////////////////////////////////////////  
    //  Function equivalents of __min and __max macros

    template <class T> inline T st_min(const T& a, const T& b)
    {
        return (a < b) ? a : b;
    }

    template <class T> inline T st_max(const T& a, const T& b)
    {
        return (a > b) ? a : b;
    }

}


#ifdef ST_SUPPORTS_PRAGMA_PACK
    #pragma pack(pop)
#endif
