////////////////////////////////////////////////////////////////////////////
//	Created		: 30.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_actions.h"
#include "model_editor.h"

#pragma managed( push, off )
#include <xray/collision/object.h>
#pragma managed( pop )

namespace xray{
namespace model_editor{

me_transform_control_helper::me_transform_control_helper( model_editor^ me ) :
	super			( me->scene().c_ptr(), me->scene_view().c_ptr(), me->render_output_window().c_ptr() ),
	m_model_editor	( me )
{
}

float4x4 me_transform_control_helper::get_inverted_view_matrix( )
{
	return m_model_editor->m_view_window->get_inverted_view_matrix();
}

float4x4 me_transform_control_helper::get_projection_matrix( )
{
	return m_model_editor->m_view_window->get_projection_matrix();
}

void me_transform_control_helper::get_mouse_ray( float3& origin, float3& direction )
{
	m_model_editor->m_view_window->get_mouse_ray( origin, direction );
}

mouse_action_editor_control::mouse_action_editor_control( System::String^ name, 
															model_editor^ me, 
															int button_id, 
															bool plane_mode )
:action_continuous	( name ),
m_model_editor		( me ),
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
	xray::collision::object const* picked_collision = NULL;

	editor_base::editor_control_base^ active_control = m_model_editor->get_active_control();
	if(!active_control)
		return false;

	math::float3 picked_position;
	
	if( active_control && m_model_editor->m_view_window->ray_query( active_control->acceptable_collision_type(), &picked_collision, &picked_position ) )
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

action_select_active_control::action_select_active_control ( System::String^ name, 
															model_editor^ me, 
															editor_base::editor_control_base^ c )
:super				( name ),
m_model_editor		( me ),
m_control			( c )
{
}

bool action_select_active_control::do_it()
{
	if(m_model_editor->get_active_control())
		m_model_editor->get_active_control()->activate(false);

	m_model_editor->set_active_control ( m_control );

	if(m_control)
		m_control->activate(true);

	return true;
}

editor_base::checked_state action_select_active_control::checked_state( )
{
	return (m_model_editor->get_active_control() == m_control) ?
		editor_base::checked_state::checked	: 
		editor_base::checked_state::unchecked;
}


} //namespace model_editor
} //namespace xray
