////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_INTEGER_HPP_INCLUDED
#define PROPERTY_INTEGER_HPP_INCLUDED

public ref class property_integer : public xray::editor::wpf_controls::i_property_value {

public:
							property_integer	(
								integer_getter_type^ getter,
								integer_setter_type^ setter
							);
	virtual					~property_integer	();
							!property_integer	();
	virtual System::Object	^get_value			();
	virtual void			set_value			(System::Object ^object);
	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::Int32::typeid;}
	}
private:
	integer_getter_type		^m_getter;
	integer_setter_type		^m_setter;
}; // ref class property_integer

#endif // ifndef PROPERTY_INTEGER_HPP_INCLUDED