////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_color_reference.h"

property_color_reference::property_color_reference	( color& value ) :
	m_value		(NEW(value_holder<color>)(value))	
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

Object^ property_color_reference::get_value		()
{
	color value  = m_value->get();
	return	System::Windows::Media::Color::FromScRgb(1, value.get_Rf(), value.get_Gf(), value.get_Bf() );
}

void property_color_reference::set_value		(Object^ value)
{
	System::Windows::Media::Color c =  safe_cast<System::Windows::Media::Color>(value);
	m_value->set					( color( c.ScR, c.ScG, c.ScB, c.ScA ) );
}