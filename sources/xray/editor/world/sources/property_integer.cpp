////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_integer.h"

property_integer::property_integer			(
		integer_getter_type^ getter,
		integer_setter_type^ setter
	) :
	m_getter				(getter),
	m_setter				(setter)
{
}

property_integer::~property_integer			()
{
	this->!property_integer	();
}

property_integer::!property_integer			()
{
}

System::Object ^property_integer::get_value	()
{
	return					(m_getter());
}

void property_integer::set_value			(System::Object ^object)
{
	m_setter				(safe_cast<int>(object));
}