////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_decal.h"
#include "object_base.h"
#include "project.h"

#include <xray/editor/base/collision_object_types.h>

#pragma managed(push,off)
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/render/facade/decal_properties.h>
#include <xray/render/facade/material_effects_instance_cook_data.h>
#include <xray/render/facade/vertex_input_type.h>
#include <xray/collision/collision_object.h>
#pragma managed(pop)

using namespace xray::editor::wpf_controls;
typedef xray::editor::wpf_controls::select_behavior select_behavior;


namespace xray {
namespace editor {


public ref class material_instance_ui_type_editor : public property_editors::i_external_property_editor
{
	typedef xray::editor::wpf_controls::property_descriptor property_descriptor;

public:
	virtual	void run_editor( property_editors::property^ prop )
	{
		System::String^ current = nullptr;
		System::String^ filter = nullptr;
		System::String^ selected = nullptr;
		
		if(prop->value)
			current = safe_cast<System::String^>(prop->value);
		
		if( xray::editor_base::resource_chooser::choose( "material_instance", current, filter, selected, false ) )
		{
			prop->value = selected;
		}
	};
}; // ref class material_instance_ui_type_editor

using xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;

object_decal::object_decal(tool_base^ t, render::scene_ptr const& scene):
	super			(t),
	m_scene			(NEW(render::scene_ptr) (scene)),
	m_material_ptr	(NEW(resources::unmanaged_resource_ptr)),
	m_cook_data		(NEW(xray::render::material_effects_instance_cook_data*))
{
	
}

object_decal::~object_decal()
{
	DELETE								(m_cook_data);
	DELETE								(m_scene);
	DELETE								(m_material_ptr);
}

void object_decal::material_name::set(System::String^ value)
{
	if (m_material_name!= value)
		m_material_name					= value;
	requery_material					();
}

void object_decal::load_props(configs::lua_config_value const& t)
{
	super::load_props					(t);
	System::String^ s					= gcnew System::String((pcstr)t["decal_material"]);
	
	if (m_material_name!= s)
	{
		m_material_name					= s;
		requery_material				();
	}
	
	m_projection_on_static_geometry		= t.value_exists("projection_on_static_geometry") ? 
										  bool(t["projection_on_static_geometry"]) : true;
	
	m_projection_on_skeleton_geometry	= t.value_exists("projection_on_skeleton_geometry") ? 
										  bool(t["projection_on_skeleton_geometry"]) : true;
	
	m_projection_on_terrain_geometry	= t.value_exists("projection_on_terrain_geometry") ? 
										  bool(t["projection_on_terrain_geometry"]) : true;
	
	m_projection_on_speedtree_geometry	= t.value_exists("projection_on_speedtree_geometry") ? 
										  bool(t["projection_on_speedtree_geometry"]) : true;
	
	m_projection_on_particle_geometry	= t.value_exists("projection_on_particle_geometry") ? 
										  bool(t["projection_on_particle_geometry"]) : true;
	
	m_alpha_angle						= t.value_exists("alpha_angle") ? 
										  float(t["alpha_angle"]) : -90.0f;
	
	m_clip_angle						= t.value_exists("clip_angle") ? 
										  float(t["clip_angle"]) : -90.0f;
	
	m_decal_far_distance				= t.value_exists("decal_far_distance") ? 
										  float(t["decal_far_distance"]) : 1.0f;

	m_decal_width						= t.value_exists("decal_width") ? 
										  float(t["decal_width"]) : 1.0f;

	m_decal_height						= t.value_exists("decal_height") ? 
										  float(t["decal_height"]) : 1.0f;
}

void object_decal::save( configs::lua_config_value t )
{
	super::save(t);
	t["decal_material"]						= unmanaged_string(m_material_name).c_str();
	t["game_object_type"]					= "decal";
	t["projection_on_static_geometry"]		= m_projection_on_static_geometry;
	t["projection_on_skeleton_geometry"]	= m_projection_on_skeleton_geometry;
	t["projection_on_terrain_geometry"]		= m_projection_on_terrain_geometry;
	t["projection_on_speedtree_geometry"]	= m_projection_on_speedtree_geometry;
	t["projection_on_particle_geometry"]	= m_projection_on_particle_geometry;
	t["alpha_angle"]						= m_alpha_angle;
	t["clip_angle"]							= m_clip_angle;
	t["decal_far_distance"]					= m_decal_far_distance;
	t["decal_width"]						= m_decal_width;
	t["decal_height"]						= m_decal_height;
	
}

void object_decal::load_contents( )
{
	if(!m_collision->initialized())
		initialize_collision			();
}

void object_decal::initialize_collision ()
{
	ASSERT								(!m_collision->initialized());

	float3					extents		(0.2f, 0.2f, 0.2f);
	collision::geometry_instance* geom	= &*collision::new_box_geometry_instance(&debug::g_mt_allocator, math::create_scale(extents) );
	m_collision->create_from_geometry	(
		true,
		this,
		geom,
		editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch
	);
	m_collision->insert					( m_transform );
}

void object_decal::update_render_decal	()
{
	if(get_visible())
	{
		get_editor_renderer().scene().update_decal(
			*m_scene, 
			id(), 
			xray::render::decal_properties(
				get_transform(),
				*m_material_ptr,
				float3(m_decal_width, m_decal_height, m_decal_far_distance),
				m_alpha_angle / 90.0f,
				m_clip_angle / 90.0f,
				m_projection_on_terrain_geometry,
				m_projection_on_static_geometry,
				m_projection_on_speedtree_geometry,
				m_projection_on_skeleton_geometry,
				m_projection_on_particle_geometry
			)
		);
	}
}

void object_decal::on_static_geometry::set(bool value)
{
	m_projection_on_static_geometry		= value;
	update_render_decal					();
}

void object_decal::on_skeleton_geometry::set(bool value)
{
	m_projection_on_skeleton_geometry	= value;
	update_render_decal					();
}

void object_decal::on_terrain_geometry::set(bool value)
{
	m_projection_on_terrain_geometry	= value;
	update_render_decal					();
}

void object_decal::on_speedtree_geometry::set(bool value)
{
	m_projection_on_speedtree_geometry	= value;
	update_render_decal					();
}

void object_decal::on_particle_geometry::set(bool value)
{
	m_projection_on_particle_geometry	= value;
	update_render_decal					();
}

void object_decal::alpha_angle::set(float value)
{
	m_alpha_angle						= value;
	update_render_decal					();
}

void object_decal::clip_angle::set(float value)
{
	m_clip_angle						= value;
	update_render_decal					();
}

void object_decal::decal_far_distance::set(float value)
{
	m_decal_far_distance				= value;
	update_render_decal					();
}

void object_decal::decal_width::set(float value)
{
	m_decal_width						= value;
	update_render_decal					();
}

void object_decal::decal_height::set(float value)
{
	m_decal_height						= value;
	update_render_decal					();
}

void object_decal::unload_contents(bool bdestroy)
{
	XRAY_UNREFERENCED_PARAMETERS		(bdestroy);

	destroy_collision					();
}

void object_decal::destroy_collision()
{
	if (m_collision->initialized())
		m_collision->destroy			(&debug::g_mt_allocator);
}

void object_decal::render( )
{
	super::render							();
	bool const is_selected					= m_project_item->get_selected();
	
	math::float3 const scale				= is_selected ? 
											  float3(m_decal_width, m_decal_height, m_decal_far_distance) : 
											  float3(0.2f, 0.2f, m_decal_far_distance);
	
	math::float3 const direction			= math::normalize((*m_transform).k.xyz());
	math::float3 const right_offset			= math::normalize((*m_transform).i.xyz());
	math::float3 const up_offset			= math::normalize((*m_transform).j.xyz());
	
	math::float3 const position				= m_transform->c.xyz();
	
	// 0 1
	// 2 3
	
	math::color const line_color			= math::color(255, 0, 0);
	
	math::float3 const corner_position_0	= position + -right_offset * scale.x * 0.5f + up_offset * scale.y * 0.5f;
	math::float3 const corner_position_1	= position +  right_offset * scale.x * 0.5f + up_offset * scale.y * 0.5f;
	math::float3 const corner_position_2	= position + -right_offset * scale.x * 0.5f - up_offset * scale.y * 0.5f;
	math::float3 const corner_position_3	= position +  right_offset * scale.x * 0.5f - up_offset * scale.y * 0.5f;
	
	get_debug_renderer().draw_line			(*m_scene, corner_position_0, corner_position_1, line_color);
	get_debug_renderer().draw_line			(*m_scene, corner_position_1, corner_position_3, line_color);
	get_debug_renderer().draw_line			(*m_scene, corner_position_3, corner_position_2, line_color);
	get_debug_renderer().draw_line			(*m_scene, corner_position_2, corner_position_0, line_color);
	
	get_debug_renderer().draw_line			(*m_scene, position - direction * 0.25f, corner_position_0, line_color);
	get_debug_renderer().draw_line			(*m_scene, position - direction * 0.25f, corner_position_1, line_color);
	get_debug_renderer().draw_line			(*m_scene, position - direction * 0.25f, corner_position_2, line_color);
	get_debug_renderer().draw_line			(*m_scene, position - direction * 0.25f, corner_position_3, line_color);
	
	get_debug_renderer().draw_line			(*m_scene, position, position + direction * scale.z + direction * 0.25f, line_color);	
}

void object_decal::load_defaults()
{
	super::load_defaults				();
}

void object_decal::requery_material()
{
	if(m_material_name->Length==0)
		return; // no material assigned
	
	unmanaged_string request_path			(m_material_name);
	resources::request request[]			= { request_path.c_str(), resources::material_effects_instance_class };
	
	resources::user_data_variant* temp_data[] = {NEW(resources::user_data_variant)};
	
	*m_cook_data							= 
		NEW(xray::render::material_effects_instance_cook_data)(
			xray::render::decal_vertex_input_type,
			NULL,
			false,
			xray::render::cull_mode_none
		);
	
	temp_data[0]->set						(*m_cook_data);
	
	query_result_delegate* query_delegate	= NEW(query_result_delegate)(
		gcnew query_result_delegate::Delegate(this, &object_decal::material_ready), 
		g_allocator
	);
	
	resources::query_resource_params params	(
		request, 
		NULL, 
		1,
		boost::bind(&query_result_delegate::callback, query_delegate, _1),
		g_allocator,
		0,
		0,
		(resources::user_data_variant const**)temp_data
	);
	
	resources::query_resources				(params);
	
	DELETE									(temp_data[0]);
}

void object_decal::material_ready(resources::queries_result& data)
{
	DELETE								(*m_cook_data);
	
	if (!data.is_successful())
		return;
	
	*m_material_ptr						= data[0].get_unmanaged_resource();
	update_render_decal					();
}

void object_decal::set_visible( bool bvisible )
{
	ASSERT(bvisible!=get_visible());
	
 	if(bvisible)
 	{
		update_render_decal				();
 	}
 	else
	{
		get_editor_renderer().scene().remove_decal(*m_scene, id());
	}
	super::set_visible					(bvisible);
}

void object_decal::set_transform(float4x4 const& transform)
{
	super::set_transform				(transform);
	update_render_decal					();
}

void object_decal::on_selected(bool is_selected)
{
	if (!is_selected)
		m_current_property_container	= nullptr;
}

wpf_controls::property_container^ 
object_decal::get_property_container()
{
	wpf_controls::property_container^ container		= super::get_property_container();
	m_current_property_container					= container;
	
	container->properties->remove					("general/scale");
	
	container->properties["decal/decal material"]->dynamic_attributes->add( 
		gcnew external_editor_attribute(material_instance_ui_type_editor::typeid)
	);
	
	return container;
}

} //namespace editor
} //namespace xray
