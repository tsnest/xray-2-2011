////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <functional>
#include "member_to_functor.h"
#include "resources_managed_allocator.h"
#include "resources_manager.h"

#define DO_RESOURCE_LOGGING			0

#if !XRAY_PLATFORM_PS3
#	define TEST_UNMOVABLES			1
#endif // #if !XRAY_PLATFORM_PS3

namespace xray		{
namespace resources {

managed_resource_allocator::managed_resource_allocator (u32 const temp_arena_size,
														u32 const reserved_size,
														u32 const granularity)
							:	resource_allocator(granularity), 
								m_temp_arena(granularity),
								m_arena_id(NULL)
{
	m_temp_arena_size					=	temp_arena_size;
	m_reserved_size						=	reserved_size;
	m_defragmenting						=	false;
	m_num_unpinned_objects				=	0;
	m_defrag_iteration					=	1;
	m_mode								=	NULL;
	m_defrag_unmovables					=	NULL;
	m_whole_moved_bytes					=	0;
	m_unmovables.reserve					(32);
}

void   managed_resource_allocator::initialize_impl (pvoid arena, u64 size, pcstr arena_id)
{
	pbyte const temp_arena_ptr			=	(pbyte)arena;
	size_t const temp_arena_size		=	m_temp_arena_size;

	pbyte const arena_ptr				=	(pbyte)arena + m_temp_arena_size;
	size_t const arena_size				=	(size_t)(size - m_temp_arena_size);

	m_temp_arena.resource_allocator::initialize	(temp_arena_ptr, temp_arena_size);
	resource_allocator::initialize			(arena_ptr, arena_size);
	m_arena_id							=	arena_id;
}

pvoid   managed_resource_allocator::call_malloc (size_t size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION)
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;
	XRAY_UNREFERENCED_PARAMETER				( XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER );
	XRAY_UNREFERENCED_PARAMETER				(size);
	NOT_IMPLEMENTED							(return NULL);
}

pvoid   managed_resource_allocator::call_realloc (pvoid pointer, size_t new_size XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_DECLARATION XRAY_CORE_DEBUG_PARAMETERS_DECLARATION)
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;
	XRAY_UNREFERENCED_PARAMETER				( XRAY_CORE_DEBUG_PARAMETERS_DESCRIPTION_PARAMETER );
	XRAY_UNREFERENCED_PARAMETERS			(new_size, pointer);
	NOT_IMPLEMENTED							(return NULL);
}

void   managed_resource_allocator::call_free (pvoid pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION)
{
	XRAY_CORE_DEBUG_PARAMETERS_UNREFERENCED_GUARD;
	XRAY_UNREFERENCED_PARAMETER				(pointer);
	NOT_IMPLEMENTED							();
}

bool   managed_resource_allocator::can_allocate	(u32 const buffer_size) const
{
	return									m_free_size - m_reserved_size >= buffer_size;
}

managed_resource *   managed_resource_allocator::allocate (u32 const size)
{
	u32 const full_size					=	math::align_up(size + (u32)sizeof(managed_node), m_granularity);

	ASSERT									(full_size <= m_free_size-m_reserved_size, 
											 "not enough memory even if we defragmented");

	ASSERT									(full_size <= m_temp_arena.total_size(),
											 "temp arena cant hold such block" );

	check_consistency						();
	managed_node * node					=	super::allocate(size);
	check_consistency						();
	if ( !node )
	{
		defragment							(full_size);
		check_consistency					();
		node							=	super::allocate(size);
		check_consistency					();
		if ( !node )
			return							NULL;
	}

	managed_resource * const out_resource	=	RES_NEW(managed_resource) (size);
	node->m_manager_info.data_size		=	size;
	
	out_resource->m_node				=	node;
	node->m_resource					=	out_resource;

#if DO_RESOURCE_LOGGING
	LOGI_INFO("resources:allocator",		"%s %s", "allocated", res->log_string().c_str());
#endif // #if DO_RESOURCE_LOGGING

	return									out_resource;
}

