////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_EXTENSIONS_H_INCLUDED
#define XRAY_EXTENSIONS_H_INCLUDED

#ifdef SN_TARGET_PS3
	namespace scestd = std;
	#include <assert.h>
	namespace std {
		void _SCE_Assert(const char *first, const char *second);
		void _Assert(const char *first, const char *second);
	} // namespace std
#endif // #ifdef SN_TARGET_PS3

#define BOOST_NO_EXCEPTIONS   
#include <boost/noncopyable.hpp>

#include <xray/macro_extensions.h>
#include <xray/optimization_extensions.h>
#include <xray/type_extensions.h>
#include <xray/platform_extensions.h>
#include <xray/detail_noncopyable.h>
#include <xray/debug/extensions.h>
#include <xray/memory_override_operators.h>
#include <xray/memory_extensions.h>
#include <xray/stdlib_extensions.h>
#include <xray/debug/static_cast_checked.h>
#include <xray/debug/pointer_cast.h>
#include <xray/command_line_extensions.h>
#include <xray/os_extensions.h>
#include <xray/timing_extensions.h>
#include <xray/threading_extensions.h>
#include <xray/uninitialized_reference.h>
#include <xray/type_enum_flags.h>
#include <xray/memory_debug_allocator.h>
#include <xray/strings_extensions.h>
#include <xray/debug/check_no_multithread.h>
#include <xray/logging/extensions.h>
#include <xray/pointer_extensions.h>
#include <xray/debug/debug_pointers.h>
#include <xray/containers_extensions.h>
#include <xray/math_extensions.h>
#include <xray/memory_chunk_reader.h>
#include <xray/memory_writer.h>
#include <xray/tasks_system.h>
#include <xray/configs.h>
#include <xray/resources.h>

#if !XRAY_CORE_BUILDING
	namespace xray {
		using math::float2;
		using math::float3;
		using math::float4;
		using math::float4x4;
	} // namespace xray
#endif // #if !XRAY_CORE_BUILDING

#endif // #ifndef XRAY_EXTENSIONS_H_INCLUDED