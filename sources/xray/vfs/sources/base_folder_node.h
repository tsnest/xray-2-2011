////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_FOLDER_NODE_H_INCLUDED
#define VFS_FOLDER_NODE_H_INCLUDED

#include <xray/vfs/base_node.h>
#include "node_lock.h"

namespace xray {
namespace vfs {

#pragma pack(push, 8)

template <platform_pointer_enum T = platform_pointer_default>
class base_folder_node
{
public:
	enum				{ flags = vfs_node_is_folder };
	
	typedef				base_node<T>					node_type;

public:
						base_folder_node() 
	:	base(flags), m_first_child(NULL) {}

 	node_type *			get_first_child () { return m_first_child; }
 	node_type const *	get_first_child () const { return m_first_child; }
 	node_type *			find_child		(pcstr name, node_type * * out_prev_node);
 	node_type const *	find_child		(pcstr name, node_type const * * out_prev_node) const { return const_cast<base_folder_node *>(this)->find_child(name, (node_type * *)out_prev_node); }
	bool				find_child		(base_node<T> * child, node_type * * out_prev_node);
 	bool				find_child		(base_node<T> * child, node_type const * * out_prev_node) const { return const_cast<base_folder_node *>(this)->find_child(child, (node_type * *)out_prev_node); }
 
 	void				prepend_child	(node_type * child);
 	bool				unlink_child	(node_type * child, bool assert_if_not_child = true);

	void				reverse_bytes	()
	{
		vfs::reverse_bytes					(m_first_child);
		vfs::reverse_bytes					(m_readers_writers_counters);
		base.reverse_bytes					();
	}

	bool				try_lock		(lock_type_enum lock_type);
	bool				lock			(lock_type_enum lock_type, lock_operation_enum lock_operation);
	void				unlock			(lock_type_enum lock_type);
	void				upgrade_lock	(lock_type_enum from_lock, lock_type_enum to_lock);
	void				exchange_write_to_child_write_lock	();
	bool				has_some_lock	() const;
	bool				has_lock		(lock_type_enum lock_type);

	inline u32 sizeof_with_name ()		{ return	sizeof(*this) + strings::length(base.get_name()) + 1; }


	static base_folder_node<T> *		create (memory::base_allocator *	allocator,
												mount_root_node_base<T> *	mount_root,
												fs_new::virtual_path_string const & name);

public:
	void				set_first_child	(typename node_type::node_pointer child) { m_first_child = child; }

private:
	typename node_type::node_pointer		m_first_child;
	vfs_reader_writer_lock					m_readers_writers_counters;
public:
	node_type								base;
}; // base_folder_node

#pragma pack(pop)

template <platform_pointer_enum T> inline
base_node<T> const *	cast_node		(base_folder_node<T> const * node) { return & node->base; }

template <platform_pointer_enum T> inline
base_node<T> *			cast_node		(base_folder_node<T> * node) { return & node->base; }

} // namespace vfs
} // namespace xray

#include "base_folder_node_inline.h"

#endif // #ifndef VFS_FOLDER_NODE_H_INCLUDED