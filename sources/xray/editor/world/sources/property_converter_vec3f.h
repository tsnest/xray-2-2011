////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_CONVERTER_VEC3F_HPP_INCLUDED
#define PROPERTY_CONVERTER_VEC3F_HPP_INCLUDED

private ref class property_converter_vec3f : public System::ComponentModel::TypeConverter {
private:
	typedef System::ComponentModel::TypeConverter					inherited;
	typedef System::ComponentModel::PropertyDescriptorCollection	PropertyDescriptorCollection;
	typedef System::ComponentModel::ITypeDescriptorContext			ITypeDescriptorContext;
	typedef System::Object											Object;
	typedef System::Attribute										Attribute;
	typedef System::Globalization::CultureInfo						CultureInfo;
	typedef System::Type											Type;

public:
	typedef System::ComponentModel::TypeConverter::StandardValuesCollection	StandardValuesCollection;

public:
	virtual	PropertyDescriptorCollection^	GetProperties			(
												ITypeDescriptorContext^ context,
												Object^ value,
												array<Attribute^>^ attributes
											) override;
	virtual	bool							GetPropertiesSupported	(ITypeDescriptorContext^ context) override;

public:
	virtual	bool	CanConvertTo	(
						ITypeDescriptorContext^ context,
						Type^ destination_type
					) override;
	virtual Object^	ConvertTo		(
						ITypeDescriptorContext^ context,
						CultureInfo^ culture,
						Object^ value,
						Type^ destination_type
					) override;
	virtual	bool	CanConvertFrom	(
						ITypeDescriptorContext^ context,
						Type^ source_type
					) override;
	virtual Object^	ConvertFrom		(
						ITypeDescriptorContext^ context,
						CultureInfo^ culture,
						Object^ value
					) override;
}; // ref class property_converter_vec3f

// vec2f

private ref class property_converter_vec2f : public System::ComponentModel::TypeConverter {
private:
	typedef System::ComponentModel::TypeConverter					inherited;
	typedef System::ComponentModel::PropertyDescriptorCollection	PropertyDescriptorCollection;
	typedef System::ComponentModel::ITypeDescriptorContext			ITypeDescriptorContext;
	typedef System::Object											Object;
	typedef System::Attribute										Attribute;
	typedef System::Globalization::CultureInfo						CultureInfo;
	typedef System::Type											Type;

public:
	typedef System::ComponentModel::TypeConverter::StandardValuesCollection	StandardValuesCollection;

public:
	virtual	PropertyDescriptorCollection^	GetProperties			(
												ITypeDescriptorContext^ context,
												Object^ value,
												array<Attribute^>^ attributes
											) override;
	virtual	bool							GetPropertiesSupported	(ITypeDescriptorContext^ context) override;

public:
	virtual	bool	CanConvertTo	(
						ITypeDescriptorContext^ context,
						Type^ destination_type
					) override;
	virtual Object^	ConvertTo		(
						ITypeDescriptorContext^ context,
						CultureInfo^ culture,
						Object^ value,
						Type^ destination_type
					) override;
	virtual	bool	CanConvertFrom	(
						ITypeDescriptorContext^ context,
						Type^ source_type
					) override;
	virtual Object^	ConvertFrom		(
						ITypeDescriptorContext^ context,
						CultureInfo^ culture,
						Object^ value
					) override;
}; // ref class property_converter_vec3f

#endif // ifndef PROPERTY_CONVERTER_VEC3F_HPP_INCLUDED