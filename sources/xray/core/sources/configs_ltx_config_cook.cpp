////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "configs_ltx_config_cook.h"
#include "configs_ltx_config.h"
#include "configs_binary_config.h"
#include <xray/resources_query_result.h>

namespace xray {
namespace core {
namespace configs {

mutable_buffer   ltx_config_cook::allocate_resource	(resources::query_result_for_cook &	in_query, 
													 const_buffer						raw_file_data, 
													 bool								file_exist)
{
	XRAY_UNREFERENCED_PARAMETERS			(&in_query, &raw_file_data, file_exist);
	return									mutable_buffer (XRAY_MALLOC_IMPL(xray::debug::g_mt_allocator, sizeof(xray::configs::binary_config), "lua"), 
															sizeof(xray::configs::binary_config));
}

void   ltx_config_cook::destroy_resource	(resources::unmanaged_resource * resource)
{
	binary_config * config			= static_cast_checked<binary_config *>(resource);
	R_ASSERT						(config);
	config->~binary_config			();
}

void   ltx_config_cook::deallocate_resource	(pvoid buffer)
{
	XRAY_FREE_IMPL					(xray::debug::g_mt_allocator, buffer);
}

void   ltx_config_cook::create_resource (resources::query_result_for_cook &	in_out_query, 
										 const_buffer							raw_file_data,
										 mutable_buffer						in_out_unmanaged_resource_buffer)
{
	xray::memory::reader reader		((u8 const*)raw_file_data.c_ptr(), raw_file_data.size());

	pbyte	binary_buff				= NULL;
	u32		binary_buff_size		= 0;

	parse_string_data				(reader, & xray::debug::g_mt_allocator, & binary_buff, binary_buff_size);

	binary_config * const config	= new (in_out_unmanaged_resource_buffer.c_ptr()) binary_config 
									(binary_buff, binary_buff_size, & xray::debug::g_mt_allocator);

	in_out_query.set_unmanaged_resource	(config, resources::nocache_memory, in_out_unmanaged_resource_buffer.size());
	in_out_query.finish_query		(result_success);
}

} // namespace core
} // namespace configs
} // namespace xray