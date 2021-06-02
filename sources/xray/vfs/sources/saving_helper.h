////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SAVING_HELPER_H_INCLUDED
#define VFS_SAVING_HELPER_H_INCLUDED

namespace xray {
namespace vfs {

template <platform_pointer_enum T>
struct save_nodes_helper
{
	template <class T2>
	static u32 get_node_offs ()
	{
		u32 const offset				=	u32((pbyte)& ((T2 *)NULL)->base - (pbyte)((T2 *)NULL));
		return								offset;
	}

	static u32 get_node_offs			(base_node<T> * const node, bool do_reverse_bytes)
	{
		u16 node_flags					=	(u16)node->get_flags();
		COMPILE_ASSERT						(sizeof(node->m_flags) == sizeof(node_flags), change_node_flags_to_have_correct_size);
		if ( do_reverse_bytes )
			reverse_bytes					(node_flags);

		if ( node_flags & vfs_node_is_external_sub_fat )
		{
			return							(u32) & ((external_subfat_node<T> *)NULL)->base;
		}
		else if ( node_flags & vfs_node_is_mount_root )
		{
			return							(u32) & ((archive_folder_mount_root_node<T> *)NULL)->folder.base;
		}
		else if ( node_flags & vfs_node_is_folder )
		{
			return							get_node_offs<base_folder_node<T> >();
		}
		else if ( node_flags & vfs_node_is_soft_link )
		{
			return							get_node_offs<soft_link_node<T> >();
		}
		else if ( node_flags & vfs_node_is_hard_link )
		{
			return							get_node_offs<hard_link_node<T> >();
		}
		else if ( node_flags & vfs_node_is_inlined )
		{
			return							(node_flags & vfs_node_is_compressed) ?	
											get_node_offs<archive_inline_compressed_file_node<T> >() :
											get_node_offs<archive_inline_file_node<T> >();
		}
		else if ( node_flags & vfs_node_is_erased )
		{
			return							get_node_offs<erased_node<T> >();
		}
		else 
		{
			if ( node_flags & vfs_node_is_compressed )
				return						get_node_offs<archive_compressed_file_node<T> >();
			else
			{
				R_ASSERT					(node_flags == vfs_node_is_archive 
														||
											(node_flags == (vfs_node_is_sub_fat | vfs_node_is_archive)));

				return						get_node_offs<archive_file_node<T> >();
			}
		}
	}
}; // helper

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_SAVING_HELPER_H_INCLUDED