////////////////////////////////////////////////////////////////////////////
//	Created 	: 19.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_string_shared_str.h"

property_string_shared_str::property_string_shared_str			( xray::shared_string& value) :
	m_value					(&value)
{
}

property_string_shared_str::~property_string_shared_str			()
{
	this->!property_string_shared_str	();
}

property_string_shared_str::!property_string_shared_str			()
{
}

System::Object ^property_string_shared_str::get_value	()
{
	return					( gcnew System::String(m_value->c_str()));
}

void property_string_shared_str::set_value			(System::Object ^object)
{
	*m_value				= xray::editor_base::unmanaged_string( safe_cast< System::String^ >( object ) ).c_str( );
}
