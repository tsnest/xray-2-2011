////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_float_enum_value.h"

using System::String;

property_float_enum_value::property_float_enum_value	(
		float_getter_type^ getter,
		float_setter_type^ setter,
		pair *values,
		u32 const &value_count
	) :
	inherited				(getter, setter, .05f),
	m_collection			(gcnew collection_type())
{
	for (u32 i=0; i<value_count; ++i) {
		ValuePair^			pair = gcnew ValuePair();
		pair->first			= values[i].first;
		pair->second		= gcnew System::String(values[i].second);
		m_collection->Add	(pair);
	}
}

System::Object ^property_float_enum_value::get_value		()
{
	float						value = safe_cast<float>(inherited::get_value());
	for each (ValuePair^ i in m_collection) {
		if (i->first != value)
			continue;

		return				(value);
	}

	return					(safe_cast<ValuePair^>(m_collection[0])->first);
}

void property_float_enum_value::set_value					(Object ^object)
{
	String^					string_value = dynamic_cast<String^>(object);

	for each (ValuePair^ i in m_collection) {
		if (!i->second->Equals(string_value))
			continue;

		inherited::set_value(i->first);
		return;
	}

	inherited::set_value	(safe_cast<ValuePair^>(m_collection[0])->first);
}

void property_float_enum_value::increment					(float const% increment)
{
	XRAY_UNREFERENCED_PARAMETER		( increment );
}