////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_INTEGER_VALUES_VALUE_REFERENCE_HPP_INCLUDED
#define PROPERTY_INTEGER_VALUES_VALUE_REFERENCE_HPP_INCLUDED

#include "property_integer_reference.h"
#include "property_integer_values_value_base.h"

public ref class property_integer_values_value_reference :
	public property_integer_reference,
	public property_integer_values_value_base
{
private:
	typedef property_integer_reference							inherited;
	typedef System::Collections::ArrayList						collection_type;
	typedef System::Object										Object;
	typedef System::Collections::IList							IList;

public:
						property_integer_values_value_reference	(
							int& value,
							LPCSTR const* values,
							u32 const &value_count
						);
	virtual Object^		get_value								() override;
	virtual void		set_value								(Object ^object) override;
	virtual	IList^		collection								();

private:
	collection_type^	m_collection;
}; // ref class property_integer_values_value_reference

#endif // ifndef PROPERTY_INTEGER_VALUES_VALUE_REFERENCE_HPP_INCLUDED