////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_MANAGED_RESOURCE_H_INCLUDED
#define XRAY_RESOURCES_MANAGED_RESOURCE_H_INCLUDED

#include <xray/fs_path_string.h>
#include <xray/resources_classes.h>
#include <xray/resources_intrusive_base.h>
#include <xray/resources_resource_base.h>
#include <xray/resources_resource_ptr.h>
#include <xray/managed_node_owner.h>

namespace xray {

namespace memory {
	class tester_pinned_resource;
} // namespace memory

namespace resources {

template <class T>
class	pinned_ptr_const;

class XRAY_CORE_API managed_resource :	public resource_base, 
										public managed_intrusive_base,
										public memory::managed_node_owner
{
public:
						   ~managed_resource		();

private:
							managed_resource		(u32 size, class_id_enum class_id);

	managed_resource *		get_next_delay_delete	() const { return m_next_delay_delete; }
	virtual void			on_managed_node_resized (u32 new_size);
	u32						get_buffer_size			() const;
	void					late_set_fat_it			(vfs::vfs_iterator it);
	virtual log_string_type	log_string				() const;
	void					set_creation_source		(creation_source_enum creation_source, pcstr request_path);
	void					set_sub_fat_resource	(vfs_sub_fat_resource * sub_fat);
	vfs_sub_fat_resource *	get_sub_fat				() const { return m_sub_fat.c_ptr(); }

private:
	vfs_sub_fat_child_resource_ptr	m_sub_fat;

	managed_resource *		m_next_delay_delete;
#ifndef MASTER_GOLD
	fs_new::virtual_path_string			m_request_path;
#endif

	friend class			resource_base;
	friend class			query_result;
	friend class			::xray::memory::managed_allocator;
	friend class			resources_manager;
	friend class			thread_local_data;
	friend class			managed_intrusive_base;
	friend class			base_of_intrusive_base;
	friend class			cook_base;
	friend class			game_resources_manager;

	template <class T>
	friend class			pinned_ptr_base;

	friend class			::xray::memory::tester_pinned_resource; // tester class
};

typedef	resource_ptr		<managed_resource, managed_intrusive_base>		managed_resource_ptr;
typedef	child_resource_ptr	<managed_resource, managed_intrusive_base>		child_managed_resource_ptr;

namespace resources_detail {
	
	template class XRAY_CORE_API xray::intrusive_ptr< xray::resources::managed_resource, 
													  xray::resources::managed_intrusive_base, 
													  xray::threading::simple_lock >;

	template class XRAY_CORE_API xray::resources::resource_ptr< xray::resources::managed_resource, 
																xray::resources::managed_intrusive_base >;

	template class XRAY_CORE_API xray::resources::child_resource_ptr<	xray::resources::managed_resource, 
																		xray::resources::managed_intrusive_base >;

} // namespace resources_detail

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_MANAGED_RESOURCE_H_INCLUDED