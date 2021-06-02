////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SOFT_LINK_NODE_H_INCLUDED
#define VFS_SOFT_LINK_NODE_H_INCLUDED

#include <xray/fs/path_string_utils.h>

namespace xray {
namespace vfs {
	
template <class Ordinal>
inline void	  reverse_bytes				(Ordinal & res);

#pragma pack(push, 8)

template <platform_pointer_enum T = platform_pointer_default>
class soft_link_node 
{
public:
	enum { flags = vfs_node_is_soft_link };

	typedef		base_node<T>	node_type;
public:
	soft_link_node() : base(flags) {}

	void	absolute_path_to_referenced	(fs_new::virtual_path_string * out_path) const;
	void	reverse_bytes ()
	{
		vfs::reverse_bytes					(relative_path);
		base.reverse_bytes					();
	}

public:
	typename platform_pointer<char, T>::type		relative_path;
	node_type							base;

}; // class soft_link_node

#pragma pack(pop)

template <platform_pointer_enum T>
base_node<T> const *		cast_node	(soft_link_node<T> const * node) { return & node->base; }

template <platform_pointer_enum T>
base_node<T> *				cast_node	(soft_link_node<T> * node) { return & node->base; }

template <platform_pointer_enum T>
soft_link_node<T> *   cast_soft_link	(base_node<T> * node)
{
	R_ASSERT								(node->is_soft_link());

	return									cast_base_node_to< soft_link_node<T> >(node);
}

template <platform_pointer_enum T>
soft_link_node<T> const *   cast_soft_link (base_node<T> const * node)
{
	return									node_cast<soft_link_node>(const_cast<base_node<T> *>(node));
}


template <platform_pointer_enum T>
void   soft_link_node<T>::absolute_path_to_referenced	(fs_new::virtual_path_string * out_path) const
{
	node_cast<base_node>(this)->get_full_path	(out_path);
	bool append_result					=	append_relative_path(out_path, (pcstr)relative_path);
	R_ASSERT_U								(append_result);
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_SOFT_LINK_NODE_H_INCLUDED