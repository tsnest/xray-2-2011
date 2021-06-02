////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_BASE_FOLDER_NODE_INLINE_H_INCLUDED
#define VFS_BASE_FOLDER_NODE_INLINE_H_INCLUDED

namespace xray {
namespace vfs {

template <platform_pointer_enum T>
bool   base_folder_node<T>::try_lock	(lock_type_enum lock_type)
{
	return									m_readers_writers_counters.try_lock(lock_type);
}

template <platform_pointer_enum T>
bool   base_folder_node<T>::lock		(lock_type_enum lock_type, lock_operation_enum lock_operation)
{
	return									m_readers_writers_counters.lock(lock_type, lock_operation);
}

template <platform_pointer_enum T>
bool   base_folder_node<T>::has_lock	(lock_type_enum lock_type)
{
	return									m_readers_writers_counters.has_lock(lock_type);
}

template <platform_pointer_enum T>
void   base_folder_node<T>::unlock		(lock_type_enum lock_type)
{	
	m_readers_writers_counters.unlock		(lock_type);
}

template <platform_pointer_enum T>
void   base_folder_node<T>::exchange_write_to_child_write_lock ()
{
	m_readers_writers_counters.exchange_write_to_child_write_lock	();
}

template <platform_pointer_enum T>
bool   base_folder_node<T>::has_some_lock () const
{
	return									m_readers_writers_counters.has_some_lock();
}

template <platform_pointer_enum T>
void   base_folder_node<T>::upgrade_lock	(lock_type_enum from_lock, lock_type_enum to_lock)
{
	m_readers_writers_counters.upgrade		(from_lock, to_lock, lock_operation_lock);
}

template <platform_pointer_enum T>
base_folder_node<T> *   base_folder_node<T>::create (memory::base_allocator * allocator,
													 mount_root_node_base<T> * mount_root,
													 fs_new::virtual_path_string const & name)
{
	base_folder_node<> * const new_node	=	(base_folder_node<> *)
											XRAY_MALLOC_IMPL(allocator, sizeof(base_folder_node<T>) + 
															 name.length() + 1, "base_folder_node");

	new	(new_node)							base_folder_node<T>;

	base_node<T> * const base			=	node_cast<base_node>(new_node);

	base->set_flags							(vfs_node_is_folder);
	typename base_node<T>::mount_root_pointer		mount_root_pointer;	
	mount_root_pointer					=	mount_root;
	base->set_mount_root					(mount_root_pointer);
	strings::copy							(base->m_name, name.length()+1, name.c_str());

	return									new_node;
}

template <platform_pointer_enum T>
void   base_folder_node<T>::prepend_child	(base_node<T> * child)
{
	ASSERT									(!find_child(child, NULL));
	child->m_parent						=	this;
	child->m_next						=	m_first_child;
	m_first_child						=	child;
}

template <platform_pointer_enum T>
bool   base_folder_node<T>::unlink_child	(node_type * in_child, bool assert_if_not_child)
{
	base_node<> * prev_node				=	NULL;
	base_node<> * child					=	find_child(in_child->get_name(), & prev_node);
	
	if ( child != in_child )
	{
		if ( assert_if_not_child )
			R_ASSERT						(child == in_child);
		else
			return							false;
	}
	
	if ( prev_node )
		prev_node->set_next					(child->get_next());
	else
		m_first_child					=	child->get_next();

	return									true;
}

template <platform_pointer_enum T>
base_node<T> *   base_folder_node<T>::find_child (pcstr name, base_node<T> * * out_prev_node)
{
	for ( base_node<T> *	it_node		=	m_first_child,
					   *	it_prev		=	NULL;
							it_node		!=	NULL;
							it_prev		=	it_node,
							it_node		=	it_node->get_next() )
	{
		if ( strings::equal(it_node->get_name(), name) )
		{
			if ( out_prev_node )
				* out_prev_node			=	it_prev;
			return							it_node;
		}
	}

	return									NULL;
}

template <platform_pointer_enum T>
bool   base_folder_node<T>::find_child (base_node<T> * child, node_type * * out_prev_node)
{
	for ( base_node<T> *	it_node		=	m_first_child,
					   *	it_prev		=	NULL;
							it_node		!=	NULL;
							it_prev		=	it_node,
							it_node		=	it_node->get_next() )
	{
		if ( it_node == child )
		{
			if ( out_prev_node )
				* out_prev_node			=	it_prev;
			return							true;
		}
	}

	return									false;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_BASE_FOLDER_NODE_INLINE_H_INCLUDED