////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_WRAPPER_COOK_H_INCLUDED
#define TEXTURE_WRAPPER_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

struct texture_converter_params;

struct texture_cook_wrapper : public resources::translate_query_cook {
public:
					texture_cook_wrapper			();

	virtual void	translate_query					(resources::query_result_for_cook & parent);
	virtual void	delete_resource					(resources::resource_base * resource);
	
private:

	void			query_converted_texture			(resources::query_result_for_cook * parent);

#ifndef MASTER_GOLD
	void			on_fs_iterators_ready			(resources::queries_result & result);

	void			request_convertion				(pcstr source_texture_path, pcstr options_texture_path, 
													 resources::query_result_for_cook * parent);

	void			on_raw_data_loaded				(resources::queries_result & result);

	void			on_texture_converted			(resources::queries_result & result, texture_converter_params * data_to_delete);
	void			on_texture_created				(resources::managed_resource_ptr	converted_resource, 
													 pcbyte								pinned_data, 
													 resources::queries_result &		result);
#endif // #ifndef MASTER_GOLD

	void			on_texture_loaded				(resources::queries_result & result);

	void			make_converted_path				(fs_new::virtual_path_string * out_converted_path, 
													 resources::query_result_for_cook & result);

}; // struct texture_cooker

} // namespace render
} // namespace xray

#endif // #ifndef TEXTURE_WRAPPER_COOK_H_INCLUDED