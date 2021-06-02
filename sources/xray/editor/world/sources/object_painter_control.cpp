////////////////////////////////////////////////////////////////////////////
//	Created		: 10.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_painter_control.h"
#include "level_editor.h"
#include "object_solid_visual.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/collision/collision_object.h>
#pragma managed( pop )

namespace xray {
namespace editor {


object_painter_control::object_painter_control( level_editor^ le, object_painter_tool_tab^ tool_tab )
:super			( ),
m_level_editor	( le ),
m_tool_tab		( tool_tab )
{
	m_control_id = "object_painter";
}

object_painter_control::~object_painter_control( )
{}


void object_painter_control::initialize( )
{}

void object_painter_control::destroy( )
{}

void object_painter_control::activate( bool b_activate )
{
	XRAY_UNREFERENCED_PARAMETERS	( b_activate );
}

void object_painter_control::update( )
{}

void object_painter_control::start_input( int mode, int button, collision::object const* coll, xray::math::float3 const& pos )
{
	super::start_input	( mode, button, coll, pos );
}

void object_painter_control::execute_input( )
{}

void object_painter_control::end_input( )
{}

void object_painter_control::draw(render::scene_view_pointer scene, xray::render::render_output_window_pointer )
{
	XRAY_UNREFERENCED_PARAMETER	( scene );
}

u32 object_painter_control::acceptable_collision_type( )
{
	return editor_base::collision_object_type_dynamic;
}

void object_painter_control::load_settings( RegistryKey^ key )
{
	XRAY_UNREFERENCED_PARAMETERS	( key );
}

void object_painter_control::save_settings( RegistryKey^ key )
{
	XRAY_UNREFERENCED_PARAMETERS	( key );
}

// painted object
painted_object::painted_object( )
:m_vertices_count	( 0 ),
m_vertices			( NULL ),
m_paint_data		( NULL )
{
	m_render_model_ptr = NEW(render::render_model_instance_ptr)();
}

painted_object::~painted_object( )
{
	DELETE			( m_render_model_ptr );
}

void painted_object::setup( object_solid_visual^ src )
{
	reset				( );
	m_src_object		= src;

	unmanaged_string s( m_src_object->get_library_name() );
	query_result_delegate* q = NEW(query_result_delegate)(
			gcnew query_result_delegate::Delegate(this, &painted_object::on_model_loaded),
			g_allocator
			);

	resources::query_resource	(
		s.c_str(),
		resources::static_model_instance_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
		);

}

void painted_object::reset( )
{
	m_src_object		= nullptr;
	m_vertices_count	= 0;

	XRAY_FREE_IMPL( debug::g_mt_allocator, m_vertices );
	m_vertices			= NULL;
	XRAY_FREE_IMPL( debug::g_mt_allocator, m_paint_data );
	m_paint_data		= NULL;
}

bool painted_object::ready( )
{
	return m_vertices_count!=0;
}

void painted_object::on_model_loaded(  xray::resources::queries_result& data )
{
	R_ASSERT	( data.is_successful() );
}

float3 painted_object::get_position( )
{
	return m_src_object->get_position();
}

} // namespace editor
} // namespace xray

