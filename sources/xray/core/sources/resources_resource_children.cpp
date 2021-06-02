////////////////////////////////////////////////////////////////////////////
//	Created		: 09.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_resource_children.h>
#include <xray/resources_resource_base.h>
#include "resources_allocators.h"

namespace xray {
namespace resources {

void   resource_children::link_parent_resource (resource_base * parent, u32 quality)
{
//	resource_link * const new_link		=	memory::g_resources_links_allocator.allocate();
	resource_link* const new_link		=	XRAY_NEW_IMPL( memory::g_resources_links_allocator, resource_link )();
	CURE_ASSERT								(new_link, return);
//	new (new_link)							resource_link;
	new_link->resource					=	parent;
	new_link->quality_value				=	quality;
	m_parent_resources.push_back			(new_link);
}

void   resource_children::link_child_resource (resource_base * child, u32 quality)
{
	if ( quality != resource_link::not_a_quality_link )
		child->set_flags			(resource_flag_is_quality_resource);

//	resource_link * const new_link		=	memory::g_resources_links_allocator.allocate();
	resource_link* const new_link		=	XRAY_NEW_IMPL( memory::g_resources_links_allocator, resource_link )();
	CURE_ASSERT								(new_link, return);
//	new (new_link)							resource_link;
	new_link->resource					=	child;
	new_link->quality_value				=	quality;
	m_children_resources.push_back			(new_link);
}

struct find_resource_link_predicate
{
	find_resource_link_predicate (resource_base * resource) 
		:	resource_(resource), found_link_(NULL) {}
	
	bool operator () (resource_link * link)
	{
		if ( found_link_ )
			return							false;

		if ( link->resource == resource_ )
		{
			found_link_					=	link;
			return							true;
		}

		return								false;
	}

	resource_base *		resource_;
	resource_link *		found_link_;
};

void   resource_children::unlink_parent_resource (resource_base * parent)
{
	find_resource_link_predicate			erase_predicate(parent);
	CURE_ASSERT								(m_parent_resources.remove_if(erase_predicate), return);
	ASSERT									(erase_predicate.found_link_);
	//memory::g_resources_links_allocator.deallocate	(erase_predicate.found_link_);
	XRAY_DELETE_IMPL						( memory::g_resources_links_allocator, erase_predicate.found_link_ );
}

bool   resource_children::has_child (resource_base * child)
{
	find_resource_link_predicate			find_predicate(child);
	return									!!m_children_resources.find_if(find_predicate);
}

void   resource_children::unlink_child_resource (resource_base * child)
{
	find_resource_link_predicate			erase_predicate(child);
 	CURE_ASSERT								(m_children_resources.remove_if(erase_predicate), return);
	resource_link * const link			=	erase_predicate.found_link_;
	ASSERT_U								(link);
	//memory::g_resources_links_allocator.deallocate	(erase_predicate.found_link_);
	XRAY_DELETE_IMPL						( memory::g_resources_links_allocator, erase_predicate.found_link_ );
}

void   resource_children::unlink_from_children	()
{
	while ( !m_children_resources.empty() )
	{
		resource_base * const child		=	m_children_resources.front()->resource;
		child->unlink_parent_resource		(this->cast_resource_base());
		unlink_child_resource				(child);
	}
}

void   resource_children::unlink_from_parents	()
{
	resource_base * const this_ptr		=	cast_resource_base();
	while ( m_parent_resources.size() )
	{
		resource_base * const parent	=	m_parent_resources.front()->resource;
		((resource_children *)parent)->unlink_child_resource	(this_ptr);
		unlink_parent_resource				(parent);		
	}
}

} // namespace resources
} // namespace xray
