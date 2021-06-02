////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_INTEGER_VALUES_VALUE_BASE_HPP_INCLUDED
#define PROPERTY_INTEGER_VALUES_VALUE_BASE_HPP_INCLUDED

public interface class property_integer_values_value_base {
public:
	typedef System::Collections::IList	IList;

public:
	virtual	IList^	collection	() = 0;
}; // interface class property_integer_values_value_base

#endif // ifndef PROPERTY_INTEGER_VALUES_VALUE_BASE_HPP_INCLUDED