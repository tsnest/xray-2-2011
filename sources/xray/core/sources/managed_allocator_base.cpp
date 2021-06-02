#include "pch.h"
#include <xray/managed_allocator_base.h>
#include <xray/managed_node_owner.h>
#include "managed_node.h"

//#ifdef DEBUG
//#define	CHECK_ALLOCATOR_CONSISTENCY
//#endif

namespace xray		{
namespace memory	{

//----------------------------------------------------
// managed_allocator_base
//----------------------------------------------------

managed_allocator_base::managed_allocator_base (u32 const granularity)
{
	ASSERT									(granularity >= sizeof(managed_node));
	m_granularity						=	granularity;

	m_arena_size						=	0;
	m_arena								=	NULL;

	m_temp_arena						=	NULL;
	m_temp_arena_size					=	0;

	m_free_size							=	0;
	m_first_free						=	NULL;

	m_know_largest_free_block			=	false;
	m_largest_free_block				=	NULL;
	m_num_unpinned_objects				=	0;

	m_pinned							=	NULL;
}

void   managed_allocator_base::initialize (pvoid arena, size_t size)
{
	ASSERT									( (size % m_granularity) == 0 );
	m_arena								=	(pbyte)arena;
	m_arena_size						=	size;
	m_free_size							=	size;
	m_first_free						=	(managed_node*)m_arena;
	new ( m_first_free )					managed_node(managed_node_free, size);
}

void   managed_allocator_base::dump_resource_leaks ()
{
	if ( m_free_size == m_arena_size )
		return;

	LOGI_ERROR								("resources:allocator",	"resources LEAKED, managed arena dump:");
	log_nodes								(true);
}

bool   managed_allocator_base::is_first_node (managed_node* const node) const 
{
	return									(pbyte)node == m_arena; 
}

managed_node*   managed_allocator_base::get_first_node () const 					
{
	return									(managed_node*)m_arena; 
}

void   managed_allocator_base::notify_unpinned_object ()
{
	threading::interlocked_increment		(m_num_unpinned_objects);
}

managed_node*   managed_allocator_base::allocate_in_node (size_t const			full_size,
													  managed_node* const	node,
													  size_t const			offset_in_node,
													  managed_node*			prev_free_node)
{
	ASSERT									((offset_in_node % m_granularity) == 0);
	ASSERT									(node->is_free());

	if ( node->m_size - offset_in_node < full_size )
		return								NULL;

	if ( node != m_first_free && !prev_free_node )
	{
		prev_free_node					=	node->find_prev_free();
		ASSERT								(prev_free_node);
	}

	if ( m_know_largest_free_block && node == m_largest_free_block )
	{
		m_know_largest_free_block		=	false;
		m_largest_free_block			=	NULL;
	}

	size_t const		node_size		=	node->m_size;
	managed_node* const node_next_free	=	node->m_next_free;
	managed_node* const node_next		=	node->m_next;
	managed_node* const node_prev		=	node->m_prev;

	managed_node* const new_node		=	(managed_node*)(pbyte(node) + offset_in_node);
	managed_node*		new_prev_node	=	NULL;
	managed_node*		new_next_node	=	NULL;

	{
		new (new_node)						managed_node(managed_node_allocated, full_size);
	}

	if ( offset_in_node )
	{
		new_prev_node					=	node;
		new (new_prev_node)					managed_node(managed_node_free, offset_in_node);
	}

	if ( node_size - offset_in_node > full_size )
	{
		size_t const next_size			=	node_size - offset_in_node - full_size;
		ASSERT								( (next_size % m_granularity) == 0 );

		new_next_node					=	(managed_node*)
											((pbyte)node + offset_in_node + full_size);

		new (new_next_node)					managed_node(managed_node_free, next_size);
	}

	if ( new_prev_node )
	{
		new_prev_node->m_prev			=	node_prev;
		new_prev_node->m_next			=	new_node;
		if ( node_prev )
			node_prev->m_next			=	new_prev_node;

		new_prev_node->m_next_free		=	new_next_node ? new_next_node : node_next_free;
	}

	new_node->m_prev					=	new_prev_node ? new_prev_node : node_prev;
	new_node->m_next					=	new_next_node ? new_next_node : node_next;

	if ( new_next_node )
	{
		new_next_node->m_next			=	node_next;
 		if ( node_next )
 			node_next->m_prev			=	new_next_node;

		new_next_node->m_prev			=	new_node;
		new_next_node->m_next_free		=	node_next_free;
	}

	if ( node_next )
		node_next->m_prev				=	new_next_node ? new_next_node : new_node;

	m_free_size							-=	full_size;

	managed_node* const real_next_free_node	
										=	new_prev_node ? new_prev_node :
											( new_next_node ? new_next_node : node_next_free);

	if ( node == m_first_free )
		m_first_free					=	real_next_free_node;
	else
		prev_free_node->m_next_free		=	real_next_free_node;

	new_node->m_next_free				=	NULL;
	return									(managed_node*)new_node;
}

managed_node*   managed_allocator_base::allocate (size_t const			requested_size, 
												  managed_node* const	start_free_node)
{
	size_t const full_size				=	math::align_up(requested_size + (u32)sizeof(managed_node), 
																			(size_t)m_granularity);
	// warning: cant do early return based on m_free_size < size_with_header
	// cause we need to calculate m_largest_free_block

	managed_node*	cur_free_node		=	start_free_node ? start_free_node : m_first_free;
	managed_node*	prev_free_node		=	NULL;

	bool const		can_find_largest	=	start_free_node == NULL;
	managed_node*	largest_free_block	=	NULL;

	// first-fit strategy
	while ( cur_free_node )
	{
		managed_node* const	res			=	allocate_in_node(full_size,
															 cur_free_node, 
															 0,
															 prev_free_node);
		if ( !res )
		{
			if ( can_find_largest && 
				(!largest_free_block || largest_free_block->m_size < cur_free_node->m_size) )
			{
				largest_free_block		=	cur_free_node;
			}

			prev_free_node				=	cur_free_node;
			cur_free_node				=	cur_free_node->m_next_free;
			continue;
		}

		return								res; // RETURN
	}

	if ( can_find_largest )
	{
		m_know_largest_free_block		=	true;
		m_largest_free_block			=	largest_free_block;
	}

	return									NULL;	
}

void   managed_allocator_base::resize_down (managed_node* const node, size_t const new_lesser_size)
{
	R_ASSERT								(new_lesser_size <= node->m_size );
	R_ASSERT								(new_lesser_size >= sizeof(managed_node));

	size_t const granulared_lesser_size	=	math::align_up((size_t)new_lesser_size, (size_t)m_granularity);
	size_t const size_diff				=	node->m_size - granulared_lesser_size;

	if ( !size_diff )
		return;

	node->m_size						=	granulared_lesser_size;

	managed_node* const new_next_node	=	(managed_node*)((pbyte)node + node->m_size);
	managed_node* const old_next_node	=	node->m_next;

	bool const old_next_is_free			=	old_next_node && old_next_node->is_free();
	
	size_t const new_next_size			=	old_next_is_free ? 
											size_diff + old_next_node->m_size : 
											size_diff;

	new (new_next_node)						managed_node(managed_node_free, new_next_size);

	if ( old_next_is_free )
	{
		new_next_node->m_next			=	 old_next_node->m_next;

		if ( old_next_node->m_next )
			old_next_node->m_next->m_prev	=	new_next_node;
	}
	else
	{
		new_next_node->m_next			=	old_next_node;
		if ( old_next_node )
			old_next_node->m_prev		=	new_next_node;
	}

	new_next_node->m_prev				=	node;
	node->m_next						=	new_next_node;

	managed_node* const prev_free		=	node->find_prev_free();

	if ( old_next_is_free )
	{
		(prev_free ? prev_free->m_next_free : m_first_free)
										=	new_next_node;
		new_next_node->m_next_free		=	old_next_node->m_next_free;
	}
	else
	{
		if ( prev_free )
		{
			new_next_node->m_next_free	=	prev_free->m_next_free;
			prev_free->m_next_free		=	new_next_node;
		}
		else
		{
			new_next_node->m_next_free	=	m_first_free;
			m_first_free				=	new_next_node;
		}
	}

	if ( m_know_largest_free_block )
	{
		if ( new_next_node->m_size > m_largest_free_block->m_size )
		{
			m_largest_free_block		=	new_next_node;
		}
	}

	m_free_size							+=	size_diff;
}

bool   managed_allocator_base::know_largest_free_block (size_t & largest_free_block) const
{
	if ( m_know_largest_free_block )
		largest_free_block				=	m_largest_free_block ? m_largest_free_block->m_size : 0;

	return									m_know_largest_free_block;
}

managed_node *   managed_allocator_base::deallocate (managed_node * const node)
{
	ASSERT									(!node->is_free());

	m_free_size							+=	node->m_size;

//	LOGI_DEBUG								("resources:allocator", "deleted node with fat_it: %s, address: %d", node->m_fat_it.get_virtual_path().c_str(), size_t(node));

	managed_node*	cur_free_node		=	m_first_free; 
	managed_node*	prev_free_node		=	NULL;

	while ( cur_free_node )
	{
		if ( cur_free_node > node )
		{
			break;			
		}
		prev_free_node					=	cur_free_node;
		cur_free_node					=	cur_free_node->m_next_free;
	}

	managed_node*	next_free_node		=	cur_free_node;

	bool const		can_join_prev		=	prev_free_node && prev_free_node->m_next == node;
	bool const		can_join_next		=	next_free_node && next_free_node->m_prev == node;

	size_t largest_free_size			=	(m_know_largest_free_block && m_largest_free_block) ? 
											m_largest_free_block->m_size : 0;

	managed_node*	result_node			=	NULL;

	if ( can_join_prev && can_join_next )
	{
		prev_free_node->m_size			+=	node->m_size + next_free_node->m_size;
		prev_free_node->m_next			=	next_free_node->m_next;
		if ( next_free_node->m_next )
		{
			next_free_node->m_next->m_prev	
										=	prev_free_node;
		}
		prev_free_node->m_next_free		=	next_free_node->m_next_free;
		result_node						=	prev_free_node;
	}
	else if ( can_join_prev )
	{
		prev_free_node->m_size			+=	node->m_size;
		prev_free_node->m_next			=	node->m_next;
		if ( node->m_next )
		{
			node->m_next->m_prev		=	prev_free_node;
		}
		result_node						=	prev_free_node;
	}
	else // !can_join_prev
	{
		node->set_is_free					();
		if ( can_join_next )
		{
			node->m_size				=	node->m_size + next_free_node->m_size;
			node->m_next				=	next_free_node->m_next;
			if ( next_free_node->m_next )
			{
				next_free_node->m_next->m_prev
										=	node;
			}
			node->m_next_free			=	next_free_node->m_next_free;
		}
		else
		{
			node->m_next_free			=	next_free_node;
		}

		(prev_free_node ? prev_free_node->m_next_free : m_first_free)
										=	node;
		result_node						=	node;
	}

	ASSERT									(result_node);

	if ( m_know_largest_free_block && result_node->m_size > largest_free_size )
		m_largest_free_block			=	result_node;
	
	return									result_node;
}

bool   managed_allocator_base::check_valid_node (managed_node* const node) const
{
	return									(pbyte)node >= m_arena && (pbyte)node < m_arena + m_arena_size;
}

void   managed_allocator_base::check_consistency () const
{
#ifdef CHECK_ALLOCATOR_CONSISTENCY
	
	managed_node*	cur_node			=	m_first_free;
	size_t free_nodes_size				=	0;
	while ( cur_node )
	{
		if ( cur_node->is_free() && cur_node->m_next_free )
		{
			R_ASSERT						(cur_node->m_next_free > cur_node);
		}

		free_nodes_size					+=	cur_node->m_size;
		cur_node						=	cur_node->m_next_free;
	}

	R_ASSERT								(free_nodes_size == m_free_size);

	cur_node							=	get_first_node();
	size_t allocated_nodes_size			=	0;
	while ( cur_node )
	{

		if ( cur_node->m_next )
		{
			R_ASSERT						(cur_node->m_next > cur_node);
			R_ASSERT						(cur_node->m_next->m_prev ==  cur_node);
		}

		R_ASSERT							(check_valid_node(cur_node));
		if ( cur_node->is_allocated() )
		{
			allocated_nodes_size		+=	cur_node->m_size;
			R_ASSERT						(allocated_nodes_size + m_free_size <= m_arena_size);
		}

		cur_node						=	cur_node->m_next;
	}
	
	R_ASSERT								(allocated_nodes_size + m_free_size == m_arena_size);
	
#endif 
}

void    managed_allocator_base::log_nodes (bool const log_only_allocated) const
{
	LOGI_INFO								("resources:allocator", "arena size: %d, free size: %d", 
											m_arena_size, m_free_size);

	managed_node* cur_node				=	get_first_node();

	while ( cur_node )
	{
		if ( log_only_allocated && !cur_node->is_allocated() )
		{
			cur_node					=	cur_node->m_next;
			continue;			
		}

		fixed_string512						node_name;
		if ( cur_node->is_free() )
		{
			node_name.appendf				("free block %d bytes", cur_node->m_size);
		}
		else
		{
			R_ASSERT						( cur_node->is_allocated() );
			node_name					=	cur_node->owner()->log_string();
		}

		//u32 const	block_offs			=	(u32)((pbyte)cur_node - m_arena);
				
		LOGI_INFO							("resources:allocator", node_name.c_str());
		cur_node						=	cur_node->m_next;
	}
}

} // namespace memory
} // namespace xray
