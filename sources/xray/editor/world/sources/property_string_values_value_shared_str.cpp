////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_string_values_value_shared_str.h"

property_string_values_value_shared_str::property_string_values_value_shared_str	(
		xray::editor::engine* engine,
		xray::shared_string& value,
		LPCSTR const* values,
		u32 const &value_count
	) :
	inherited				(engine, value),
	m_collection			(gcnew collection_type())
{
	for (u32 i=0; i<value_count; ++i)
		m_collection->Enqueue	(gcnew System::String(values[i]));
}
