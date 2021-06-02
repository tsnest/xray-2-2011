////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_particle.h"
#include "tool_particle.h"


#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/collision/collision_object.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

object_particle::object_particle( tool_particle^ t, render::scene_ptr const& scene ) :
	super			( t ), 
	m_tool_particle	( t ),
	m_scene			( NEW(render::scene_ptr) (scene) ),
	m_selected		( false )
{
	image_index			= xray::editor_base::particle_tree_icon;
	
	m_particle_system_instance_ptr	= NEW(particle::particle_system_instance_ptr)();
}

object_particle::~object_particle( )
{
	R_ASSERT		( !get_visible() );
	
	get_editor_renderer().scene().remove_particle_system_instance( *m_scene, *m_particle_system_instance_ptr);
	
	DELETE			( m_particle_system_instance_ptr );
	DELETE			( m_scene );
}

void object_particle::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( &debug::g_mt_allocator );
}

void object_particle::initialize_collision( )
{
	ASSERT( !m_collision->initialized() );
	float3 extents				(1.0f,1.0f,1.0f);
	
	collision::geometry_instance* geom	= &*collision::new_box_geometry_instance( &debug::g_mt_allocator, create_scale( extents ) );
	m_collision->create_from_geometry	(
					true,
					this,
					geom,
					editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch
				);
	m_collision->insert				( m_transform );
}

void object_particle::set_library_name( System::String^ name )
{
	m_lib_name		= check_valid_library_name( name );

	R_ASSERT(m_lib_name!=nullptr);

	unmanaged_string s(m_lib_name);
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &object_particle::on_resource_loaded), g_allocator);
	
	resources::user_data_variant* ud = NEW(resources::user_data_variant)();
	ud->set( &get_editor_renderer().scene().particle_world( *m_scene ) );
	
	resources::query_resource(
		s.c_str(),
		resources::particle_system_instance_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator,
		ud
	);
	
	DELETE(ud);
}

void object_particle::update_particle_system( bool pause )
{
	if (m_particle_system_instance_ptr->c_ptr())
	{
		get_editor_renderer().select_particle_system_instance( *m_scene, *m_particle_system_instance_ptr, m_selected);
		get_editor_renderer().scene().update_particle_system_instance( *m_scene, *m_particle_system_instance_ptr, get_transform(), get_visible(), pause);
	}
}

void object_particle::set_visible( bool bvisible )
{
	ASSERT(bvisible!=get_visible());
	super::set_visible(bvisible);

	update_particle_system( false );
}

void object_particle::set_transform			(float4x4 const& transform)
{
	super::set_transform			(transform);

	update_particle_system( false );
}

void object_particle::on_selected(bool selected)
{
	super::on_selected(selected);

	m_selected = selected;

	update_particle_system( false );
}

void object_particle::on_resource_loaded( xray::resources::queries_result& data )
{
	R_ASSERT							( data.is_successful() );
	
	
	*m_particle_system_instance_ptr = xray::static_cast_resource_ptr<xray::particle::particle_system_instance_ptr>( data[0].get_unmanaged_resource() );
	get_editor_renderer().scene().play_particle_system( *m_scene, *m_particle_system_instance_ptr, get_transform() );
	
	if(!m_collision->initialized())
		initialize_collision();
}

void object_particle::save( configs::lua_config_value t )
{
	super::save				( t );
	t["game_object_type"]	= "particle";
}


} //namespace editor
} //namespace xray
