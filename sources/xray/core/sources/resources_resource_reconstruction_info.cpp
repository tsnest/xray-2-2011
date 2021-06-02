////////////////////////////////////////////////////////////////////////////
//	Created		: 10.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_resource_reconstruction_info.h>

namespace xray {
namespace resources {

resource_reconstruction_info::resource_reconstruction_info ()
: m_reconstruction_info_actuality_tick(0), m_reconstruction_size(u32(-1))
{
}

u32   resource_reconstruction_info::reconstruction_size () const 
{ 
	ASSERT										(m_reconstruction_size != u32(-1)); 
	return										m_reconstruction_size;
}

void   resource_reconstruction_info::update_reconstruction_info (u64 update_tick)
{
	if ( m_reconstruction_info_actuality_tick == update_tick )
		return;

	resource_base * const this_ptr			=	static_cast_checked<resource_base *>(this);

	m_reconstruction_size					=	this_ptr->memory_usage().size;
	resource_link_list & parents			=	this_ptr->get_parents();

	m_reconstruction_info_actuality_tick	=	update_tick;

	if ( parents.empty() )
		return;
	
	resource_link_list::mutex_raii	raii	(parents);
	for ( resource_link *	it_link	=	resource_link_list_front_no_dying(parents);
							it_link;
							it_link	=	resource_link_list_next_no_dying(it_link) )
	{
		it_link->resource->update_reconstruction_info	(update_tick);
		if ( !it_link->is_quality_link() )
			m_reconstruction_size			+=	it_link->resource->reconstruction_size();
	}
}

} // namespace resources
} // namespace xray
