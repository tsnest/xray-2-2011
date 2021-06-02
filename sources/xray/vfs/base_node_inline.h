////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

namespace xray {
namespace vfs {

template <class Node, platform_pointer_enum T>
inline Node *   cast_base_node_to		(base_node<T> * node)
{
	Node * out_result					=	NULL;
	size_t const offs					=	(char*)(& out_result->base) - (char*)(out_result);

	out_result							=	(Node *)((char*)(node) - offs);
	return									out_result;
}

template <platform_pointer_enum T>
struct	base_node_list_node
{
	base_node<T> const *	node;
	base_node_list_node	*	next;
};

template <platform_pointer_enum T>
base_node<T>::base_node					(u16 const flags)
	:	m_next(NULL), m_parent(NULL), m_flags(flags), m_association(NULL), 
		m_next_overlapped(NULL), m_hashset_next(NULL), m_association_lock(0)
{
	m_mount_root						=	NULL;
}

template <platform_pointer_enum T>
threading::atomic32_type &   base_node<T>::ref_lock	()
{
	return									(threading::atomic32_type &)m_flags;
}

namespace detail {
inline
int		bits_to_flags_byte				() { return	platform::big_endian() ? 15 : 16; }
} // namespace detail

template <platform_pointer_enum T>
void   base_node<T>::lock_associated	()
{
	threading::atomic32_value_type unlocked_value	=	0;
	threading::atomic32_value_type locked_value		=	0;

	do {
		unlocked_value					=	ref_lock()		&  ~(1 << detail::bits_to_flags_byte());
		locked_value					=	unlocked_value	|	(1 << detail::bits_to_flags_byte());

	} while ( threading::interlocked_compare_exchange(ref_lock(), locked_value, unlocked_value) != unlocked_value );
}

template <platform_pointer_enum T>
void   base_node<T>::unlock_associated	()
{
	threading::atomic32_value_type unlocked_value	=	0;
	threading::atomic32_value_type locked_value		=	0;

	do {
		locked_value					=	ref_lock();
		R_ASSERT							(locked_value & (1 << detail::bits_to_flags_byte()));

		unlocked_value					=	locked_value & ~(1 << detail::bits_to_flags_byte());

	} while ( threading::interlocked_compare_exchange(ref_lock(), unlocked_value, locked_value) != locked_value );
}

template <platform_pointer_enum T>
void   base_node<T>::access_association	(association_callback const & callback)
{
	lock_associated							();

	vfs_association * association		=	m_association;
	callback								(association);
	m_association						=	association;
	
	unlock_associated						();
}

template <platform_pointer_enum> class	physical_file_mount_root_node;
template <platform_pointer_enum> class	physical_file_node;
template <platform_pointer_enum> class	physical_folder_mount_root_node;
template <platform_pointer_enum> class	physical_folder_node;
template <platform_pointer_enum> class	archive_folder_mount_root_node;
template <platform_pointer_enum> class	archive_file_node;
template <platform_pointer_enum> class	base_folder_node;

} // namespace vfs
} // namespace xray