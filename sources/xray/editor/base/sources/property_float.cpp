////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_float.h"

property_float::property_float			(
		float_getter_type^ getter,
		float_setter_type^ setter
	) :
	m_getter				(getter),
	m_setter				(setter)
{
}

property_float::property_float	(
		Object^ obj,
		PropertyInfo^ prop_info)
{
	m_getter = safe_cast<float_getter_type^>(System::Delegate::CreateDelegate(float_getter_type::typeid, obj, prop_info->GetGetMethod()));
	m_setter = safe_cast<float_setter_type^>(System::Delegate::CreateDelegate(float_setter_type::typeid, obj, prop_info->GetSetMethod()));
}

System::Object ^property_float::get_value	()
{
	return					(m_getter());
}

void property_float::set_value			(System::Object ^object)
{
	m_setter				(safe_cast<float>(object));
}

