////////////////////////////////////////////////////////////////////////////
//	Created		: 08.10.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_FILE_SYSTEM_SUB_FAT_COOK_H_INCLUDED
#define FS_FILE_SYSTEM_SUB_FAT_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace fs {

class sub_fat_cook : public resources::inplace_unmanaged_cook
{
public:
							sub_fat_cook			();

	virtual	mutable_buffer	allocate_resource		(resources::query_result_for_cook &	in_query, 
													 u32								file_size, 
													 u32 &								out_offset_to_file, 
													 bool								file_exist) ;
	virtual void			deallocate_resource		(pvoid buffer);

	virtual void			create_resource			(resources::query_result_for_cook & in_out_query, 
													 mutable_buffer						in_out_unmanaged_resource_buffer);

	virtual void			destroy_resource		(resources::unmanaged_resource *	resource);

}; // class sub_fat_cook

} // namespace fs
} // namespace xray

#endif // #ifndef FS_FILE_SYSTEM_SUB_FAT_COOK_H_INCLUDED