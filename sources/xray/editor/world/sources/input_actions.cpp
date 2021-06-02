////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_actions.h"
#include "editor_world.h"
#include "project.h"
#include "collision_object_dynamic.h"
#include <xray/editor/base/collision_object_types.h>
#include "level_editor.h"
#include "terrain_modifier_control.h"
#include "tool_base.h"

// commands
#include "command_select.h"
#include "command_delete_object.h"
#include "command_snap_objects.h"
#include "command_place_objects.h"

#pragma managed( push, off )
#include <xray/collision/space_partitioning_tree.h>
#pragma managed( pop )

namespace xray {
namespace editor {

mouse_action_editor_control::mouse_action_editor_control( System::String^ name, level_editor^ le, int button_id, bool plane_mode )
:super				( name ),
m_level_editor		( le ),
m_active_handler	( nullptr ),
m_plane_mode		( plane_mode ),
m_button_id			( button_id )
{}

bool mouse_action_editor_control::execute(  )
{
	if( m_active_handler )
		m_active_handler->execute_input();

	return true;
}

bool mouse_action_editor_control::capture( )
{
	if( !m_level_editor->view_window()->is_mouse_in_view() )
		return false;

	collision::object const* picked_collision = NULL;

	editor_base::editor_control_base^ active_control = m_level_editor->get_active_control();
	if(!active_control)
		return false;

	math::float3 picked_position;
	
	if( active_control && m_level_editor->view_window()->ray_query( active_control->acceptable_collision_type(), &picked_collision, &picked_position ) )
	{
		m_active_handler					= active_control;
		m_active_handler->start_input		( (m_plane_mode)?1:0, m_button_id, picked_collision, picked_position );
		return								true;
	}

	return									false;
}

void mouse_action_editor_control::release( ) 
{
	if( m_active_handler )
	{
		m_active_handler->end_input	( );
		m_active_handler			= nullptr;
	}
}




struct check_object_predicate
{
	check_object_predicate():m_result(NULL, 0.0f){}
	bool callback ( collision::ray_object_result const& r )
	{
		if(r.object == m_result.object)
		{
			m_result = r;
			return true;
		}else
			return false;
	}

