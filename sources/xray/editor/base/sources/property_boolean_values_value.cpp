////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_boolean_values_value.h"

using System::String;

property_boolean_values_value::property_boolean_values_value	(
		boolean_getter_type^ getter,
		boolean_setter_type^ setter,
		LPCSTR values[2]
	) :
	inherited				(getter, setter),
	m_collection			(gcnew collection_type())
{
	for (u32 i=0; i<2; ++i)
		m_collection->Add	(gcnew System::String(values[i]));
}

void property_boolean_values_value::set_value					(Object ^object)
{
	String^					string_value = dynamic_cast<String^>(object);
	int						index = m_collection->IndexOf(string_value);
	ASSERT					((index < 2));
	ASSERT					((index >= 0));
	inherited::set_value	((index == 1));
}