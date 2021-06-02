////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.01.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_string_values_value_shared_str_getter.h"

typedef property_string_values_value_shared_str_getter::collection_type	collection_type;

property_string_values_value_shared_str_getter::property_string_values_value_shared_str_getter	(
		xray::editor::engine* engine,
		xray::shared_string& value,
		string_collection_getter_type^ collection_getter,
		string_collection_size_getter_type^ collection_size_getter
	) :
	inherited				(engine, value),
	m_collection_getter(collection_getter),
	m_collection_size_getter(collection_size_getter)
{
}

collection_type^ property_string_values_value_shared_str_getter::values							()
{
//.	LPCSTR const*			values = (*m_collection_getter)();

	collection_type^		collection = gcnew collection_type();
	u32 count				= m_collection_size_getter();
	for (u32 i=0; i<count; ++i)
	{
		System::String^ value = m_collection_getter(i);
		collection->Enqueue	( value );
	}

	return					(collection);
}