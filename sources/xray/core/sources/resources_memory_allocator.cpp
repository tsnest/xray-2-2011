#include "pch.h"
#include "resources_memory_allocator.h"

namespace xray		{
namespace core		{
namespace resources {

//----------------------------------------------------
// managed_node
//----------------------------------------------------

void   managed_node::init (type_t const type, u32 const size)
{
	m_type								=	(char)type;
	m_size								=	size;
	m_pin_count							=	0;
	m_next								=	NULL;
	m_prev								=	NULL;
	m_next_free							=	NULL;
	m_prev_kept							=	NULL;
	m_res								=	NULL;
}

managed_node*   managed_node::find_prev_free () const
{
	managed_node* cur					=	m_prev;
	while ( cur )
	{
		if ( cur->m_type == is_free )
		{
			break;
		}

		cur								=	cur->m_prev;
	}

	return									cur;
}

//----------------------------------------------------
// managed_memory_allocator
//----------------------------------------------------

managed_memory_allocator::managed_memory_allocator(u32 const size)
{
	m_first_kept						=	NULL;
	m_last_kept							=	NULL;
	m_arena_size						=	1000;

	m_temp_arena_size					=	4*1024*1024;
	VERIFY									(size > m_temp_arena_size);
	m_arena_size						=	size - m_temp_arena_size;
	m_arena								=	RES_ALLOC(u8, m_arena_size);
	m_temp_arena						=	RES_ALLOC(u8, m_temp_arena_size);

	m_free_size							=	m_arena_size;
	m_first_free						=	(managed_node*)m_arena;
	m_first_free->init						(managed_node::is_free, m_arena_size);
}

managed_memory_allocator::~managed_memory_allocator ()
{
	RES_FREE								(m_arena);
	RES_FREE								(m_temp_arena);
}

bool   managed_memory_allocator::is_first_node (managed_node* const node) const 
{
	return									(pbyte)node == m_arena; 
}

managed_node*   managed_memory_allocator::get_first_node () const 					
{
	return									(managed_node*)m_arena; 
}

managed_node*   managed_memory_allocator::unlink_kept_node (fat_iterator fat_it)
{
	managed_node*	node				=	m_last_kept;
	managed_node*	prev_kept_node		=	NULL;
	while ( node )
	{
		if ( node->m_fat_it == fat_it )
		{
			if ( prev_kept_node )
			{
				prev_kept_node->m_prev_kept
										=	node->m_prev_kept;
			}
			else
			{
				m_last_kept				=	node->m_prev_kept;
			}

			if ( node == m_first_kept )
			{
				m_first_kept			=	prev_kept_node;
			}

			break;
		}

		prev_kept_node					=	node;
		node							=	node->m_prev_kept;
	}

	return									node;
}

resource*   managed_memory_allocator::allocate (u32 const			buffer_size,
												u32					data_size,
												fat_iterator* const	fat_it,
												u32	const			offs_to_data)
{
	managed_node*	node				=	fat_it ? unlink_kept_node(*fat_it) : NULL;
	if ( node )
	{
		u32 const size_with_header		=	buffer_size + sizeof(managed_node);
		VERIFY								( node->m_size == size_with_header );
		node->m_type					=	managed_node::is_allocated;
		m_free_size						-=	size_with_header;
	}

	bool const	is_new					=	node == NULL;

	if ( !node )
	{
		node							=	allocate_node(m_first_free, buffer_size);
		if ( !node )
		{
			return							NULL;
		}

		if ( fat_it )
		{
			node->m_fat_it				=	*fat_it;
		}
		else
		{
			node->m_fat_it.set_null			();
		}
	}

	resource*	res						=	RES_NEW(resource) (data_size, offs_to_data);
	res->m_node							=	node;
	node->m_res							=	res;

	LOGI_INFO("resources/allocator")		("%s %s", 
											 is_new ? "allocated" : "reclaimed", 
											 get_resource_log_string(res).c_str());

	return									res;
}

managed_node*   managed_memory_allocator::allocate_node (managed_node* const	start_free_node, 
														  u32 const				requested_size)
{
	u32 const size_with_header			=	 requested_size + sizeof(managed_node);
	VERIFY									(m_free_size >= size_with_header);
	if ( m_free_size < size_with_header )
	{
		return								NULL;
	}

	managed_node*	cur_free_node		=	start_free_node;
	managed_node*	prev_free_node		=	NULL;

	// first-fit strategy
	while ( cur_free_node )
	{
		u32  const cur_node_size		=	cur_free_node->m_size;

		if ( cur_node_size < size_with_header )
		{		
			prev_free_node				=	cur_free_node;
			cur_free_node				=	cur_free_node->m_next_free;
			continue;
		}

		managed_node*	free_node		=	NULL;

		if ( cur_node_size > size_with_header + s_max_ignored_remainder )
		{
			free_node					=	(managed_node*)((pbyte)cur_free_node + size_with_header);

			free_node->m_type			=	managed_node::is_free;
			free_node->m_pin_count		=	0;
			free_node->m_next			=	cur_free_node->m_next;
			if ( cur_free_node->m_next )
			{
				cur_free_node->m_next->m_prev = free_node;
			}
			free_node->m_prev			=	cur_free_node;
			free_node->m_next_free		=	cur_free_node->m_next_free;
			free_node->m_size			=	cur_free_node->m_size - size_with_header;

			cur_free_node->m_next		=	free_node;
		}
		else
		{
			free_node					=	cur_free_node->m_next_free;
		}

		cur_free_node->m_type			=	managed_node::is_allocated;
		cur_free_node->m_size			=	size_with_header;
		m_free_size						-=	size_with_header;

		if ( cur_free_node == m_first_free )
		{
			m_first_free				=	free_node;
		}
		else
		{
			if ( !prev_free_node )
			{
				prev_free_node			=	cur_free_node->find_prev_free();
				VERIFY						(prev_free_node);
			}

			prev_free_node->m_next_free	=	cur_free_node->m_next_free;
		}

		// RETURN
		return								(managed_node*)cur_free_node;
	}

	managed_node*	kept_cur			=	m_last_kept;
	while ( kept_cur )
	{
		managed_node* const kept_next	=	kept_cur->m_prev_kept;
		managed_node* free_block		=	deallocate_impl(kept_cur);

		if ( free_block->m_size >= size_with_header )
		{
			// RETURN
			m_last_kept					=	kept_next;
			return							allocate_node(free_block, requested_size);
		}

		kept_cur						=	kept_next;
	}

	m_last_kept							=	NULL;
	m_first_kept						=	NULL;

	defragment								();
	// RETURN
	return									allocate_node(m_first_free, requested_size);	
}

void   managed_memory_allocator::test_defragment ()
{
	VERIFY									(m_first_free && !m_first_free->m_next_free);
	managed_node*	node				=	get_first_node();
	VERIFY									(node->m_type == managed_node::is_allocated &&
											 node->m_next == m_first_free);

	u32 const			  node_size		=	node->m_size;
//	managed_node::type_t node_type		=	(managed_node::type_t)node->m_type;

	R_VERIFY								(m_temp_arena_size >= node_size);
	resource*		res					=	node->m_res;
	VERIFY									(res && res->m_node == node);

	memory::copy							(m_temp_arena, node, node_size);
	managed_node*	temp_node			=	(managed_node*)m_temp_arena;
	temp_node->m_pin_count				=	0;
	
	platform::interlocked_exchange_pointer	((pvoid&)res->m_node, temp_node);
	LOGI_INFO("managed_memory_allocator")	("exchanged resource ptr to: %d", 
											(pbyte)temp_node + sizeof(managed_node));
	
	timing::timer timer;
	float const   max_sec_to_unpin		=	100;
	timer.start								();

	while (	node->m_pin_count && timer.get_elapsed_sec() < max_sec_to_unpin )
	{ } // wait for unpins

	bool failed_to_unpin_in_time		=	timer.get_elapsed_sec() >= max_sec_to_unpin;

	managed_node*	moved_node			=	(managed_node*)m_arena;

	if ( !failed_to_unpin_in_time )
	{
		memory::copy						(m_arena, m_temp_arena, node_size);

		moved_node->m_pin_count			=	0;

		platform::interlocked_exchange_pointer	((pvoid&)res->m_node, moved_node);
		LOGI_INFO("resources/allocator")		("exchanged resource ptr to: %d", 
												(pbyte)moved_node + sizeof(managed_node));

		timer.start							();

		while ( temp_node->m_pin_count && timer.get_elapsed_sec() < max_sec_to_unpin )
		{ } // wait for unpins

		failed_to_unpin_in_time			=	timer.get_elapsed_sec() >= max_sec_to_unpin;
	}

	if ( failed_to_unpin_in_time )
	{
		fs::path_string						resource_path;
		res->m_node->m_fat_it.get_full_path	(resource_path);
		FATAL								("resource %s is not unpinned after %d sec", 
											resource_path.c_str(), max_sec_to_unpin);
	}
}

void   managed_memory_allocator::defragment ()
{
	managed_node*	node				=	get_first_node();
	managed_node*	last_allocated		=	NULL;

	LOGI_INFO("resources/allocator")		("defragmentation started");

	pbyte	top_data					=	m_arena;
	u32		num_moved_bytes				=	0;
	while ( node )
	{
		u32 const node_size				=	node->m_size;
		managed_node::type_t node_type	=	(managed_node::type_t)node->m_type;
		managed_node* next				=	node->m_next;

		if ( node_type	==	managed_node::is_allocated && 
			(pbyte)node		!=	top_data )
		{
			R_VERIFY						(m_temp_arena_size >= node_size);
			resource*  res				=	node->m_res;
			VERIFY							(res && res->m_node == node);

			memory::copy					(m_temp_arena, node, node_size);
			managed_node*	temp_node	=	(managed_node*)m_temp_arena;
			temp_node->m_pin_count		=	0;

			num_moved_bytes				+=	node_size;

			platform::interlocked_exchange_pointer
											((pvoid&)res->m_node, temp_node);
			
			timing::timer timer;
			float const   max_sec_to_unpin	
										=	1;
			timer.start						();

			while (	node->m_pin_count && timer.get_elapsed_sec() < max_sec_to_unpin )	
			{ } // wait for unpins

			bool failed_to_unpin_in_time	
										=	timer.get_elapsed_sec() >= max_sec_to_unpin;

			managed_node*	moved_node	=	(managed_node*)top_data;

			if ( !failed_to_unpin_in_time )
			{
				memory::copy					(top_data, m_temp_arena, node_size);

				moved_node->m_pin_count	=	0;

				platform::interlocked_exchange_pointer	((pvoid&)res->m_node, moved_node);

				timer.start					();

				while ( temp_node->m_pin_count && timer.get_elapsed_sec() < max_sec_to_unpin )
				{ } // wait for unpins

				failed_to_unpin_in_time	=	timer.get_elapsed_sec() >= max_sec_to_unpin;
			}

			if ( failed_to_unpin_in_time )
			{
				fs::path_string				resource_path;
				res->m_node->m_fat_it.get_full_path	(resource_path);
				FATAL						("resource %s is not unpinned after %d sec", 
											resource_path.c_str(), max_sec_to_unpin);
			}
			
			if ( last_allocated )
			{
				moved_node->m_prev		=	last_allocated;
				last_allocated->m_next	=	moved_node;
			}
		}
		
		if ( node_type == managed_node::is_allocated )
		{
			last_allocated				=	(managed_node*)top_data;
			top_data					+=	node_size;
		}

		node							=	next;
	}

	LOGI_INFO("resources/allocator")		("defragmentation ended, moved %d bytes", num_moved_bytes);

	managed_node* const one_free_node	=	(managed_node*)top_data;
	if ( m_free_size > sizeof(managed_node) )
	{
		one_free_node->init					(managed_node::is_free, m_free_size);
		one_free_node->m_next			=	NULL;
		one_free_node->m_prev			=	last_allocated;
		last_allocated->m_next			=	one_free_node;
		m_first_free					=	one_free_node;
	}
	else
	{
		m_first_free					=	NULL;
	}
}

void   managed_memory_allocator::deallocate (resource* res)
{
	managed_node* node					=	res->m_node;
	VERIFY									(node->m_type == managed_node::is_allocated);
	if ( node->m_type != managed_node::is_allocated )
	{
		return;
	}

	fs::path_string							full_path;
	res->get_full_path						(full_path);
	LOGI_INFO("resources/allocator")		("deallocated %s", get_resource_log_string(res).c_str());

	node->m_type						=	managed_node::is_kept;
	node->m_res							=	NULL;
	m_free_size							+=	node->m_size;
	if ( m_first_kept )
	{
		m_first_kept->m_prev_kept		=	node;
		m_first_kept					=	node;
	}
	else
	{
		m_first_kept					=	node;
		m_last_kept						=	node;
	}

	RES_DELETE								(res);
}

u32   managed_memory_allocator::get_real_node_size (managed_node* node) const
{
	if ( node->m_next )
	{
		return								(u32)((pbyte)node->m_next - (pbyte)node);
	}
	else
	{
		pbyte const top_border			=	m_arena + m_arena_size;
		return								(u32)(top_border - (pbyte)node);
	}
}

managed_node*   managed_memory_allocator::deallocate_impl (managed_node* node)
{
	VERIFY									( node->m_type == managed_node::is_kept );

	fs::path_string							full_path;
	node->m_res->get_full_path				(full_path);
	LOGI_INFO("resources/allocator")		("deleted %s", get_resource_log_string(node->m_res).c_str());

	u32 const		node_real_size		=	get_real_node_size(node);
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

	node->m_type						=	managed_node::is_free;

	if ( can_join_prev && can_join_next )
	{
		prev_free_node->m_size			+=	node_real_size + next_free_node->m_size;
		prev_free_node->m_next			=	next_free_node->m_next;
		if ( next_free_node->m_next )
		{
			next_free_node->m_next->m_prev	
										=	prev_free_node;
		}
		prev_free_node->m_next_free		=	next_free_node->m_next_free;
		return								prev_free_node;
	}
	else if ( can_join_prev )
	{
		prev_free_node->m_size			+=	node_real_size;
		prev_free_node->m_next			=	node->m_next;
		if ( node->m_next )
		{
			node->m_next->m_prev		=	prev_free_node;
		}
		return								prev_free_node;
	}
	else if ( can_join_next )
	{
		node->m_size					=	node_real_size + next_free_node->m_size;
		node->m_next					=	next_free_node->m_next;
		if ( next_free_node->m_next )
		{
			next_free_node->m_next->m_prev
										=	node;
		}
		node->m_next_free				=	next_free_node->m_next_free;

		(prev_free_node ? prev_free_node->m_next_free : m_first_free)
										=	node;
		return								node;
	}
	else
	{
		node->m_size					=	node_real_size;
		(prev_free_node ? prev_free_node->m_next_free : m_first_free)
										=	node;

		node->m_next_free				=	next_free_node;
		return								node;
	}
}

void   managed_memory_allocator::check_consistency ()
{
	managed_node*	cur_node			=	m_first_free;
	u32 free_nodes_size					=	0;
	while ( cur_node )
	{
		free_nodes_size					+=	cur_node->m_size;
		cur_node						=	cur_node->m_next_free;
	}

	cur_node							=	m_last_kept;
	while ( cur_node )
	{
		free_nodes_size					+=	cur_node->m_size;
		cur_node						=	cur_node->m_prev_kept;
	}

	R_VERIFY								(free_nodes_size == m_free_size);

	cur_node							=	get_first_node();
	u32 allocated_nodes_size			=	0;
	while ( cur_node )
	{
		VERIFY								(cur_node->m_size <= get_real_node_size(cur_node));

		if ( cur_node->m_type == managed_node::is_allocated )
		{
			allocated_nodes_size		+=	cur_node->m_size;
			R_VERIFY						( allocated_nodes_size + m_free_size <= m_arena_size );
		}

		cur_node						=	cur_node->m_next;
	}
	
	R_VERIFY								( allocated_nodes_size + m_free_size == m_arena_size );
}

void    managed_memory_allocator::log_nodes () const
{
	LOGI_INFO								("resources/allocator")
											("arena size: %d, free size: %d", 
											m_arena_size, m_free_size);

	managed_node* cur_node				=	get_first_node();

	while ( cur_node )
	{
		pcstr		type_string			=	"";
		string1024	data_size_string	=	"";
		u32 const   data_size			=	cur_node->m_size;
		u32 const   block_size			=	get_real_node_size(cur_node);

		fs::path_string						full_path;

		if ( cur_node->m_type == managed_node::is_allocated )
		{
			type_string					=	"allocated";
			sprintf_s						(data_size_string, "data_size: %d,", data_size);
		}
		else if ( cur_node->m_type == managed_node::is_free )
		{
			type_string					=	"free";
		}
		else
		{
			VERIFY							( cur_node->m_type == managed_node::is_kept );
			type_string					=	"kept";
		}
		
		if ( cur_node->m_type != managed_node::is_free )
		{
			cur_node->m_fat_it.get_full_path (full_path);
		}

		u32 const	block_offs			=	(u32)((pbyte)cur_node - m_arena);
		
		LOGI_INFO							("resources/allocator/detail") 
			("block offs: %d, size: %d, %s %s, type: %s", 
			block_offs, block_size, data_size_string, full_path.c_str(), type_string);
		
		cur_node						=	cur_node->m_next;
	}
}

//----------------------------------------------------------
// get_resource_log_string
//----------------------------------------------------------

fixed_string<512>   get_resource_log_string (resource* resource)
{
	fixed_string<512>						result;

	fs::path_string							full_path;
	resource->get_full_path					(full_path);

	result.appendf							("resource \"%s\", raw_size = %d, size = %d, buffer_size = %d", 
											 full_path.c_str(),
											 resource->get_raw_size(),
											 resource->get_size(),
											 resource->get_buffer_size());

	return									result;
}

} // namespace xray		
} // namespace core		
} // namespace resources 