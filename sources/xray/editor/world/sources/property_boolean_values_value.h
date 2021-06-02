////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_BOOLEAN_VALUES_VALUE_HPP_INCLUDED
#define PROPERTY_BOOLEAN_VALUES_VALUE_HPP_INCLUDED

#include "property_boolean.h"

public ref class property_boolean_values_value : public property_boolean {

private:
	typedef property_boolean								inherited;
	typedef System::Collections::ArrayList					collection_type;
	typedef System::Object									Object;

public:
					property_boolean_values_value	(
						boolean_getter_type^ getter,
						boolean_setter_type^ setter,
						LPCSTR values[2]
					);
	virtual void	set_value						(Object ^object) override;

public:
	collection_type	^m_collection;
}; // ref class property_boolean_values_value

#endif // ifndef PROPERTY_BOOLEAN_VALUES_VALUE_HPP_INCLUDED