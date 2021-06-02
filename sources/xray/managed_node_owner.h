////////////////////////////////////////////////////////////////////////////
//	Created		: 31.01.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MANAGED_NODE_OWNER_H_INCLUDED
#define MANAGED_NODE_OWNER_H_INCLUDED

namespace xray		{
namespace memory	{

typedef fixed_string<512>	log_string_type;

class						managed_node;
class						managed_allocator;

class XRAY_CORE_API managed_node_owner
{
public:
							managed_node_owner		(managed_allocator * allocator);
							virtual ~managed_node_owner	();

	void					set_managed_node		(managed_node * node) { R_ASSERT(!m_node); m_node = node; }
	managed_node *			get_managed_node		() const { return m_node; }

	// is called from resize_down
	virtual void			on_managed_node_resized	(u32 new_size) { XRAY_UNREFERENCED_PARAMETER(new_size); }

	managed_node *			grab_managed_node		();
	void					set_is_unmovable		(bool is_unmovable);
	bool					is_unmovable			() const;
	void					resize_down				(u32 new_size);
	pcbyte					pin						();
	void					unpin					(pcbyte pinned_data);

private:
	u32						get_buffer_size			() const;
	virtual log_string_type	log_string				() const = 0;

private:
	managed_node *			m_node;
	managed_allocator *		m_allocator;

	friend class			managed_allocator_base;
	friend class			managed_allocator;

	template <class T>
	friend class			pinned_ptr_base;

	friend class			resource_base;
	friend class			tester_pinned_resource; // tester class
};

} // namespace memory
} // namespace xray

#endif // #ifndef MANAGED_NODE_OWNER_H_INCLUDED