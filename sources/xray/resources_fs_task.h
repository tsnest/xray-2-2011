////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_FS_TASK_H_INCLUDED
#define RESOURCES_FS_TASK_H_INCLUDED

#include <xray/resources_fs_iterator.h>
#include <xray/fs_watcher.h>
#include <xray/intrusive_list.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/fs/native_path_string.h>
#include <xray/resources_query_result.h>

namespace xray {
namespace resources {

class fs_task
{
public:
	enum	type_enum				{	type_undefined, 
										type_fs_iterator,
										type_mount_operations_start,
											type_mount_physical, 
											type_mount_archive, 
											type_unmount,
											type_erase_file,
											type_mount_composite, // list of mount/unmount operations
										type_mount_operations_end,
										// take care to insert all mount operations between guards
									};

public:
					fs_task								(type_enum type,  
														 memory::base_allocator * allocator,
														 query_result_for_cook * parent_query = NULL);
	virtual			~fs_task							() {}

	virtual void	execute_may_destroy_this			() = 0;
	virtual void	call_user_callback					() {}

	u32				thread_id							() const { return m_thread_id; }
	type_enum		type								() const { return m_type; }
	virtual bool	is_helper_query_for_mount			() const { return false; }
	bool			is_mount_task						() const;
	bool			is_sub_request						() const { return m_parent_query != NULL; }
	memory::base_allocator *	allocator				() const { return m_allocator; }

protected:
	void			on_task_ready_may_destroy_this		();

protected:
	fs_task *							m_next;
	bool								m_result;

private:
	type_enum							m_type;
	memory::base_allocator *			m_allocator;
	u32									m_thread_id;
	query_result_for_cook *				m_parent_query;

	friend class						resources_manager;
	friend class						thread_local_data;
	friend class						fs_task_composite;
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_FS_TASK_H_INCLUDED