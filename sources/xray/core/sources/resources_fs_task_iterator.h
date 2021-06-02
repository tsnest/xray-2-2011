////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_FS_TASK_ITERATOR_H_INCLUDED
#define RESOURCES_FS_TASK_ITERATOR_H_INCLUDED

#include <xray/resources_fs.h>
#include <xray/resources_fs_task.h>
#include <xray/vfs/locked_iterator.h>
#include <xray/vfs/types.h>

namespace xray {
namespace resources {

class fs_task_iterator : public fs_task
{
public:
	fs_task_iterator					(fs_new::virtual_path_string const &	path, 
						 				 query_vfs_iterator_callback			callback, 
										 recursive_bool							recursive,
						 				 memory::base_allocator *				allocator,
						 				 query_result_for_cook *				parent);
										 
	virtual void	execute_may_destroy_this	();
	virtual void	call_user_callback			();
	virtual bool	is_helper_query_for_mount	() const { return m_parent_query && m_parent_query->is_helper_query_for_mount(); }

private:
	void	on_vfs_iterator_ready		(vfs::vfs_locked_iterator const & iterator, vfs::result_enum result);
	void	try_async_query				();

private:
	fs_new::virtual_path_string			m_virtual_path;
	query_vfs_iterator_callback			m_callback;
	recursive_bool						m_recursive;
	vfs::vfs_locked_iterator			m_iterator;
	vfs::result_enum					m_vfs_result;
	bool								m_in_sync_call;
	bool								m_iterator_ready;
	query_result_for_cook *				m_parent_query;

}; // class fs_task_iterator

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_FS_TASK_ITERATOR_H_INCLUDED