////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_modifier_command.h"
#include "command_set_object_transform.h"
#include "terrain_modifier_control.h"
#include "terrain_selection_control.h"
#include "terrain_object.h"
#include "level_editor.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include "collision_object_dynamic.h"
#include <xray/collision/object.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

terrain_modifier_command::terrain_modifier_command( terrain_control_base^ control, terrain_core^ core)
:m_control(control),
m_terrain_core(core),
m_b_in_preview(false),
m_k(-1)
{}

bool terrain_modifier_command::commit()
{
	if(m_b_in_preview)
	{
		m_control->do_work		();
		return					true;
	}

	for each(terrain_node_key key in m_object_list)
		commit_terrain				(key, m_k);

	m_k							*= -1;
	return						true;
}

bool terrain_modifier_command::end_preview()
{
	m_b_in_preview					= false;
	return true;
}

void terrain_modifier_command::start_preview()
{
	m_b_in_preview = true;
}

void terrain_modifier_command::rollback()
{
	commit	();
}

terrain_modifier_height_command::terrain_modifier_height_command( terrain_modifier_control_base^ control, terrain_core^ core)
:super(control, core)
{
}

terrain_modifier_height_command::~terrain_modifier_height_command( )
{
	for each(xray::editor_base::command^ cmd in m_transform_commands)
		delete cmd;
	
	m_transform_commands.Clear( );
}

void terrain_modifier_height_command::rollback()
{
	super::rollback		( );

	for each(xray::editor_base::command^ cmd in m_transform_commands)
		cmd->rollback( );
}

bool terrain_modifier_height_command::end_preview()
{
	bool res					= super::end_preview();
	
	if (!res)
		return res;

	// collect affected objects
	math::aabb					query_aabb = math::create_invalid_aabb();

	for each(terrain_node_key key in m_object_list)
	{
		height_diff_dict::Enumerator^ it	= m_height_diff_list[key]->GetEnumerator();

		terrain_node^ terrain				= m_terrain_core->m_nodes[key];
		while(it->MoveNext())
		{
			float3	vp						= terrain->position_g(it->Current.Key);
			query_aabb.modify				(vp);
		}// all vertices in terrain node
	}// for each terrain node

	if( m_control->affect_objects_snap )
	{
		float3 extents = query_aabb.extents();
		if(math::is_zero(extents.x) || math::is_zero(extents.z))
			return res;

		query_aabb.min.y						= -10000;//math::float_min;
		query_aabb.max.y						= 10000; //math::float_max;

		xray::collision::space_partitioning_tree* tree	= m_control->m_level_editor->get_collision_tree();
		collision::objects_type							selected_objects(g_allocator);
		tree->aabb_query						(	editor_base::collision_object_type_dynamic, 
													query_aabb, 
													selected_objects 
												);
	
		collision::objects_type::iterator	it	= selected_objects.begin();
		collision::objects_type::iterator	it_e = selected_objects.end();

		for(; it!=it_e; ++it)
		{
			collision::object const * co	= *it;

			ASSERT( co->get_type() & editor_base::collision_object_type_dynamic );

			collision_object_dynamic const* dynamic	= static_cast_checked<collision_object_dynamic const*>(co);
			object_base^ o					= dynamic->get_owner_object();
			enum_terrain_interaction ti		= o->get_terrain_interaction();
			

			if(ti==enum_terrain_interaction::tr_vertical_snap)
			{
				//LOG_INFO("affected object: %s", unmanaged_string(o->get_name()).c_str());
				float4x4 object_new_transform	= o->get_transform( );

				float p = m_terrain_core->get_height	(object_new_transform.c.xyz());
				object_new_transform.c.y				= p;
				command_set_object_transform^ cmd		= gcnew command_set_object_transform(m_control->m_level_editor, o->id(), object_new_transform);
				m_transform_commands.Add				( cmd );
			}
		}


		for each(xray::editor_base::command^ cmd in m_transform_commands)
			cmd->commit		( );
	}

	return res;
}

void terrain_modifier_height_command::set_data(terrain_node_key key, height_diff_dict^ diff_list)
{
	ASSERT				(!m_object_list.Contains(key));
	ASSERT				(!m_height_diff_list.ContainsKey(key));

	m_object_list.Add		(key);
	height_diff_dict^ l		= gcnew height_diff_dict;

	height_diff_dict::Enumerator^ it = diff_list->GetEnumerator();

	while(it->MoveNext())
		l->Add		(it->Current.Key, it->Current.Value);

	m_height_diff_list.Add	(key, l);
}

void terrain_modifier_height_command::commit_terrain(terrain_node_key key, s8 k)
{
	height_diff_dict::Enumerator^ it	= m_height_diff_list[key]->GetEnumerator();

	terrain_node^ terrain				= m_terrain_core->m_nodes[key];
	
	while(it->MoveNext())
	{
		terrain_vertex% v				= terrain->m_vertices.get(it->Current.Key);
		v.height						+= it->Current.Value * k;
		terrain->m_vertices.set			(v, it->Current.Key);
	}

	vert_id_list	vert_ids;
	for each(u16 id in 	m_height_diff_list[key]->Keys)
		vert_ids.Add(id);

	m_terrain_core->sync_visual_vertices		(terrain, %vert_ids);
	terrain->initialize_collision				();

	m_terrain_core->get_editor_renderer().terrain_update_cell_aabb( m_terrain_core->scene(), 
																	terrain->get_model()->m_render_model, 
																	terrain->get_local_aabb());
}

terrain_modifier_color_command::terrain_modifier_color_command( terrain_modifier_control_base^ control, terrain_core^ core)
:super(control, core)
{
}

bool terrain_modifier_color_command::end_preview( )
{
	bool res = super::end_preview();
	if (!res)
		return res;

	return m_object_list.Count!=0;
}

void terrain_modifier_color_command::set_data(terrain_node_key key, vertex_dict^ diff_list)
{
	ASSERT					(!m_object_list.Contains(key));
	ASSERT(diff_list->Count>0);

	m_object_list.Add		(key);
	m_vertex_list.Add		(key, diff_list);
}

void terrain_modifier_color_command::commit_terrain(terrain_node_key key, s8 /*k*/)
{
	vertex_dict::Enumerator^ it		= m_vertex_list[key]->GetEnumerator();

	terrain_node^ terrain			= m_terrain_core->m_nodes[key];

	vert_id_list	vert_ids;
	vertex_dict^	revert			= gcnew vertex_dict;

	while(it->MoveNext())
	{
		u16 vert_idx				= it->Current.Key;
		revert->Add					( vert_idx, terrain->m_vertices.get(vert_idx) );

		terrain->m_vertices.set		( it->Current.Value, vert_idx );
		vert_ids.Add				( it->Current.Key );
	}

	m_vertex_list[key]					= revert;
	m_terrain_core->sync_visual_vertices(terrain, %vert_ids);
}

terrain_selector_command::terrain_selector_command( terrain_core^ core, key_vert_id_dict^ list, enum_selection_method method )
:m_terrain_core	( core ),
m_method		( method )
{
	m_selected_quads_list	= list;
}

bool terrain_selector_command::commit()
{
	key_vert_id_dict^	revert		= gcnew key_vert_id_dict (m_terrain_core->selected_quads( ));

	m_terrain_core->select_quads	( m_selected_quads_list, m_method );

	m_selected_quads_list			= revert;
	m_method						= enum_selection_method_set;
	return							true;
}

void terrain_selector_command::rollback( )
{
	commit();
}

} // namespace editor
} // namespace xray
