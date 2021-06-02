////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_STRING_VALUES_VALUE_SHARED_STR_HPP_INCLUDED
#define PROPERTY_STRING_VALUES_VALUE_SHARED_STR_HPP_INCLUDED

#include "property_string_shared_str.h"
#include "property_string_values_value_base.h"

public ref class property_string_values_value_shared_str :
	public property_string_shared_str,
	public property_string_values_value_base
{
private:
	typedef property_string_shared_str		inherited;
	typedef property_string_values_value_base::collection_type		collection_type;

public:
							property_string_values_value_shared_str	(
								xray::editor::engine* engine,
								xray::shared_string& value,
								LPCSTR const* values,
								u32 const &value_count
							);

	virtual	collection_type	^values									() {return m_collection;};

public:
	collection_type	^m_collection;
}; // ref class property_string_values_value_shared_str

#endif // ifndef PROPERTY_STRING_VALUES_VALUE_SHARED_STR_HPP_INCLUDED