////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_property_container.h"
#include "property_holder.h"

using System::Object;

property_property_container::property_property_container(xray::editor::property_holder_ptr object) 
{
	m_object = NEW(xray::editor::property_holder_ptr)(object);
}

Object^ property_property_container::get_value			()
{
	::property_holder*			real_value = dynamic_cast<::property_holder*>(&**m_object);

	return		((real_value)->container());
}

void property_property_container::set_value				(Object ^object)
{
	XRAY_UNREFERENCED_PARAMETER		( object );
	UNREACHABLE_CODE();
}