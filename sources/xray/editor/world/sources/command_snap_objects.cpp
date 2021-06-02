////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_snap_objects.h"
#include "object_base.h"
#include "level_editor.h"
#include "project.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/collision/collision_object.h>
#pragma managed( pop )

namespace xray {
namespace editor {


command_snap_objects::command_snap_objects ( level_editor^ le ):
m_level_editor			(le),
m_preview_mode			(true)
{
	object_base_list^ selection = m_level_editor->selected_objects();

	object_base_list objects_sorted		= selection;
	objects_sorted.Sort				(gcnew object_height_predicate);

	for each(object_base^ o in objects_sorted)
	{
		id_matrix	itm;
		itm.id		= o->id();
		itm.matrix	= NEW (math::float4x4)(o->get_transform());
		m_id_matrices.Add(itm);
	}
}

command_snap_objects::~command_snap_objects( )
{
	for each(id_matrix^ itm in m_id_matrices)
		DELETE ( itm->matrix	);

	m_id_matrices.Clear( );
}

bool command_snap_objects::commit	()
{

	if( m_preview_mode )
	{
		float3 snap_center;
		
		math::aabb bbox		= create_aabb_min_max( float3( math::float_max, math::float_max, math::float_max ), 
											float3( math::float_min, math::float_min, math::float_min ) );

		object_base_list				objects;

		for each(id_matrix^ idm in m_id_matrices)
		{
			object_base^ object = m_level_editor->object_by_id(idm->id);
			bbox.modify			(object->get_aabb());
			objects.Add			(object);
		}

		float			distance;
		float3			origin;
		float3			direction;

		m_level_editor->view_window()->get_mouse_ray( origin, direction );

		editor_base::collision_ray_objects^ collision_results = gcnew editor_base::collision_ray_objects;
		m_level_editor->view_window()->ray_query( editor_base::collision_object_type_dynamic, origin, direction, collision_results );

		if( !get_valid_collision( collision_results, %objects, distance ) )
		{

			if( !xray::editor_base::transform_control_base::plane_ray_intersect(	math::float3( 0, 0, 0 ),	
																math::float3( 0, 1, 0 ),
																origin,						
																direction,
																snap_center) )
				return true;
		}else
			snap_center = origin+direction*distance;

		float3 trans_vector			= snap_center - bbox.center();
		trans_vector.y				= 0;

		for( int object_idx=0; object_idx < objects.Count; ++object_idx)
		{
			float3 const direction		( 0, -1, 0 );
			object_base^ curr			= objects[object_idx];
			float4x4 transform			= curr->get_transform();
			float	distance;
			
			float3 const shift_up	= float3( 0, 1.f, 0 );

			float3	origin			= transform.c.xyz() + trans_vector + shift_up;

			collision_results->clear();
			m_level_editor->view_window()->ray_query( editor_base::collision_object_type_dynamic, origin, direction, collision_results );
			
			if( get_valid_collision	( collision_results, objects.GetRange(object_idx, 1), distance ) )
			{
				transform.c.xyz()	= origin + direction*distance;
				curr->set_transform	( transform );
			}else
			{
				origin -= shift_up;
				m_level_editor->view_window()->ray_query( editor_base::collision_object_type_dynamic, origin, -direction, collision_results );
				if( get_valid_collision	( collision_results, objects.GetRange(object_idx, 1), distance ) )
				{
					transform.c.xyz() = origin - direction*distance;
					curr->set_transform( transform );
				}else
				{
					transform.c.xyz() = float3( 1, 0, 1 )*origin;
					curr->set_transform( transform );
				}
			}
		}
	}else // preview
	{
		math::float4x4				tmp_mat;

		for each(id_matrix^ idm in m_id_matrices)
		{
			object_base^ object		= m_level_editor->object_by_id(idm->id);

			tmp_mat					= object->get_transform();
			object->set_transform	(*idm->matrix);
			*idm->matrix			= tmp_mat;
		}
	}


	return true;
}

bool command_snap_objects::end_preview	()
{
	m_preview_mode = false;
	return true;
}

void command_snap_objects::rollback		()
{
	ASSERT	( !m_preview_mode );
	commit	( );
}

} // namespace editor
} // namespace xray