void   managed_resource_allocator::deallocate (managed_resource * res)
{
	managed_node * node					=	res->m_node;
	CURE_ASSERT								(node->is_allocated(),
											 return,
											 "resources:allocator",	"bah! already deallocated resource: %s", res->log_string().c_str());

#if DO_RESOURCE_LOGGING
	LOGI_INFO("resources:allocator",		"deallocated %s", res->log_string().c_str());
#endif // #if DO_RESOURCE_LOGGING

	check_consistency						();
	resource_allocator::deallocate			(node);

	RES_DELETE								(res);
	check_consistency						();
}

managed_node *   managed_resource_allocator::find_node_of_type (managed_node * const			start_node,
															    managed_node::type_t const	type)
{
	managed_node *	node					=	start_node;
	while ( node && node->m_type != type )
		node								=	node->m_next;
	
	return										node;
}

managed_node *   managed_resource_allocator::allocate_in_node_defrag  (size_t const	    full_size, 
																	   managed_node *	node,
																	   size_t const	    offset_in_node,
																	   managed_node *	prev_free_node)
{
	ASSERT										(m_defragmenting);
	managed_node * const	result			=	super::allocate_in_node(full_size,
																		node,
																		offset_in_node,
																		prev_free_node);

	if ( !result )
		return									NULL;

	mode_state* mode_states[]				=	{ & m_main_mode, & m_no_temp_mode };
	u32 const num_iterations				=	m_mode == & m_main_mode ? 1 : 2;

	for ( u32 i=0; i<num_iterations; ++i )
	{
		ASSERT									(mode_states[i]->node != node);

		managed_node * & place_node			=	mode_states[i]->place_node;
		pbyte const	place_pos				=	mode_states[i]->place_pos;

		if ( place_node == node && place_pos >= (pbyte)result + result->m_size )
		{
			place_node						=	result->m_next;
			ASSERT								(place_pos < (pbyte)place_node + place_node->m_size);
		}
	}

	return										result;
}

bool   managed_resource_allocator::check_valid_place_node () const
{
	managed_node* cur_node					=	get_first_node();

	while ( cur_node )
	{
		if ( cur_node == m_main_mode.place_node )
			return								true;

		cur_node							=	cur_node->m_next;
	}

	return										false;
}

managed_node*	managed_resource_allocator::deallocate_defrag (managed_node* node)
{
	ASSERT										(m_defragmenting);
	managed_node* const	result				=	super::deallocate(node);

	mode_state* mode_states[]				=	{ &m_main_mode, &m_no_temp_mode };
	u32 const num_iterations				=	m_mode == &m_main_mode ? 1 : 2;

	for ( u32 i=0; i<num_iterations; ++i )
	{
		managed_node*&	place_node			=	mode_states[i]->place_node;
		pbyte const	place_pos				=	mode_states[i]->place_pos;

		ASSERT									(mode_states[i]->node != node);

		if ( place_node > result && (pbyte)place_node < (pbyte)result + result->m_size )
			place_node						=	result;

		if ( place_pos >= (pbyte)place_node + place_node->m_size )
		{
			place_node						=	place_node->m_next;
			ASSERT								(place_pos < (pbyte)place_node + place_node->m_size);
		}
	}

	return										result;
}

void    managed_resource_allocator::log_defragmenter_nodes (resource_allocator const& alloc) const
{
	LOGI_INFO("resources:defrag",  "<%s> size: %d, allocated: %d, free: %d", 
											&alloc == this ? "Main arena" : "Temp arena",
											alloc.m_arena_size - (&alloc == this ? m_reserved_size : 0), 
											alloc.m_arena_size - alloc.m_free_size
															   - (&alloc == this ? m_reserved_size : 0),
											alloc.get_free_size());

	managed_node* cur_node				=	alloc.get_first_node();

	while ( cur_node )
	{
		pcstr		type_string			=	"";
		size_t const   data_size		=	cur_node->m_size;

		fs::path_string						full_path;

		type_string						=	 cur_node->is_allocated() ?
											 "allocated" : "free";
		if ( !cur_node->is_free() )
			cur_node->m_fat_it.get_full_path (full_path);

		fixed_string<1024>	special_type;
		for ( u32 i=0; i<m_defrag_unmovables->size(); ++i )
		{
			unmovable&	cur_unmovable	=	m_defrag_unmovables->at(i);
			if ( cur_unmovable.node == cur_node )
			{
				if ( cur_unmovable.state == unmovable::ignored )
				{
					special_type	+=	"unmovable I ";
				}
				else if ( cur_unmovable.state == unmovable::waited )
				{
					special_type	+=	"unmovable W ";
				}
				else 
				{
					special_type	+=	"unmovable ";
				}

				break;
			}
		}
		if ( cur_node == m_main_mode.node )
			special_type				+=	"main_cur_node ";
		if ( cur_node == m_main_mode.place_node )
			special_type				+=	"main_place_node ";

		if ( is_waiting_for_unpin(cur_node) )
			special_type				+=	"unpin_wait ";

		if ( m_mode == &m_no_temp_mode )
		{
			if ( cur_node == m_no_temp_mode.node )
				special_type			+=	"temp_cur_node ";
			if ( cur_node == m_no_temp_mode.place_node )
				special_type			+=	"temp_place_node ";
		}

		if ( special_type.length() )
			special_type.set_length(special_type.length()-1);
				
		u32 const offs					=	(u32)((pbyte)cur_node - (pbyte)alloc.m_arena);

		LOGI_INFO("resources:defrag", "offs: %d, size: %d %s [%s]",
												 offs,
												 data_size,
												 type_string,
												 special_type.c_str(),
												 full_path.c_str());
		cur_node						=	cur_node->m_next;
	}
}

