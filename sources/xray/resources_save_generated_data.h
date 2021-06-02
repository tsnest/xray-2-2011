////////////////////////////////////////////////////////////////////////////
//	Created		: 14.10.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_SAVE_GENERATED_DATA_H_INCLUDED
#define XRAY_RESOURCES_SAVE_GENERATED_DATA_H_INCLUDED

#include <xray/resources.h>

namespace xray {
namespace resources {

class save_generated_data
{
public:
	static save_generated_data	*	create (const_buffer const &				data,
											bool								copy_data,
											fs_new::native_path_string const &	physical_path, 
											fs_new::virtual_path_string const &	virtual_path = "");

	static save_generated_data	*	create (managed_resource_ptr const &		resource,
											fs_new::native_path_string const &	physical_path, 
											fs_new::virtual_path_string const &	virtual_path = "");

	void   delete_this						();

	pcstr get_physical_path					() const { return m_physical_path; }
	pcstr get_virtual_path					() const { return m_virtual_path; }

	const_buffer	get_data				() const { return m_data; }
	managed_resource_ptr	get_resource	() const { return m_resource; }

private:
	static save_generated_data	*	allocate (fs_new::native_path_string const &	physical_path, 
											  fs_new::virtual_path_string const &	virtual_path);

	save_generated_data					(fs_new::native_path_string const &		in_physical_path, 
										 fs_new::virtual_path_string const &	in_virtual_path);
private:
	mutable_buffer						m_data;
	bool								m_allocated;
	pstr								m_physical_path;
	pstr								m_virtual_path;
	managed_resource_ptr				m_resource;
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_SAVE_GENERATED_DATA_H_INCLUDED