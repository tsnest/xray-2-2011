////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_VEC3F_HPP_INCLUDED
#define PROPERTY_VEC3F_HPP_INCLUDED

#include "property_vec3f_base.h"

public ref class property_vec3f : public property_vec3f_base {
public:
	typedef property_vec3f_base							inherited;

public:
							property_vec3f	(
								float3_getter_type^ getter,
								float3_setter_type^ setter,
								float3 const& default_value,
								property_holder::readonly_enum read_only,
								property_holder::notify_parent_on_change_enum notify_parent,
								property_holder::password_char_enum password,
								property_holder::refresh_grid_on_change_enum refresh_grid);

	virtual					~property_vec3f	();
							!property_vec3f	();
	virtual float3			get_value_raw	() override;
	virtual void			set_value_raw	(float3 value) override;

private:
	float3_getter_type^		m_getter;
	float3_setter_type^		m_setter;
}; // ref class property_vec3f

//vec2f
public ref class property_vec2f : public property_vec2f_base {
public:
	typedef property_vec2f_base							inherited;

public:
							property_vec2f	(
								float2_getter_type^ getter,
								float2_setter_type^ setter,
								float2 const& default_value,
								property_holder::readonly_enum read_only,
								property_holder::notify_parent_on_change_enum notify_parent,
								property_holder::password_char_enum password,
								property_holder::refresh_grid_on_change_enum refresh_grid);

	virtual					~property_vec2f	();
							!property_vec2f	();
	virtual float2			get_value_raw	() override;
	virtual void			set_value_raw	(float2 value) override;

private:
	float2_getter_type^		m_getter;
	float2_setter_type^		m_setter;
}; // ref class property_vec2f

#endif // ifndef PROPERTY_VEC3F_HPP_INCLUDED