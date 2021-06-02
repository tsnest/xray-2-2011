////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "find.h"
#include <xray/vfs/find_results.h>

namespace xray {
namespace vfs {

using namespace fs_new;

struct async_link_helper
{
	find_callback						original_callback;
	base_node<> *						original_node;
	u32									original_mount_operation_id;
	memory::base_allocator *			allocator;
	bool								is_full_path;
	find_struct * 						original_find_results;
	find_enum							find_flags;
	vfs_hashset *						hashset;
#pragma warning(push)
#pragma warning(disable:4200)
	char								path_across_link[];
#pragma warning(pop)

	async_link_helper					(find_callback				original_callback,
										 base_node<> *				original_node,
										 u32						original_mount_operation_id,
										 find_enum					find_flags,
										 vfs_hashset *				hashset,
										 memory::base_allocator *	allocator) 
										 : 
										 original_callback(original_callback),
										 original_node(original_node),
										 original_mount_operation_id(original_mount_operation_id),
									  	 allocator(allocator), 
										 hashset(hashset),
										 find_flags(find_flags)	{}
	
	void   on_link_received				(vfs_locked_iterator const & iterator, result_enum result)
	{
		original_callback					(iterator, result);
		unlock_and_decref_branch			(original_node, lock_type_read, original_mount_operation_id);
		delete_this							();
	}

	void   delete_this					()
	{
		memory::base_allocator * const my_allocator	=	allocator;
		async_link_helper * this_ptr	=	this;
		XRAY_FREE_IMPL						(my_allocator, this_ptr);
	}

}; // struct async_link_helper

void   find_async_across_link			(find_env & env)
{
	virtual_path_string						path_across_link;
	find_link_target_path					(env.node, & path_across_link);
	path_across_link					+=	env.path_to_find + strings::length(env.partial_path);
	
	pvoid params_buffer					=	XRAY_MALLOC_IMPL(env.allocator, 
															 sizeof(async_link_helper) + path_across_link.length() + 1,
															 "async_link_helper");
	if ( !params_buffer )
	{
		unlock_and_decref_branch			(env.node, lock_type_read, env.mount_operation_id);
		env.callback						(vfs_locked_iterator::end(), result_out_of_memory);
		return;
	}

	async_link_helper * helper			=	new (params_buffer) async_link_helper(env.callback, env.node, env.mount_operation_id, 
																				  env.find_flags, env.hashset(), env.allocator);

	strings::copy							(helper->path_across_link, path_across_link.length() + 1, path_across_link.c_str());

	try_find_async							(helper->path_across_link, 
											 boost::bind(& async_link_helper::on_link_received,
														 helper, _1, _2), 
											 helper->find_flags, 
											 env.file_system,
											 env.allocator);
}

} // namespace vfs
} // namespace xray 