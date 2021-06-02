////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_MANAGER_H_INCLUDED
#define MATERIAL_MANAGER_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include "material.h"
#include <xray/render/facade/vertex_input_type.h>
#include "effect_material_base.h"

namespace xray {

namespace vfs {
	struct vfs_notification;
} // namespace fs

namespace render {

struct post_process_parameters;
class material;
typedef	resources::resource_ptr<material,resources::unmanaged_intrusive_base> material_ptr;

class res_effect;
class resource_intrusive_base;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

enum enum_render_stage_type;

class material_shader_base;

struct material_effects_instance;
typedef	resources::resource_ptr<
	material_effects_instance, 
	resources::unmanaged_intrusive_base
> material_effects_instance_ptr;

struct material_effects_entry
{
	material_effects_entry(
			material_effects_instance_ptr*	in_material_effects_instance_ptr,
			fs_new::virtual_path_string		in_material_name):
		m_material_effects_instance_ptr(in_material_effects_instance_ptr),
		m_material_name(in_material_name)
	{}
	material_effects_instance_ptr*	m_material_effects_instance_ptr;
	fs_new::virtual_path_string		m_material_name;
};

class material_manager: public quasi_singleton<material_manager>
{
public:
	typedef render::vector<material_effects_entry>								material_effects_entries_type;	
	typedef render::map<post_process_parameters*, fs_new::virtual_path_string>	post_process_parameters_type;
	
	material_manager					();
	~material_manager					();
	
	void add_material_effects			(material_effects_instance_ptr& in_material_effects_instance, 
										 fs_new::virtual_path_string const& in_material_name);
	void remove_material_effects		(material_effects_instance_ptr const& in_material_effects_instance);
	
	void initialize_post_process_parameters	(post_process_parameters* out_post_process_parameters, material_ptr mtl, bool force_recompilation);
	
	void remove_post_process_parameters	(post_process_parameters* in_post_process_parameters);
	
	void get_output_text				(fs_new::virtual_path_string* out_text, u32& num_lines);
	
#ifndef MASTER_GOLD
	void update_material_from_config	(fs_new::virtual_path_string const& material_name,  xray::configs::lua_config_value const& config_value);
#endif // #ifndef MASTER_GOLD

private:
	
#ifndef MASTER_GOLD
	material_ptr create_material		(fs_new::virtual_path_string const& material_name, xray::configs::lua_config_value const& config_value);
	void on_material_created			(xray::resources::queries_result& data, material_ptr* out_material_ptr);
	material_ptr create_material		(xray::configs::lua_config_value const& config_value);
#endif // #ifndef MASTER_GOLD
	
	pcstr get_materials_path			() const;
	pcstr get_material_instances_path	() const;
	
	void on_material_effects_instance_loaded (xray::resources::queries_result& data, 
											  material_effects_instance_ptr* mtl_effects_instance_ptr);
	void on_material_loaded				(xray::resources::queries_result& data, 
										 post_process_parameters* in_out_post_process_parameters);
#ifndef MASTER_GOLD
	void on_material_source_changed		(xray::vfs::vfs_notification const & info);
	void on_material_source_changed		(fs_new::virtual_path_string const& material_name, material_ptr const& in_material = NULL);
#endif // #ifndef MASTER_GOLD
	
private:
	material_effects_entries_type		m_material_effects;
	post_process_parameters_type		m_post_process_parameters;
	u32									m_watcher_materials_subscribe_id;
	u32									m_watcher_material_instanced_subscribe_id;
}; // class material_manager

} // namespace render 
} // namespace xray 


#endif // #ifndef MATERIAL_MANAGER_H_INCLUDED
