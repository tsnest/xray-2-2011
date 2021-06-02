////////////////////////////////////////////////////////////////////////////
//	Created		: 11.10.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_sub_fat_resource.h>
#include "fs_helper.h"

namespace xray {
namespace fs {

bool   sub_fat_resource::contains (fat_node<> const * work_node) const
{
	return ((pcstr)work_node > nodes_buffer()) && 
		   ((pcstr)work_node < nodes_buffer() + nodes_buffer_size());
}

void   sub_fat_resource::remove_from_parent ()
{
	if ( parent_db )
	{
		parent_db->remove_child	(this);
		parent_db			=	NULL;
	}
}

void   sub_fat_resource::add_child (sub_fat_resource * child)
{
	children.push_back			(child);
}

void   sub_fat_resource::remove_child (sub_fat_resource * child)
{
	children.erase				(child);
}

u32   sub_fat_resource::change_reference_count_change_may_destroy_this (int change)
{
	if ( change > 0 )
	{
		return								threading::interlocked_increment(m_reference_count);
	}
	else if ( change == -1 )
	{
		R_ASSERT							( m_reference_count );

		sub_fat_resource_ptr const			resource_holder( this );
		u32 const out_result			=	threading::interlocked_decrement(m_reference_count);
		R_ASSERT_CMP						( out_result, >=, 1 );
		return								out_result - 1;
	}
	else
		UNREACHABLE_CODE					( return 0 );
}

void   fat_header::reverse_bytes ()
{ 
	xray::fs::reverse_bytes		(num_nodes); 
	xray::fs::reverse_bytes		(buffer_size); 
}

void   fat_header::set_big_endian () 
{ 
	strings::copy				(endian_string, big_endian_string); 
}

void   fat_header::set_little_endian () 
{ 
	strings::copy				(endian_string, little_endian_string);
}

bool   fat_header::is_big_endian () 
{ 
	R_ASSERT					(endian_string[0]); 
	return						strings::equal(endian_string, big_endian_string);
}

} // namespace fs
} // namespace xray
