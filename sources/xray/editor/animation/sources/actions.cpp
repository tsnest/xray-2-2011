////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "actions.h"
#include "animation_editor.h"
#include "animation_editor_form.h"
#include "viewer_editor.h"
#include <xray/collision/object.h>

using xray::animation_editor::action_select_active_control;
using xray::animation_editor::mouse_action_editor_control;
using xray::animation_editor::action_navigation_mode;
using xray::animation_editor::action_navigation_pause;
using xray::animation_editor::action_switch_saving_navigation_scene;
using xray::animation_editor::action_change_navigation_vector;
using xray::animation_editor::action_navigation_jump;
using xray::animation_editor::action_render_debug_info;
using xray::animation_editor::animation_editor;
////////////////////////////////////////////////////////////////////////////
//-- public ref class action_select_active_control ------------------------------------------
////////////////////////////////////////////////////////////////////////////
action_select_active_control::action_select_active_control(System::String^ name, animation_editor^ ae, xray::editor_base::editor_control_base^ c)
:super(name),
m_animation_editor(ae),
m_control(c)
{
}

bool action_select_active_control::do_it()
{
	if(m_animation_editor->is_in_navigation_mode())
		return false;

	if(m_animation_editor->active_control)
		m_animation_editor->active_control->activate(false);

	m_animation_editor->active_control = m_control;

	if(m_animation_editor->active_control)
		m_animation_editor->active_control->activate(true);

	return true;
}

xray::editor_base::checked_state action_select_active_control::checked_state( )
{
	return (m_animation_editor->active_control == m_control) ?
		editor_base::checked_state::checked	: 
		editor_base::checked_state::unchecked;
}
////////////////////////////////////////////////////////////////////////////
//-- public ref class mouse_action_editor_control -------------------------------------------
////////////////////////////////////////////////////////////////////////////
mouse_action_editor_control::mouse_action_editor_control(System::String^ name, animation_editor^ ae, int button_id, bool plane_mode)
:super(name),
m_animation_editor(ae),
m_active_handler(nullptr),
m_plane_mode(plane_mode),
m_button_id(button_id)
{}

bool mouse_action_editor_control::execute()
{
	if( m_active_handler )
		m_active_handler->execute_input();

	return true;
}

bool mouse_action_editor_control::capture()
{
	if(!m_animation_editor->viewport->Visible)
		return false;

	if(!m_animation_editor->viewport->is_mouse_in_view())
		return false;

	if(m_animation_editor->form->active_content!=m_animation_editor->viewer)
		return false;

	xray::collision::object const* picked_collision = NULL;
	editor_base::editor_control_base^ active_control = m_animation_editor->active_control;
	if(!active_control)
		return false;

	math::float3 picked_position;
	if(active_control && m_animation_editor->viewport->ray_query(active_control->acceptable_collision_type(), &picked_collision, &picked_position))
	{
		m_active_handler = active_control;
		m_active_handler->start_input((m_plane_mode)?1:0, m_button_id, picked_collision, picked_position);
		return true;
	}

	return false;
}

void mouse_action_editor_control::release() 
{
	if(m_active_handler)
	{
		m_active_handler->end_input();
		m_active_handler = nullptr;
	}
}
////////////////////////////////////////////////////////////////////////////
//-- public ref class action_navigation_mode ------------------------------------------------
////////////////////////////////////////////////////////////////////////////
action_navigation_mode::action_navigation_mode(System::String^ name, animation_editor^ ae)
:super(name),
m_animation_editor(ae)
{
}

bool action_navigation_mode::do_it()
{
	m_animation_editor->switch_navigation_mode();
	return true;
}

xray::editor_base::checked_state action_navigation_mode::checked_state( )
{
	return (m_animation_editor->is_in_navigation_mode()) ?
		editor_base::checked_state::checked	: 
		editor_base::checked_state::unchecked;
}
////////////////////////////////////////////////////////////////////////////
//-- public ref class action_navigation_pause ------------------------------------------------
////////////////////////////////////////////////////////////////////////////
action_navigation_pause::action_navigation_pause(System::String^ name, animation_editor^ ae)
:super(name),
m_animation_editor(ae)
{
}

bool action_navigation_pause::do_it()
{
	if(!m_animation_editor->is_in_navigation_mode())
		return false;

	m_animation_editor->pause_navigation();
	return true;
}

