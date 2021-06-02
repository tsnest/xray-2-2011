////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_vec3f_reference.h"

property_vec3f_reference::property_vec3f_reference	(
		float3& value,
		float3 const& default_value,
		property_holder::readonly_enum read_only,
		property_holder::notify_parent_on_change_enum notify_parent,
		property_holder::password_char_enum password,
		property_holder::refresh_grid_on_change_enum refresh_grid
	) :
	inherited					( default_value, read_only, notify_parent, password, refresh_grid ),
	m_value						(NEW(value_holder<float3>)(value))

{
}

property_vec3f_reference::~property_vec3f_reference	()
{
	this->!property_vec3f_reference	();
}

property_vec3f_reference::!property_vec3f_reference	()
{
	DELETE							(m_value);
}

float3 property_vec3f_reference::get_value_raw		()
{
	return							(m_value->get());
}

void property_vec3f_reference::set_value_raw		(float3 value)
{
	m_value->set					(value);
}