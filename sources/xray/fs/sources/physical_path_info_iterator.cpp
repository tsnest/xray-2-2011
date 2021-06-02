////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/physical_path_iterator.h>
#include <xray/fs/device_file_system_proxy.h>

namespace xray {
namespace fs_new {

physical_path_info::physical_path_info () : device(NULL), parent(NULL)
{
}

physical_path_info::physical_path_info (physical_path_initializer const & initializer) 
	: parent(initializer.parent), device(initializer.device), data(initializer.data)
{
}

void   physical_path_info::get_full_path (native_path_string * out_path) const
{
	initialize_full_path_if_needed			();

	* out_path							=	data.path;
}

native_path_string 	 physical_path_info::get_full_path		() const
{
	native_path_string						out_path;
	get_full_path							(& out_path);
	return									out_path;
}

void   physical_path_info::initialize_full_path_if_needed () const
{
	if ( data.path_type == physical_path_info_data::path_type_contains_full_path )
		return;

	ASSERT									(parent);
	native_path_string const name		=	data.path;
	parent->get_full_path					(& data.path);
	data.path							+=	native_path_string::separator;
	data.path							+=	name;
	data.path_type						=	physical_path_info_data::path_type_contains_full_path;
}

physical_path_initializer   physical_path_info::children_begin () const
{
	if ( data.type == physical_path_info_data::type_file )
		return								children_end();

	physical_path_initializer				initializer;
	initializer.device					=	device;
	initializer.parent					=	this;
	initializer.search_handle			=	u64(-1);

	initialize_full_path_if_needed			();

	u32 const saved_full_path_size		=	data.path.length();

	data.path							+=	native_path_string::separator;
	data.path							+=	'*';
	device->find_first						(& initializer.search_handle, & initializer.data, data.path.c_str());

	data.path.set_length					(saved_full_path_size);
	return									initializer;
}

physical_path_initializer   physical_path_info::children_end () const
{
	return									physical_path_initializer();
}

physical_path_iterator::~physical_path_iterator ()
{
	if ( search_handle != u64(-1) )
	{
		device->find_close					(search_handle);
		search_handle					=	0;
	}
}

void   physical_path_iterator::operator ++ () 
{ 
	CURE_ASSERT								(device, return);
	physical_path_initializer	initializer;

	u64 saved_handle					=	search_handle;
	if ( !device->find_next(& search_handle, & data) )
	{
		device->find_close					(saved_handle);
		search_handle					=	u64(-1);
	}
}

} // namespace fs_new
} // namespace xray