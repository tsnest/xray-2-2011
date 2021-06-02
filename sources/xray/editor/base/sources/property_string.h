////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_STRING_HPP_INCLUDED
#define PROPERTY_STRING_HPP_INCLUDED

public ref class property_string : public xray::editor::wpf_controls::i_property_value {

public:
							property_string	(
								string_getter_type^ getter,
								string_setter_type^ setter
							);
	virtual					~property_string();
							!property_string();
	virtual System::Object	^get_value		();
	virtual void			set_value		(System::Object ^object);
	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::String::typeid;}
	}
private:
	string_getter_type^		m_getter;
	string_setter_type^		m_setter;
}; // ref class property_string

#endif // ifndef PROPERTY_STRING_HPP_INCLUDED