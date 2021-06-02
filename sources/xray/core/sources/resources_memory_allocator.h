#ifndef MANAGED_MEMORY_ALLOCATOR
#define MANAGED_MEMORY_ALLOCATOR

#include "resources_helper.h"

#include <xray/core/intrusive_ptr.h>
#include <xray/core/resources_resource.h>

namespace xray		{
namespace core		{
namespace resources {

class managed_memory_allocator;

class managed_node
{
private:
	enum				type_t			{ is_allocated, is_free, is_kept };
	fat_iterator		m_fat_it;
	char				m_type;
	u16					m_pin_count;
	managed_node*		m_next;
	managed_node*		m_prev;
	u32					m_size;
	resource*			m_res;
	union
	{
		managed_node*	m_next_free;
		managed_node*	m_prev_kept;
	};

public:
	void				init			(type_t type, u32 size);
	managed_node*		find_prev_free	() const;

	friend class		resource;
	friend class		managed_memory_allocator;
};

class managed_memory_allocator
{
public:
						managed_memory_allocator	(u32 size);
						~managed_memory_allocator	();

	resource*			allocate 			(u32			buffer_size,
											 u32			data_size, // data_size <= buffer_size
											 fat_iterator*	fat_it,
											 u32			offs_to_data = 0);

	void				deallocate			(resource* ptr);
	u32					get_free_size		() const { return m_free_size; }

	void				log_nodes			() const;

	void				test_defragment		();

	void				check_consistency	();

private:
	managed_node*		unlink_kept_node	(fat_iterator fat_it);
	managed_node*		allocate_node		(managed_node* start_free_node, u32 size);
	managed_node*		deallocate_impl		(managed_node* ptr);
	managed_node*		find_prev_free_node (managed_node* free_node);
	void				defragment			();
	bool				is_first_node		(managed_node* node) const;
	managed_node*		get_first_node		() const;
	u32					get_real_node_size	(managed_node* node) const;

	static u32 const	s_max_ignored_remainder	=	16;

private:
	managed_node*		m_first_kept;
	managed_node*		m_last_kept;
	managed_node*		m_first_free;

	u32					m_free_size;
	pbyte				m_arena;
	u32					m_arena_size;

	pbyte				m_temp_arena;
	u32					m_temp_arena_size;
};

fixed_string<512>		get_resource_log_string	(resource* resource);

} // namespace xray		
} // namespace core		
} // namespace resources 

#endif // MANAGED_MEMORY_ALLOCATOR