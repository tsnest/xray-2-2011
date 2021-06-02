////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_PHYSICAL_PATH_INFO_H_INCLUDED
#define FS_PHYSICAL_PATH_INFO_H_INCLUDED

#include <xray/fs/native_path_string.h>
#include <xray/fs/physical_path_info_data.h>
#include <xray/fs/device_file_system_proxy.h>
#include <xray/fs/path_string_utils.h>

namespace xray {
namespace fs_new {

class	physical_path_info;

struct physical_path_initializer
{
	u64								search_handle;
	physical_path_info const *		parent;
	physical_path_info_data			data;
	device_file_system_interface *	device;

	physical_path_initializer() 
		: search_handle(u64(-1)), parent(NULL), device(NULL) {}
};

class physical_path_info
{
public:
									physical_path_info	();
									physical_path_info	(physical_path_initializer const & initializer);

	bool							exists				() const { return data.exists(); }
	bool							is_file				() const { return data.is_file(); }
	bool							is_folder			() const { return data.is_folder(); }
	bool							does_exist_and_is_file		() const { return data.exists() && data.is_file(); }
	bool							does_exist_and_is_folder	() const { return data.exists() && data.is_folder(); }
	file_size_type					get_file_size		() const { R_ASSERT(is_file()); return data.file_size; }
	u32								last_time_of_write	() const { return data.last_time_of_write; }
	void							get_full_path		(native_path_string * out_path) const;
	native_path_string				get_full_path		() const;
	template <class StringType>
	void							get_name			(StringType * out_name) const;

 	physical_path_initializer		children_begin		() const;
 	physical_path_initializer		children_end		() const;

	physical_path_info const *		get_parent			() const { R_ASSERT(exists()); return parent; }
private:
	void							initialize_full_path_if_needed	() const;
protected:
	device_file_system_interface *	device;
	physical_path_info_data			data;
private:
	physical_path_info const *		parent;
};

template <class StringType> inline
void   physical_path_info::get_name (StringType * out_name) const
{
	if ( data.path_type == physical_path_info_data::path_type_contains_name )
		* out_name					=	data.path.c_str();
	else if ( data.path_type == physical_path_info_data::path_type_contains_full_path )
		* out_name					=	fs_new::file_name_from_path(data.path);
	else
		NODEFAULT						("called get_name on invalid object");
}

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_PHYSICAL_PATH_INFO_H_INCLUDED