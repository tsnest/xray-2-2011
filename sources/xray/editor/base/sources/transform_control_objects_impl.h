////////////////////////////////////////////////////////////////////////////
//	Created		: 10.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_OBJECTS_IMPL_H_INCLUDED
#define TRANSFORM_CONTROL_OBJECTS_IMPL_H_INCLUDED

#include "transform_control_object.h"

namespace xray {
namespace editor_base {

public delegate void value_modified ( cli::array<System::String^>^ names );

public ref class transform_value_object : public transform_control_object
{
public:
					transform_value_object	( math::float4x4* m, value_modified^ d, bool execute_on_preview );
	virtual			~transform_value_object( );
	virtual void	start_modify		( transform_control_base^ ) override;
	virtual void	execute_preview		( transform_control_base^ ) override;
	virtual void	end_modify			( transform_control_base^ ) override;
	virtual float4x4 get_ancor_transform		( ) override;
private:
	value_modified^					m_changed_delegate;
	math::float4x4*					m_value_transform;
	math::float4x4*					m_start_transform;
	bool							m_execute_on_preview;
}; // transform_control_object


public ref class transform_lua_config_object : public transform_control_object
{
public:
					transform_lua_config_object	( configs::lua_config_value& t_position, configs::lua_config_value& t_rotation, configs::lua_config_value& t_scale, math::float4x4* world_transform, value_modified^ d, cli::array<System::String^>^ prop_names );
	virtual			~transform_lua_config_object( );
	virtual void	start_modify		( transform_control_base^ ) override;
	virtual void	execute_preview		( transform_control_base^ ) override;
	virtual void	end_modify			( transform_control_base^ ) override;
	virtual float4x4 get_ancor_transform		( ) override;
private:
	cli::array<System::String^>^	m_prop_names;
	value_modified^					m_changed_delegate;
	configs::lua_config_value*		m_t_position;
	configs::lua_config_value*		m_t_rotation;
	configs::lua_config_value*		m_t_scale;
	math::float4x4*					m_start_transform;
	math::float4x4*					m_world_transform;
}; // transform_control_object

} // namespace editor_base
} // namespace xray

#endif // #ifndef TRANSFORM_CONTROL_OBJECTS_IMPL_H_INCLUDED