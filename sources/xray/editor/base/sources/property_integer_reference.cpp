////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_integer_reference.h"

property_integer_reference::property_integer_reference			(int& value) :
	m_value		(NEW(value_holder<int>)(value))
{
}

property_integer_reference::~property_integer_reference			()
{
	this->!property_integer_reference	();
}

property_integer_reference::!property_integer_reference			()
{
	DELETE		(m_value);
}

System::Object ^property_integer_reference::get_value	()
{
	return		(m_value->get());
}

void property_integer_reference::set_value			(System::Object ^object)
{
	int			value = safe_cast<int>(object);
	m_value->set(value);
}