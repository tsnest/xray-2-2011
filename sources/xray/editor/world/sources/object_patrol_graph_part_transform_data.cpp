////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_patrol_graph_part_transform_data.h"
#include "object_patrol_graph_part.h"

using xray::editor_base::transform_control_translation;
using xray::editor_base::transform_control_rotation;
using xray::editor_base::transform_control_scaling;

namespace xray{
namespace editor{

object_patrol_graph_part_transform_data::object_patrol_graph_part_transform_data	( object_patrol_graph_part^ obj ):
	m_graph_part		( obj )
{
	m_start_transform = CRT_NEW(math::float4x4)( );
	m_start_transform->identity( );
}

object_patrol_graph_part_transform_data::~object_patrol_graph_part_transform_data ( )
{
	CRT_DELETE ( m_start_transform );
}

object_patrol_graph_part_transform_data::!object_patrol_graph_part_transform_data ( )
{
	this->~object_patrol_graph_part_transform_data( );
}

bool object_patrol_graph_part_transform_data::attach ( transform_control_base^ transform )
{
	if( xray::editor::wpf_controls::dot_net_helpers::is_type<transform_control_translation^>( transform ) )
		return true;

	return false;
}

void object_patrol_graph_part_transform_data::start_modify ( transform_control_base^ control )
{
	XRAY_UNREFERENCED_PARAMETERS	( control );
	( *m_start_transform )			= math::create_translation( m_graph_part->position );
}

void object_patrol_graph_part_transform_data::execute_preview ( transform_control_base^ control )
{
	float4x4 transform		= control->calculate		( *m_start_transform );
	m_graph_part->position	= Float3( transform.c.xyz( ) );
}

void object_patrol_graph_part_transform_data::end_modify ( transform_control_base^ control )
{
	XRAY_UNREFERENCED_PARAMETERS	( control );
}

math::float4x4	object_patrol_graph_part_transform_data::get_ancor_transform ( )
{
	return math::create_translation( m_graph_part->position );
}


} // namespace editor
} // namespace xray