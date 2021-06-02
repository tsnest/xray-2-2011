////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "actions.h"
#include "editor_world.h"
#include "project.h"
#include "object_base.h"
#include "command_drop_objects.h"
#include "level_editor.h"
#include "terrain_modifier_control.h"
#include "le_transform_control_helper.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/collision/collision_object.h>
#include <xray/render/facade/scene_view_mode.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/sound/world.h>
#include <xray/sound/world_user.h>
#pragma managed( pop )

using namespace System;
using namespace System::Windows::Forms;

namespace xray {	
namespace editor {

bool g_show_render_statistics	= false;

editor_world_action::editor_world_action(System::String^ name, editor_world* world)
:action_single		( name ),
m_editor_world		( world ) 
{
}

bool action_undo::do_it	() 
{
	m_editor_world->rollback( -1 );
	return true;
}

bool action_undo::enabled	()
{
	return !m_editor_world->undo_stack_empty( );
}

bool action_redo::do_it	() 
{
	m_editor_world->rollback( 1 );
	return true;
}

bool action_redo::enabled	()
{
	return !m_editor_world->redo_stack_empty( );
}

bool action_show_sound_editor::do_it	() 
{
	m_editor_world->show_sound_editor();
	return true;
}

bool action_show_animation_editor::do_it	() 
{
	m_editor_world->show_animation_editor();
	return true;
}

bool action_show_dialog_editor::do_it	() 
{
	m_editor_world->show_dialog_editor();
	return true;
}

bool action_show_particle_editor::do_it	() 
{
	m_editor_world->show_particle_editor();
	return true;
}

bool action_show_texture_editor::do_it	() 
{
	m_editor_world->show_texture_editor();
	return true;
}

bool action_show_material_editor::do_it	() 
{
	m_editor_world->show_material_editor();
	return true;
}

bool action_show_material_instance_editor::do_it	() 
{
	m_editor_world->show_material_instance_editor();
	return true;
}

bool action_show_model_editor::do_it	() 
{
	m_editor_world->show_model_editor();
	return true;
}

bool action_show_render_options_editor::do_it	() 
{
	m_editor_world->show_render_options_editor();
	return true;
}

bool action_pause::do_it	() 
{
	bool current	= m_editor_world->paused();
	m_editor_world->pause(!current);
	return true;
}

bool action_reload_shaders::do_it	() 
{
	m_editor_world->get_renderer().scene().reload_shaders();
	return true;
}

bool action_reload_modified_textures::do_it	() 
{
	m_editor_world->get_renderer().scene().reload_modified_textures();
	return true;
}

bool action_terrain_debug_mode::do_it	() 
{
	current_state = !current_state;
	m_editor_world->get_renderer().enable_terrain_debug_mode(current_state);
	return current_state;
}

editor_base::checked_state action_terrain_debug_mode::checked_state( )
{
	return current_state ? 
		editor_base::checked_state::checked : 
	editor_base::checked_state::unchecked;
}

bool action_show_render_statistics::do_it	() 
{
	current_state				= !current_state;
	g_show_render_statistics	= current_state;
	return						current_state;
}

editor_base::checked_state action_show_render_statistics::checked_state( )
{
	return current_state ? 
		editor_base::checked_state::checked : 
		editor_base::checked_state::unchecked;
}

editor_base::checked_state action_pause::checked_state()
{
	return m_editor_world->paused() ? 
		editor_base::checked_state::checked : 
		editor_base::checked_state::unchecked;
}

bool action_editor_mode::do_it	() 
{
	bool current = m_editor_world->editor_mode();
	m_editor_world->editor_mode(!current);
	return true;
}

editor_base::checked_state action_editor_mode::checked_state()
{
	return m_editor_world->editor_mode() ? 
		editor_base::checked_state::checked : 
		editor_base::checked_state::unchecked;
}

action_focus::action_focus( System::String^ name, level_editor^ le, bool grab_pivot_only)
:super				( name ),
m_level_editor		( le ) ,
m_halo_factor		( 1.f ),
m_grab_pivot_only	( grab_pivot_only )
{
}

bool action_focus::enabled()
{
	return true;
}

bool action_focus::do_it() 
{
	if(m_level_editor->selection_not_empty())
		do_focus_selection( );
	else
		do_focus_pick_collision( );

	return false;
}

void action_focus::do_focus_pick_collision( )
{
	float distance		= m_level_editor->focus_distance( );
	collision::object const* picked_collision = NULL;
	float3					picked_position;

	bool res = m_level_editor->view_window()->ray_query(editor_base::collision_object_type_dynamic, &picked_collision, &picked_position);


	if(res)
	{
		if(m_grab_pivot_only)
			m_level_editor->view_window()->action_focus_to_point( picked_position );
		else
			m_level_editor->focus_camera_to( picked_position, distance );
	}
}

void action_focus::do_focus_selection( )
{
	object_base_list^ list = m_level_editor->selected_objects();
	if( list->Count==0 )
		return;

	math::aabb view_aabb = create_aabb_min_max( float3( math::float_max, math::float_max, math::float_max ), float3( math::float_min, math::float_min, math::float_min ) );

	for each(object_base^ o in list)
		view_aabb.modify( o->get_focus_aabb( ) );

	if(m_grab_pivot_only)
	{
		m_level_editor->view_window()->action_focus_to_point( view_aabb.center() );
	}else
	{
		m_level_editor->view_window()->action_focus_to_bbox( view_aabb );
	}
}

action_select_editor_control::action_select_editor_control ( System::String^ name, level_editor^ le, System::String^ control_id )
:super				( name ),
m_custom_editor		( le ),
m_control_id		( control_id )
{
}

bool action_select_editor_control::do_it( ) 
{
	m_custom_editor->set_active_control(m_control_id);
	return				true;
}

editor_base::checked_state action_select_editor_control::checked_state()
{
	if(m_control_id==nullptr && m_custom_editor->get_active_control()==nullptr)
		return xray::editor_base::checked_state::checked;
	else
	if(m_custom_editor->get_active_control() && m_custom_editor->get_active_control()->name == m_control_id) 
		return xray::editor_base::checked_state::checked;
	else
		return xray::editor_base::checked_state::unchecked;
}

action_drop_objects::action_drop_objects ( System::String^ name, level_editor^ le )
:super			( name ),
m_level_editor	( le )
{
}

bool action_drop_objects::do_it	() 
{
	m_level_editor->get_command_engine()->run( gcnew command_drop_objects( m_level_editor ));
	return true;
}

bool action_drop_objects::enabled( )
{
	return m_level_editor->selection_not_empty();
}

action_sound_debug_stream_writer::action_sound_debug_stream_writer( System::String^ name, level_editor^ le )
:	super			( name ),
	m_level_editor	( le ),
	m_current_state	( false )
{

}

bool action_sound_debug_stream_writer::do_it( ) 
{
	m_current_state					= !m_current_state;
	
	editor_world& ew				= m_level_editor->get_editor_world();
	sound::world_user& user 		= ew.engine().get_sound_world().get_editor_world_user();
	if ( m_current_state )
		user.enable_debug_stream_writing	( ew.sound_scene() );
	else
		user.disable_debug_stream_writing	( ew.sound_scene() );
	
	return m_current_state;
}

editor_base::checked_state action_sound_debug_stream_writer::checked_state()
{
	return	m_current_state ? 
			editor_base::checked_state::checked : 
			editor_base::checked_state::unchecked;

}

action_sound_debug_stream_writer_dump::action_sound_debug_stream_writer_dump( System::String^ name, level_editor^ le )
:	super			( name ),
	m_level_editor	( le )
{

}

bool action_sound_debug_stream_writer_dump::do_it( ) 
{
	
	editor_world& ew				= m_level_editor->get_editor_world();
	sound::world_user& user 		= ew.engine().get_sound_world().get_editor_world_user();
	if ( ew.sound_scene().c_ptr() )
		user.dump_debug_stream_writing	( ew.sound_scene() );
	
	return true;
}

bool action_sound_debug_stream_writer_dump::enabled	( )
{
	editor_world& ew				= m_level_editor->get_editor_world();
	sound::world_user& user 		= ew.engine().get_sound_world().get_editor_world_user();

	if ( !ew.sound_scene().c_ptr() )
		return false;

	if ( user.is_debug_stream_writing_enabled( ew.sound_scene() ) )
		return true;
	else
		return false;
}

} // namespace editor
} // namespace xray
