////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_color.h"

property_color::property_color		(
		color_getter_type^ getter,
		color_setter_type^ setter		
	) :
	m_getter				(getter),
	m_setter				(setter)
{
}

property_color::property_color		(
		Object^ obj,
		PropertyInfo^ prop_info
	)
{
	m_getter =	safe_cast<color_getter_type^>( System::Delegate::CreateDelegate(color_getter_type::typeid, obj, prop_info->GetGetMethod()));
	m_setter =	safe_cast<color_setter_type^>( System::Delegate::CreateDelegate(color_setter_type::typeid, obj, prop_info->GetSetMethod()));
}

Object^ property_color::get_value	()
{
	color value  = m_getter();	
	return	System::Windows::Media::Color::FromScRgb(1, value.get_Rf(), value.get_Gf(), value.get_Bf() );
}

void property_color::set_value		(Object^ value)
{
	System::Windows::Media::Color c =  safe_cast<System::Windows::Media::Color>(value);
	m_setter				( xrayColor( color( c.ScR, c.ScG, c.ScB, c.ScA ) ) );
}