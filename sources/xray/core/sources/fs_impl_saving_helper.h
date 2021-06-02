////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_FILE_SYSTEM_IMPL_SAVING_HELPER_H_INCLUDED
#define FS_FILE_SYSTEM_IMPL_SAVING_HELPER_H_INCLUDED

namespace xray {
namespace fs {

template <platform_pointer_enum target_pointer_size>
struct save_nodes_helper
{
	template <class T>
	static u32 get_node_offs ()
	{
		u32 const offset			=	u32((pbyte)& ((T *)NULL)->m_base - (pbyte)((T *)NULL));
		return							offset;

	}

	static u32 get_node_offs (fat_node<target_pointer_size> * const node, bool do_reverse_bytes)
	{
		u16 node_flags				=	(u16)node->get_flags();
		COMPILE_ASSERT					(sizeof(node->m_flags) == sizeof(node_flags), change_node_flags_to_have_correct_size);
		if ( do_reverse_bytes )
			reverse_bytes				(node_flags);

		if ( node_flags & file_system::is_soft_link )
		{
			return						get_node_offs<fat_db_soft_link_node<target_pointer_size> >();
		}
		else if ( node_flags & file_system::is_hard_link )
		{
			return						get_node_offs<fat_db_hard_link_node<target_pointer_size> >();
		}
		else if ( node_flags & file_system::is_folder )
		{
			return						get_node_offs<fat_folder_node<target_pointer_size> >();
		}
		else if ( node_flags & file_system::is_inlined )
		{
			return						(node_flags & file_system::is_compressed) ?	
										get_node_offs<fat_db_inline_compressed_file_node<target_pointer_size> >() :
										get_node_offs<fat_db_inline_file_node<target_pointer_size> >();
		}
		else
		{
			return						(node_flags & file_system::is_compressed) ?	
										get_node_offs<fat_db_compressed_file_node<target_pointer_size> >() :
										get_node_offs<fat_db_file_node<target_pointer_size> >();
		}
	}
}; // helper

} // namespace fs
} // namespace xray

#endif // #ifndef FS_FILE_SYSTEM_IMPL_SAVING_HELPER_H_INCLUDED