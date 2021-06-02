////////////////////////////////////////////////////////////////////////////
//	Created		: 10.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_objects_impl.h"
#include "transform_control_base.h"
#include "property_visualizers.h"

namespace xray{
namespace editor_base{

math::float4x4 get_transform(	configs::lua_config_value* t_position, 
								configs::lua_config_value* t_rotation,
								configs::lua_config_value* t_scale )
{
	math::float3 rotation		= *t_rotation;
	math::float3 translation	= *t_position;
	math::float3 scale			= *t_scale;

	return create_scale( scale ) * create_rotation( rotation ) * create_translation( translation );
}

void set_transform(	configs::lua_config_value* t_position, 
					configs::lua_config_value* t_rotation,
					configs::lua_config_value* t_scale,
					math::float4x4 const& m )
{
	float3 rotation		= m.get_angles_xyz();

	*t_position			= m.c.xyz();
	*t_rotation			= rotation;

	float3				scale;
	scale.x 			= m.i.length();
	scale.y 			= m.j.length();
	scale.z 			= m.k.length();

	*t_scale			= scale;
}


///
transform_value_object::transform_value_object( math::float4x4* m, value_modified^ d, bool execute_on_preview )
:m_changed_delegate ( d ),
m_execute_on_preview( execute_on_preview )
{
	m_value_transform	= m;
	m_start_transform	= NEW(math::float4x4)(*m);
}

transform_value_object::~transform_value_object( )
{
	DELETE		( m_start_transform );
}

void transform_value_object::start_modify( transform_control_base^ control )
{
	XRAY_UNREFERENCED_PARAMETERS	(control);
	(*m_start_transform)			= *m_value_transform;
}

void transform_value_object::execute_preview( transform_control_base^ control )
{
	math::float4x4 m	= control->calculate( *m_start_transform );

	bool bneed_update = m_execute_on_preview &&
			(!math::is_similar(m.i.xyz(), (*m_value_transform).i.xyz())||
			!math::is_similar(m.j.xyz(), (*m_value_transform).j.xyz())	||
			!math::is_similar(m.k.xyz(), (*m_value_transform).k.xyz())	||
			!math::is_similar(m.c.xyz(), (*m_value_transform).c.xyz())
		);

	if(bneed_update)
	{
		*m_value_transform	= m;
		if(m_changed_delegate)
			m_changed_delegate	( nullptr );
	}
}

void transform_value_object::end_modify( transform_control_base^ )
{
	if(m_changed_delegate)
		m_changed_delegate	( nullptr );
}

float4x4 transform_value_object::get_ancor_transform( )
{
	return *m_value_transform;
}


///
transform_lua_config_object::transform_lua_config_object(	configs::lua_config_value& t_position, 
															configs::lua_config_value& t_rotation, 
															configs::lua_config_value& t_scale,
															math::float4x4* world_transform,
															value_modified^ d,
															cli::array<System::String^>^ prop_names)
:m_world_transform	( world_transform ),
m_changed_delegate	( d ),
m_prop_names		( prop_names )
{
	ASSERT(t_position.get_type() == configs::t_float3);
	ASSERT(t_rotation.get_type() == configs::t_float3);
	ASSERT(t_scale.get_type() == configs::t_float3);

	m_t_position	= NEW(configs::lua_config_value)(t_position);
	m_t_rotation	= NEW(configs::lua_config_value)(t_rotation);
	m_t_scale		= NEW(configs::lua_config_value)(t_scale);
	m_start_transform= NEW(math::float4x4)();
	(*m_start_transform) = get_transform( m_t_position, m_t_rotation, m_t_scale );
}

transform_lua_config_object::~transform_lua_config_object( )
{
	DELETE(m_t_position);
	DELETE(m_t_rotation);
	DELETE(m_t_scale);
	DELETE(m_start_transform);
}

void transform_lua_config_object::start_modify( transform_control_base^ control )
{
	XRAY_UNREFERENCED_PARAMETERS	(control);
	(*m_start_transform)			= get_transform( m_t_position, m_t_rotation, m_t_scale );
}

void transform_lua_config_object::execute_preview( transform_control_base^ control )
{
	math::float4x4 m = control->calculate( *m_start_transform );

	set_transform(m_t_position, m_t_rotation, m_t_scale, m);
}

void transform_lua_config_object::end_modify( transform_control_base^ )
{
	if(m_changed_delegate)
		m_changed_delegate( m_prop_names );
}

float4x4 transform_lua_config_object::get_ancor_transform( )
{
	m_t_position->refresh_value_from_parent();
	m_t_rotation->refresh_value_from_parent();
	m_t_scale->refresh_value_from_parent();
	return get_transform(m_t_position, m_t_rotation, m_t_scale) * (*m_world_transform);
}


} //namespace editor_base
} //namespace xray
