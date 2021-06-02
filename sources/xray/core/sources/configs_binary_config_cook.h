////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONFIGS_BINARY_CONFIG_COOK_H_INCLUDED
#define CONFIGS_BINARY_CONFIG_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include <xray/resources_fs.h>

namespace xray {
namespace core {
namespace configs {

class binary_config_cook_impl : public resources::unmanaged_cook
{
public:
								binary_config_cook_impl		();

	virtual	mutable_buffer		allocate_resource		(resources::query_result_for_cook &	in_query, 
														 const_buffer						raw_file_data, 
														 bool								file_exist);

	virtual void				destroy_resource		(resources::unmanaged_resource *	resource);
	virtual void				deallocate_resource		(pvoid buffer);
	virtual void				create_resource			(resources::query_result_for_cook &	in_out_query, 
												 		 const_buffer						raw_file_data,
													 	 mutable_buffer						in_out_unmanaged_resource_buffer);
}; // class binary_config_cook_impl

class binary_config_cook : public resources::translate_query_cook {
	typedef resources::translate_query_cook				super;
public:
								binary_config_cook		();
	virtual	void				translate_query			(resources::query_result_for_cook& parent);
	virtual void				delete_resource			(resources::resource_base* resource);

private:
#ifndef MASTER_GOLD
			void				on_lua_config_loaded	(resources::queries_result& data, resources::query_result_for_cook* parent_query);
#endif // #ifndef MASTER_GOLD
			void				on_fs_iterators_ready	(resources::queries_result & results);
			void				on_binary_config_loaded	(resources::queries_result& data, resources::query_result_for_cook* parent_query);
}; // class binary_config_cook

} // namespace configs
} // namespace core
} // namespace xray

#endif // #ifndef CONFIGS_BINARY_CONFIG_COOK_H_INCLUDED
