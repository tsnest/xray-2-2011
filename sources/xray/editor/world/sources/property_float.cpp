////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_float.h"

property_float::property_float			(
		float_getter_type^ getter,
		float_setter_type^ setter,
		float const% increment_factor
	) :
	m_getter				(getter),
	m_setter				(setter),
	m_increment_factor		(increment_factor)
{
}

property_float::~property_float			()
{
	this->!property_float	();
}

property_float::!property_float			()
{
}

System::Object ^property_float::get_value	()
{
	return					(m_getter());
}

void property_float::set_value			(System::Object ^object)
{
	m_setter				(safe_cast<float>(object));
}

void property_float::increment			(float const% increment)
{
	set_value				(safe_cast<float>(get_value()) + increment*m_increment_factor);
}