////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_collision_geometry.h"
#include "object_collision_geometry_mesh.h"
#include "object_collision_geometry_mesh_move_command.h"
#include "project.h"
#include "project_items.h"
#include "level_editor.h"
#include "tool_base.h"

#pragma managed(push, on)
#include <xray/collision/primitives.h>
#pragma managed(pop)

namespace xray{
namespace editor{

static math::float4x4 start_modify_matrix;

object_collision_geometry_mesh_move_command::object_collision_geometry_mesh_move_command( object_collision_geometry^ geometry, Int32 mesh_index, Boolean is_anti_mesh, editor_base::transform_control_base^ transform_control ):
	m_transform_control			( transform_control ),
	m_level_editor				( geometry->owner_tool( )->get_level_editor( ) ),
	m_geometry_full_name		( geometry->owner_project_item->get_full_name( ) ),
	m_mesh_index				( mesh_index ),
	m_is_anti_mesh				( is_anti_mesh ),
	m_is_preview				( true )
{
	object_collision_geometry_mesh^ mesh = geometry->get_mesh( m_is_anti_mesh, m_mesh_index );

	m_start_position	=		mesh->position; 
	m_start_rotation	=		mesh->rotation;
	m_start_scale		=		mesh->scale;

	if( wpf_controls::dot_net_helpers::is_type<editor_base::transform_control_scaling^>( m_transform_control ) )
		start_modify_matrix = math::create_scale( mesh->scale ) * geometry->get_transform( );
	else
		start_modify_matrix = mesh->matrix * geometry->get_transform( );
}

bool object_collision_geometry_mesh_move_command::commit ( )
{
	object_collision_geometry^		geometry	= safe_cast<object_collision_geometry^>( m_level_editor->get_project( )->find_by_path( m_geometry_full_name, true )->get_object( ) );
	object_collision_geometry_mesh^	mesh		= geometry->get_mesh	( m_is_anti_mesh, m_mesh_index );

	if( m_is_preview )
	{
		math::float4x4	inverted_transform	= math::invert4x3		( geometry->get_transform( ) );
		math::float4x4	m					= m_transform_control->calculate	( start_modify_matrix ) * inverted_transform;

		if( wpf_controls::dot_net_helpers::is_type<editor_base::transform_control_scaling^>( m_transform_control ) )
		{
			float3 new_scale	= m.get_scale( );

			switch( mesh->primitive.type )
			{
			case collision::primitive_sphere: 
			case collision::primitive_truncated_sphere: 
			{
				float new_single_scale	= new_scale.x;
				float scale_delta		= 0;
				float max_scale_delta	= 0;

				scale_delta = math::abs( m_start_scale.x - new_scale.x );
				if( scale_delta > max_scale_delta )
				{
					new_single_scale	= new_scale.x;
					max_scale_delta		= scale_delta;
				}

				scale_delta = math::abs( m_start_scale.y - new_scale.y );
				if( scale_delta > max_scale_delta )
				{
					new_single_scale	= new_scale.y;
					max_scale_delta		= scale_delta;
				}

				scale_delta = math::abs( m_start_scale.z - new_scale.z );
				if( scale_delta > max_scale_delta )
				{
					new_single_scale	= new_scale.z;
					max_scale_delta		= scale_delta;
				}

				mesh->scale				= Float3( float3( new_single_scale, new_single_scale, new_single_scale ) );
				break;
			}
			case collision::primitive_cylinder: 
			case collision::primitive_capsule: 
			{
				float new_single_scale = new_scale.x;
				float scale_delta		= 0;
				float max_scale_delta	= 0;

				scale_delta = math::abs( m_start_scale.x - new_scale.x );
				if( scale_delta > max_scale_delta )
				{
					new_single_scale	= new_scale.x;
					max_scale_delta		= scale_delta;
				}

				scale_delta = math::abs( m_start_scale.z - new_scale.z );
				if( scale_delta > max_scale_delta )
				{
					new_single_scale	= new_scale.z;
					max_scale_delta		= scale_delta;
				}

				mesh->scale				= Float3( float3( new_single_scale, new_scale.y, new_single_scale ) );
				break;
			}
			default:
				mesh->scale				= Float3( m.get_scale( ) );
				break;
			}

			mesh->update_collision	( );
		}
		else
		{
			mesh->update_position	( m );
		}

		m_end_position	= mesh->position;
		m_end_rotation	= mesh->rotation;
		m_end_scale		= mesh->scale;
	}
	else
	{
		if( wpf_controls::dot_net_helpers::is_type<editor_base::transform_control_scaling^>( m_transform_control ) )
		{
			mesh->scale				= m_end_scale;
			mesh->update_collision	( );
		}
		else
		{
			float3 position	( m_end_position.x, m_end_position.y, m_end_position.z );
			float3 rotation	( m_end_rotation.x, m_end_rotation.y, m_end_rotation.z );
			float3 angles_rad		= rotation * ( math::pi / 180.0f );
			float4x4 end_matrix		= math::create_rotation( angles_rad ) * math::create_translation( position );
			mesh->update_position	( end_matrix );
		}
	}

	return true;
}
void object_collision_geometry_mesh_move_command::rollback ( )
{
	object_collision_geometry^		geometry	= safe_cast<object_collision_geometry^>( m_level_editor->get_project( )->find_by_path( m_geometry_full_name, true )->get_object( ) );
	object_collision_geometry_mesh^	mesh		= geometry->get_mesh	( m_is_anti_mesh, m_mesh_index );

	if( wpf_controls::dot_net_helpers::is_type<editor_base::transform_control_scaling^>( m_transform_control ) )
	{
		mesh->scale				= m_start_scale;
		mesh->update_collision	( );
	}
	else
	{
		float3 position			( m_start_position.x, m_start_position.y, m_start_position.z );
		float3 rotation			( m_start_rotation.x, m_start_rotation.y, m_start_rotation.z );
		float3 angles_rad		= rotation * ( math::pi / 180.0f );
		float4x4 start_matrix	= math::create_rotation( angles_rad ) * math::create_translation( position );
		mesh->update_position	( start_matrix );
	}
}

void object_collision_geometry_mesh_move_command::start_preview ( )
{
	
}

bool object_collision_geometry_mesh_move_command::end_preview ( )
{
	m_is_preview = false;
	return true;
}

} // namespace editor
} // namespace xray
