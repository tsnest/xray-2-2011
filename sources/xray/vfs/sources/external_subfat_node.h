////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_EXTERNAL_SUBFAT_NODE_H_INCLUDED
#define VFS_EXTERNAL_SUBFAT_NODE_H_INCLUDED

namespace xray {
namespace vfs {

template <class Ordinal>
inline void	  reverse_bytes				(Ordinal & res);

#pragma pack(push, 8)

template <platform_pointer_enum T = platform_pointer_default>
class external_subfat_node
{
public:
	enum									{ flags =	vfs_node_is_external_sub_fat | 
														vfs_node_is_archive  | 
														vfs_node_is_sub_fat	};

	typedef		base_node<T>				node_type;
public:
	external_subfat_node() : base(flags), external_fat_size(0), relative_path_to_external(NULL) {}

	u32		sizeof_with_name				() { return sizeof(*this) + 
														strings::length(relative_path_to_external) + 1 +
														strings::length(base.get_name()) + 1; }
	void	reverse_bytes ()
	{
		vfs::reverse_bytes					(relative_path_to_external);
		vfs::reverse_bytes					(external_fat_size);
		base.reverse_bytes					();
	}

public:
	typename platform_pointer<char, T>::type	relative_path_to_external;
	u32											external_fat_size;
	node_type									base;

}; // class external_subfat_node

#pragma pack(pop)

template <platform_pointer_enum T>
base_node<T> const *		cast_node	(external_subfat_node<T> const * node) { return & node->base; }

template <platform_pointer_enum T>
base_node<T> *				cast_node	(external_subfat_node<T> * node) { return & node->base; }

template <platform_pointer_enum T>
external_subfat_node<T> *   cast_external_node		(base_node<T> * node)
{
	R_ASSERT								(node->is_external_sub_fat());
	return									cast_base_node_to< external_subfat_node<T> >(node);
}

template <platform_pointer_enum T>
external_subfat_node<T> const *   cast_external_node (base_node<T> const * node)
{
	return									node_cast<external_subfat_node>(const_cast<base_node<T> *>(node));
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_EXTERNAL_SUBFAT_NODE_H_INCLUDED