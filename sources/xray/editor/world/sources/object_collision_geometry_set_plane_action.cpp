////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_collision_geometry_mesh.h"
#include "object_collision_geometry_set_plane_action.h"
#include "object_collision_geometry.h"
#include "project.h"
#include <xray/editor/base/collision_object_types.h>
#include <xray/collision/primitives.h>

namespace xray{
namespace editor{

object_collision_geometry_set_plane_action::object_collision_geometry_set_plane_action(  String^ name, editor_base::scene_view_panel^ view  )
:super			( name ),
m_window_view	( view )
{}

bool object_collision_geometry_set_plane_action::do_it						( )
{
	if( !m_window_view->is_mouse_in_view( ) )
		return false;

	if( m_object_collision_geometry_mesh == nullptr )
		return false;

	float3 plane_position;
	float3 plane_normal;
	editor_base::collision_ray_triangles^ triangles = gcnew editor_base::collision_ray_triangles( );
	
	if( m_window_view->ray_query( editor_base::collision_object_type_dynamic | editor_base::collision_object_type_terrain, plane_position, plane_normal ) )
	{
		float3 mesh_position			= m_object_collision_geometry_mesh->get_absolute_position( );
		float3 plane_to_mesh			= mesh_position - plane_position;
		float projection				= plane_to_mesh | plane_normal;
		float radius					= m_object_collision_geometry_mesh->primitive.truncated_sphere( ).radius;

		m_object_collision_geometry_mesh->set_plane( float4( -plane_normal, projection / radius ) );

		delete triangles;

		 m_object_collision_geometry_mesh->collision_object->get_project( )->select_object(
			 m_object_collision_geometry_mesh->collision_object->owner_project_item,
			 enum_selection_method_set
		);

		return true;
	}
	
	delete triangles;
	return false;
}

void object_collision_geometry_set_plane_action::set_collision_geometry_mesh	( object_collision_geometry_mesh^ mesh )
{
	m_object_collision_geometry_mesh = mesh;
}

} // namespace editor
} // namespace xray
