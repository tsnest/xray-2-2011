////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/search_base.h>

namespace xray {
namespace ai {
namespace planning {

static const u32		s_max_vertex_count	= 4096;
static const u32		s_hash_size			= 256;

search_base::search_base( ) :
	m_vertex_allocator	( s_max_vertex_count ),
	m_vertex_manager	( m_vertex_allocator, s_hash_size, s_max_vertex_count ),
	m_priority_queue	( m_vertex_manager, s_max_vertex_count )
{
}

u32	 search_base::max_vertex_count	( )
{
	return				s_max_vertex_count;
}

} // namespace planning
} // namespace ai
} // namespace xray