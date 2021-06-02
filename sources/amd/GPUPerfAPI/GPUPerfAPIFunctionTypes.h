//=============================================================================
//
// Author: GPU Developer Tools
//         AMD, Inc.
//
// This file defines function types to make it easier to dynamically load
// different GPUPerfAPI DLLs into an application that supports multiple APIs.
// Applications which statically link to GPUPerfAPI do not need to include
// this file.
//=============================================================================
// Copyright (c) 2010 Advanced Micro Devices, Inc.  All rights reserved.
//=============================================================================

#ifndef _GPUPERFAPI_FUNCTION_TYPES_H_
#define _GPUPERFAPI_FUNCTION_TYPES_H_

#include "GPUPerfAPITypes.h"

// Startup / exit
typedef GPA_Status( *GPA_InitializePtrType )();
typedef GPA_Status( *GPA_DestroyPtrType )();

// Context
typedef GPA_Status( *GPA_OpenContextPtrType )( void* context );
typedef GPA_Status( *GPA_CloseContextPtrType )();
typedef GPA_Status( *GPA_SelectContextPtrType )( void* context );

// Counter Interrogation
typedef GPA_Status( *GPA_GetNumCountersPtrType )( gpa_uint32* count );
typedef GPA_Status( *GPA_GetCounterNamePtrType )( gpa_uint32 index, const char** name );
typedef GPA_Status( *GPA_GetCounterDescriptionPtrType )( gpa_uint32 index, const char** description );
typedef GPA_Status( *GPA_GetCounterDataTypePtrType )( gpa_uint32 index, GPA_Type* counterDataType );
typedef GPA_Status( *GPA_GetCounterUsageTypePtrType ) ( gpa_uint32 index, GPA_Usage_Type* counterUsageType );
typedef GPA_Status( *GPA_GetDataTypeAsStrPtrType )( GPA_Type counterDataType, const char** typeStr );
typedef GPA_Status( *GPA_GetUsageTypeAsStrPtrType )( GPA_Usage_Type counterUsageType, const char** typeStr );
typedef const char*( *GPA_GetStatusAsStrPtrType ) ( GPA_Status status );

typedef GPA_Status( *GPA_EnableCounterPtrType )( gpa_uint32 index );
typedef GPA_Status( *GPA_DisableCounterPtrType )( gpa_uint32 index );
typedef GPA_Status( *GPA_GetEnabledCountPtrType )( gpa_uint32* count );
typedef GPA_Status( *GPA_GetEnabledIndexPtrType )( gpa_uint32 enabledNumber, gpa_uint32* enabledCounterIndex );

typedef GPA_Status( *GPA_IsCounterEnabledPtrType)( gpa_uint32 counterIndex );

typedef GPA_Status( *GPA_EnableCounterStrPtrType )( const char* counter );
typedef GPA_Status( *GPA_DisableCounterStrPtrType )( const char* counter );

typedef GPA_Status( *GPA_EnableAllCountersPtrType )();
typedef GPA_Status( *GPA_DisableAllCountersPtrType )();
typedef GPA_Status( *GPA_GetCounterIndexPtrType )( const char* counter, gpa_uint32* index );

typedef GPA_Status( *GPA_GetPassCountPtrType )( gpa_uint32* numPasses );

typedef GPA_Status( *GPA_BeginSessionPtrType )( gpa_uint32* sessionID );
typedef GPA_Status( *GPA_EndSessionPtrType )();

typedef GPA_Status( *GPA_BeginPassPtrType )();
typedef GPA_Status( *GPA_EndPassPtrType )();

typedef GPA_Status( *GPA_BeginSamplePtrType )( gpa_uint32 sampleID );
typedef GPA_Status( *GPA_EndSamplePtrType )();

typedef GPA_Status( *GPA_GetSampleCountPtrType ) ( gpa_uint32 sessionID, gpa_uint32 *samples );

typedef GPA_Status( *GPA_IsSampleReadyPtrType )( bool* readyResult, gpa_uint32 sessionID, gpa_uint32 sampleID );
typedef GPA_Status( *GPA_IsSessionReadyPtrType )( bool* readyResult, gpa_uint32 sessionID );
typedef GPA_Status( *GPA_GetSampleUInt64PtrType )( gpa_uint32 sessionID, gpa_uint32 sampleID, gpa_uint32 counterID, gpa_uint64* result );
typedef GPA_Status( *GPA_GetSampleUInt32PtrType )( gpa_uint32 sessionID, gpa_uint32 sampleID, gpa_uint32 counterIndex, gpa_uint32* result );
typedef GPA_Status( *GPA_GetSampleFloat32PtrType )( gpa_uint32 sessionID, gpa_uint32 sampleID, gpa_uint32 counterIndex, gpa_float32* result );
typedef GPA_Status( *GPA_GetSampleFloat64PtrType )( gpa_uint32 sessionID, gpa_uint32 sampleID, gpa_uint32 counterIndex, gpa_float64* result );

#endif // _GPUPERFAPI_FUNCTION_TYPES_H_
