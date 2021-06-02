////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONFIGS_LUA_CONFIG_COOK_H_INCLUDED
#define CONFIGS_LUA_CONFIG_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include <xray/configs.h>

namespace xray {
namespace core {
namespace configs {

xray::configs::lua_config*  create_lua_config_inplace	( mutable_buffer const & in_out_buffer, 
														  pcstr file_name, 
														  memory::reader& reader );
pvoid						lua_allocate				( u32 const size );
void						lua_deallocate				( pvoid buffer );
void						destroy_lua_config			( xray::resources::unmanaged_resource* config );

class lua_config_cook : public resources::unmanaged_cook
{
public:
	lua_config_cook () :
	unmanaged_cook	(resources::lua_config_class, reuse_true, use_cook_thread_id, use_current_thread_id) {}

	virtual	mutable_buffer		allocate_resource		(resources::query_result_for_cook &	in_query, 
														 const_buffer						raw_file_data, 
														 bool								file_exist);

	virtual void				destroy_resource		(resources::unmanaged_resource *	resource);
	virtual void				deallocate_resource		(pvoid								buffer);
	virtual void				create_resource			(resources::query_result_for_cook &	in_out_query, 
											 			 const_buffer						raw_file_data,
												 		 mutable_buffer						in_out_unmanaged_resource_buffer);
};

} // namespace configs
} // namespace core
} // namespace xray

#ifdef MASTER_GOLD
#error class must not be used in MASTER_GOLD
#endif

#endif // #ifndef CONFIGS_LUA_CONFIG_COOK_H_INCLUDED