////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_skeleton_visual.h"
#include "tool_solid_visual.h"
#include "project_items.h"

#include <xray/collision/api.h>
#include <xray/render/world.h>
#include <xray/editor/world/engine.h>

#pragma managed( push, off )
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

object_skeleton_visual::object_skeleton_visual(tool_solid_visual^ t, xray::render::scene_ptr const& scene ):
	super				( t ), 
	m_scene				( NEW(render::scene_ptr) (scene) ),
	m_tool_solid_visual	( t ),
	m_model_instance	( NEW(render::skeleton_model_ptr)() )
{
	image_index			= editor_base::node_mesh;
}

object_skeleton_visual::~object_skeleton_visual()
{
	R_ASSERT			( !get_visible() );
	DELETE				( m_model_instance );
	DELETE				( m_scene );
}

void object_skeleton_visual::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( g_allocator );
}

void object_skeleton_visual::save( configs::lua_config_value t )
{
	super::save				( t );
	t["game_object_type"]	= "skeleton_visual";
}

void object_skeleton_visual::set_visible(bool bvisible)
{
	ASSERT(bvisible!=get_visible());

	if((*m_model_instance).c_ptr())
	{
		if ( bvisible ) {
			get_editor_renderer().scene().add_model	( *m_scene, (*m_model_instance)->m_render_model, *m_transform );
			get_editor_renderer().select_model		( *m_scene, (*m_model_instance)->m_render_model, m_project_item->get_selected() );
		}
		else
			get_editor_renderer().scene().remove_model	( *m_scene, (*m_model_instance)->m_render_model );
	}

	super::set_visible(bvisible);
}

void object_skeleton_visual::set_transform			(float4x4 const& transform)
{
	super::set_transform			(transform);

	if (get_visible() && (*m_model_instance).c_ptr()) {
		get_editor_renderer().scene().update_model	( *m_scene, (*m_model_instance)->m_render_model, *m_transform);
		get_editor_renderer().select_model			( *m_scene, (*m_model_instance)->m_render_model, m_project_item->get_selected());
	}
	update_bbox();
}

void object_skeleton_visual::on_selected(bool selected)
{
	super::on_selected(selected);

	if (get_visible() && (*m_model_instance).c_ptr()) {
		get_editor_renderer().scene().update_model	( *m_scene, (*m_model_instance)->m_render_model, *m_transform );
		get_editor_renderer().select_model			( *m_scene, (*m_model_instance)->m_render_model, selected);
	}
}

void object_skeleton_visual::on_resource_loaded( xray::resources::queries_result& data )
{
	R_ASSERT			( data.is_successful() );

	*m_model_instance	= static_cast_resource_ptr<render::skeleton_model_ptr>(data[0].get_unmanaged_resource());

	if (get_visible()) {
		get_editor_renderer().scene().add_model	( *m_scene, (*m_model_instance)->m_render_model, *m_transform );
		get_editor_renderer().select_model		( *m_scene, (*m_model_instance)->m_render_model, m_project_item->get_selected() );
	}

	initialize_collision( );
}

void object_skeleton_visual::initialize_collision	()
{
	// create sphere collision here
	//m_aabbox->identity			( );
	//ASSERT						( *m_model_instance );
	//ASSERT						( (*m_model_instance)->m_collision_geometry );

	//ASSERT						( !m_collision.initialized() );

	//m_collision.create_from_geometry	( this, (*m_model_instance)->m_collision_geometry.c_ptr(), xray::editor::collision_type_dynamic );
	//m_collision.insert					( );
	//m_collision.set_matrix				( m_transform );
}

void object_skeleton_visual::update_bbox( )
{
	float3 size(0.5f,0.5f,0.5f);
	*m_aabbox = math::create_aabb_min_max(-size, size).modify( *m_transform );
}

void object_skeleton_visual::load_contents( )
{
	if(m_needed_quality_)
		return;

	m_needed_quality_		= true;
	if( (*m_model_instance).c_ptr() )
		return;

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &object_skeleton_visual::on_resource_loaded), g_allocator);

	resources::query_resource	(
		unmanaged_string(m_lib_name).c_str(),
		resources::skeleton_model_instance_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
		);
}

void object_skeleton_visual::unload_contents( bool )
{
	if( !(*m_model_instance).c_ptr() )
		return;

	if(!m_needed_quality_)
		return;

	if(get_visible())
		get_editor_renderer().scene().remove_model	( *m_scene, (*m_model_instance)->m_render_model );

	destroy_collision		( );

	(*m_model_instance)		= 0;
}


} // namespace editor
} // namespace xray
