////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_INTEGER_ENUM_VALUE_HPP_INCLUDED
#define PROPERTY_INTEGER_ENUM_VALUE_HPP_INCLUDED

#include "property_integer.h"

public ref class property_integer_enum_value : public property_integer {
public:
	typedef Pair<int, System::String^>						ValuePair;

private:
	typedef property_integer								inherited;
	typedef System::Collections::ArrayList					collection_type;
	typedef System::Object									Object;
	typedef std::pair<int, LPCSTR>							pair;

public:
					property_integer_enum_value	(
						integer_getter_type^ getter,
						integer_setter_type^ setter,
						pair *values,
						u32 const &value_count
					);
	virtual Object	^get_value						() override;
	virtual void	set_value						(Object ^object) override;

public:
	collection_type^ m_collection;
}; // ref class property_integer_enum_value

#endif // ifndef PROPERTY_INTEGER_ENUM_VALUE_HPP_INCLUDED