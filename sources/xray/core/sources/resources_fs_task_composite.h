////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_FS_TASK_COMPOSITE_H_INCLUDED
#define RESOURCES_FS_TASK_COMPOSITE_H_INCLUDED

#include <xray/intrusive_list.h>
#include <xray/resources_fs_task.h>

namespace xray {
namespace resources {

class fs_task_composite : public fs_task
{
public:
	fs_task_composite					(memory::base_allocator * allocator);
	virtual void   execute_may_destroy_this ();

	void	add_child					(fs_task * const child);

	void	lock_children				() { m_children.lock(); }
	void	unlock_children				() { m_children.unlock(); }
	
	fs_task * pop_all_and_clear			() { return m_children.pop_all_and_clear(); }

private:
	typedef	intrusive_list<fs_task, fs_task *, & fs_task::m_next>	children_list;
	children_list						m_children;	// for composite task

}; // class fs_task_composite

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_FS_TASK_COMPOSITE_H_INCLUDED