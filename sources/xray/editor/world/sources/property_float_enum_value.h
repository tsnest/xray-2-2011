////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_FLOAT_ENUM_VALUE_HPP_INCLUDED
#define PROPERTY_FLOAT_ENUM_VALUE_HPP_INCLUDED

#include "property_float.h"

public ref class property_float_enum_value : public property_float {
public:
	typedef Pair<float, System::String^>						ValuePair;

private:
	typedef property_float								inherited;
	typedef System::Collections::ArrayList					collection_type;
	typedef System::Object									Object;
	typedef std::pair<float, LPCSTR>							pair;

public:
					property_float_enum_value	(
						float_getter_type^ getter,
						float_setter_type^ setter,
						pair *values,
						u32 const &value_count
					);
	virtual Object	^get_value						() override;
	virtual void	set_value						(Object ^object) override;
	virtual void	increment						(float const% increment) override;

public:
	collection_type^ m_collection;
}; // ref class property_float_enum_value

#endif // ifndef PROPERTY_FLOAT_ENUM_VALUE_HPP_INCLUDED