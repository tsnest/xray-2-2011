////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_ALLOCATOR_H_INCLUDED
#define RESOURCES_ALLOCATOR_H_INCLUDED

#include <xray/intrusive_ptr.h>
#include <xray/resources_resource.h>
#include "resources_helper.h"

namespace xray {
namespace resources {

class managed_node
{
public:
	enum				type_t				{ allocated_node, free_node };

						managed_node		(type_t type, size_t size);
	managed_node *		find_prev_free		() const;

	void				set_is_allocated	() { m_type = allocated_node; }
	void				set_is_free			() { m_type = free_node; }
	bool				is_allocated		() const { return m_type == allocated_node; }
	bool				is_free				() const { return m_type == free_node; }
	bool				is_unmovable		() const { return m_is_unmovable != 0; }

	void				init				(managed_node*);
	void				set_is_unmovable	(bool is_unmovable);

private:
	struct manager_info
	{
		u32				data_size;
		bool			need_cook;
	};
	manager_info		m_manager_info;

	fat_iterator		m_fat_it;
	managed_resource *	m_resource;
	managed_node *		m_next;
	managed_node *		m_prev;
	managed_node *		m_next_free;

	resource_allocator*	m_unpin_notify_allocator;

	//	used for defragmentation
	pbyte				m_place_pos;
	managed_node *		m_next_pinned;			
	managed_node *		m_next_wait_for_free;
	threading::atomic32_type	m_is_unmovable;
	u32					m_defrag_iteration;
	size_t				m_size;

	threading::atomic32_type	m_pin_count;
	char				m_type;

	friend class		managed_resource;
	friend class		resource_allocator;
	friend class		managed_resource_allocator;
	friend class		resource_base;
};

class resource_allocator
{
public:
						resource_allocator	(u32 granularity);

	void				initialize			(pvoid arena, size_t size);

						~resource_allocator	();

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

	managed_node *		unlink_kept_node	(fat_iterator	fat_it);
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

	friend class		managed_resource_allocator;
	friend class		managed_resource;
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_MEMORY_ALLOCATOR_H_INCLUDED