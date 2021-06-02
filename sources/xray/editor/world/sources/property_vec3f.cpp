////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_vec3f.h"

property_vec3f::property_vec3f		(
		float3_getter_type^ getter,
		float3_setter_type^ setter,
		float3 const& default_value,
		property_holder::readonly_enum read_only,
		property_holder::notify_parent_on_change_enum notify_parent,
		property_holder::password_char_enum password,
		property_holder::refresh_grid_on_change_enum refresh_grid) :

	m_getter				(getter),
	m_setter				(setter),
	inherited				(default_value, read_only, notify_parent, password, refresh_grid )
{
}

property_vec3f::~property_vec3f		()
{
	this->!property_vec3f	();
}

property_vec3f::!property_vec3f		()
{
}

float3 property_vec3f::get_value_raw	()
{
	return					(m_getter());
}

void property_vec3f::set_value_raw	(float3 value)
{
	m_setter				(value);
}

//vec2f
property_vec2f::property_vec2f		(
		float2_getter_type^ getter,
		float2_setter_type^ setter,
		float2 const& default_value,
		property_holder::readonly_enum read_only,
		property_holder::notify_parent_on_change_enum notify_parent,
		property_holder::password_char_enum password,
		property_holder::refresh_grid_on_change_enum refresh_grid) :

	m_getter				(getter),
	m_setter				(setter),
	inherited				(default_value, read_only, notify_parent, password, refresh_grid )
{
}

property_vec2f::~property_vec2f		()
{
	this->!property_vec2f	();
}

property_vec2f::!property_vec2f		()
{
}

float2 property_vec2f::get_value_raw	()
{
	return					(m_getter());
}

void property_vec2f::set_value_raw	(float2 value)
{
	m_setter				(value);
}