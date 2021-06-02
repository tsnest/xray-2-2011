////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_STRING_VALUES_VALUE_BASE_HPP_INCLUDED
#define PROPERTY_STRING_VALUES_VALUE_BASE_HPP_INCLUDED

public interface class property_string_values_value_base {
public:
	typedef System::Collections::Generic::Queue<System::String^>	collection_type;

public:
	virtual	collection_type	^values	() = 0;
}; // ref class property_string_values_value

#endif // ifndef PROPERTY_STRING_VALUES_VALUE_BASE_HPP_INCLUDED