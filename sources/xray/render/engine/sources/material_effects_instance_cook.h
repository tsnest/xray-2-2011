////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_MATERIAL_EFFECTS_INSTANCE_COOK_H_INCLUDED
#define XRAY_RENDER_MATERIAL_EFFECTS_INSTANCE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

enum	enum_vertex_input_type;
class	custom_config_value;
class	material;
struct	effect_options_descriptor;
struct	material_effects;
struct	material_effects_instance_cook_data;

typedef	resources::resource_ptr<material,resources::unmanaged_intrusive_base> material_ptr;

class material_effects_instance_cook: public resources::translate_query_cook
{
public:
						material_effects_instance_cook		();
	virtual		void	translate_query						(resources::query_result_for_cook & parent);
	virtual		void	delete_resource						(resources::resource_base * resource);
				void	gather_request_user_data			(resources::user_data_variant* user_data, 
															 custom_config_value const* root_config,
															 effect_options_descriptor const* additional_parameters);
				void	on_material_ready					(resources::queries_result& data, 
															 material_effects_instance_cook_data* cook_data);
				void	query_effects						(resources::query_result_for_cook& parent,
															 material_effects_instance_cook_data* cook_data);
				void	on_effect_ready						(resources::queries_result& data, 
															 material_effects_instance_cook_data* cook_data);
}; // class material_effects_instance_cook

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_MATERIAL_EFFECTS_INSTANCE_COOK_H_INCLUDED