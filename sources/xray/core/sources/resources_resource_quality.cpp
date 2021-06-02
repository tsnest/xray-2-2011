////////////////////////////////////////////////////////////////////////////
//	Created		: 09.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/resources_resource_base.h>
#include <xray/resources_managed_resource.h>
#include <xray/resources_unmanaged_resource.h>
#include <xray/resources_resource_quality.h>

namespace xray {
namespace resources {

float	min_satisfaction_of_unreferenced_resource()
{
	return			1024.f;
}

float	max_satisfaction_of_unreferenced_resource()
{
	return			2048.f;
}

resource_quality::resource_quality (class_id_enum class_id, u32 quality_levels_count, resource_flags_enum flags) 
:	resource_children(flags), 
	m_current_satisfaction_update_tick(0),
	m_quality_levels_count(quality_levels_count),
	m_current_satisfaction(0),
	m_target_satisfaction(0),
	m_target_quality_level(u32(-1)),
	m_current_quality_level(u32(-1)),
	m_next_in_increase_quality_queue(NULL),
	m_last_fail_of_increasing_quality(0),
	m_class_id(class_id)
{
	R_ASSERT_CMP							(quality_levels_count, >=, 1);
}

float	resource_quality::satisfaction_with (u32 quality_level, math::float4x4 const & user_matrix, u32 users_count) const
{
	cook_base * cook						=	cook_base::find_cook(m_class_id);
	R_ASSERT									(cook);
	return	cook->satisfaction_with(quality_level, user_matrix, users_count);
}

u32   resource_quality::positional_users_count (resource_base const * resource_user) const
{
	if ( resource_user )
		if ( positional_unmanaged_resource const * positional = resource_user->cast_positional_unmanaged_resource() )
			return								1;

	resource_link_list const & parents		=	resource_user ? resource_user->get_parents() : get_parents();
	resource_link_list::mutex_raii	raii		(parents);
	resource_link * it_parent				=	resource_link_list_front_no_dying(parents);

	u32 out_positional_users_count			=	0;
	while ( it_parent )
	{
		out_positional_users_count			+=	positional_users_count(it_parent->resource);
		it_parent							=	resource_link_list_next_no_dying(it_parent);
	}

	return										out_positional_users_count;
}

float   resource_quality::update_satisfaction_for_resource_with_quality ()
{
	R_ASSERT									(m_quality_levels_count);
	resource_link_list const & children		=	get_children();
	children.lock								();
	resource_link * it_child				=	children.front();
	u32	max_quality_level					=	(u32)resource_link::not_a_quality_link;
	while ( it_child )
	{
		if ( max_quality_level == resource_link::not_a_quality_link ||
			 it_child->quality_value < max_quality_level )
		{
			max_quality_level				=	it_child->quality_value;
		}
		it_child							=	children.get_next_of_object(it_child);
	}
	children.unlock								();

	m_current_satisfaction					=	satisfaction(max_quality_level);
	return										m_current_satisfaction;
}

float   resource_quality::calculate_best_satisfaction_based_on_size () const
{
	if ( cast_resource_base()->has_user_references() )
		return									0;

	float const max_resource_size			=	16 * 1024*1024;
	float const out_satisfaction			=	min_satisfaction_of_unreferenced_resource() + 
												(max_satisfaction_of_unreferenced_resource() - min_satisfaction_of_unreferenced_resource()) 
														* 
												(1 - m_memory_usage_self.size / max_resource_size);
	return										out_satisfaction;
}

float   resource_quality::satisfaction (u32 quality_level, resource_base const * resource_user, u32 positional_users_count) const
{
	if ( query_result const * query = cast_query_result() )
		return									query->satisfaction_with(quality_level);

	if ( resource_user )
	if ( positional_unmanaged_resource const * positional = resource_user->cast_positional_unmanaged_resource() )
		return									satisfaction_with(quality_level, positional->current_matrix(), positional_users_count);

	if ( !resource_user )
		positional_users_count				=	this->positional_users_count(NULL);

	resource_link_list const & parents		=	resource_user ? resource_user->get_parents() : get_parents();
	resource_link_list::mutex_raii	raii		(parents);
	resource_link * it_parent				=	resource_link_list_front_no_dying(parents);

	float const best_satisfaction			=	max_satisfaction_of_unreferenced_resource();
	float min_satisfaction					=	best_satisfaction;
	while ( it_parent )
	{
		float const parent_worst_satisfaction	=	satisfaction(quality_level, it_parent->resource, positional_users_count);
		min_satisfaction					=	math::min(min_satisfaction, parent_worst_satisfaction);
		
		it_parent							=	resource_link_list_next_no_dying(it_parent);
	}

	if ( min_satisfaction >= min_satisfaction_of_unreferenced_resource() )
		min_satisfaction					=	calculate_best_satisfaction_based_on_size();

	return										min_satisfaction;
}

float   resource_quality::update_satisfaction (u64 update_tick)
{
	if ( m_current_satisfaction_update_tick == update_tick )
		return									m_current_satisfaction;

	m_current_satisfaction_update_tick		=	update_tick;

	if ( is_resource_with_quality() )
		return									update_satisfaction_for_resource_with_quality();

	resource_link_list const & parents		=	get_parents();
	resource_link_list::mutex_raii	raii		(parents);
	resource_link * it_parent				=	resource_link_list_front_no_dying(parents);

	float min_satisfaction					=	max_satisfaction_of_unreferenced_resource();

	while ( it_parent )
	{
		if ( it_parent->is_quality_link() )
		{
			min_satisfaction				=	math::min(it_parent->resource->satisfaction(it_parent->quality_value),
														  min_satisfaction);
		}
		else
			min_satisfaction				=	math::min(it_parent->resource->update_satisfaction(update_tick),
														  min_satisfaction);

		it_parent							=	resource_link_list_next_no_dying(it_parent);
	}

	if ( min_satisfaction >= min_satisfaction_of_unreferenced_resource() )
		min_satisfaction					=	calculate_best_satisfaction_based_on_size();

	m_current_satisfaction					=	min_satisfaction;
	return										m_current_satisfaction;
}

bool   resource_quality::is_on_quality_branch () const
{
	if ( is_quality_resource() )
		return						true;

	resource_link_list::mutex_raii	raii	(m_children_resources);
	for ( resource_link *	it_parent	=	m_parent_resources.front();
							it_parent;
							it_parent	=	m_parent_resources.get_next_of_object(it_parent) )
	{
		if ( it_parent->resource->is_on_quality_branch() )
			return					true;
	}

	return							false;
}

u32   resource_quality::child_quality (resource_base * parent) const
{
	resource_link_list::mutex_raii	raii	(m_children_resources);
	for ( resource_link *	it_child	=	m_children_resources.front();
							it_child;
							it_child	=	m_children_resources.get_next_of_object(it_child) )
	{
		if ( it_child->resource == parent )
			return							it_child->quality_value;
	}

	return									0;
}

void   resource_quality::set_target_satisfaction (float target_satisfaction)
{
	ASSERT_CMP								(m_quality_levels_count, ==, 1);
	m_target_satisfaction				=	target_satisfaction;
}

void   resource_quality::set_target_quality	(u32 quality_level, float target_satisfaction)
{
	ASSERT_CMP								(m_quality_levels_count, >, 1);
	m_target_quality_level				=	quality_level;
	m_target_satisfaction				=	target_satisfaction;
}

u32	  resource_quality::calculate_max_child_quality_level () const
{
	if ( quality_levels_count() == 1 )
		return						0;

	u32 out_current_level		=	u32(-1);	

	resource_link_list::mutex_raii	raii	(m_children_resources);
	for ( resource_link *	it_quality	=	m_children_resources.front();
							it_quality;
							it_quality	=	m_children_resources.get_next_of_object(it_quality) )
	{
		if ( !it_quality->is_quality_link() )
			continue;

		out_current_level		=	math::min(out_current_level, it_quality->quality_value);
	}

	return							out_current_level;
}

u32	  resource_quality::current_quality_level () const
{
	ASSERT_CMP						(m_quality_levels_count, >, 1);
	ASSERT_CMP						(m_current_quality_level, ==, calculate_max_child_quality_level());
	return							m_current_quality_level;
}

void   resource_quality::increase_quality (u32 target_quality, float target_satisfaction, query_result_for_cook * parent_query)
{
	set_target_quality				(target_quality, target_satisfaction);
	increase_quality_to_target		(parent_query);
}

void   resource_quality::link_child_resource (resource_base * child, u32 quality)
{
	resource_children::link_child_resource		(child, quality);
	if ( quality != resource_link::not_a_quality_link )
	{
		R_ASSERT					(is_increasing_quality());
		m_current_quality_level	=	math::min(m_current_quality_level, quality);
	}
}

void   resource_quality::unlink_child_resource (resource_base * child)
{
	resource_children::unlink_child_resource	(child);
	m_current_quality_level		=	calculate_max_child_quality_level();
}

} // namespace resources
} // namespace xray
