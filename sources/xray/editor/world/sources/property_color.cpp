////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_color.h"

property_color::property_color		(
		color_getter_type^ getter,
		color_setter_type^ setter,
		array<System::Attribute^>^ attributes,
		property_holder::readonly_enum read_only,
		property_holder::notify_parent_on_change_enum notify_parent,
		property_holder::password_char_enum password,
		property_holder::refresh_grid_on_change_enum refresh_grid
	) :
	m_getter				(getter),
	m_setter				(setter),
	inherited				(getter(), 
	attributes,
	read_only,
	notify_parent,
	password,
	refresh_grid)
{
}

property_color::~property_color		()
{
	this->!property_color	();
}

property_color::!property_color		()
{
}

color property_color::get_value_raw	()
{
	return					(m_getter());
}

void property_color::set_value_raw	(color value)
{
	m_setter				(value);
}