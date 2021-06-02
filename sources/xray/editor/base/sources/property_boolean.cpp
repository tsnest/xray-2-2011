////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_boolean.h"

property_boolean::property_boolean			(
		boolean_getter_type^ getter,
		boolean_setter_type^ setter
	) :
	m_getter				(getter),
	m_setter				(setter)
{
}

property_boolean::~property_boolean			()
{
	this->!property_boolean	();
}

property_boolean::!property_boolean			()
{
}

System::Object ^property_boolean::get_value	()
{
	return					(m_getter());
}

void property_boolean::set_value			(System::Object ^object)
{
	m_setter				(safe_cast<bool>(object));
}