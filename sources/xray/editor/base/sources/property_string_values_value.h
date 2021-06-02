////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_STRING_VALUES_VALUE_HPP_INCLUDED
#define PROPERTY_STRING_VALUES_VALUE_HPP_INCLUDED

#include "property_string.h"
#include "property_string_values_value_base.h"

public ref class property_string_values_value :
	public property_string,
	public property_string_values_value_base
{
private:
	typedef property_string								inherited;
	typedef property_string_values_value_base::collection_type					collection_type;

public:
	property_string_values_value	(
		string_getter_type^ getter,
		string_setter_type^ setter,
		System::Collections::ArrayList^ values
	);

	virtual	collection_type	^values	() {return m_collection;};

public:
	collection_type	^m_collection;
}; // ref class property_string_values_value

#endif // ifndef PROPERTY_STRING_VALUES_VALUE_HPP_INCLUDED