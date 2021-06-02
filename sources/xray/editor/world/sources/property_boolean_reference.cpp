////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_boolean_reference.h"

property_boolean_reference::property_boolean_reference	(bool& value) :
	m_value					(NEW(value_holder<bool>)(value))
{
}

property_boolean_reference::~property_boolean_reference	()
{
	this->!property_boolean_reference	();
}

property_boolean_reference::!property_boolean_reference	()
{
	DELETE					(m_value);
}

System::Object ^property_boolean_reference::get_value	()
{
	return					(m_value->get());
}

void property_boolean_reference::set_value				(System::Object ^object)
{
	bool					value = safe_cast<bool>(object);
	m_value->set			(value);
}