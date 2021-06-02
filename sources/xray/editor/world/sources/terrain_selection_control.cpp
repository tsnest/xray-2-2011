////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_selection_control.h"
#include "terrain_control_cursor.h"
#include "terrain_object.h"
#include "level_editor.h"
#include "tool_terrain.h"
#include "terrain_modifier_command.h"
#include "editor_world.h"
#include "window_ide.h"

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/collision/object.h>
#pragma managed( pop )

namespace xray{
namespace editor{

terrain_selection_control::terrain_selection_control(level_editor^ le, tool_terrain^ tool)
:super				( le, tool)
{
	m_control_id		= "terrain_selection_control";
}

terrain_selection_control::~terrain_selection_control()
{
}

void terrain_selection_control::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	m_processed_quad_id_list.Clear	( );
	super::start_input				( mode, button, coll, pos );
}

void terrain_selection_control::end_input( )
{
	enum_selection_method	method = enum_selection_method_set;
	
	if(m_level_editor->ide()->get_held_keys()->Contains(System::Windows::Forms::Keys::ControlKey))
		method = enum_selection_method_subtract;
	else
	if(m_level_editor->ide()->get_held_keys()->Contains(System::Windows::Forms::Keys::ShiftKey))
		method = enum_selection_method_add;

	m_level_editor->get_command_engine()->run( 
		gcnew terrain_selector_command(m_tool_terrain->get_terrain_core(), %m_processed_quad_id_list, method)
		);

	super::end_input				( );
}

void terrain_selection_control::do_work( )
{
	xray::render::debug_vertices_type	vertices(g_allocator);
	xray::render::debug_indices_type	indices(g_allocator); 
	
	math::float3 cp, cd;
	m_level_editor->view_window()->get_camera_props( cp, cd );
	key_vert_id_dict::Enumerator^ e		= m_processed_quad_id_list.GetEnumerator();
	while(e->MoveNext())
	{
		terrain_node^ curr_terrain		= m_tool_terrain->get_terrain_core()->m_nodes[e->Current.Key];
		vert_id_list^	vlist			= e->Current.Value;

		terrain_quad					quad;
		for each (u16 quad_idx in vlist)
		{
			if(curr_terrain->get_quad(quad, quad_idx))
				quad.debug_draw				( vertices, indices, 0xffffffff, cp );
		}
	}

	if(!vertices.empty())
		m_level_editor->get_editor_renderer().debug().draw_triangles( m_level_editor->get_editor_world().scene(), vertices, indices );
}

void terrain_selection_control::load_settings( RegistryKey^ key )
{
	super::load_settings		(key);
}

void terrain_selection_control::save_settings( RegistryKey^ key )
{
	super::save_settings		(key);
}

void terrain_selection_control::create_command( )
{
//.	m_apply_command					= gcnew terrain_selector_command(this, m_tool_terrain->get_terrain_core());
}

void terrain_selection_control::create_cursor( render::scene_ptr const& scene )
{
	m_terrain_cursor		= gcnew terrain_selector_cursor(this, scene);
}

void terrain_selection_control::execute_input()
{
	System::Drawing::Point screen_xy = m_level_editor->view_window()->get_mouse_position();

	if(screen_xy != m_last_screen_xy)
		refresh_active_working_set	(shape_type);

	m_last_screen_xy			= screen_xy;

	do_work							();
}

void terrain_selection_control::refresh_active_working_set(modifier_shape t)
{
	m_active_id_list.Clear				();

	collision::object const* picked_collision = NULL;
	
	if(m_level_editor->view_window()->ray_query( acceptable_collision_type(), &picked_collision, m_center_position_global ) )
	{
		ASSERT( picked_collision->get_type() & acceptable_collision_type() );
	
		m_tool_terrain->get_terrain_core()->select_quads(*m_center_position_global, radius, t, %m_active_id_list);

		for each (terrain_node_key key in m_active_id_list.Keys)
		{
			if(!m_processed_quad_id_list.ContainsKey(key))
				m_processed_quad_id_list.Add		(key, gcnew vert_id_list);

			vert_id_list^ curr_processed_quad_id_list = m_processed_quad_id_list[key];

			for each (u16 quad_idx in m_active_id_list[key])
			{
				if(	!curr_processed_quad_id_list->Contains(quad_idx) )
					curr_processed_quad_id_list->Add( quad_idx );
			}// verts
		}
	}

	on_working_set_changed				();
}

void terrain_selection_control::activate( bool b_activate )
{
	super::activate( b_activate );
	if(!b_activate)
		m_tool_terrain->get_terrain_core()->selected_quads()->Clear();

}

} //namespace editor
} //namespace xray
