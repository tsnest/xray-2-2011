////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_BASE_NODE_H_INCLUDED
#define VFS_BASE_NODE_H_INCLUDED

#include <xray/vfs/platform_configuration.h>
#include <xray/vfs/association.h>
#include <xray/fs/virtual_path_string.h>

namespace xray {
namespace vfs {

template <	template <platform_pointer_enum T> class ToType, 
			template <platform_pointer_enum T> class FromType,
			platform_pointer_enum	T>
ToType<T> *				node_cast		(FromType<T> * node);
template <	template <platform_pointer_enum T> class ToType, 
			template <platform_pointer_enum T> class FromType,
			platform_pointer_enum	T>
ToType<T> const *		node_cast		(FromType<T> const * node);

#pragma pack(push, 8)

template <platform_pointer_enum> class	base_folder_node;
template <platform_pointer_enum> class	physical_folder_node;
template <platform_pointer_enum> class	mount_root_node_base;
template <platform_pointer_enum> class	mount_helper_node;

enum reverse_direction_enum				{	reverse_direction_to_native, 
											reverse_direction_from_native	};

enum vfs_node_enum						{	vfs_node_is_folder							=	1 << 0,
											vfs_node_is_physical						=	1 << 1,
											vfs_node_is_archive							=	1 << 2,
											vfs_node_is_mount_root						=	1 << 3,
											vfs_node_is_compressed						=	1 << 4,
											vfs_node_is_replicated						=	1 << 5,
											vfs_node_is_inlined							=	1 << 6,
											vfs_node_is_sub_fat							=	1 << 7,
											vfs_node_is_soft_link						=	1 << 8,
											vfs_node_is_hard_link						=	1 << 9,
											vfs_node_is_mount_helper					=	1 << 10,	
											vfs_node_is_erased							=	1 << 11,
											vfs_node_is_external_sub_fat				=	1 << 12,
											vfs_node_is_universal_file					=	1 << 13,
											vfs_node_is_marked_to_unlink_from_parent	=	1 << 14,	};

typedef boost::function< void (vfs_association * &) >	association_callback;

template <platform_pointer_enum T>
class base_node
{
public:
	typedef	typename 	fs_new::platform_to_file_size_type<T>::file_size_type		file_size_type;
	typedef	typename 	platform_pointer<base_node, T>::type					node_pointer;
	typedef	typename 	platform_pointer<mount_root_node_base<T>, T>::pod_type	mount_root_pointer;
	typedef	typename 	platform_pointer<mount_helper_node<T>, T>::pod_type		mount_helper_node_pointer;
	typedef	typename 	platform_pointer<base_folder_node<T>, T>::type			folder_node_pointer;
	typedef	typename 	platform_pointer<vfs_association, T>::type				association_pointer;

public:
						base_node		(u16 const flags = 0);

	base_node *			get_next		() const { return m_next; }
	mount_root_node_base<T> *	get_mount_root	() const { return has_flags(vfs_node_is_mount_root) ? 
															node_cast<mount_root_node_base>(const_cast<base_node<T> *>(this)) : m_mount_root; }
	mount_helper_node<T> *		get_mount_helper_parent	() const { return m_mount_helper_parent; }
	void				set_next		(node_pointer node) { m_next = node; }
	void				set_mount_root	(mount_root_pointer node) { m_mount_root = node; }
	void				set_mount_helper_parent (mount_helper_node<T> * node) { m_mount_helper_parent = node; }
	void				set_name		(pcstr name) { R_ASSERT(!m_name[0]); strings::copy(m_name, strings::length(name) + 1, name); } // only support a call of this function when empty name is changed to non-empty
	pcstr				get_name		() const { return m_name; }
	void				set_parent		(folder_node_pointer node) { m_parent = node; }
	base_folder_node<T> *	get_parent	() const { return m_parent; }

	void				get_full_path	(fs_new::virtual_path_string * out_string) const;
	u16					get_flags		() const { return m_flags; }
	bool				has_flags		(u32 flags) const { return ((u32)m_flags & flags) == flags; }
	void				set_flags		(enum_flags<vfs_node_enum> flags) { threading::interlocked_exchange((threading::atomic32_type &)m_flags, (threading::atomic32_value_type)(u32)flags); }
	void				add_flags		(enum_flags<vfs_node_enum> flags) { threading::interlocked_or((threading::atomic32_type &)m_flags, (threading::atomic32_value_type)(u32)flags); }
	void				unset_flags		(enum_flags<vfs_node_enum> flags) { threading::interlocked_and((threading::atomic32_type &)m_flags, (threading::atomic32_value_type)(u32)~flags); }
	
