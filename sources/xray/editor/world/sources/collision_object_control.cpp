////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_object_control.h"

namespace xray {
namespace editor {

using math::float4x4;

collision_object_transform_control::collision_object_transform_control(	xray::collision::geometry const* geometry, 
														transform_control_base^ owner_control, 
														enum_transform_axis axis ):
collision_object	( geometry ),
m_owner				( owner_control ),
m_axis				( axis )
{
	m_transform_original.identity();
	m_transform_general.identity();
	set_type					( collision_type_control );
}

enum_transform_axis	collision_object_transform_control::get_axis	( ) const
{
	return m_axis;
}

void collision_object_transform_control::set_matrix		( xray::math::float4x4 const& matrix )
{
	m_transform_general = matrix;
	collision_object::set_matrix		( m_transform_original*m_transform_general );
}

void collision_object_transform_control::set_matrix_original		( xray::math::float4x4 const& matrix )
{
	m_transform_original = matrix;
	set_matrix( m_transform_general );
}

xray::math::float4x4 const& collision_object_transform_control::get_matrix_original	( ) const
{
	return m_transform_original;
}

//enum_collision_object_type collision_object_transform_control::get_type		( ) const
//{
//	return enum_collision_object_type_control;
//}

}// namespace editor
}// namespace xray
