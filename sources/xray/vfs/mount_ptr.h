////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNT_PTR_H_INCLUDED
#define VFS_MOUNT_PTR_H_INCLUDED

#include <xray/vfs/api.h>
#include <xray/vfs/platform_configuration.h>
#include <xray/intrusive_double_linked_list.h>

namespace xray {
namespace vfs {

class vfs_mount;
class virtual_file_system;
class vfs_intrusive_mount_base;

typedef intrusive_ptr<vfs_mount, vfs_intrusive_mount_base, threading::simple_lock >	
		vfs_mount_ptr;

template class XRAY_CORE_API xray::intrusive_ptr< xray::vfs::vfs_mount, 
												  xray::vfs::vfs_intrusive_mount_base, 
												  xray::threading::simple_lock >;

class XRAY_VFS_API vfs_intrusive_mount_base
{
public:
	u32			get_reference_count		() const { return m_reference_count; }

	vfs_intrusive_mount_base			() : m_reference_count(0) {}
	void		destroy					(vfs_mount * object) const;
	void		unlink_children			(vfs_mount * object, virtual_file_system & file_system) const;
	threading::atomic32_type			m_reference_count;

	vfs_mount_ptr						prev_in_children;
	vfs_mount_ptr						next_in_children;
};

class XRAY_VFS_API vfs_mount : public vfs_intrusive_mount_base
{
public:
	vfs_mount							(mount_root_node_base<> * mount_root,
										 memory::base_allocator * allocator)
		:	m_mount_root		(mount_root), 
			m_allocator			(allocator),
			next_mount_history	(NULL), 
			prev_mount_history	(NULL), 
			m_destroy_count		(1), 
			parent				(NULL),
			user_data			(NULL)	{}

	void set_mount_root					(mount_root_node_base<> * mount_root) { m_mount_root = mount_root; }

	pcstr	get_physical_path			() const;
	pcstr	get_virtual_path			() const;
	pcstr	get_descriptor				() const;

	vfs_mount_ptr	make_pointer_possibly_increase_destroy_count	();

public:
	vfs_mount *							next_mount_history;
	vfs_mount *							prev_mount_history;
	vfs_mount_ptr						next_scheduled_to_unmount;

	pvoid								user_data;

	typedef intrusive_double_linked_list<
		vfs_intrusive_mount_base, 
		vfs_mount_ptr, 
		& vfs_intrusive_mount_base::prev_in_children,
		& vfs_intrusive_mount_base::next_in_children,
		threading::simple_lock,
		size_policy

	>	children_list;

	children_list						children;
	vfs_mount *							parent;

	void	unlink_from_parent			();

	mount_root_node_base<> *			get_mount_root() const { return m_mount_root; }
	u32									change_reference_count_change_may_destroy_this(int change);
	u32									get_mount_size() const;

	u32		decrement_destroy_count		() { return threading::interlocked_decrement(m_destroy_count); }
	void	increment_destroy_count		() { threading::interlocked_increment(m_destroy_count); }
	memory::base_allocator *			get_allocator () const { return m_allocator; }

private:
	mount_root_node_base<> *			m_mount_root;
	memory::base_allocator *			m_allocator;

	// this counter shows after how many calls of destroy
	// object can be safely deleted
	threading::atomic32_type			m_destroy_count;

	friend class						vfs_intrusive_mount_base;
}; // class vfs_mount

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNT_PTR_H_INCLUDED