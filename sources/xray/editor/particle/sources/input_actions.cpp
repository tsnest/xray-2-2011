////////////////////////////////////////////////////////////////////////////
//	Created		: 10.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_actions.h"
#include "particle_editor.h"
#include "particle_graph_document.h"

namespace xray {
namespace particle_editor {

mouse_action_editor_control::mouse_action_editor_control( System::String^ name, particle_editor^ pe, int button_id, bool plane_mode )
:action_continuous	( name ),
m_particle_editor	( pe ),
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
	collision::object const* picked_collision = NULL;

	editor_base::editor_control_base^ active_control = m_particle_editor->active_control;
	if(!active_control)
		return false;

	math::float3 picked_position;
	
	if( active_control && m_particle_editor->view_window->ray_query( active_control->acceptable_collision_type(), &picked_collision, &picked_position ) )
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

action_select_active_control::action_select_active_control ( System::String^ name, particle_editor^ pe, editor_base::editor_control_base^ c )
:super				( name ),
m_particle_editor	( pe ),
m_control			( c )
{
}

bool action_select_active_control::do_it()
{
	if(m_particle_editor->active_control)
		m_particle_editor->active_control->activate(false);

	m_particle_editor->active_control = m_control;

	if(m_particle_editor->active_control)
		m_particle_editor->active_control->activate(true);

	return true;
}

editor_base::checked_state action_select_active_control::checked_state( )
{
	return (m_particle_editor->active_control == m_control) ?
		editor_base::checked_state::checked	: 
		editor_base::checked_state::unchecked;
}



action_focus::action_focus( System::String^ name,  particle_editor^ pe)
:super				( name ),
m_particle_editor		( pe )
{
}

bool action_focus::enabled()
{
	return m_particle_editor->multidocument_base->active_document != nullptr;
}

bool action_focus::do_it() 
{
	float3 position =  (safe_cast<particle_graph_document^>(m_particle_editor->multidocument_base->active_document))->particle_system_root_matrix.c.xyz() ;
	float distance =	20.0f;

	float4x4 view_inv	= m_particle_editor->view_window->get_inverted_view_matrix();
	float4x4 view		= math::invert4x3( view_inv );


	float3 new_pos		= view_inv.k.xyz()*(-distance) + position;
	float4x4 translate	= create_translation( new_pos - view_inv.c.xyz() );

	m_particle_editor->view_window->set_view_point_distance( distance );
	m_particle_editor->view_window->set_flying_camera_destination( math::invert4x3( view_inv*translate ) );

	//m_particle_editor->view_window->action_focus_to_point( (safe_cast<particle_graph_document^>(m_particle_editor->multidocument_base->active_document))->particle_system_root_matrix.c.xyz() );

	return true;
}



} // namespace particle_editor
} // namespace xray
