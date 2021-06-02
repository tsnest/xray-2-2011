////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MANAGED_NODE_H_INCLUDED
#define MANAGED_NODE_H_INCLUDED

namespace xray {
namespace memory {

class	managed_node_owner;
class	managed_allocator_base;

enum	managed_node_type		{ managed_node_allocated, managed_node_free };

class managed_node
{
public:
								managed_node		(managed_node_type type, size_t size);
	managed_node *				find_prev_free		() const;

	void						set_is_allocated	() { m_type = managed_node_allocated; }
	void						set_is_free			() { m_type = managed_node_free; }
	bool						is_allocated		() const { return m_type == managed_node_allocated; }
	bool						is_free				() const { return m_type == managed_node_free; }
	bool						is_unmovable		() const { return m_is_unmovable != 0; }

	void						init				(managed_node *);
	void						set_is_unmovable	(bool is_unmovable);

	void						set_owner			(managed_node_owner * owner) { R_ASSERT(!m_owner); m_owner = owner; }
	managed_node_owner *		owner				() const { return m_owner; }

private:
	managed_node_owner *		m_owner;
	managed_node *				m_next;
	managed_node *				m_prev;
	managed_node *				m_next_free;

	managed_allocator_base *	m_unpin_notify_allocator;

	//	used for defragmentation
	pbyte						m_place_pos;
	managed_node *				m_next_pinned;			
	managed_node *				m_next_wait_for_free;
	threading::atomic32_type	m_is_unmovable;
	u32							m_defrag_iteration;
	size_t						m_size;

	threading::atomic32_type	m_pin_count;
	char						m_type;

	friend class				managed_node_owner;
	friend class				managed_allocator_base;
	friend class				managed_allocator;
	friend class				resource_base;
};

} // namespace memory
} // namespace xray

#endif // #ifndef MANAGED_NODE_H_INCLUDED