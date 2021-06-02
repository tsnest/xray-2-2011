////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_actions.h"
#include "transform_control_translation.h"
#include "scene_view_panel.h"

namespace xray{
namespace editor_base{

mouse_action_editor_control_snap::mouse_action_editor_control_snap( System::String^ name, 
																   active_control_getter^ getter, 
																   snap_mode snap_mode,
																   scene_view_panel^ p)
:super					( name ),
m_active_control_getter	( getter ),
m_active_handler		( nullptr ),
m_snap_mode				( snap_mode ),
m_scene_view_panel		( p )
{}

bool mouse_action_editor_control_snap::execute(  )
{
	if( m_active_handler )
		m_active_handler->execute_input();

	return true;
}

bool mouse_action_editor_control_snap::capture( )
{
	collision::object const* picked_collision = NULL;

	editor_base::editor_control_base^ active_control = m_active_control_getter();
	if(!active_control || (active_control->GetType() != editor_base::transform_control_translation::typeid) )
		return false;

	editor_base::transform_control_translation^ c = safe_cast<editor_base::transform_control_translation^>(active_control);

	if(c->helper()->m_object==nullptr)
		return false;

	math::float3 picked_position;
	
	if(m_snap_mode==snap_mode::vertex_snap ||m_snap_mode==snap_mode::face_snap)
	{
		m_active_handler					= safe_cast<editor_base::transform_control_translation^>(active_control);
		m_active_handler->set_snap_mode		( m_snap_mode );
		m_active_handler->start_input		( 0, 0, picked_collision, picked_position );
		return								true;
	}else
	{
		if( m_scene_view_panel->ray_query( active_control->acceptable_collision_type(), &picked_collision, &picked_position ) )
		{
			m_active_handler					= safe_cast<editor_base::transform_control_translation^>(active_control);
			m_active_handler->set_snap_mode		( m_snap_mode );
			m_active_handler->start_input		( 0, 0, picked_collision, picked_position );
			return								true;
		}
		
		return false;
	}
}

void mouse_action_editor_control_snap::release( ) 
{
	if( m_active_handler )
	{
		m_active_handler->end_input	( );
		m_active_handler			= nullptr;
	}
}

} //namespace editor_base
} //namespace xray