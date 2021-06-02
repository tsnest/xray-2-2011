////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_model_wrapper.h"

#pragma managed( push, off )
#	include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray {
namespace model_editor {
render_model_wrapper::render_model_wrapper( ) :
	mode				( 0 )
{
	m_static_model		= NEW ( render::static_model_ptr)();
	m_skeleton_model	= NEW ( render::skeleton_model_ptr)();
	m_hq_collision		= NEW ( collision::geometry_ptr )();
}

render_model_wrapper::~render_model_wrapper( )
{
	DELETE		( m_skeleton_model );
	DELETE		( m_static_model );
	DELETE		( m_hq_collision );
}

void render_model_wrapper::set_resource( resources::unmanaged_resource_ptr model, resources::unmanaged_resource_ptr hq_collision )
{
	if(model->get_class_id() == resources::skeleton_model_instance_class)
	{
		*m_skeleton_model	= static_cast_resource_ptr<render::skeleton_model_ptr>( model );
		mode = 1;
	}
	else
	{
		*m_static_model		= static_cast_resource_ptr<render::static_model_ptr>( model );
		mode = 2;
	}

	*m_hq_collision		= static_cast_resource_ptr<collision::geometry_ptr>( hq_collision );
}

void render_model_wrapper::reset( )
{
	*m_static_model		= NULL;
	*m_skeleton_model	= NULL;
	*m_hq_collision		= NULL;
	mode				= 0;
}

render::render_model_instance_ptr render_model_wrapper::get_render_model_( )
{
	if(mode==1)
		return (*m_skeleton_model)->m_render_model;
	else
	if(mode==2)
		return (*m_static_model)->m_render_model;
	else
		UNREACHABLE_CODE( return NULL );
}

collision::geometry_ptr render_model_wrapper::get_collision_geometry( )
{
	return *m_hq_collision;
}

void render_model_wrapper::remove_from_scene( render::scene_renderer& scene_renderer, render::scene_ptr const&	scene_ptr )
{
	scene_renderer.remove_model	( scene_ptr, get_render_model_() );
	reset( );
}

void render_model_wrapper::add_to_scene( render::scene_renderer& scene_renderer, render::scene_ptr const& scene_ptr, math::float4x4 const& m )
{
	scene_renderer.add_model( scene_ptr, get_render_model_(), m );
}

void render_model_wrapper::move( render::scene_renderer& scene_renderer, render::scene_ptr const& scene_ptr, math::float4x4 const& m )
{
	scene_renderer.update_model( scene_ptr, get_render_model_(), m );
}

bool render_model_wrapper::empty( )
{
	return (mode==0);
}

} // namespace model_editor
} // namespace xray

