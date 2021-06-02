////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_NODES_H_INCLUDED
#define VFS_NODES_H_INCLUDED

#include "physical_folder_node.h"
#include "physical_file_node.h"
#include "mount_root_node_functions.h"
#include "archive_folder_mount_root_node.h"
#include "archive_compressed_file_node.h"
#include "archive_file_node.h"
#include "archive_inline_file_node.h"
#include "archive_inline_compressed_file_node.h"
#include "soft_link_node.h"
#include "hard_link_node.h"
#include "erased_node.h"
#include "external_subfat_node.h"
#include "universal_file_node.h"

namespace xray {
namespace vfs {

template <class Ordinal>
void	  reverse_bytes					(Ordinal & res);

base_node<> *   find_referenced_link_node	(base_node<> const * node);

template <platform_pointer_enum T>
u32   base_node<T>::sizeof_with_name	()
{
	if ( is_link() )
	{
		NOT_IMPLEMENTED						(return 0);
	}
	else if ( is_erased() )
	{
		erased_node<T> * node			=	node_cast<erased_node> (this);
		return								node->sizeof_with_name();
	}
	else if ( is_physical() )
	{
		if ( is_file() )
		{
			if ( is_mount_root() )
			{
				physical_file_mount_root_node<T> * node = node_cast<physical_file_mount_root_node> (this);
				return						node->sizeof_with_name();
			}
			else
			{
				physical_file_node<T> * node = node_cast<physical_file_node>(this);
				return						node->sizeof_with_name();
			}
		}
		else if ( is_folder() )
		{
			if ( is_mount_root() )
			{
				physical_folder_mount_root_node<T> * node = node_cast<physical_folder_mount_root_node> (this);
				return						node->sizeof_with_name();
			}
			else
			{
				physical_folder_node<T> * node = node_cast<physical_folder_node>(this);
				return						node->sizeof_with_name();
			}
		}
		else
			NOT_IMPLEMENTED					(return 0);
	}
	else if ( is_archive() )
	{
		if ( is_external_sub_fat() )
		{
			external_subfat_node<T> * node = node_cast<external_subfat_node>(this);
			return							node->sizeof_with_name();
		}
		else if ( is_file() )
		{
			if ( is_compressed() )
			{
				archive_compressed_file_node<T> * node = node_cast<archive_compressed_file_node>(this);
				return						node->sizeof_with_name();
			}
			else
			{
				archive_file_node<T> * node = node_cast<archive_file_node>(this);
				return						node->sizeof_with_name();
			}
		}
		else if ( is_folder() )
		{
			if ( is_mount_root() )
			{
				archive_folder_mount_root_node<T> * node = node_cast<archive_folder_mount_root_node> (this);
				return						node->sizeof_with_name();
			}
			else
			{
				base_folder_node<T> * node = node_cast<base_folder_node>(this);
				return						node->sizeof_with_name();
			}
		}
		else
			NOT_IMPLEMENTED					(return 0);
	}
	else if ( is_folder() )
	{
		base_folder_node<T> * node		=	node_cast<base_folder_node>(this);
		return								node->sizeof_with_name();
	}
	else
		NOT_IMPLEMENTED						(return 0);
}

template <platform_pointer_enum T>
void   base_node<T>::reverse_bytes_for_final_class (reverse_direction_enum const direction)
{
	if ( direction == reverse_direction_to_native )
		vfs::reverse_bytes					(m_flags);

	if ( is_external_sub_fat() )
	{
		external_subfat_node<T> * const node	=	node_cast<external_subfat_node> (this);
 		node->reverse_bytes					();
	}
	else if ( is_soft_link() )
	{
 		soft_link_node<T> * const link	=	node_cast<soft_link_node> (this);
 		link->reverse_bytes					();
	}
	else if ( is_hard_link() )
	{
 		hard_link_node<T> * const link	=	node_cast<hard_link_node> (this);
 		link->reverse_bytes					();
	}
	else if ( is_folder() )
	{
		if ( is_archive() )
		{
			base_folder_node<T> * folder	=	node_cast<base_folder_node>(this);
			ASSERT							(folder);
			folder->reverse_bytes			();
		}
		else
			UNREACHABLE_CODE();
	}
	else if ( is_inlined() )
	{
		if ( is_compressed() )
		{
			archive_inline_compressed_file_node<T> * compressed_inline_file =	node_cast<archive_inline_compressed_file_node>(this);
			ASSERT							(compressed_inline_file);
			compressed_inline_file->reverse_bytes	();
		}
		else
		{
			archive_inline_file_node<T> *	inline_file	=	node_cast<archive_inline_file_node>(this);
			ASSERT							(inline_file);
			inline_file->reverse_bytes		();
		}
	}
	else
	{
		if ( is_physical() )
		{
			UNREACHABLE_CODE();
		}
		else if ( is_compressed() )
		{
			archive_compressed_file_node<T> * compressed_file	=	node_cast<archive_compressed_file_node>(this);
			ASSERT							(compressed_file);
			compressed_file->reverse_bytes	();
		}
		else
		{
			archive_file_node<T> * const archive_file	=	node_cast<archive_file_node>(this);
			ASSERT							(archive_file);
			archive_file->reverse_bytes		();
		}
	}

	if ( direction == reverse_direction_to_native )
		vfs::reverse_bytes					(m_flags); // compensate one redundant reverse
}

template <class Ordinal>
inline void	  reverse_bytes				(Ordinal & res)
{
	char* const	it_begin				=	reinterpret_cast<char*>(& res);
	char* const	it_end					=	it_begin + sizeof(Ordinal);
	std::reverse							(it_begin, it_end);
}

template <platform_pointer_enum T>
void   base_node<T>::reverse_bytes		()
{
	vfs::reverse_bytes						(m_mount_root);

	vfs::reverse_bytes						(m_next_overlapped);
	vfs::reverse_bytes						(m_hashset_next);

	vfs::reverse_bytes						(m_next);
	vfs::reverse_bytes						(m_parent);

	vfs::reverse_bytes						(m_association);

	vfs::reverse_bytes						(m_flags);
}

template <platform_pointer_enum T>
base_node<T> *   base_node<T>::get_first_child ()
{
	return									is_folder() ? node_cast<base_folder_node>(this)->get_first_child() : NULL;
}

template <platform_pointer_enum T>
base_node<T> const *   base_node<T>::get_first_child () const
{
	return									const_cast<base_node*>(this)->get_first_child();
}

template <platform_pointer_enum T>
u32   get_file_size						(base_node<T> const * node) 
{
	R_ASSERT								(!node->is_link());
 	
	if ( node->is_folder() )
		NOT_IMPLEMENTED						(return 0);

	if ( node->is_universal_file() )
	{
		universal_file_node<> const * const universal_file	=	node_cast<universal_file_node>(node);
		return								universal_file->uncompressed_size;
	}

	if ( node->is_archive() )
	{
		if ( node->is_external_sub_fat() )
		{
			external_subfat_node<T> const * full	=	node_cast<external_subfat_node>(node);
			return							full->external_fat_size;
		}
		else if ( node->is_inlined() )
		{
			if ( node->is_compressed() )
			{
				archive_inline_compressed_file_node<T> const * 
					full	=	node_cast<archive_inline_compressed_file_node>(node);

				return						full->uncompressed_size;
			}
			else
			{
				archive_inline_file_node<T> const * 
					full	=	node_cast<archive_inline_file_node>(node);

				return						full->size_in_db;
			}
		}
		else
		{
			if ( node->is_compressed() )
			{
				archive_compressed_file_node<T> const *
					full	=	node_cast<archive_compressed_file_node>(node);
				
				return						full->uncompressed_size;
			}
			else
			{
				archive_file_node<T> const *
					full	=	node_cast<archive_file_node>(node);
				
				return						full->size_in_db;
			}
		}
	}

	physical_file_node<T> const * full	=	node_cast<physical_file_node>(node);
	R_ASSERT								(full);
	return									full->size();
}

template <platform_pointer_enum T>
u32   get_file_hash						(base_node<T> const * node) 
{
	R_ASSERT								(!node->is_link());

	CURE_ASSERT								(!node->is_folder(), return 0);
	CURE_ASSERT								(node->is_archive(), return 0);

	if ( node->is_inlined() )
	{
		return	node->is_compressed() ?	node_cast<archive_inline_compressed_file_node>(node)->hash : 
										node_cast<archive_inline_file_node>(node)->hash;
	}
	else
	{
		return node->is_compressed() ?	node_cast<archive_compressed_file_node>(node)->hash:
										node_cast<archive_file_node>(node)->hash;
	}
}

template <platform_pointer_enum T>
u32   get_raw_file_size					(base_node<T> const * node) 
{
	if ( node->is_link() )
	{
		base_node<T> * const referenced	=	find_referenced_link_node(node);
		R_ASSERT							(referenced);
		return								get_raw_file_size(referenced);					
	}

	if ( node->is_universal_file() )
	{
		universal_file_node<> const * const universal_file	=	node_cast<universal_file_node>(node);
		return								universal_file->size;
	}

	if ( !node->is_folder() )
	{
		if ( node->is_archive() )
		{
			if ( node->is_inlined() )
			{
				return						node->is_compressed() ?	
												node_cast<archive_inline_compressed_file_node>(node)->size_in_db : 
												node_cast<archive_inline_file_node>(node)->size_in_db;
			}
			else
			{
				return						node->is_compressed() ? 
												node_cast<archive_compressed_file_node>(node)->size_in_db :
												node_cast<archive_file_node>(node)->size_in_db;
			}
		}

		return								node_cast<physical_file_node>(node)->size();
	}

	NOT_IMPLEMENTED							(return 0);
}


template <platform_pointer_enum T>
typename base_node<T>::file_size_type   get_file_offs (base_node<T> const * node)
{
	R_ASSERT								(!node->is_link());

	CURE_ASSERT								(!node->is_folder(), return 0);

	if ( node->is_universal_file() )
	{
		universal_file_node<> const * const universal_node	=	node_cast<universal_file_node>(node);
		return								universal_node->offs;
	}

	if ( !node->is_archive() )
		return								0;

	if ( node->is_external_sub_fat() )
		return								0;

	CURE_ASSERT								(!node->is_inlined(), return 0, "you should not call get_file_offs over inline node");

	return									node->is_compressed()	?	node_cast<archive_compressed_file_node>(node)->pos_in_db : 
																		node_cast<archive_file_node>(node)->pos_in_db;
}

template <platform_pointer_enum T>
u32   get_compressed_file_size			(base_node<T> const * node)
{
	R_ASSERT								(!node->is_link());

	CURE_ASSERT								(node->is_compressed(), return 0);
	return									get_raw_file_size(node);
}

template <platform_pointer_enum T>
u32   calculate_count_of_nodes			(base_node<T> const * node)
{
	if ( node->is_link() )
	{
		base_node<> * const target		=	find_referenced_link_node(node);
		return								calculate_count_of_nodes(target);
	}

	if ( !node->is_folder() )
		return								1;	
	
	base_folder_node<T> const * folder	=	node_cast<base_folder_node>(node);
	u32	out_count						=	1;

	for ( base_node<T> const *	it_child	=	folder->get_first_child();
								it_child;
								it_child	=	it_child->get_next() )
	{
		out_count						+=	calculate_count_of_nodes(it_child);
	}
	
	return									out_count;
}

template <platform_pointer_enum T>
u32   calculate_count_of_children			(base_node<T> const * node, bool count_erased = false)
{
	R_ASSERT								(!node->is_link());
// 	if ( base_node<T> const * const referenced = find_referenced_link_node(node, hashset) ) 
// 		return								calculate_count_of_children(referenced, hashset);

	if ( !node->is_folder() )
		return								0;	
	
	base_folder_node<T> const * folder	=	node_cast<base_folder_node>(node);
	u32	out_count						=	0;

	for ( base_node<T> const *	it_child	=	folder->get_first_child();
								it_child;
								it_child	=	it_child->get_next() )
	{
		if ( !it_child->is_erased() || count_erased )
			++out_count;
	}

	return									out_count;
}

template <platform_pointer_enum T>
bool   get_inline_data					(base_node<T> const * node, const_buffer * out_buffer)
{		
	R_ASSERT								(!node->is_link());
// 	if ( base_node<T> const * const referenced = find_referenced_link_node(node, hashset) ) 
// 		return								get_inline_data(referenced, out_buffer, hashset);

	if ( !node->is_inlined() )
		return								false;

	if ( node->is_compressed() )
		* out_buffer					=	node_cast<archive_inline_compressed_file_node>(node)->get_inlined_data();
	else
		* out_buffer					=	node_cast<archive_inline_file_node>(node)->get_inlined_data();

	return									true;
}

template <platform_pointer_enum T>
bool   set_inline_data					(base_node<T> * node, const_buffer const & buffer)
{		
	R_ASSERT								(!node->is_link());
// 	if ( base_node<T> * const referenced = find_referenced_link_node(node, hashset) ) 
// 		return								set_inline_data(referenced, buffer, hashset);

	CURE_ASSERT								(node->is_inlined(), return	false);

	if ( node->is_compressed() )
	{
		archive_inline_compressed_file_node<T> * const inline_compressed_file	=	node_cast<archive_inline_compressed_file_node>(node);
		ASSERT								(inline_compressed_file);
		inline_compressed_file->set_inlined_data	(buffer);
	}
	else
	{
		archive_inline_file_node<T> * const inline_file	=	node_cast<archive_inline_file_node>(node);
		ASSERT								(inline_file);
		inline_file->set_inlined_data		(buffer);
	}

	return									true;
}

template <platform_pointer_enum T>
void   find_link_target_path			(base_node<T> const * node, fs_new::virtual_path_string * out_path)
{
	ASSERT									(node->is_link());
	if ( node->is_hard_link() )
	{
		base_node<T> * const referenced_node	=	node_cast<hard_link_node>(node)->referenced;
		referenced_node->get_full_path		(out_path);
		return;
	}

	node_cast<soft_link_node>(node)->absolute_path_to_referenced	(out_path);
}

template <platform_pointer_enum T>
vfs_mount *   mount_of_node				(base_node<T> * node)
{
	if ( node->is_mount_helper() )
		return								NULL;
	mount_root_node_base<T> * mount_root	=	node->get_mount_root();
	return									mount_root->mount;
}

template <platform_pointer_enum T>
u32   mount_id_of_node					(base_node<T> * node)
{
	if ( node->is_mount_helper() )
	{
		mount_helper_node<T> * helper	=	node_cast<mount_helper_node>(node);
		return								helper->mount_id;
	}

	mount_root_node_base<T> * mount_root	=	node->get_mount_root();
	return									mount_root->mount_id;
}

template <template <platform_pointer_enum T> class ToType>
struct node_cast_helper {};


#define NODE_CAST_HELPER_GENERATOR(node_class, castfunction)	\
	template <>																										\
	struct node_cast_helper< node_class >																			\
	{																												\
		template <template <platform_pointer_enum T> class FromType, platform_pointer_enum	T>						\
		static node_class<T> *			cast_from	(FromType<T> * node)		{ return	castfunction(node); }	\
		template <template <platform_pointer_enum T> class FromType, platform_pointer_enum	T>						\
		static node_class<T> const *	cast_from	(FromType<T> const * node)	{ return	castfunction(node); }	\
	};

NODE_CAST_HELPER_GENERATOR(base_node, cast_node);
NODE_CAST_HELPER_GENERATOR(base_folder_node, cast_folder);
NODE_CAST_HELPER_GENERATOR(mount_helper_node, cast_mount_helper_node);
NODE_CAST_HELPER_GENERATOR(mount_root_node_base, cast_mount_root_node_base);
NODE_CAST_HELPER_GENERATOR(physical_file_mount_root_node, cast_physical_file_mount_root);
NODE_CAST_HELPER_GENERATOR(physical_folder_mount_root_node, cast_physical_folder_mount_root);
NODE_CAST_HELPER_GENERATOR(physical_file_node, cast_physical_file);
NODE_CAST_HELPER_GENERATOR(physical_folder_node, cast_physical_folder);
NODE_CAST_HELPER_GENERATOR(archive_folder_mount_root_node, cast_archive_folder_mount_root);
NODE_CAST_HELPER_GENERATOR(archive_file_node, cast_archive_file);
NODE_CAST_HELPER_GENERATOR(archive_compressed_file_node, cast_archive_compressed_file);
NODE_CAST_HELPER_GENERATOR(archive_inline_file_node, cast_archive_inline_file);
NODE_CAST_HELPER_GENERATOR(archive_inline_compressed_file_node, cast_archive_inline_compressed_file);
NODE_CAST_HELPER_GENERATOR(soft_link_node, cast_soft_link);
NODE_CAST_HELPER_GENERATOR(hard_link_node, cast_hard_link);
NODE_CAST_HELPER_GENERATOR(erased_node, cast_erased_node);
NODE_CAST_HELPER_GENERATOR(external_subfat_node, cast_external_node);
NODE_CAST_HELPER_GENERATOR(universal_file_node, cast_universal_file_node);

template <	template <platform_pointer_enum T> class ToType, 
			template <platform_pointer_enum T> class FromType,
			platform_pointer_enum	T>
ToType<T> *   node_cast (FromType<T> * node)
{
	if ( !node )
		return								NULL;
	return									node_cast_helper<ToType>::cast_from	(node);
}

template <	template <platform_pointer_enum T> class ToType, 
			template <platform_pointer_enum T> class FromType,
			platform_pointer_enum	T>
ToType<T> const *   node_cast (FromType<T> const * node)
{
	if ( !node )
		return								NULL;
	return									node_cast_helper<ToType>::cast_from	(node);
}


} // namespace vfs
} // namespace xray


#endif // #ifndef VFS_NODES_H_INCLUDED