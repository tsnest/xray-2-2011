////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_integer_limited.h"

property_integer_limited::property_integer_limited	(
		integer_getter_type^ getter,
		integer_setter_type^ setter,
		int const %min,
		int const %max
	) :
	inherited				(getter, setter),
	m_min					(min),
	m_max					(max)
{
}

System::Object ^property_integer_limited::get_value	()
{
	int						value = safe_cast<int>(inherited::get_value());
	if (value < m_min)
		value				= m_min;

	if (value > m_max)
		value				= m_max;

	return					(value);
}

void property_integer_limited::set_value			(System::Object ^object)
{
	int						new_value = safe_cast<int>(object);

	if (new_value < m_min)
		new_value			= m_min;

	if (new_value > m_max)
		new_value			= m_max;

	inherited::set_value	(new_value);
}