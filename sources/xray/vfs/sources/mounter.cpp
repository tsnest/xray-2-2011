////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mounter.h"
#include "branch_locks.h"
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

threading::atomic32_type				s_mount_id				=	0;
threading::atomic32_type				s_mount_operation_id	=	0;
u32		next_mount_id					() { return threading::interlocked_exchange_add(s_mount_id, 1) + 1; }
u32		next_mount_operation_id			() { return threading::interlocked_exchange_add(s_mount_operation_id, 1) + 1; }

mounter::mounter						(query_mount_arguments & args, virtual_file_system & file_system)
	: m_args(args), m_file_system(file_system), m_mount_root_base(NULL), m_result(result_success), m_mount_id(0)
{
	m_file_system.pending_mounts.push_back	(this);	
}

bool   mounter::try_mount_from_history ()
{
	if ( m_args.submount_node )
		return								false;

	m_mount_ptr							=	find_in_mount_history  (m_args.virtual_path.c_str(), 
																	m_args.get_physical_path().c_str(),
																	m_file_system.mount_history);

	if ( m_mount_ptr )
	{
		LOG_INFO							("mounted from history: '%s' on '%s'", 
											 m_args.get_physical_path().c_str(),
											 m_args.virtual_path.c_str());

		finish								(mount_result(m_mount_ptr, m_result), true);
	}

	return									m_mount_ptr != NULL;
}

mounter::~mounter						()
{
	m_file_system.pending_mounts.erase			(this);

	mount_referer * referer				=	m_referers.pop_all_and_clear();
	while ( referer )
	{
		mount_referer * next_referer	=	m_referers.get_next_of_object(referer);
		referer->result					=	m_mount_ptr;
		referer->ready_list->push_back		(referer);
		referer							=	next_referer;
	}
}

void   mounter::finish					(mount_result const & result, bool reused_from_history)
{
	if ( m_args.unlock_after_mount )
		unlock_branch						(m_args.root_write_lock, lock_type_write);

	if ( !reused_from_history && m_args.submount_node && result.result == result_success )
	{
		// link child / parent relations of submounts
		vfs_mount_ptr parent_mount		=	mount_of_node(m_args.submount_node);
		if ( parent_mount != m_mount_ptr )
		{
			m_mount_ptr->parent				=	parent_mount.c_ptr();
			parent_mount->children.push_back	(m_mount_ptr);
		}
	}

	if ( m_args.callback )
		m_args.callback						(result);

	bool const is_lazy_mount			=	(m_args.type == mount_type_physical_path) && 
											m_args.submount_node && m_args.submount_node->is_folder();

	// we dont notify user about lazy mounts...
	if ( !is_lazy_mount && result.mount && !reused_from_history )
	{
		base_node<> * const mount_root	=	node_cast<base_node>(result.mount->get_mount_root());
		if ( m_file_system.on_mounted )
			m_file_system.on_mounted		(mount_root);
	}
}

void   mounter::finish_with_out_of_memory ()
{
	m_result							=	result_out_of_memory;
	finish									(mount_result(m_mount_ptr, m_result));

	destroy_this_if_needed					();
}

void   mounter::destroy_this_if_needed	()
{
	if ( m_args.submount_type != submount_type_subfat )
		return;

	if ( m_args.synchronous_device )
		return;
		
	memory::base_allocator * allocator	=	m_args.allocator;
	mounter * this_ptr					=	this;
	XRAY_DELETE_IMPL						(allocator, this_ptr);
}

} // namespace vfs
} // namespace xray