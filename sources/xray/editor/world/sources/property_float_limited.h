////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_FLOAT_LIMITED_HPP_INCLUDED
#define PROPERTY_FLOAT_LIMITED_HPP_INCLUDED

#include "property_float.h"

public ref class property_float_limited : public property_float {
private:
	typedef property_float	inherited;

public:
					property_float_limited	(
						float_getter_type^ getter,
						float_setter_type^ setter,
						float const% increment_factor,
						float const %min,
						float const %max
					);
	virtual Object	^get_value				() override;
	virtual void	set_value				(System::Object ^object) override;

private:
	float			m_min;
	float			m_max;
}; // ref class property_float_limited

#endif // ifndef PROPERTY_FLOAT_LIMITED_HPP_INCLUDED