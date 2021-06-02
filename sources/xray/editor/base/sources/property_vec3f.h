////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_VEC3F_HPP_INCLUDED
#define PROPERTY_VEC3F_HPP_INCLUDED

using namespace System;
using namespace System::Reflection;

using System::Windows::Media::Media3D::Vector3D;
using System::Windows::Vector;

public ref class property_vec3f : public  xray::editor::wpf_controls::i_property_value{

public:
							property_vec3f	(
								float3_getter_type^ getter,
								float3_setter_type^ setter
								);
							property_vec3f	(
								Object^ obj,
								PropertyInfo^ prop_info
								);

	virtual property System::Type^			value_type
	{
		System::Type^ get() {return Vector3D::typeid;}
	}

	virtual System::Object^	get_value		();
	virtual void			set_value		(System::Object^ value);

private:
	float3_getter_type^		m_getter;
	float3_setter_type^		m_setter;
}; // ref class property_vec3f

//vec2f
public ref class property_vec2f : public  xray::editor::wpf_controls::i_property_value {

public:
							property_vec2f	(
								float2_getter_type^ getter,
								float2_setter_type^ setter
								);
							property_vec2f	(
								Object^ obj,
								PropertyInfo^ prop_info
								);

	virtual property System::Type^			value_type
	{
		System::Type^ get() {return Vector::typeid;}
	}

	virtual System::Object^	get_value		();
	virtual void			set_value		(System::Object^ value);

private:
	float2_getter_type^		m_getter;
	float2_setter_type^		m_setter;
}; // ref class property_vec2f

#endif // ifndef PROPERTY_VEC3F_HPP_INCLUDED