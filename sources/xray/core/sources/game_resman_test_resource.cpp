////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_test_resource.h"
#include "game_resman_test_utils.h"

namespace xray {
namespace resources {

test_resource::test_resource (u32 allocator, pcstr name, u32 size, u32 quality_levels_count)
:	positional_unmanaged_resource	(quality_levels_count), 
	m_int_name						(0), 
	m_size							(size), 
	m_allocator						(allocator)
{
	bool const is_int_name		=	strings::convert_string_to_number	(name, & m_int_name);

	fixed_vector<pstr, 3>			allocator_names;
	allocator_names.push_back		("A");
	allocator_names.push_back		("B");
	allocator_names.push_back		("C");
	fixed_string<8>	const	allocator_name	=	(allocator != u32(-1)) ? 
												allocator_names[allocator] : "MT";

	if ( is_int_name )
		m_name.assignf				("'%d%s'", m_int_name, allocator_name.c_str());
	else
		m_name					=	name;

	m_name.appendf					(" [ruid %d] [rptr 0x%x]", uid(), dynamic_cast<pvoid>(this));

	log_test_resource				(log_test_resource_created, this);
}

test_resource::~test_resource ()
{
	log_test_resource				(log_test_resource_destroyed, this);
}

u32	  test_resource::children_count () const
{
	u32 out_result				=	0;
	for ( u32 i=0; i<array_size(m_children); ++i )
		if ( m_children[i] )
			++out_result;

	return							out_result;
}

bool   test_resource::has_child (test_resource_ptr child) const
{
	for ( u32 i=0; i<array_size(m_children); ++i )
		if ( m_children[i] == child )
			return					true;

	return							false;
}

bool   test_resource::add_child (test_resource_ptr child)
{
	R_ASSERT						(!has_child(child));

	for ( u32 i=0; i<array_size(m_children); ++i )
	{
		if ( !m_children[i] )
		{
			m_children[i].initialize_and_set_parent	(this, child.c_ptr());
			return					true;
		}
	}

	return							false;
}

} // namespace resources
} // namespace xray
