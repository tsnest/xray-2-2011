////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_place_objects.h"
#include "object_base.h"
#include "level_editor.h"
#include "project.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/collision/collision_object.h>
#pragma managed( pop )

namespace xray {
namespace editor {

command_place_objects::command_place_objects ( level_editor^ le ):
m_level_editor			(le),
m_preview_mode			(true)
{
	object_base_list^ selection		= m_level_editor->selected_objects();
 	object_base_list objects_sorted	= selection;

	for each(object_base^ o in objects_sorted)
	{
		id_matrix	itm;
		itm.id		= o->id();
		itm.matrix	= NEW (math::float4x4)(o->get_transform());
		m_id_matrices.Add(itm);
	}
}

command_place_objects::~command_place_objects( )
{
	for each(id_matrix^ itm in m_id_matrices)
		DELETE ( itm->matrix	);

	m_id_matrices.Clear( );
}

bool command_place_objects::commit	()
{
	math::aabb bbox			= create_aabb_min_max( float3( math::float_max, math::float_max, math::float_max ), 
											  float3( math::float_min, math::float_min, math::float_min ) );
	
	object_base_list				objects;
	float3					snap_center;
	float3					object_center;

	for each(id_matrix^ idm in m_id_matrices)
	{
		object_base^ object = m_level_editor->object_by_id(idm->id);
		ASSERT				( object != nullptr );
		bbox.modify			(object->get_aabb());
		objects.Add			(object);
	}


	if( objects.Count == 1 )
		object_center = (objects[0])->get_transform().c.xyz();
	else
		object_center = bbox.center()+ float3(0, -bbox.extents().y, 0);

	
	if( m_preview_mode )
	{
		float distance;
		float3 origin;
		float3 direction;
		m_level_editor->view_window()->get_mouse_ray( origin, direction );

		editor_base::collision_ray_objects^  collision_results = gcnew editor_base::collision_ray_objects;
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
		{
			snap_center = origin+direction*distance;
		}

		float3 trans_vector = snap_center - object_center;

		for each (object_base^ o in objects)
			o->set_position( o->get_transform().c.xyz() + trans_vector );
	}
	
	math::float4x4 tmp_mat;

	for each(id_matrix^ idm in m_id_matrices)
	{
		object_base^ object = m_level_editor->object_by_id(idm->id);
		tmp_mat				= object->get_transform();
		object->set_transform(*idm->matrix);
		*idm->matrix		= tmp_mat;
	}

	return true;
}

bool command_place_objects::end_preview	()
{
	m_preview_mode = false;
	return true;
}

void command_place_objects::rollback		()
{
	ASSERT	( !m_preview_mode );
	commit	( );
}

} // namespace editor
} // namespace xray

