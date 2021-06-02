////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_string.h"

property_string::property_string			(
		string_getter_type^ getter,
		string_setter_type^ setter
	) :
	m_getter(getter),
	m_setter(setter)
{
}

property_string::~property_string			()
{
	this->!property_string	();
}

property_string::!property_string			()
{
}

System::Object ^property_string::get_value	()
{
	return					( m_getter( ) );
}

void property_string::set_value			(System::Object ^object)
{
	m_setter				( safe_cast< System::String^ >( object ) );
}