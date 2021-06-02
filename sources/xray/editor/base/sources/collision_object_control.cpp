////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/editor/base/collision_object_control.h>
#include <xray/editor/base/collision_object_types.h>

namespace xray {
namespace editor_base {

using math::float4x4;

collision_object_transform_control::collision_object_transform_control	(
		collision::geometry_instance* geometry, 
		transform_control_base^ owner_control, 
		enum_transform_axis axis
	) :
	super							( g_allocator, collision_object_type_control, geometry ),
	m_owner							( owner_control ),
	m_axis							( axis )
{
	m_transform_original.identity	( );
	m_transform_general.identity	( );
}

enum_transform_axis	collision_object_transform_control::get_axis( ) const
{
	return m_axis;
}

void collision_object_transform_control::set_matrix( math::float4x4 const& matrix )
{
	m_transform_general		= matrix;
	super::set_matrix		( m_transform_original*m_transform_general );
}

void collision_object_transform_control::set_matrix_original( math::float4x4 const& matrix )
{
	m_transform_original = matrix;
	set_matrix( m_transform_general );
}

}// namespace editor_base
}// namespace xray