void   managed_resource_allocator::log_defragmenter_state () const
{
	LOGI_INFO("resources:defrag" , "-----------defragmentation log %d-----------", m_log_iteration);
	LOGI_INFO("resources:defrag" , "mode: %s, main_place_pos: %d, temp_place_pos: %d", 
											m_mode == &m_main_mode ? "main" : "no_temp",
											(u32)(m_main_mode.place_pos - m_arena), 
											(u32)(m_no_temp_mode.place_pos - m_arena));
	LOGI_INFO("resources:defrag" , "main_unpinned: %d, temp_unpinned: %d", 
											 m_num_unpinned_objects, 
											 m_temp_arena.m_num_unpinned_objects);

	log_defragmenter_nodes					(*this);
	log_defragmenter_nodes					(m_temp_arena);

	m_temp_arena.check_consistency			();
	check_consistency						();
#if !XRAY_PLATFORM_PS3
	test_unmovables_list_valid				();
#endif // #if !XRAY_PLATFORM_PS3

	++m_log_iteration;
}

void   managed_resource_allocator::notify_unmovable_changed	(managed_node* const node)
{
#ifndef MASTER_GOLD
	if ( is_defragmenting() )
		R_ASSERT							(!node->is_unmovable());
#endif // #ifndef MASTER_GOLD
	
	threading::mutex_raii					raii(m_unmovables_mutex);

	if ( node->m_is_unmovable )
	{
		unmovable	new_unmovable		=	{ node, unmovable::undefined };
		unmovables_t::iterator it		=	std::lower_bound(m_unmovables.begin(), m_unmovables.end(), 
														 new_unmovable, unmovables_less());

		R_ASSERT							(it == m_unmovables.end() || (*it).node != node);
		m_unmovables.insert					(it, new_unmovable);
	}
	else
	{
		unmovables_t::iterator it		=	std::remove_if(	m_unmovables.begin(), 
															m_unmovables.end(), 
															pass_member_to_functor(
																&unmovable::node, 
														std::bind1st(std::equal_to<managed_node*>(), node))
															);
		R_ASSERT							(it != m_unmovables.end());
		m_unmovables.erase					(it);
	}
}

resource_allocator&   managed_resource_allocator::get_node_arena (managed_node* const node)
{
	return									node >= get_first_node() && 
											(pbyte)node < m_arena + m_arena_size ? 
											*this : m_temp_arena;
}

void   managed_resource_allocator::transfer_resource (managed_node* const	dest_node, 
													  managed_node* const	src_node)
{
	pvoid const dest_data				=	pbyte(dest_node) + sizeof(managed_node);
	pvoid const src_data				=	pbyte(src_node)	 + sizeof(managed_node);
	memory::copy							(dest_data, (u32)(dest_node->m_size - sizeof(managed_node)), src_data, (u32)(src_node->m_size - sizeof(managed_node)));
	m_num_moved_bytes					+=	src_node->m_size;

	dest_node->init							(src_node);
	dest_node->m_pin_count				=	0;
	dest_node->m_place_pos				=	src_node->m_place_pos;

	dest_node->m_defrag_iteration		=	m_defrag_iteration;
	src_node->m_defrag_iteration		=	m_defrag_iteration;

	managed_resource *  res				=	src_node->m_resource;
	ASSERT									(res && res->m_node == src_node);

	threading::interlocked_exchange_pointer	((pvoid&)res->m_node, dest_node);

	//^ log_test_resources					();

	add_unpin_looked_node					(src_node);
}

