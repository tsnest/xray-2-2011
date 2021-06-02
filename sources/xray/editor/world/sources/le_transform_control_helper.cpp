////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "le_transform_control_helper.h"
#include "level_editor.h"
#include "tool_base.h"
#include "object_track.h"
#include "object_composite.h"
#include "command_apply_control_transform.h"
#include "project_items.h"

using xray::editor_base::transform_control_translation;
using xray::editor_base::transform_control_rotation;
using xray::editor_base::transform_control_scaling;

namespace xray {
namespace editor {

le_helper::le_helper( level_editor^ le ) :
	editor_base::transform_control_helper	( le->scene().c_ptr(), le->scene_view().c_ptr(), le->render_output_window().c_ptr() ),
	m_level_editor							( le )
{
}

float4x4 le_helper::get_inverted_view_matrix( )
{
	return m_level_editor->view_window()->get_inverted_view_matrix();
}

float4x4 le_helper::get_projection_matrix( )
{
	return m_level_editor->view_window()->get_projection_matrix();
}

void le_helper::get_mouse_ray( float3& origin, float3& direction)
{
	return m_level_editor->view_window()->get_mouse_ray( origin, direction );
}

object_transform_data::object_transform_data( level_editor^ le )
:m_level_editor	( le ),
m_apply_command	( nullptr )
{}

object_transform_data::~object_transform_data()
{
}

bool object_transform_data::attach( transform_control_base^ transform_control )
{
	object_base_list^ list = m_level_editor->selected_objects( );
	for each( object_base^ obj in list )
	{
		if( obj->attach_transform( transform_control ) == false )
			return false;
	}
	return true;
}

void object_transform_data::detach( transform_control_base^ )
{
}

void object_transform_data::start_modify( transform_control_base^ c )
{
	R_ASSERT	( m_apply_command == nullptr );

	object_base_list^ selection = m_level_editor->selected_objects();

	if(current_mode()==super::mode::edit_transform)
		m_apply_command = gcnew command_apply_object_transform( c, selection );
	else
		m_apply_command = gcnew command_apply_object_pivot( c, selection );

	m_level_editor->get_command_engine()->run_preview( m_apply_command );
}

void object_transform_data::execute_preview( transform_control_base^ )
{
	R_ASSERT		( m_apply_command != nullptr );
	m_level_editor->get_command_engine()->preview( );
}

void object_transform_data::end_modify( transform_control_base^ )
{
	R_ASSERT		( m_apply_command != nullptr );
	m_level_editor->get_command_engine()->end_preview();
	m_apply_command = nullptr;
}

math::float4x4 object_transform_data::get_ancor_transform( )
{
	object_base_list^ sel		= m_level_editor->selected_objects();
	object_base^ pivot_object	= (sel->Count!=0) ? sel[0] : nullptr;

	if(pivot_object)
	{
		return create_translation(pivot_object->get_pivot()) * pivot_object->get_transform();
	}else
		return math::float4x4().identity();
}

u32 object_transform_data::get_collision( collision_objects_list^% r )
{
	object_base_list^ sel			= m_level_editor->selected_objects();
	for each( object_base^ o in sel )
	{
		object_composite^ oc = dynamic_cast<object_composite^>(o);
		if(oc)
		{
			for each (composite_object_item^ itm in oc->m_objects)
			{
				editor_base::collision_object_wrapper w;
				w.m_collision_object	= itm->m_object->m_collision->get_collision_object();
				r->Add( w );
			}
		}else
		{
			editor_base::collision_object_wrapper w;
			w.m_collision_object	= o->m_collision->get_collision_object();
			r->Add( w );
		}
	}
	return r->Count;
}

//------------folder transform
folder_transform_data::folder_transform_data( level_editor^ le, project_item_folder^ f )
:super		( le ),
m_folder	( f )
{
	if(m_folder->m_pivot==NULL)
	{
		if(m_folder->m_objects.Count)
			m_folder->m_pivot = NEW(float3)(m_folder->m_objects[0]->get_object()->get_position());
		else
			m_folder->m_pivot = NEW(float3)(0,0,0);
	}
}
	
void folder_transform_data::start_modify( editor_base::transform_control_base^ c )
{
	R_ASSERT	( m_apply_command == nullptr );

	object_base_list^ selection = gcnew object_base_list;
	project_items_list		items_in_group;
	m_folder->get_all_items	( %items_in_group, true );

	for each(project_item_base^ itm in items_in_group)
		if(itm->get_object())
			selection->Add(itm->get_object());

	if(current_mode()==super::mode::edit_transform)
		m_apply_command = gcnew command_apply_folder_transform( c, selection, m_folder );
	else
		m_apply_command = gcnew command_apply_folder_pivot( c, m_folder );

	m_level_editor->get_command_engine()->run_preview( m_apply_command );
}

math::float4x4 folder_transform_data::get_ancor_transform( )
{
	return create_translation( *m_folder->m_pivot );
}

u32 folder_transform_data::get_collision( collision_objects_list^% r)
{
	project_items_list		objects_in_folder;
	m_folder->get_all_items	(%objects_in_folder, true);

	for each (project_item_base^ pi in objects_in_folder )
	{
		object_base^ o = pi->get_object();
		if(!o)
			continue;

		editor_base::collision_object_wrapper w;
		w.m_collision_object	= o->m_collision->get_collision_object();

		r->Add					( w );
	}

	return r->Count;
}

////////////////////////////////////////// track_object_key_transform_data  //////////////////////
track_object_key_transform_data::track_object_key_transform_data( object_track^ o, u32 key_index )
:m_object_path	( o ),
m_key_index		( key_index )
{
	m_start_transform = CRT_NEW(math::float4x4)();
	m_start_transform->identity();
}

track_object_key_transform_data::~track_object_key_transform_data( )
{
	CRT_DELETE ( m_start_transform );
}

track_object_key_transform_data::!track_object_key_transform_data( )
{
	this->~track_object_key_transform_data();
}
bool			track_object_key_transform_data::attach						( transform_control_base^ transform )
{
	if( xray::editor::wpf_controls::dot_net_helpers::is_type<transform_control_translation^>( transform ) ||
		xray::editor::wpf_controls::dot_net_helpers::is_type<transform_control_rotation^>( transform ) )
		return true;

	return false;
}

void track_object_key_transform_data::start_modify( transform_control_base^ control )
{
	XRAY_UNREFERENCED_PARAMETERS	( control );
	float4x4 const current = m_object_path->get_key_transform(m_key_index);
	(*m_start_transform) = current * m_object_path->get_transform();
}

void track_object_key_transform_data::execute_preview( transform_control_base^ control )
{
	float4x4 const key_current_transform	= control->calculate( *m_start_transform );
	float4x4 const& pt						= m_object_path->get_transform( );
	float4x4 ti = math::invert4x3( pt );

	float4x4 const trelative				= key_current_transform * ti;
	m_object_path->set_key_transform		( m_key_index, trelative );
}

void track_object_key_transform_data::end_modify( transform_control_base^ control )
{
	XRAY_UNREFERENCED_PARAMETERS	( control );
}

math::float4x4 track_object_key_transform_data::get_ancor_transform( )
{
	math::float4x4 ret_t = create_translation( m_object_path->get_key_transform( m_key_index ).c.xyz( ) ) * m_object_path->get_transform( );
	return ret_t;
}

u32 track_object_key_transform_data::get_collision( collision_objects_list^% r)
{
	editor_base::collision_object_wrapper w;
	w.m_collision_object			= (*m_object_path->m_keys_collision)[m_key_index];
	r->Add							( w );
	
	return r->Count;
}

////////////////////////////////////////// track_object_key_tangent_transform_data  /////
track_object_key_tangent_transform_data::track_object_key_tangent_transform_data	( object_track^ o, u32 key_index, int tangent_idx  )
:m_object_path	( o ),
m_key_index		( key_index ),
m_tangent_index	( tangent_idx )
{
	m_start_transform = CRT_NEW(math::float4x4)();
	m_start_transform->identity();
}

track_object_key_tangent_transform_data::~track_object_key_tangent_transform_data	( )
{
	CRT_DELETE ( m_start_transform );
}

void track_object_key_tangent_transform_data::!track_object_key_tangent_transform_data	( )
{
	this->~track_object_key_tangent_transform_data	( );
}

bool			track_object_key_tangent_transform_data::attach						( transform_control_base^ transform )
{
	if( xray::editor::wpf_controls::dot_net_helpers::is_type<transform_control_translation^>( transform ) )
		return true;

	return false;
}

void			track_object_key_tangent_transform_data::start_modify							( transform_control_base^ control )
{
	XRAY_UNREFERENCED_PARAMETERS	( control );
	(*m_start_transform) = math::create_translation( m_object_path->get_key_tangent_position( m_key_index, m_tangent_index ) ) * m_object_path->get_transform( );
	//R_ASSERT	( m_apply_command == nullptr );

	//m_apply_command = gcnew command_apply_control_transform( m_level_editor, c );
	//m_level_editor->get_command_engine()->run_preview( m_apply_command );
}

void			track_object_key_tangent_transform_data::execute_preview			( transform_control_base^ control )
{
	//R_ASSERT		( m_apply_command != nullptr );
	//m_level_editor->get_command_engine()->preview( );
	float4x4 transform = control->calculate(*m_start_transform) * math::invert4x3( m_object_path->get_transform( ) );
	
	m_object_path->set_key_tangent_position	( m_key_index, m_tangent_index, transform.c.xyz( ) );
}

void			track_object_key_tangent_transform_data::end_modify					( transform_control_base^ control )
{
	XRAY_UNREFERENCED_PARAMETERS	( control );
	//R_ASSERT		( m_apply_command != nullptr );
	//m_level_editor->get_command_engine()->end_preview();
	//m_apply_command = nullptr;
}

math::float4x4	track_object_key_tangent_transform_data::get_ancor_transform( )
{
	return math::create_translation( m_object_path->get_key_tangent_position( m_key_index, m_tangent_index ) ) * m_object_path->get_transform( );
}

////////////////////////////////////////// object_incompound_transform_data  ////////////////////////////////////////////

object_incompound_transform_data::object_incompound_transform_data( composite_object_item^ o )
:m_compound_object_item ( o )
{
	m_start_transform = CRT_NEW(math::float4x4)();
	m_start_transform->identity();
}

object_incompound_transform_data::~object_incompound_transform_data( )
{
	CRT_DELETE	( m_start_transform );
}

object_incompound_transform_data::!object_incompound_transform_data( )
{
	this->~object_incompound_transform_data();
}

void object_incompound_transform_data::start_modify( transform_control_base^ )
{
	(*m_start_transform)	= get_ancor_transform( );
}

void object_incompound_transform_data::execute_preview( transform_control_base^ control )
{
	m_compound_object_item->set_edit_transform( control->calculate( *m_start_transform ) );
}

void object_incompound_transform_data::end_modify( transform_control_base^ )
{}

math::float4x4 object_incompound_transform_data::get_ancor_transform( )
{
	return *(m_compound_object_item->m_relative_transform) * m_compound_object_item->m_object->get_parent_object()->get_transform();
}


} //namespace editor
} //namespace xray