	bool				is_folder		() const { return has_flags(vfs_node_is_folder); }
	bool				is_file			() const { return !has_flags(vfs_node_is_folder); }
	bool				is_physical		() const { return has_flags(vfs_node_is_physical); }
	bool				is_archive		() const { return has_flags(vfs_node_is_archive); }
	bool				is_archive_check_overlapped () const;
	bool				is_replicated	() const { return has_flags(vfs_node_is_replicated); }
	bool				is_compressed	() const { return has_flags(vfs_node_is_compressed); }
	bool				is_inlined		() const { R_ASSERT(!is_link()); return has_flags(vfs_node_is_inlined); }
	bool				is_sub_fat		() const { return has_flags(vfs_node_is_sub_fat); }
	bool				is_soft_link	() const { return has_flags(vfs_node_is_soft_link); }
	bool				is_hard_link	() const { return has_flags(vfs_node_is_hard_link); }
	bool				is_link			() const { return is_soft_link() || is_hard_link(); }
	bool				is_mount_root	() const { return has_flags(vfs_node_is_mount_root); }
	bool				is_mount_helper	() const { return has_flags(vfs_node_is_mount_helper); }
	bool				is_erased		() const { return has_flags(vfs_node_is_erased); }
	bool				is_external_sub_fat	() const { return has_flags(vfs_node_is_external_sub_fat); }
	bool				is_expanded		() const;
	bool				is_universal_file	() const { return has_flags(vfs_node_is_universal_file); }

	base_node *			get_next_overlapped	() const { return m_next_overlapped; }
	void				set_next_overlapped	(base_node * node) { m_next_overlapped = node; }
	base_node *			get_hashset_next	() const { return m_hashset_next; }
	void				set_hashset_next	(base_node * node) { m_hashset_next = node; }

	u32					sizeof_with_name();

	void				reverse_bytes_for_final_class	(reverse_direction_enum direction);
 	void				reverse_bytes	();
	base_node *			get_first_child	();
	base_node const *	get_first_child	() const;
		
	void				access_association		(association_callback const & callback);
	vfs_association *	get_association_unsafe	() const { return m_association; }
	void				set_association_unsafe	(vfs_association * const association) { m_association = association; }

	pvoid				get_mount_root_user_data	() const;
	void				set_mount_root_user_data	(pvoid data);


 	threading::atomic32_type &	ref_lock	();
	void				lock_associated		();
	void				unlock_associated	();

	union { 
	mount_root_pointer					m_mount_root;			// valid if node resides in some mount (and is not a mount root itself)
	mount_helper_node_pointer			m_mount_helper_parent;  // valid if node is a mount root or a helper node
	}; 

	node_pointer						m_next_overlapped;
	node_pointer						m_hashset_next;

	node_pointer						m_next;
	folder_node_pointer					m_parent;

	association_pointer					m_association;

	// this ordering is very important: m_association_lock must after 2-bytes flags
	u16									m_flags;
	u8									m_association_lock;

#pragma warning (push)
#pragma warning (disable:4200)
	char								m_name[];
#pragma warning (pop)

}; // class base_node

#pragma pack(pop)

template <platform_pointer_enum T>
base_node<T> *				cast_node	(base_node<T> * node) { return node; }
template <platform_pointer_enum T>
base_node<T> const *		cast_node	(base_node<T> const * node) { return node; }

template <platform_pointer_enum T>
base_folder_node<T> *		cast_folder	(base_node<T> * node);

template <platform_pointer_enum T>
base_folder_node<T> const *	cast_folder	(base_node<T> const * node) { return cast_folder(const_cast<base_node<T> *>(node)); }

template <platform_pointer_enum T>
physical_folder_node<T> *		cast_physical_folder	(base_node<T> * node);

template <platform_pointer_enum T>
physical_folder_node<T> const *	cast_physical_folder	(base_node<T> const * node) { return cast_physical_folder(const_cast<base_node*>(this)); }

template <class Node, platform_pointer_enum T>
inline Node *					cast_base_node_to		(base_node<T> * node);

template <class Ordinal>
inline void	  reverse_bytes				(Ordinal & res);

class	vfs_mount;
template <platform_pointer_enum T>
vfs_mount *   mount_of_node				(base_node<T> * node);

} // namespace vfs
} // namespace xray

#include "base_node_inline.h"

#endif // #ifndef VFS_BASE_NODE_H_INCLUDED