bool   managed_resource_allocator::is_waiting_for_unpin (managed_node* const node) const
{
	managed_node*  cur_node				=	m_pinned;
	while ( cur_node )
	{
		if ( cur_node == node )
			return							true;

		cur_node						=	cur_node->m_next_pinned;
	}

	return									false;
}

managed_node *   managed_resource_allocator::allocate_duplicate_in_main_arena (managed_node * const src_node,
																			   managed_node * const dest_node,
																			   managed_node * &	   depend_node)
{
	depend_node							=	NULL;
	managed_node *	new_node			=	NULL;

	ptrdiff_t const offset_in_node		=	(ptrdiff_t)(src_node->m_place_pos - pbyte(dest_node));
	ASSERT									(offset_in_node < (ptrdiff_t)dest_node->m_size);

	if ( dest_node->is_free() )
	{
		new_node						=	allocate_in_node_defrag(src_node->m_size,
																	dest_node,
																	offset_in_node,
																	NULL);
		if ( !new_node )
			depend_node					=	dest_node->m_next;
	}
	else
	{
		depend_node						=	dest_node;
	}

	return									new_node;
}

void   managed_resource_allocator::free_unpinned (arena_t arena_type)
{
	ASSERT										(arena_type == main_arena || arena_type == temp_arena);

	resource_allocator&	arena				=	arena_type == main_arena ? *this : m_temp_arena;
	u32	num_unpinned						=	threading::interlocked_exchange
												(arena.m_num_unpinned_objects, 0);

	if ( !num_unpinned )
		return;

	managed_node*	cur_pinned				=	arena.m_pinned;
	managed_node*	prev_pinned				=	NULL;

	while ( cur_pinned && num_unpinned )
	{
		managed_node* const	next_pinned		=	cur_pinned->m_next_pinned;
		if ( !cur_pinned->m_pin_count )
		{
			if ( prev_pinned )
				prev_pinned->m_next_pinned	=	next_pinned;
			else
				arena.m_pinned				=	next_pinned;

			managed_node*	wait_for_free	=	cur_pinned->m_next_wait_for_free;
			managed_node*	free_node		=	arena_type == main_arena ?
												deallocate_defrag(cur_pinned) : 
												m_temp_arena.deallocate(cur_pinned);

			if ( arena_type == main_arena )
			{
				while ( wait_for_free )
				{
					managed_node* const next_wait	
											=	wait_for_free->m_next_wait_for_free;

					managed_node* depend_node	
											=	NULL;

					managed_node* dest_node	=	free_node;
					while ( wait_for_free->m_place_pos >= (pbyte)dest_node + dest_node->m_size )
					{
						dest_node			=	dest_node->m_next;
						ASSERT					(dest_node);
					}

					ASSERT						(dest_node->is_free());
					managed_node* const new_node	
											=	allocate_duplicate_in_main_arena(wait_for_free,
																				 dest_node,
																				 depend_node);

					if ( new_node )
						transfer_resource		(new_node, wait_for_free);

					if ( depend_node )
						set_dependence			(depend_node, wait_for_free);
					
					wait_for_free			=	next_wait;
				}
			}

			log_defragmenter_state				();

			--num_unpinned;
		}
		else
		{
			prev_pinned						=	cur_pinned;
		}
	
		cur_pinned							=	next_pinned;
	}

	//log_defragmenter_state						();
}

void   managed_resource_allocator::add_unpin_looked_node (managed_node* const node)
{
	resource_allocator&	arena				=	get_node_arena(node);
	managed_node*&		pinned				=	arena.m_pinned;

	node->m_unpin_notify_allocator			=	&arena;
	node->m_next_pinned						=	pinned;
	pinned									=	node;

	if ( !node->m_pin_count )
		arena.notify_unpinned_object			();
}

