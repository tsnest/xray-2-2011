////////////////////////////////////////////////////////////////////////////
//	Created		: 06.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_IMPL_FINDING_ASYNC_CALLBACKS_DATA_H_INCLUDED
#define FS_IMPL_FINDING_ASYNC_CALLBACKS_DATA_H_INCLUDED

#include "nodes.h"
#include "find_environment.h"

namespace xray {
namespace vfs {

class virtual_file_system;
class vfs_hashset;

struct node_to_expand
{
	enum type_enum		{ type_subfat, type_physical };
	base_node<> *		node;
	base_node<> *		node_parent;

	node_to_expand *	next;
	type_enum			type;
	u32					increment;

	node_to_expand						(type_enum type, base_node<> * node, base_node<> * node_parent, u32 increment)
											: type(type), node(node), node_parent(node_parent), increment(increment) {}
};

typedef intrusive_list< node_to_expand, node_to_expand *, & node_to_expand::next, threading::single_threading_policy, size_policy >		
										node_to_expand_list;

struct	async_callbacks_data
{
	enum type_enum						{ type_branch, type_tree };
	u32									callbacks_count;
	u32									callbacks_called_count;
	bool								all_queries_done;

	find_environment					env;
	result_enum							result;
	node_to_expand_list					nodes_to_expand;
	node_to_expand_list					previous_nodes_to_expand;
	type_enum							type;

#pragma warning(push)
#pragma warning(disable:4200)
	char								path_to_find[];
#pragma warning(pop)

	vfs_hashset &	hashset						() { return env.file_system->hashset; }

	async_callbacks_data						(type_enum type, find_env const & in_env) ;	
	void	  	on_lazy_mounted					(mount_result mount);
	void	  	on_automatic_archive_or_subfat_mounted	(mount_result mount, u32 increment);	
	void  		on_callback_may_destroy_this	(result_enum in_result);
	void  		try_finish_may_destroy_this		();
	void		finish_with_out_of_memory		();
	void		on_out_of_memory				() {}

private:
	void	  	continue_find_tree				();
	void	  	finish_branch_destroy_this		();
	void		finish_tree_may_destroy_this	();
	void		delete_this						();
};

} // namespace vfs
} // namespace xray

#endif // #ifndef FS_IMPL_FINDING_ASYNC_CALLBACKS_DATA_H_INCLUDED