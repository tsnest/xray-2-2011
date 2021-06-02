////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.01.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_integer_values_value_getter.h"

using System::Collections::IList;
using System::Collections::ArrayList;
using System::Object;
using System::String;

property_integer_values_value_getter::property_integer_values_value_getter	(
		integer_getter_type^ getter,
		integer_setter_type^ setter,
		string_collection_getter_type^ collection_getter,
		string_collection_size_getter_type^ collection_size_getter
	) :
	inherited				(getter, setter),
	m_collection_getter		(collection_getter),
	m_collection_size_getter(collection_size_getter)
{
}

Object ^property_integer_values_value_getter::get_value						()
{
	int						value = safe_cast<int>(inherited::get_value());
	if (value < 0)
		value				= 0;

	int						count = collection()->Count;
	if (value >= count)
		value				= count - 1;

	return					(value);
}

void property_integer_values_value_getter::set_value						(Object ^object)
{
	String^					string_value = dynamic_cast<String^>(object);
	int						index = collection()->IndexOf(string_value);
	ASSERT					((index >= 0));
	inherited::set_value	(index);
}

IList^ property_integer_values_value_getter::collection						()
{
	ArrayList^				collection = gcnew ArrayList();

	u32 count				= m_collection_size_getter();
	for (u32 i=0; i<count; ++i)
	{
		collection->Add		(m_collection_getter(i));
	}

	return					(collection);
}