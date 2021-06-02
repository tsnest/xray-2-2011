////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_UNIVERSAL_FILE_NODE_H_INCLUDED
#define VFS_UNIVERSAL_FILE_NODE_H_INCLUDED

namespace xray {
namespace vfs {

template <platform_pointer_enum T = platform_pointer_default>
class universal_file_node 
{
	public:
	universal_file_node					() 
		: base(vfs_node_is_universal_file), physical_path(NULL), size(0), offs(0) {}

	static universal_file_node<T> *  create (memory::base_allocator *				allocator,
											 mount_root_node_base<T> *				mount_root,
											 fs_new::virtual_path_string const &	name, 
											 fs_new::native_path_string const &		file_path, 
									 	     u32 const								file_size,
											 file_size_type const					file_offs,
											 bool const								is_compressed,
											 u32 const								uncompressed_size);
public:
	pcstr								physical_path;
	u32									size;
	u32									uncompressed_size;
	file_size_type						offs;
	base_node<T> 						base;

}; // class vfs_universal_file_node

template <platform_pointer_enum T>
base_node<T> *			cast_node 		(universal_file_node<T> * node)			{ return & node->base; }
template <platform_pointer_enum T>
base_node<T> const *	cast_node 		(universal_file_node<T> const * node)	{ return & node->base; }

template <platform_pointer_enum T>
universal_file_node<T> *	 cast_universal_file_node (base_node<T> * node)
{
	R_ASSERT								(!node->is_link());

	if ( !node->has_flags(vfs_node_is_universal_file) )
		return								NULL;

	return									cast_base_node_to< universal_file_node<T> >(node);
}

template <platform_pointer_enum T>
universal_file_node<T> const *	 cast_universal_file_node (base_node<T> const * node)
{
	return									node_cast<universal_file_node>(const_cast<base_node<T> *>(node));
}

template <platform_pointer_enum T>
universal_file_node<T> *   universal_file_node<T>::create 
											(memory::base_allocator *				allocator,
											 mount_root_node_base<T> *				mount_root,
											 fs_new::virtual_path_string const &	name, 
											 fs_new::native_path_string const &		file_path, 
									 	     u32 const								file_size,
											 file_size_type const					file_offs,
											 bool const								is_compressed,
											 u32 const								uncompressed_size)
{
	
	u32 const node_size					=	sizeof(universal_file_node<T>) + 
												name.length() + 1 + file_path.length() + 1;
	pstr const node_data				=	(pstr)XRAY_MALLOC_IMPL(allocator, node_size, "universal_file_node");
	
	universal_file_node<T> * new_node	=	new (node_data)	universal_file_node<T>();
	new_node->offs						=	file_offs;
	new_node->size						=	file_size;
	new_node->uncompressed_size			=	uncompressed_size;

	base_node<T> *	base				=	node_cast<base_node>(new_node);
	typename base_node<T>::mount_root_pointer		mount_root_pointer;	
	mount_root_pointer					=	mount_root;
	base->set_mount_root					(mount_root_pointer);

	if ( is_compressed )
		base->add_flags						(vfs_node_is_compressed);

	pstr ptr							=	& base->m_name[0];
	strings::copy							(ptr, name.length() + 1, name.c_str());
	ptr									+=	name.length() + 1;
	strings::copy							(ptr, file_path.length() + 1, file_path.c_str());

	new_node->physical_path				=	ptr;


	return									new_node;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_UNIVERSAL_FILE_NODE_H_INCLUDED