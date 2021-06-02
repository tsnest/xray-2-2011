////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_scene_actions.h"
#include "sound_scene_document.h"
#include "sound_object_collision_dynamic.h"
#include <xray/collision/object.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/render/facade/editor_renderer.h>

using xray::sound_editor::action_select_active_control;
using xray::sound_editor::mouse_action_editor_control;
using xray::sound_editor::mouse_action_select_object;
using xray::sound_editor::start_all_sound_action;
using xray::sound_editor::stop_all_sound_action;
using xray::sound_editor::sound_scene_document;
using xray::sound_editor::enum_selection_method;
using namespace xray::editor_base;
//-------------------------------------------------------------------------------------------
//-- public ref class action_select_active_control ------------------------------------------
//-------------------------------------------------------------------------------------------
action_select_active_control::action_select_active_control(String^ name, sound_scene_document^ doc, editor_control_base^ c)
:super(name),
m_document(doc),
m_control(c)
{
}

bool action_select_active_control::do_it()
{
	if(m_document->active_control)
		m_document->active_control->activate(false);

	m_document->active_control = m_control;

	if(m_document->active_control)
		m_document->active_control->activate(true);

	return true;
}

checked_state action_select_active_control::checked_state( )
{
	return (m_document->active_control == m_control) ?
		checked_state::checked	: 
		checked_state::unchecked;
}
//-------------------------------------------------------------------------------------------
//-- public ref class mouse_action_editor_control -------------------------------------------
//-------------------------------------------------------------------------------------------
mouse_action_editor_control::mouse_action_editor_control(String^ name, sound_scene_document^ doc, int button_id, bool plane_mode)
:super(name),
m_document(doc),
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
	if(!m_document->viewport->Visible)
		return false;

	if(!m_document->viewport->is_mouse_in_view())
		return false;

	xray::collision::object const* picked_collision = NULL;
	editor_base::editor_control_base^ active_control = m_document->active_control;
	if(!active_control)
		return false;

	math::float3 picked_position;
	if(active_control && m_document->viewport->ray_query(active_control->acceptable_collision_type(), &picked_collision, &picked_position))
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
//-------------------------------------------------------------------------------------------
//-- public ref class mouse_action_select_object --------------------------------------------
//-------------------------------------------------------------------------------------------
bool xray::sound_editor::check_picked_object(xray::collision::space_partitioning_tree* collision_tree, xray::collision::object const* object, float3 const& cam_origin, float3 const& cam_direction)
{
	check_object_predicate obj;
	obj.m_result.object = object;
	xray::collision::ray_objects_type collision_results(g_allocator);
	float3 back_direction = -cam_direction;
	bool res1 = collision_tree->ray_query(
		xray::editor_base::collision_object_type_dynamic,
		cam_origin, 
		back_direction, 
		10000.f,  
		collision_results, 
		xray::collision::objects_predicate_type(&obj, &check_object_predicate::callback)	
	);
	return !res1;
}

mouse_action_select_object::mouse_action_select_object(String^ name, sound_scene_document^ doc, enum_selection_method method)
:super(name),
m_document(doc),
m_selected(nullptr),
m_select_method(method)
{
	m_start_xy = System::Drawing::Point(0,0);
}

bool mouse_action_select_object::capture()
{
	if(!m_document->viewport->is_mouse_in_view())
		return false;

	m_selected = nullptr;
	m_start_xy = m_document->viewport->get_mouse_position();
	collision::object const* object = NULL;
	if(m_document->viewport->ray_query(collision_object_type_dynamic, &object, NULL))
	{
		ASSERT(object->get_type() & collision_object_type_dynamic);
		sound_object_collision_dynamic const* c = static_cast_checked<sound_object_collision_dynamic const*>(object);
		m_selected = c->get_owner_object();
		float4x4 const& m = m_document->viewport->get_inverted_view_matrix();
		if(!check_picked_object(m_document->viewport->m_collision_tree, c, m.c.xyz(), m.k.xyz()))
			m_selected = nullptr;
	}

	return true;
}

bool mouse_action_select_object::execute()
{
	System::Drawing::Point screen_xy = m_document->viewport->get_mouse_position();
	System::Drawing::Size half_screen_size = m_document->viewport->get_view_size();
	half_screen_size.Width = half_screen_size.Width/2;
	half_screen_size.Height = half_screen_size.Height/2;
	screen_xy += half_screen_size;
	System::Drawing::Point start_xy = m_start_xy;
	start_xy += half_screen_size;
	m_document->get_renderer()->draw_screen_rectangle(
		m_document->scene(),
		math::float2((float)start_xy.X, (float)start_xy.Y),
		math::float2((float)screen_xy.X, (float)screen_xy.Y),
		math::color(128, 200, 128), 
		0xcccccccc
	);
	return true;
}

void mouse_action_select_object::release()
{
	System::Drawing::Point start_xy = m_document->viewport->get_mouse_position();
	if(math::abs(start_xy.X - m_start_xy.X)<2 && math::abs(start_xy.Y - m_start_xy.Y)<2)
	{
		m_document->clear_selection();
		m_document->select_object(m_selected);
		return;
	}
	
	System::Drawing::Point min, max;
	min.X = math::min( start_xy.X, m_start_xy.X);
	min.Y = math::min( start_xy.Y, m_start_xy.Y);
	max.X = math::max( start_xy.X, m_start_xy.X);
	max.Y = math::max( start_xy.Y, m_start_xy.Y);

	if(min.X==max.X)
		max.X+=1;

	if(min.Y==max.Y)
		max.Y+=1;

	collision_objects^ coll_objects = gcnew collision_objects;
	if(m_document->viewport->frustum_query(collision_object_type_dynamic, min, max, coll_objects))
	{
		List<sound_object_instance^>^ lst = gcnew List<sound_object_instance^>();
		u32 count = coll_objects->count();
		math::float4x4 const& m = m_document->viewport->get_inverted_view_matrix();
		for(u32 i=0 ;i<count; ++i)
		{
			collision::object const* co = coll_objects->get(i);
			if(!check_picked_object(m_document->viewport->m_collision_tree, co, m.c.xyz(), m.k.xyz()))
				continue;

			ASSERT(co->get_type() & editor_base::collision_object_type_dynamic);
			sound_object_collision_dynamic const* c = static_cast_checked<sound_object_collision_dynamic const*>(co);
			sound_object_instance^ inst = c->get_owner_object();
			if(!lst->Contains(inst))
				lst->Add(inst);
		}

		if(lst->Count)
			m_document->select_objects(lst);
	}
}
//-------------------------------------------------------------------------------------------
//-- public ref class start_all_sound_action ------------------------------------------------
//-------------------------------------------------------------------------------------------
start_all_sound_action::start_all_sound_action(String^ name, sound_scene_document^ doc)
:super(name),
m_document(doc)
{
}

bool start_all_sound_action::do_it()
{
	m_document->play_all();
	return true;
}

checked_state start_all_sound_action::checked_state( )
{
	return (m_document->all_playing()) ?
		checked_state::checked	: 
		checked_state::unchecked;
}
//-------------------------------------------------------------------------------------------
//-- public ref class stop_all_sound_action -------------------------------------------------
//-------------------------------------------------------------------------------------------
stop_all_sound_action::stop_all_sound_action(String^ name, sound_scene_document^ doc)
:super(name),
m_document(doc)
{
}

bool stop_all_sound_action::do_it()
{
	m_document->stop_all();
	return true;
}

checked_state stop_all_sound_action::checked_state( )
{
	return (m_document->all_stoped()) ?
		checked_state::checked	: 
		checked_state::unchecked;
}