////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_VEC3F_REFERENCE_HPP_INCLUDED
#define PROPERTY_VEC3F_REFERENCE_HPP_INCLUDED

#include "property_value_include.h"

public ref class property_vec3f_reference : public xray::editor::wpf_controls::i_property_value {

public:
					property_vec3f_reference	(	float3& value );

	virtual			~property_vec3f_reference	();
					!property_vec3f_reference	();
	virtual System::Object^		get_value		();
	virtual void				set_value		(System::Object^ value);

	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::Windows::Media::Media3D::Vector3D::typeid;}
	}

private:
	value_holder<float3>*	m_value;
}; // ref class property_vec3f_reference

#endif // ifndef PROPERTY_VEC3F_REFERENCE_HPP_INCLUDED