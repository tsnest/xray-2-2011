////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_color_reference.h"

property_color_reference::property_color_reference	(
		color& value,
		array<System::Attribute^>^ attributes,
		property_holder::readonly_enum read_only,
		property_holder::notify_parent_on_change_enum notify_parent,
		property_holder::password_char_enum password,
		property_holder::refresh_grid_on_change_enum refresh_grid 
	) :
	m_value							(NEW(value_holder<color>)(value)),
	inherited						(	value, 
										attributes,
										read_only,
										notify_parent,
										password,
										refresh_grid )
{
}

property_color_reference::~property_color_reference	()
{
	this->!property_color_reference	();
}

property_color_reference::!property_color_reference	()
{
	DELETE						(m_value);
}

color property_color_reference::get_value_raw		()
{
	return							(m_value->get());
}

void property_color_reference::set_value_raw		(color value)
{
	m_value->set					(value);
}