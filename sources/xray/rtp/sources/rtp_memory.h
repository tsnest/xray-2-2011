////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RTP_MEMORY_H_INCLUDED
#define RTP_MEMORY_H_INCLUDED

#include <xray/rtp/api.h>

namespace xray {
namespace rtp {

extern allocator_type*							g_allocator;

#define USER_ALLOCATOR							*::xray::rtp::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

} // namespace rtp
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::rtp::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::rtp::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::rtp::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::rtp::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::rtp::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::rtp::g_allocator, type, count )

#endif // #ifndef RTP_MEMORY_H_INCLUDED