managed_node*   managed_resource_allocator::to_next_ignored_unmovable (u32& index)
{
	buffer_vector<unmovable>&	unmovables	=	*m_defrag_unmovables;
	if ( index == unmovables.size() )	
		return									NULL;

	++index;
	while ( index < unmovables.size() )	
	{
		if ( unmovables[index].state == unmovable::ignored )
			return								unmovables[index].node;

		++index;
	}

	return										NULL;
}

u32   managed_resource_allocator::calculate_place_pos_and_free_space (buffer_vector<u32>&	looses,
																	  u32 const				start_index,
																	  pbyte					place_pos,
																	  u32&					result_index)
{
	R_ASSERT									(looses.max_size() >= m_defrag_unmovables->size());
	buffer_vector<unmovable>&	unmovables	=	*m_defrag_unmovables;

	looses.clear								();
	for ( u32 i=0; i<start_index; ++i )
		looses.push_back						(0);

	u32					place_index			=	start_index - 1;
	to_next_ignored_unmovable					(place_index);
	u32					cur_index			=	place_index;

	managed_node*		cur_node			=	unmovables[start_index].node;

	while ( cur_node )
	{
		managed_node*	place_unmovable		=	place_index < unmovables.size() ? 
												unmovables[place_index].node : NULL;
		unmovable*		cur_unmovable		=	cur_index < unmovables.size() ? &unmovables[cur_index] : NULL;

		bool const		cur_is_unmovable	=	cur_unmovable && 
												cur_unmovable->node == cur_node &&
												cur_unmovable->state == unmovable::ignored;

		if ( !cur_is_unmovable )
		{
			while ( place_unmovable && place_pos + cur_node->m_size > (pbyte)place_unmovable )
			{
				u32 loose					=	(u32)( (pbyte)place_unmovable - place_pos );
				looses.push_back				(loose);
				place_pos					=	(pbyte)place_unmovable + place_unmovable->m_size;
				
				place_unmovable				=	to_next_ignored_unmovable(place_index);
			}
		}
		else
		{
			to_next_ignored_unmovable			(cur_index);
		}
		
		cur_node->m_place_pos				=	place_pos;

		if ( !cur_is_unmovable )
			place_pos						+=	cur_node->m_size;

		cur_node							=	get_next_defragmentation_node(cur_node);
	}

	result_index							=	place_index;

	pbyte const	largest_free_end			=	result_index < unmovables.size() ? 
												(pbyte)unmovables[result_index].node : 
												m_arena + m_arena_size;
	u32 const largest_free_size				=	(u32)(largest_free_end - place_pos);

	for ( u32 i=result_index; i<unmovables.size(); ++i )
	{
		pbyte const	loose_end				=	i+1 < unmovables.size() ?
												(pbyte)unmovables[i+1].node : m_arena + m_arena_size;

		pbyte const cur_unmovable_end		=	(pbyte)unmovables[i].node + unmovables[i].node->m_size;

		u32 const loose_value				=	(u32)(loose_end - cur_unmovable_end);
		looses.push_back						(loose_value);
	}

	return										largest_free_size;
}

