////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_VEC3F_REFERENCE_HPP_INCLUDED
#define PROPERTY_VEC3F_REFERENCE_HPP_INCLUDED

#include "property_vec3f_base.h"

public ref class property_vec3f_reference : public property_vec3f_base {
public:
	typedef property_vec3f_base							inherited;

public:
					property_vec3f_reference	(	float3& value,
													float3 const& default_value,
													property_holder::readonly_enum read_only,
													property_holder::notify_parent_on_change_enum notify_parent,
													property_holder::password_char_enum password,
													property_holder::refresh_grid_on_change_enum refresh_grid);

	virtual			~property_vec3f_reference	();
					!property_vec3f_reference	();
	virtual float3	get_value_raw				() override;
	virtual void	set_value_raw				(float3 value) override;

private:
	value_holder<float3>*	m_value;
}; // ref class property_vec3f_reference

#endif // ifndef PROPERTY_VEC3F_REFERENCE_HPP_INCLUDED