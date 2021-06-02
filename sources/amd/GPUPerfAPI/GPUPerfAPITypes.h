//=============================================================================
//
// Author: GPU Developer Tools
//         AMD, Inc.
//
// Defines the data types and enumerations used by GPUPerfAPI.
// This file does not need to be directly included by an application
// that uses GPUPerfAPI.
//=============================================================================
// Copyright (c) 2010 Advanced Micro Devices, Inc.  All rights reserved.
//=============================================================================
#ifndef _GPUPERFAPI_TYPES_H_
#define _GPUPERFAPI_TYPES_H_

#include <limits.h>

// Type definitions
#ifdef _WIN32
typedef char    gpa_int8;
typedef short   gpa_int16;
typedef int     gpa_int32;
typedef __int64 gpa_int64;
typedef float   gpa_float32;
typedef double  gpa_float64;
typedef unsigned char    gpa_uint8;
typedef unsigned short   gpa_uint16;
typedef unsigned int     gpa_uint32;
typedef unsigned __int64 gpa_uint64;
#ifndef __cplusplus
typedef gpa_uint8 bool;
#endif
#endif // _WIN32

// Limit definitions
#define GPA_INT8_MAX SCHAR_MAX
#define GPA_INT16_MAX SHRT_MAX
#define GPA_INT32_MAX INT_MAX
#define GPA_INT64_MAX LLONG_MAX

#define GPA_UINT8_MAX UCHAR_MAX
#define GPA_UINT16_MAX USHRT_MAX
#define GPA_UINT32_MAX UINT_MAX
#define GPA_UINT64_MAX ULLONG_MAX


/// Status enumerations
typedef enum
{
   GPA_STATUS_OK = 0,
   GPA_STATUS_ERROR_NULL_POINTER,
   GPA_STATUS_ERROR_COUNTERS_NOT_OPEN,
   GPA_STATUS_ERROR_COUNTERS_ALREADY_OPEN,
   GPA_STATUS_ERROR_INDEX_OUT_OF_RANGE,
   GPA_STATUS_ERROR_NOT_FOUND,
   GPA_STATUS_ERROR_ALREADY_ENABLED,
   GPA_STATUS_ERROR_NO_COUNTERS_ENABLED,
   GPA_STATUS_ERROR_NOT_ENABLED,
   GPA_STATUS_ERROR_SAMPLING_NOT_STARTED,
   GPA_STATUS_ERROR_SAMPLING_ALREADY_STARTED,
   GPA_STATUS_ERROR_SAMPLING_NOT_ENDED,
   GPA_STATUS_ERROR_NOT_ENOUGH_PASSES,
   GPA_STATUS_ERROR_PASS_NOT_ENDED,
   GPA_STATUS_ERROR_PASS_NOT_STARTED,
   GPA_STATUS_ERROR_PASS_ALREADY_STARTED,
   GPA_STATUS_ERROR_SAMPLE_NOT_STARTED,
   GPA_STATUS_ERROR_SAMPLE_ALREADY_STARTED,
   GPA_STATUS_ERROR_SAMPLE_NOT_ENDED,
   GPA_STATUS_ERROR_CANNOT_CHANGE_COUNTERS_WHEN_SAMPLING,
   GPA_STATUS_ERROR_SESSION_NOT_FOUND,
   GPA_STATUS_ERROR_SAMPLE_NOT_FOUND,
   GPA_STATUS_ERROR_SAMPLE_NOT_FOUND_IN_ALL_PASSES,
   GPA_STATUS_ERROR_COUNTER_NOT_OF_SPECIFIED_TYPE,
   GPA_STATUS_ERROR_READING_COUNTER_RESULT,
   GPA_STATUS_ERROR_VARIABLE_NUMBER_OF_SAMPLES_IN_PASSES,
   GPA_STATUS_ERROR_FAILED,
   GPA_STATUS_ERROR_HARDWARE_NOT_SUPPORTED,
} GPA_Status;


/// Value type definitions
typedef enum
{
   GPA_TYPE_FLOAT32,             ///< Result will be a 32-bit float
   GPA_TYPE_FLOAT64,             ///< Result will be a 64-bit float
   GPA_TYPE_UINT32,              ///< Result will be a 32-bit unsigned int
   GPA_TYPE_UINT64,              ///< Result will be a 64-bit unsigned int
   GPA_TYPE_INT32,               ///< Result will be a 32-bit int
   GPA_TYPE_INT64,               ///< Result will be a 64-bit int
   GPA_TYPE__LAST                ///< Marker indicating last element
} GPA_Type;

/// Result usage type definitions
typedef enum
{
   GPA_USAGE_TYPE_RATIO,         ///< Result is a ratio of two different values or types
   GPA_USAGE_TYPE_PERCENTAGE,    ///< Result is a percentage, typically within [0,100] range, but may be higher for certain counters
   GPA_USAGE_TYPE_CYCLES,        ///< Result is in clock cycles
   GPA_USAGE_TYPE_MILLISECONDS,  ///< Result is in milliseconds
   GPA_USAGE_TYPE_BYTES,         ///< Result is in bytes
   GPA_USAGE_TYPE_ITEMS,         ///< Result is a count of items or objects (ie, vertices, triangles, threads, pixels, texels, etc)
   GPA_USAGE_TYPE_KILOBYTES,     ///< Result is in kilobytes
   GPA_USAGE_TYPE__LAST          ///< Marker indicating last element
} GPA_Usage_Type;

#endif // _GPUPERFAPI_TYPES_H_

