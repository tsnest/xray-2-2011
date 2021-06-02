////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_memory.h>

namespace xray {
namespace resources {

memory_type		nocache_memory			("memory_type_non_cacheable");
memory_type		managed_memory			("managed");
memory_type		unmanaged_memory		("unmanaged");

memory_type::memory_type				(pcstr name)
	: 
	m_name				(name), 
	sort_actuality_tick	(0), 
	listen_type			(listen_none),
	m_next				(NULL),
	in_list				(false)
{
}

fixed_string512	  memory_usage_type::log_string () const
{
	fixed_string512							out_result;	
	out_result.appendf						("%d ", size);

	if ( !type )
		out_result						+=	"unknown allocator";
	else 
		out_result						+=	type->name();

	out_result							+=	" bytes";
	return									out_result;
}

} // namespace resources
} // namespace xray


