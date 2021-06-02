////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_system_color.h"


property_system_color::property_system_color		(
									 color_getter_type^ getter,
									 color_setter_type^ setter
									 ) :
m_getter				(getter),
m_setter				(setter)
{
}

property_system_color::~property_system_color		()
{
	this->!property_system_color	();
}

property_system_color::!property_system_color		()
{
}

System::Object^ property_system_color::get_value	()
{
	color value  = m_getter();	
	return	System::Windows::Media::Color::FromScRgb(1, value.get_Rf(), value.get_Gf(), value.get_Bf() );
}

void property_system_color::set_value	(System::Object^ value)
{
	System::Windows::Media::Color c =  safe_cast<System::Windows::Media::Color>(value);
	m_setter				( xrayColor( color( c.ScR, c.ScG, c.ScB, c.ScA ) ) );
}
