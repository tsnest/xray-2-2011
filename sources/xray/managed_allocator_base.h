////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_ALLOCATOR_H_INCLUDED
#define RESOURCES_ALLOCATOR_H_INCLUDED

#include <xray/intrusive_ptr.h>

namespace xray {
namespace memory {

class managed_node;

class managed_allocator_base
{
public:
	void				dump_resource_leaks	();

protected:
						managed_allocator_base	(u32 granularity);

	void				initialize			(pvoid arena, size_t size);

	managed_node *		allocate			(size_t			size, 
											 managed_node*	start_free_node = NULL);

	void				resize_down			(managed_node*	node, size_t new_lesser_size);

	managed_node *		deallocate			(managed_node* 	ptr);

	size_t				total_size			() const { return m_arena_size; }
	size_t				get_free_size		() const { return m_free_size; }

	u32					get_granularity		() const { return m_granularity; }

	void				log_nodes			(bool log_only_allocated) const;
	void				check_consistency	() const;

protected:
	managed_node *		allocate_in_node	(size_t			full_size,
											 managed_node*	node,
											 size_t			offset_in_node,
											 managed_node*	prev_free_node);

	managed_node *		get_first_node		() const;
	bool				is_first_node		(managed_node*	node) const;

	bool				know_largest_free_block (size_t &	largest_free_block) const;

	void				notify_unpinned_object	();

	bool				check_valid_node	(managed_node*  node) const;


protected:
	managed_node *		m_first_free;
	managed_node *		m_pinned;					// used for defragmentation

	bool				m_know_largest_free_block;
	managed_node *		m_largest_free_block;		// valid if m_know_largest_free_block is true

	u32					m_granularity;
	size_t				m_free_size;
	pbyte				m_arena;
	size_t				m_arena_size;

	pbyte				m_temp_arena;
	size_t				m_temp_arena_size;

	threading::atomic32_type	m_num_unpinned_objects;

	friend class		managed_allocator;
	friend class		managed_node_owner;
};

} // namespace memory
} // namespace xray

#endif // #ifndef RESOURCES_MEMORY_ALLOCATOR_H_INCLUDED
