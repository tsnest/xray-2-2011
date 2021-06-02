////////////////////////////////////////////////////////////////////////////
//	Created		: 14.10.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_save_generated_data.h>

namespace xray {
namespace resources {

save_generated_data	*	save_generated_data::create (const_buffer const &					data,
 													 bool									copy_data,
													 fs_new::native_path_string const &		physical_path, 
													 fs_new::virtual_path_string const &	virtual_path)
{
#ifdef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETERS( data, copy_data, physical_path, virtual_path );
	NOT_IMPLEMENTED							("save_generated is turned off in master-gold!");
#else
	save_generated_data	* const out		=	allocate(physical_path, virtual_path);
	if ( copy_data )
	{
		out->m_data						=	memory::buffer(DEBUG_ALLOC(char, data.size()), data.size());
		memory::copy						(out->m_data, data);
	}
	else
		out->m_data						=	memory::buffer((pstr)data.c_ptr(), data.size());

	out->m_allocated					=	copy_data;
	return									out;
#endif
}	

save_generated_data	*	save_generated_data::create (managed_resource_ptr const &			resource,
													 fs_new::native_path_string const &		physical_path, 
													 fs_new::virtual_path_string const &	virtual_path)
{
	save_generated_data	* const out		=	allocate(physical_path, virtual_path);
	out->m_resource						=	resource;
	return									out;
}	

save_generated_data	*	save_generated_data::allocate (fs_new::native_path_string const &	physical_path, 
													   fs_new::virtual_path_string const &	virtual_path)
{
#ifdef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETERS( physical_path, virtual_path );
	NOT_IMPLEMENTED							("save_generated is turned off in master-gold!");
#else 
	u32 const allocation_size			=	sizeof(save_generated_data) + physical_path.length() + 1
											+ virtual_path.length() + 1;
	save_generated_data * const	out		=	(save_generated_data *)DEBUG_ALLOC(char, allocation_size);
	new (out) save_generated_data			(physical_path, virtual_path);
	return									out;
#endif
}

save_generated_data::save_generated_data	(fs_new::native_path_string const &		physical_path, 
											 fs_new::virtual_path_string const &	virtual_path) 
											 : m_allocated	(false) 
{
	m_physical_path						=	(pstr)this + sizeof(save_generated_data);
	m_virtual_path						=	m_physical_path + physical_path.length() + 1;
	strings::copy							(m_physical_path, physical_path.length() + 1, physical_path.c_str());
	strings::copy							(m_virtual_path, virtual_path.length() + 1, virtual_path.c_str());
}

void   save_generated_data::delete_this	()
{
#ifdef MASTER_GOLD
	NOT_IMPLEMENTED							("save_generated is turned off in master-gold!");
#else
	if ( m_allocated )
	{
		pvoid buffer					=	m_data.c_ptr();
		DEBUG_FREE							(buffer);
	}
	save_generated_data * this_ptr		=	this;
	this_ptr->~save_generated_data			();
	DEBUG_FREE								(this_ptr);
#endif 
}

} // namespace resources
} // namespace xray