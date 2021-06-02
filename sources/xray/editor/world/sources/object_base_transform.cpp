////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_base.h"
#include "tool_base.h"
#include "level_editor.h"
#include "project.h"
#include "command_set_object_transform.h"

#pragma managed( push, off )
#	include <xray/collision/space_partitioning_tree.h>
#pragma managed( pop )

namespace xray {
namespace editor {

float3 rad2deg(float3 const& rad)
{
	return rad * (180.0f/math::pi);
}

float3 deg2rad(float3 const& deg)
{
	return deg * (math::pi/180.0f);
}

static void set_rotation_impl	(float3 angles, float4x4& transform)
{
	transform				=
		create_scale( transform.get_scale() ) *
		create_rotation( angles ) *
		create_translation( transform.c.xyz() );
}

float const min_scale = 0.05f;
float const max_scale = 1000.0f;

void set_scale_impl				(float3 s, float4x4& transform)
{
	math::clamp( s.x, min_scale, max_scale );
	math::clamp( s.y, min_scale, max_scale );
	math::clamp( s.z, min_scale, max_scale );
	transform.set_scale		( s );
}

void object_base::set_transform( float4x4 const& t )
{
	*m_previous_angles			= get_rotation();

	float3 position_prev		= object_position;
	float3 rotation_prev		= object_rotation;
	float3 scale_prev			= object_scale;

	float3 s = t.get_scale();
	if(s.x<0.05f || s.y<0.05f || s.z<0.05f)
	{
		math::float4x4 m = t;
		math::clamp( s.x, min_scale, max_scale );
		math::clamp( s.y, min_scale, max_scale );
		math::clamp( s.z, min_scale, max_scale );
		set_scale_impl( s, m );
		*m_transform			= m;
	}else
		*m_transform			= t;
	
	if ( m_collision->initialized() )
		m_collision->set_matrix( m_transform );

	if(!position_prev.is_similar(object_position))
	{
		on_property_changed			("position");
		
		if(m_project_item)
			m_project_item->m_project->on_item_transform_changed(m_project_item, position_prev);
	}

	if(!rotation_prev.is_similar(object_rotation))
		on_property_changed			("rotation");

	if(!scale_prev.is_similar(object_scale))
		on_property_changed			("scale");

	m_object_transform_changed( this );
}

void object_base::set_position(float3 p)
{
	float4x4 transform		= *m_transform;
	transform.c.xyz()		= p;
	set_transform			(transform);
}

void object_base::set_rotation(float3 p)
{
	float4x4 transform		= *m_transform;
	*m_previous_angles		= m_transform->get_angles_xyz();
	set_rotation_impl		(p, transform);
	set_transform			(transform);
}

math::float3 object_base::get_rotation()
{
	return					math::to_close_xyz( m_transform->get_angles_xyz(), *m_previous_angles );
}

float3 object_base::get_scale()
{
	return m_transform->get_scale();
}

void object_base::set_scale( float3 s )
{
	float4x4 transform		= *m_transform;
	set_scale_impl			( s, transform );
	set_transform			( transform );
}

void object_base::set_position_revertible(float3 p)
{
	float4x4 transform		= *m_transform;
	transform.c.xyz()		= p;
	
	if(!is_slave_attribute("position"))
		owner_tool()->get_level_editor()->get_command_engine()->run( gcnew command_set_object_transform( owner_tool()->get_level_editor(), this->id(), transform ) );
	else
		set_transform		(transform);
}

void object_base::set_rotation_revertible(float3 p)
{
	float4x4 transform		= *m_transform;
	*m_previous_angles		= m_transform->get_angles_xyz();
	set_rotation_impl		(p, transform);

	if(!is_slave_attribute("rotation"))
		owner_tool()->get_level_editor()->get_command_engine()->run( gcnew command_set_object_transform( owner_tool()->get_level_editor(), this->id(), transform ) );
	else
		set_transform		(transform);
}

void object_base::set_scale_revertible(float3 p)
{
	float4x4 transform		= *m_transform;
	set_scale_impl			(p, transform);

	if(!is_slave_attribute("scale"))
		owner_tool()->get_level_editor()->get_command_engine()->run( gcnew command_set_object_transform( owner_tool()->get_level_editor(), this->id(), transform ) );
	else
		set_transform		(transform);
}

float4x4 place_object( object_base^ o, float3 const& position )
{
	XRAY_UNREFERENCED_PARAMETER	( o );
	float4x4 result_matrix		= float4x4().identity();
	result_matrix.c.xyz().set	(position.x, position.y, position.z);
	return						result_matrix;
}

} // namespace editor
} // namespace xray