void   managed_resource_allocator::calculate_unmovables_state (u32		const	start_index,
															   pbyte	const	place_pos,
															   u32		const	sufficient_contigous_space)
{
	buffer_vector<unmovable>&	unmovables	=	*m_defrag_unmovables;
	ASSERT										(start_index < unmovables.size());

	// set all unmovables state to ignored
	for ( u32 i=0; i<unmovables.size(); ++i )
		unmovables[i].state					=	unmovable::ignored;

	buffer_vector<u32>	looses					(ALLOCA(sizeof(u32) * unmovables.size()), unmovables.size());
	u32 result_index						=	0;
	u32 const largest_free_size				=	calculate_place_pos_and_free_space	
												(looses, start_index, place_pos, result_index);
	ASSERT										(looses.size() == unmovables.size());

	if ( largest_free_size >= sufficient_contigous_space )
	{
		m_main_mode.unmovable_place_index		=	start_index;
		m_no_temp_mode.unmovable_place_index	=	start_index;
		return;
	}

	// choose some unmovables to wait for, based on looses heuristics

	u32 heuristic_free_size					=	largest_free_size;
	for ( u32 i=result_index; i>0; --i )
	{
		unmovables[i-1].state				=	heuristic_free_size < sufficient_contigous_space ?
												unmovable::waited : unmovable::ignored;
		if ( heuristic_free_size < sufficient_contigous_space )
			heuristic_free_size				+=	looses[i-1];
	}

	for ( u32 i=result_index; i<unmovables.size(); ++i )
	{
		unmovables[i].state					=	heuristic_free_size < sufficient_contigous_space ?
												unmovable::waited : unmovable::ignored;
		if ( heuristic_free_size < sufficient_contigous_space )
			heuristic_free_size				+=	looses[i];
	}

	R_ASSERT									(heuristic_free_size >= sufficient_contigous_space);
	u32 const real_largest_free_size		=	calculate_place_pos_and_free_space
												(looses, start_index, place_pos, result_index);

	if ( real_largest_free_size < sufficient_contigous_space )
	{
		// Omg! heuristics failed, well, go slow way then: mark all unmovables to waited state
		LOGI_WARNING("resources/defrag",		"Omg! heuristics failed, well, go slow way then: " 
												"mark all unmovables to waited stait");

		for ( u32 i=0; i<unmovables.size(); ++i )
			unmovables[i].state				=	unmovable::waited;

		m_main_mode.unmovable_place_index		=	(u32)unmovables.size();
		m_no_temp_mode.unmovable_place_index	=	(u32)unmovables.size();
	}
	else
	{
		u32 i;
		for ( i=start_index; i<unmovables.size(); ++i )
 		{
			if ( unmovables[i].state == unmovable::ignored )
				break;
		}

		m_main_mode.unmovable_place_index		=	i;
		m_no_temp_mode.unmovable_place_index	=	i;
	}
}

managed_node*   managed_resource_allocator::get_next_defragmentation_node (managed_node* cur_node)
{
	if ( !cur_node )
		return									cur_node;

	do
	{
		cur_node							=	find_node_of_type
												(cur_node->m_next, managed_node::allocated_node);
	}
	while ( cur_node && cur_node->m_defrag_iteration == m_defrag_iteration ) ;

	return										cur_node;
}

void   managed_resource_allocator::set_dependence (managed_node* const depend_node, 
												   managed_node* const wait_for_free)
{
	ASSERT										(depend_node && wait_for_free);
	wait_for_free->m_next_wait_for_free		=	depend_node->m_next_wait_for_free;
	depend_node->m_next_wait_for_free		=	wait_for_free;
}

size_t   managed_resource_allocator::get_contigous_space_defrag (u32 const	unmovable_index)
{
	pbyte prev_unmovable_end				=	NULL;

	if ( unmovable_index )
	{
		u32		work_index					=	unmovable_index;
		while ( work_index )
		{
			const unmovable& prev_unmovable	=	(*m_defrag_unmovables)[work_index-1];
			if ( prev_unmovable.state == unmovable::ignored )
			{
				prev_unmovable_end			=	(pbyte)prev_unmovable.node + prev_unmovable.node->m_size;
				break;
			}

			--work_index;
		}
	}

	size_t contigous_free_space				=	(u32)((pbyte)m_mode->node - 
												(pbyte)math::max(prev_unmovable_end, m_mode->place_pos));

	if ( m_mode->node->m_next && m_mode->node->m_next->is_free() )
		contigous_free_space				+=	m_mode->node->m_next->m_size;
	
	return										contigous_free_space;
}

void   managed_resource_allocator::init_mode (mode_state* const mode)
{
	managed_node*	node					=	get_first_node();
	mode->unmovable_place_index				=	0;
	mode->unmovable_index					=	0;

	while ( !node->is_free() )
	{
		if ( mode->unmovable_index < (*m_defrag_unmovables).size() &&
			 node == (*m_defrag_unmovables)[mode->unmovable_index].node )
		{
			++mode->unmovable_place_index;
			++mode->unmovable_index;
		}
		node->m_defrag_iteration			=	m_defrag_iteration;
		node								=	node->m_next;
	}

	ASSERT										(node);

	mode->place_node						=	node;
	mode->place_pos							=	(pbyte)node;	
	mode->node								=	node->m_next;
	ASSERT										(mode->node);
}

