////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONFIGS_LTX_CONFIG_COOK_H_INCLUDED
#define CONFIGS_LTX_CONFIG_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace core {
namespace configs {

class ltx_config_cook : public resources::unmanaged_cook
{
public:
							ltx_config_cook		() :
	unmanaged_cook	(resources::ltx_config_class, reuse_false, use_cook_thread_id, use_current_thread_id) {}

	virtual	mutable_buffer	allocate_resource		(resources::query_result_for_cook &	in_query, 
													 const_buffer						raw_file_data, 
													 bool								file_exist);

	virtual void			destroy_resource		(resources::unmanaged_resource *	resource);
	virtual void			deallocate_resource		(pvoid								buffer);
	virtual void			create_resource			(resources::query_result_for_cook &	in_out_query, 
											 		 const_buffer						raw_file_data,
												 	 mutable_buffer						in_out_unmanaged_resource_buffer);

}; // class ltx_config_cook

} // namespace configs
} // namespace core
} // namespace xray

#ifdef MASTER_GOLD
#error class must not be used in MASTER_GOLD
#endif

#endif // #ifndef CONFIGS_LTX_CONFIG_COOK_H_INCLUDED