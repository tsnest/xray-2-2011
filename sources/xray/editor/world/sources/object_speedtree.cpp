////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_speedtree.h"
#include "tool_speedtree.h"
#include "project_items.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/collision/collision_object.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

object_speedtree::object_speedtree( tool_speedtree^ t, render::scene_ptr const& scene )
	:super			( t ), 
	m_tool_speedtree( t ),
	m_scene			( NEW(render::scene_ptr) (scene) ),
	m_selected		( false ),
	m_model_query	( NULL )
{
	image_index					= xray::editor_base::node_speedtree;
	m_speedtree_instance_ptr	= NEW(xray::render::speedtree_instance_ptr)();
}

object_speedtree::~object_speedtree( )
{
	destroy_collision	( ) ;

	R_ASSERT			( !get_visible() );
	
	DELETE			( m_speedtree_instance_ptr );
	DELETE			( m_scene );

	if(m_model_query)
		m_model_query->m_rejected = true;
}

void object_speedtree::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( &debug::g_mt_allocator );
}

void object_speedtree::initialize_collision( )
{
	ASSERT( !m_collision->initialized() );
	float3 extents					 (1.0f,1.0f,1.0f);
	
	collision::geometry_instance* geom	= &*collision::new_box_geometry_instance( &debug::g_mt_allocator, math::create_scale(extents) );
	m_collision->create_from_geometry(
		true,
		this,
		geom,
		editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch
		);
	m_collision->insert				 ( m_transform );
}

void object_speedtree::set_library_name( System::String^ name )
{
//	super::set_library_name		( name );

	unload_contents		( true );

	m_lib_name			= check_valid_library_name( name);

	load_contents		( );
}

void object_speedtree::load_contents( )
{
	if(m_needed_quality_)
		return;

	m_needed_quality_		= true;
	
	if( (*m_speedtree_instance_ptr).c_ptr() )
		return;

	unmanaged_string s(m_lib_name);
	m_model_query = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &object_speedtree::on_resource_loaded), g_allocator);
	
	resources::query_resource	(
		s.c_str(),
		resources::speedtree_instance_class,
		boost::bind(&query_result_delegate::callback, m_model_query, _1),
		g_allocator
	);
}

void object_speedtree::unload_contents( bool )
{
	if( !(*m_speedtree_instance_ptr).c_ptr() )
		return;

	if(!m_needed_quality_)
		return;

	if(get_visible())
		get_editor_renderer().scene().remove_speedtree_instance(*m_scene, *m_speedtree_instance_ptr, true);

	destroy_collision		( );

	(*m_speedtree_instance_ptr)		= 0;
	m_needed_quality_				= false;
}

void object_speedtree::set_visible(bool bvisible)
{
	ASSERT(bvisible!=get_visible());

	if((*m_speedtree_instance_ptr).c_ptr())
	{
		if(bvisible)
		{
			get_editor_renderer().scene().add_speedtree_instance( *m_scene, *m_speedtree_instance_ptr, *m_transform, true );
			get_editor_renderer().select_speedtree_instance		( *m_scene, *m_speedtree_instance_ptr, m_project_item->get_selected() );
		}else
		{
			get_editor_renderer().select_speedtree_instance( *m_scene, *m_speedtree_instance_ptr, false );
			get_editor_renderer().scene().remove_speedtree_instance( *m_scene, *m_speedtree_instance_ptr, true );
		}
	}

	super::set_visible	( bvisible );
}

void object_speedtree::update_speedtree_instance()
{
	if (m_speedtree_instance_ptr->c_ptr())
	{
		get_editor_renderer().select_speedtree_instance( *m_scene, *m_speedtree_instance_ptr, m_selected);	
		get_editor_renderer().scene().update_speedtree_instance(*m_scene, *m_speedtree_instance_ptr, get_transform(), true);
	}
}

void object_speedtree::set_transform(float4x4 const& transform)
{
	super::set_transform						(transform);
	
	update_speedtree_instance					();
}

void object_speedtree::on_selected				(bool selected)
{
	super::on_selected							(selected);
	m_selected									= selected;
	
	update_speedtree_instance					();
}

void object_speedtree::on_resource_loaded( xray::resources::queries_result& data )
{
	m_model_query = NULL;
	if (data[0].is_successful())
	{
		*m_speedtree_instance_ptr = static_cast_checked<xray::render::speedtree_instance*>( data[0].get_unmanaged_resource().c_ptr() );
		get_editor_renderer().scene().add_speedtree_instance( *m_scene, *m_speedtree_instance_ptr, get_transform(), true );
		
		if(!m_collision->initialized())
			initialize_collision();		
	}
}

void object_speedtree::save( configs::lua_config_value t )
{
	super::save				( t );
	t["game_object_type"]	= "speedtree";
}


} //namespace editor
} //namespace xray