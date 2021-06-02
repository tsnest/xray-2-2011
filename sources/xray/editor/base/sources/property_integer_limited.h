////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_INTEGER_LIMITED_HPP_INCLUDED
#define PROPERTY_INTEGER_LIMITED_HPP_INCLUDED

#include "property_integer.h"

public ref class property_integer_limited : public property_integer {
private:
	typedef property_integer	inherited;

public:
	typedef System::Object		Object;

public:
					property_integer_limited(
								integer_getter_type^ getter,
								integer_setter_type^ setter,
								int const %min,
								int const %max
							);
	virtual Object	^get_value				() override;
	virtual void	set_value				(System::Object ^object) override;

private:
	int				m_min;
	int				m_max;
}; // ref class property_integer_limited

#endif // ifndef PROPERTY_INTEGER_LIMITED_HPP_INCLUDED