xray::editor_base::checked_state action_navigation_pause::checked_state( )
{
	return (m_animation_editor->is_in_navigation_mode() && m_animation_editor->is_navigation_paused()) ?
		editor_base::checked_state::checked	: 
		editor_base::checked_state::unchecked;
}
////////////////////////////////////////////////////////////////////////////
//-- public ref class action_switch_saving_navigation_scene ---------------------------------
////////////////////////////////////////////////////////////////////////////
action_switch_saving_navigation_scene::action_switch_saving_navigation_scene(System::String^ name, animation_editor^ ae)
:super(name),
m_animation_editor(ae)
{
}

bool action_switch_saving_navigation_scene::do_it()
{
	if(m_animation_editor->is_saving_controller_scene())
		m_animation_editor->stop_saving_controller_scene();
	else
		m_animation_editor->start_saving_controller_scene();

	return true;
}

xray::editor_base::checked_state action_switch_saving_navigation_scene::checked_state( )
{
	return (m_animation_editor->is_in_navigation_mode() && m_animation_editor->is_saving_controller_scene()) ?
		editor_base::checked_state::checked	: 
		editor_base::checked_state::unchecked;
}
////////////////////////////////////////////////////////////////////////////
//-- public ref class action_change_navigation_vector ---------------------------------------
////////////////////////////////////////////////////////////////////////////
action_change_navigation_vector::action_change_navigation_vector(System::String^ name, animation_editor^ ae, float delta_x, float delta_y, bool dir)
:super(name),
m_animation_editor(ae),
m_delta_x(delta_x),
m_delta_y(delta_y),
m_change_direction(dir)
{
}

bool action_change_navigation_vector::enabled()
{
	return m_animation_editor->is_in_navigation_mode();
}

bool action_change_navigation_vector::capture()
{
	return true;
}

bool action_change_navigation_vector::execute()
{
	float2 vec = float2(m_delta_x, m_delta_y);
	float2 res_vec;
	if(m_change_direction)
		res_vec = m_animation_editor->control_view_dir;
	else
		res_vec = m_animation_editor->control_path;

	float path_mag = res_vec.length();
	float ipath_mag = 1.f / path_mag;
	float2 dir = res_vec * ipath_mag;
	float2 cros_dir = float2().cross_product(dir);
	res_vec += cros_dir * vec.x;
	res_vec.normalize();
	res_vec *= path_mag;
	res_vec += dir * vec.y;
	const float max_path_length = 1.f;
	const float min_path_length = 0.1f;
	float mag = res_vec.length();
	float mag_clamped = math::clamp_r(mag, min_path_length, max_path_length);
	res_vec *= mag_clamped/mag;
	if(m_change_direction)
		m_animation_editor->control_view_dir = res_vec;
	else
		m_animation_editor->control_path = res_vec;

	m_animation_editor->move_controller();
	return true;
}

void action_change_navigation_vector::release()
{
}
////////////////////////////////////////////////////////////////////////////
//-- public ref class action_navigation_jump ------------------------------------------------
////////////////////////////////////////////////////////////////////////////
action_navigation_jump::action_navigation_jump(System::String^ name, animation_editor^ ae)
:super(name),
m_animation_editor(ae)
{
}

bool action_navigation_jump::do_it()
{
	m_animation_editor->control_jump = true;
	m_animation_editor->move_controller();
	m_animation_editor->control_jump = false;
	return true;
}

xray::editor_base::checked_state action_navigation_jump::checked_state( )
{
	return (m_animation_editor->is_in_navigation_mode()) ?
		editor_base::checked_state::checked	: 
		editor_base::checked_state::unchecked;
}
////////////////////////////////////////////////////////////////////////////
//-- public ref class action_render_debug_info ------------------------------------------------
////////////////////////////////////////////////////////////////////////////
action_render_debug_info::action_render_debug_info(System::String^ name, animation_editor^ ae)
:super(name),
m_animation_editor(ae)
{
}

bool action_render_debug_info::do_it()
{
	if(!m_animation_editor->is_in_navigation_mode())
		return false;

	m_animation_editor->render_debug_info = !m_animation_editor->render_debug_info;
	return true;
}

xray::editor_base::checked_state action_render_debug_info::checked_state( )
{
	return (m_animation_editor->is_in_navigation_mode() && m_animation_editor->render_debug_info) ?
		editor_base::checked_state::checked	: 
		editor_base::checked_state::unchecked;
}