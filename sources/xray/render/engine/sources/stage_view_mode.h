////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_VIEW_MODE_H_INCLUDED
#define STAGE_VIEW_MODE_H_INCLUDED

#include "stage.h"
#include "render_model.h"
#include <xray/render/core/res_declaration.h>
#include <xray/render/facade/vertex_input_type.h>

namespace xray {
namespace render {

enum scene_view_mode;
class shader_constant_host;

class stage_view_mode: public stage
{
public:
	stage_view_mode(renderer_context* context);
	
	bool is_support_view_mode(scene_view_mode view_mode) const;
	
	void execute(scene_view_mode view_mode);
	
private:
	bool is_effects_ready() const;
	ref_effect m_editor_wireframe_accumulation_effect[num_vertex_input_types];
	ref_effect m_editor_texture_density_effect[num_vertex_input_types];
	ref_effect m_editor_shader_complexity_effect[num_vertex_input_types];
	ref_effect m_editor_show_miplevel_effect[num_vertex_input_types];
	ref_effect m_editor_geometry_complexity_effect[num_vertex_input_types];
	ref_effect m_editor_accumulate_overdraw_effect[num_vertex_input_types];
	
	ref_effect m_editor_apply_wireframe_shader;
	ref_effect m_editor_show_overdraw_shader;
	
	shader_constant_host* m_shader_complexity_parameter;
	shader_constant_host* m_shader_complexity_min_parameter;
	shader_constant_host* m_shader_complexity_max_parameter;
	shader_constant_host* m_current_max_texture_dimension_parameter;
	shader_constant_host* m_geometry_complexity_parameters;

	shader_constant_host* m_c_start_corner;
	
	render_surface_instances			m_dynamic_visuals;
}; // class stage_view_mode

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_VIEW_MODE_H_INCLUDED