	collision::ray_object_result	m_result;
};

bool check_picked_object(	collision::space_partitioning_tree* collision_tree, 
							collision::object const* object, 
							float3 const& cam_origin, 
							float3 const& cam_direction )
{
	check_object_predicate		obj;
	obj.m_result.object			= object;

	collision::ray_objects_type	collision_results( g_allocator );
	
	float3 back_direction			= -cam_direction;
	
	bool res1 = collision_tree->ray_query(	editor_base::collision_object_type_dynamic,
									cam_origin, 
									back_direction, 
									10000.f,  
									collision_results, 
									collision::objects_predicate_type(&obj, &check_object_predicate::callback)	);
	return	!res1;
}

static mouse_action_select_object::mouse_action_select_object ( )
{
	s_is_enabled = true;
}

mouse_action_select_object::mouse_action_select_object( System::String^ name, level_editor^ le, enum_selection_method method )
:super				( name ),
m_level_editor		( le ),
m_selected			( nullptr ),
m_select_method		( method )
{
	m_start_xy		= System::Drawing::Point(0,0);
}

bool mouse_action_select_object::capture( )
{
	if(m_level_editor->get_project()->get_pin_selection())
		return false;

	if( !m_level_editor->view_window()->is_mouse_in_view() )
		return false;

	editor_base::editor_control_base^ c = m_level_editor->get_active_control();
	
	if(c && dynamic_cast<terrain_control_base^>(c))
		return false;// bad solution

	m_selected							= nullptr;

	m_start_xy				= m_level_editor->view_window()->get_mouse_position();

	collision::object const* object		= NULL;
	if( m_level_editor->view_window()->ray_query( editor_base::collision_object_type_dynamic, &object, NULL ) )
	{
		ASSERT( object->get_type() & editor_base::collision_object_type_dynamic );

		collision_object_dynamic const* c = static_cast_checked<collision_object_dynamic const*>(object);

		if(c->get_owner_object()->get_selectable())
		{
			m_selected = c->get_owner_object();

			math::float4x4 const& m = m_level_editor->view_window()->get_inverted_view_matrix();
			if(!check_picked_object(m_level_editor->view_window()->m_collision_tree,
									c,
									m.c.xyz(),
									m.k.xyz()))
			m_selected = nullptr;
		}
	}


	return true;
}

bool mouse_action_select_object::execute(  )
{
	System::Drawing::Point screen_xy = m_level_editor->view_window()->get_mouse_position();

	System::Drawing::Size	half_screen_size = m_level_editor->view_window()->get_view_size();
	half_screen_size.Width	= half_screen_size.Width/2;
	half_screen_size.Height	= half_screen_size.Height/2;
	screen_xy				+= half_screen_size;

	System::Drawing::Point start_xy		= m_start_xy;
	start_xy							+= half_screen_size;

	m_level_editor->get_editor_renderer().draw_screen_rectangle(
		m_level_editor->scene(), 
		math::float2( (float)start_xy.X, (float)start_xy.Y ), 
		math::float2( (float)screen_xy.X, (float)screen_xy.Y ), 
		math::color( 128, 200, 128 ),
		0xcccccccc
	);
	return true;
}


void mouse_action_select_object::release( )
{
	enum_selection_method select_method = m_select_method;

	System::Drawing::Point start_xy = m_level_editor->view_window()->get_mouse_position();

	if( math::abs( start_xy.X - m_start_xy.X ) < 2 &&  math::abs( start_xy.Y - m_start_xy.Y ) < 2 )
	{
		if( m_selected )
			m_level_editor->get_command_engine()->run( gcnew command_select(m_level_editor->get_project(), m_selected->id(), select_method) );
		else
		if( select_method == enum_selection_method_set )
			m_level_editor->get_command_engine()->run( gcnew command_select(m_level_editor->get_project()));
			
		return;
	}
	
	System::Drawing::Point min, max;
	min.X = math::min( start_xy.X, m_start_xy.X);
	min.Y = math::min( start_xy.Y, m_start_xy.Y);

	max.X = math::max( start_xy.X, m_start_xy.X);
	max.Y = math::max( start_xy.Y, m_start_xy.Y);

	if( min.X == max.X )
		max.X+=1;

	if( min.Y == max.Y )
		max.Y+=1;

	editor_base::collision_objects^  coll_objects = gcnew editor_base::collision_objects;

	if( m_level_editor->view_window()->frustum_query( editor_base::collision_object_type_dynamic, min, max, coll_objects ))
	{
		item_id_list^ idlist = gcnew item_id_list;

		u32 count = coll_objects->count();
		math::float4x4 const& m = m_level_editor->view_window()->get_inverted_view_matrix();

		for(u32 i=0 ;i<count; ++i )
		{
			collision::object const* co = coll_objects->get(i);
			if(!check_picked_object(m_level_editor->view_window()->m_collision_tree,
									co,
									m.c.xyz(),
									m.k.xyz()))
				continue;

			ASSERT( co->get_type() & editor_base::collision_object_type_dynamic );

			collision_object_dynamic const* collision_dynamic = static_cast_checked< collision_object_dynamic const* >(co);
			u32 object_id = collision_dynamic->get_owner_object()->id();
			if(!idlist->Contains(object_id))
				idlist->Add( object_id );
		}
		if(idlist->Count)
			m_level_editor->get_command_engine()->run( gcnew command_select(m_level_editor->get_project(), idlist, select_method) );
	}else
		m_level_editor->get_command_engine()->run( gcnew command_select(m_level_editor->get_project()));
}

bool mouse_action_select_object::enabled ( )
{
	return s_is_enabled;
}

input_action_add_library_object::input_action_add_library_object( System::String^ name, level_editor^ le )
:super			( name ),
m_level_editor	( le )
{
}

bool input_action_add_library_object::enabled( )
{
	return  !m_level_editor->get_project()->empty();
}

bool input_action_add_library_object::capture( )
{
	if( !m_level_editor->view_window()->is_mouse_in_view() )
		return false;

	// If the ray doesn't touch the ground then just place the object on the 0 plane.
	float3	origin;
	float3	direction;
	float3	picked_position;

	m_level_editor->view_window()->get_mouse_ray	( origin, direction );

	collision::object const* picked_collision = NULL;

	bool bpick = m_level_editor->view_window()->ray_query(	editor_base::collision_object_type_dynamic, 
															origin, 
															direction, 
															&picked_collision, 
															&picked_position );

	if( !bpick )
		bpick = xray::editor_base::transform_control_base::plane_ray_intersect(float3( 0, 0, 0 ),	
															float3( 0, 1, 0 ),
															origin,						
															direction,
															picked_position);
	if( bpick )
	{
		tool_base^ tool							= m_level_editor->get_active_tool();
		if(tool)
		{
			System::String^ library_name		= tool->get_selected_library_name();
			if(!System::String::IsNullOrEmpty(library_name))
				m_level_editor->add_library_object	( picked_position, tool, library_name );
		}
	}else
	{
		// Here need to rise an error message ....
	}

	return true;
}

bool input_action_add_library_object::execute( )
{
	return true;
}

//key_action_snap::key_action_snap( System::String^ name, level_editor^ le)
//:super				( name ),
//m_level_editor		( le ),
//m_snap_command		( nullptr )
//{
//}
//
//bool key_action_snap::capture( )
//{
//	m_snap_command = gcnew command_snap_objects(m_level_editor);
//	m_level_editor->get_command_engine()->run_preview( m_snap_command );
//
//	return true;
//}
//
//bool key_action_snap::execute(  )
//{
//	ASSERT( m_snap_command );
//	m_level_editor->get_command_engine()->preview();
//	return true;
//}
//
//void key_action_snap::release( ) 
//{
//	ASSERT( m_snap_command );
//	m_level_editor->get_command_engine()->end_preview();
//
//	m_snap_command = nullptr;
//}
//
//key_action_place::key_action_place( System::String^ name, level_editor^ le)
//:super				( name ),
//m_level_editor		( le ),
//m_snap_command		( nullptr )
//{
//}
//
//bool key_action_place::capture( )
//{
//	m_snap_command = gcnew command_place_objects(m_level_editor);
//	m_level_editor->get_command_engine()->run_preview( m_snap_command );
//
//	return true;
//}
//
//bool key_action_place::execute(  )
//{
//	ASSERT( m_snap_command );
//	m_level_editor->get_command_engine()->preview();
//	return true;
//}
//
//void key_action_place::release( ) 
//{
//
//	ASSERT( m_snap_command );
//	m_level_editor->get_command_engine()->end_preview();
//
//	m_snap_command = nullptr;
//}

mouse_action_control_properties::mouse_action_control_properties(System::String^ action_name, level_editor^ le, System::String^ prop_name, float scale)
:super			( action_name ),
m_prop_name		( prop_name ),
m_level_editor	( le ),
m_scale			( scale )
{}

bool mouse_action_control_properties::enabled( )
{
	if(m_level_editor->get_active_control() == nullptr)
		return false;

	return (m_level_editor->get_active_control()->has_properties() );
}

bool mouse_action_control_properties::capture( )
{
	while ( ShowCursor( FALSE ) >= 0 );

	m_mouse_capture_pos = m_level_editor->view_window()->get_mouse_position();
	m_level_editor->get_active_control()->pin(true);
	return true;
}

void mouse_action_control_properties::release( )
{
	while ( ShowCursor( TRUE ) <= 0 );

	m_level_editor->get_active_control()->pin(false);
}

bool mouse_action_control_properties::execute( )
{
	System::Drawing::Point	mouse_position = m_level_editor->view_window()->get_mouse_position();

	if(mouse_position != m_mouse_capture_pos)
	{
		float dx		= (float)(mouse_position.X-m_mouse_capture_pos.X);
		float dy		= -(float)(mouse_position.Y-m_mouse_capture_pos.Y);

		m_level_editor->get_active_control()->change_property	( m_prop_name, dx*m_scale, dy*m_scale );
		m_level_editor->view_window()->set_mouse_position		( m_mouse_capture_pos );
	}
	return true;
}

action_control_properties::action_control_properties( System::String^ action_name, level_editor^ le, System::String^ prop_name )
:super			( action_name ),
m_prop_name		( prop_name ),
m_level_editor	( le )
{}

bool action_control_properties::enabled( )
{
	if(m_level_editor->get_active_control() == nullptr)
		return false;

	return (m_level_editor->get_active_control()->has_properties() );
}

bool action_control_properties::do_it( )
{
	m_level_editor->get_active_control()->change_property( m_prop_name, 0.0f, 0.0f );
	return true;
}

action_control_step_modify::action_control_step_modify( System::String^ action_name, 
														level_editor^ le, 
														xray::editor_base::enum_transform_axis axis, 
														float value )
:super			( action_name ),
m_value			( value ),
m_axis			( axis ),
m_level_editor	( le )
{}

bool action_control_step_modify::enabled( )
{
	if(m_level_editor->get_active_control() == nullptr)
		return false;

	return (dynamic_cast<xray::editor_base::transform_control_base^>(m_level_editor->get_active_control()) != nullptr );
}

bool action_control_step_modify::do_it( )
{
	xray::editor_base::transform_control_base^ control = dynamic_cast<xray::editor_base::transform_control_base^>(m_level_editor->get_active_control());

	control->one_step_execute( m_axis, m_value );
	return					true;
}

} // namespace editor
} // namespace xray