void   managed_resource_allocator::move_next_mode_node ()
{
	m_mode->node							=	get_next_defragmentation_node(m_mode->node);
	m_mode->place_pos						=	m_mode->node ? m_mode->node->m_place_pos : NULL;

	while ( m_mode->place_pos >= pbyte(m_mode->place_node) + m_mode->place_node->m_size )
	{	
		m_mode->place_node					=	m_mode->place_node->m_next;
		ASSERT									(m_mode->place_node);
	}
}

void   managed_resource_allocator::handle_unmovable (u32 const	sufficient_contigous_space, 
													 bool&		call_continue_in_main_cycle)
{
	XRAY_UNREFERENCED_PARAMETER					(sufficient_contigous_space);
	buffer_vector<unmovable>&	unmovables	=	*m_defrag_unmovables;

	ASSERT										(m_mode->unmovable_index < unmovables.size() &&
												 m_mode->node == unmovables[m_mode->unmovable_index].node );

	u32 const unmovable_state				=	unmovables[m_mode->unmovable_index].state;
	ASSERT										(unmovable_state != unmovable::undefined);
	++m_mode->unmovable_index;

	if ( unmovable_state == unmovable::waited )
	{
		if ( m_mode == &m_no_temp_mode && m_mode->node->m_is_unmovable )
		{
			move_next_mode_node					();
			call_continue_in_main_cycle		=	true;
			return;
		}
		R_ASSERT								(!threading::g_debug_single_thread, "how come you have unmovable in debug_single_thread?");

		while ( m_mode->node->m_is_unmovable )
		{
			// device manager threads will free this unmovable
			#if TEST_UNMOVABLES
				test_unmovable_unlock_expired	(m_defragment_timer.get_elapsed_ms());
			#endif // #if TEST_UNMOVABLES
		}
	}
	else // if ( unmovable_state == unmovable::ignored )
	{
		if ( m_mode != &m_no_temp_mode )
		{
			m_mode->node->m_defrag_iteration	=	m_defrag_iteration;
		}

		move_next_mode_node						();
		call_continue_in_main_cycle			=	true;
		return;
	}

	call_continue_in_main_cycle				=	false;
}

void   managed_resource_allocator::calculate_nodes_place_pos_and_unmovable_state (u32 const sufficient_contigous_space)
{
	managed_node* cur_node					=	m_mode->node;
	pbyte place_pos							=	m_mode->place_pos;

	while ( cur_node )
	{
		if ( m_mode->unmovable_index < (*m_defrag_unmovables).size() &&
			 cur_node == (*m_defrag_unmovables)[m_mode->unmovable_index].node )
		{
			calculate_unmovables_state			(m_mode->unmovable_index,
												 place_pos,
												 sufficient_contigous_space);
			return;
		}

		cur_node->m_place_pos				=	place_pos;
		place_pos							+=	cur_node->m_size;
		cur_node							=	get_next_defragmentation_node(cur_node);
	}
}

