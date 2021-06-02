///////////////////////////////////////////////////////////////////////  
//  ExportBegin.h
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

// storage-class specification
#ifndef ST_STORAGE_CLASS
    #if (defined(_WIN32) || defined(_XBOX)) && defined(SPEEDTREE_BUILD_SDK_AS_DLLS) && !defined(ST_STORAGE_CLASS)
        #define ST_STORAGE_CLASS __declspec(dllexport)
    #else
        #define ST_STORAGE_CLASS
    #endif
#endif

// inline function export control; ST_ILEX stands for "SpeedTree Inline Export"
#ifndef ST_ILEX
    #if (defined(_WIN32) || defined(_XBOX)) && !defined(SPEEDTREE_BUILD_SDK_AS_DLLS) && !defined(ST_ILEX)
        #define ST_ILEX __declspec(dllexport)
    #else
        #define ST_ILEX
    #endif
#endif

// specify calling convention
#ifndef ST_CALL_CONV
    #if (defined(_WIN32) || defined(_XBOX)) && !defined(ST_CALL_CONV)
        #define ST_CALL_CONV   __cdecl
    #else
        #define ST_CALL_CONV
    #endif
#endif

// pragma pack support
#ifndef ST_SUPPORTS_PRAGMA_PACK
    #if defined(_WIN32) || defined(_XBOX)
        #define ST_SUPPORTS_PRAGMA_PACK
    #endif
#endif

// mac export control
#ifdef __APPLE__
    #pragma export on
#endif



