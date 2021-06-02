////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SUB_FAT_COOK_H_INCLUDED
#define VFS_SUB_FAT_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace resources {

class vfs_sub_fat_cook : public unmanaged_cook
{
public:
							vfs_sub_fat_cook		();

	virtual	mutable_buffer	allocate_resource		(resources::query_result_for_cook &	in_query, 
													 const_buffer						raw_file_data, 
													 bool								file_exist)
	{
		XRAY_UNREFERENCED_PARAMETERS				(in_query, raw_file_data, file_exist);
		NOT_IMPLEMENTED								(return mutable_buffer::zero());
	}
	virtual void			create_resource			(resources::query_result_for_cook &	in_out_query, 
										 			 const_buffer						raw_file_data, 
													 mutable_buffer						in_out_unmanaged_resource_buffer)
	{
		XRAY_UNREFERENCED_PARAMETERS				(in_out_query, raw_file_data, in_out_unmanaged_resource_buffer);
		NOT_IMPLEMENTED								();
	}

	virtual void			deallocate_resource		(pvoid buffer)
	{
		XRAY_UNREFERENCED_PARAMETERS				(buffer);
		NOT_IMPLEMENTED								();
	}

	virtual void			destroy_resource		(unmanaged_resource * resource);

}; // class vfs_sub_fat_cook

vfs_sub_fat_cook *		get_vfs_sub_fat_cook	();

} // namespace resources
} // namespace xray


#endif // #ifndef VFS_SUB_FAT_COOK_H_INCLUDED