void   managed_resource_allocator::defragment (u32 const sufficient_contigous_space)
{
	threading::mutex_raii						raii(m_defragmentation_mutex);
	m_defragmenting							=	true;	g_resources_manager->mark_unmovables_before_defragmentation	();

	m_log_iteration							=	0;
	ASSERT										(m_free_size - m_reserved_size >= sufficient_contigous_space);

	u32		num_nodes						=	u32(-1);
	m_defragment_timer.start					();

#if TEST_UNMOVABLES
	test_unmovable_init							(m_defragment_timer.get_elapsed_ms());
#endif // #if TEST_UNMOVABLES

	LOGI_INFO("resources:allocator",			"defragmentation %d started, sufficient space: %d", 
												 m_defrag_iteration, 
												 sufficient_contigous_space);
	if ( num_nodes != u32(-1) )
		LOGI_INFO("resources:allocator",		"defragmentation nodes: %d", num_nodes);

	m_unmovables_mutex.lock						();
 	buffer_vector<unmovable>					unmovables (
													ALLOCA(sizeof(unmovable)*m_unmovables.size()),
 													m_unmovables.size(),
													m_unmovables.begin(),
													m_unmovables.end() );
	m_unmovables_mutex.unlock					();

	m_defrag_unmovables						=	&unmovables;

	m_pinned								=	NULL;
	m_temp_arena.m_pinned					=	NULL;
	m_num_moved_bytes						=	0;
	m_log_iteration							=	0;

	log_defragmenter_state						();

	init_mode									(&m_main_mode);
	m_no_temp_mode							=	m_main_mode;
	m_mode									=	&m_main_mode;
	calculate_nodes_place_pos_and_unmovable_state	(sufficient_contigous_space);

	bool		enough_contigous_space		=	false;

	while ( m_mode->node || m_pinned || m_temp_arena.m_pinned )
	{
		check_consistency						();
		ASSERT									(check_valid_place_node());
		log_defragmenter_state					();
		free_unpinned							(main_arena);
		free_unpinned							(temp_arena);

		if ( enough_contigous_space ) 
		{
			m_mode->node					=	NULL;
			// no need to move new blocks, just wait unpins and temp->main moves
			continue;							//	CONTINUE
		}

		//-----------------------------------------------------------
		// turn to main_mode if temp arena becomes large enough
		//-----------------------------------------------------------
		if ( m_mode == &m_no_temp_mode )
		{
			size_t largest_free_block;
			if ( m_temp_arena.know_largest_free_block(largest_free_block) &&
				 largest_free_block >= m_main_mode.node->m_size )
			{
				m_mode						=	&m_main_mode;
			}
		}

		if ( !m_mode->node )
			continue;							//	CONTINUE

		if ( m_mode->unmovable_index < unmovables.size() &&
			 m_mode->node == unmovables[m_mode->unmovable_index].node )
		{
			bool	call_continue_in_main_cycle	=	true;
			handle_unmovable(sufficient_contigous_space, call_continue_in_main_cycle);
			if ( call_continue_in_main_cycle )
				continue;
		}

		m_mode->place_pos					=	(pbyte)m_mode->node->m_place_pos;

		if ( (pbyte)m_mode->node == m_mode->place_pos ) // dest is src?
		{
			m_mode->node->m_defrag_iteration	=	m_defrag_iteration;
			move_next_mode_node					();
			continue;
		}

		managed_node * depend_node			=	NULL;
		managed_node * new_node				=	allocate_duplicate_in_main_arena
												(m_mode->node, m_mode->place_node, depend_node);

		ASSERT									( (new_node!=NULL) != (depend_node!=NULL) );

		if ( m_mode == &m_main_mode && depend_node )
		{
			ASSERT								(!new_node);
			size_t const node_data_size		=	m_mode->node->m_size - sizeof(managed_node);
			new_node						=	m_temp_arena.allocate(node_data_size);
		}

		if ( new_node )
		{
			transfer_resource					(new_node, m_mode->node);
			log_defragmenter_state				();
		}

		if ( m_mode == &m_main_mode )
		{
			if ( depend_node && new_node )	// new_node in temp arena
				set_dependence					(depend_node, new_node);

			if ( !new_node )
			{
				m_no_temp_mode				=	m_main_mode;
				m_mode						=	&m_no_temp_mode;
			}
		}

		if ( m_mode == &m_main_mode &&
			 get_contigous_space_defrag(m_mode->unmovable_index) >= sufficient_contigous_space )
		{
			enough_contigous_space			=	true;
		}

		move_next_mode_node						();
	}

	managed_node * cur_node					=	get_first_node();
	while ( cur_node )
	{
		ASSERT									( check_valid_node(cur_node) );
		if ( cur_node->is_allocated() )
			ASSERT								(cur_node->m_resource->m_node == cur_node);

		cur_node							=	cur_node->m_next;
	}

	LOGI_INFO("resources:allocator",			"defragmentation %d ended, moved: %d bytes, elapsed: %d ms", 
												 m_defrag_iteration,
												 m_num_moved_bytes,
												 m_defragment_timer.get_elapsed_ms());
	++m_defrag_iteration;
	m_defragmenting							=	false;
	m_whole_moved_bytes						+=	m_num_moved_bytes;

#if TEST_UNMOVABLES
	while ( m_unmovables.size() )
		test_unmovable_unlock_expired			(m_defragment_timer.get_elapsed_ms());

	managed_node*	node					=	get_first_node();
	while ( node )
	{
		R_ASSERT								(!node->m_is_unmovable);
		node								=	node->m_next;
	}
#endif // #if TEST_UNMOVABLES
}

} // namespace xray		
} // namespace resources 

