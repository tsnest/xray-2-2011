////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_integer_values_value_reference.h"

using System::String;
using System::Object;
using System::Collections::IList;

property_integer_values_value_reference::property_integer_values_value_reference	(
		int& value,
		LPCSTR const* values,
		u32 const &value_count
	) :
	inherited				(value),
	m_collection			(gcnew collection_type())
{
	for (u32 i=0; i<value_count; ++i)
		m_collection->Add	(gcnew System::String(values[i]));
}

System::Object ^property_integer_values_value_reference::get_value		()
{
	int						value = safe_cast<int>(inherited::get_value());
	if (value < 0)
		value				= 0;

	if (value >= m_collection->Count)
		value				= (int)m_collection->Count - 1;

	return					(value);
}

void property_integer_values_value_reference::set_value					(Object ^object)
{
	String^					string_value = dynamic_cast<String^>(object);
	int						index = m_collection->IndexOf(string_value);
	ASSERT					((index >= 0));
	inherited::set_value	(index);
}

IList^ property_integer_values_value_reference::collection				()
{
	return					(m_collection);
}