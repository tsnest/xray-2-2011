////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mount_referers.h"
#include "nodes.h"
#include <xray/threading_functions_tls.h>

namespace xray {
namespace vfs {

static u32 const s_ready_referers_tls_key	=	threading::tls_create_key();

ready_referers_list *	get_ready_referers_list	(memory::base_allocator * allocator)
{
	ready_referers_list * const out_list	=	threading::get_or_create_tls_value<ready_referers_list>
													(s_ready_referers_tls_key, allocator);
	return									out_list;
}

void   free_ready_referers_list			()
{
	threading::free_tls_value<ready_referers_list>	(s_ready_referers_tls_key);
}

void	dispatch_ready_referers_list	()
{
	ready_referers_list * const list	=	get_ready_referers_list(NULL);
	if ( !list )
		return;

	while ( !list->empty() )
	{
		mount_referer *	referer			=	list->pop_front();

		referer->callback					(referer->result);

		memory::base_allocator * allocator	=	referer->allocator;
		XRAY_DELETE_IMPL					(allocator, referer);
	}
}

} // namespace vfs
} // namespace xray
