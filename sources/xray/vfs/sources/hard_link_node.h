////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_HARD_LINK_NODE_H_INCLUDED
#define VFS_HARD_LINK_NODE_H_INCLUDED

namespace xray {
namespace vfs {

#pragma pack(push, 8)

template <platform_pointer_enum T = platform_pointer_default>
class hard_link_node 
{
public:
	enum { flags = vfs_node_is_hard_link };
	typedef		base_node<T>	node_type;

public:
	hard_link_node() : referenced(NULL), base(flags) {}

	inline u32 sizeof_with_name			() { return	sizeof(*this) + strings::length(base.get_name()) + 1; }
	void set_referenced_node			(node_type * node) { referenced = node; }
	node_type * get_referenced_node		() const { return referenced; }
	void reverse_bytes					()
	{
		vfs::reverse_bytes				(referenced);
		base.reverse_bytes				();
	}
public:
	typename node_type::node_pointer	referenced;
	node_type							base;

}; // class hard_link_node

#pragma pack(pop)

template <platform_pointer_enum T>
base_node<T> const *		cast_node	(hard_link_node<T> const * node) { return & node->base; }

template <platform_pointer_enum T>
base_node<T> *				cast_node	(hard_link_node<T> * node) { return & node->base; }

template <platform_pointer_enum T>
hard_link_node<T> *   cast_hard_link	(base_node<T> * node)
{
	R_ASSERT								(node->is_hard_link());

	return									cast_base_node_to< hard_link_node<T> >(node);
}

template <platform_pointer_enum T>
hard_link_node<T> const *   cast_hard_link (base_node<T> const * node)
{
	return									node_cast<hard_link_node>(const_cast<base_node<T> *>(node));
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_HARD_LINK_NODE_H_INCLUDED