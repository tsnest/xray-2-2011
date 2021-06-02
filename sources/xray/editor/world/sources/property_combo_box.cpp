////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_combo_box.h"

using namespace System;
using namespace System::Collections;
using namespace Flobbster::Windows::Forms;

property_combo_box::property_combo_box	(
		string_getter_type^ getter,
		string_setter_type^ setter,
		ArrayList^ values,
		PropertySpec^ spec
	) :
	m_getter(getter),
	m_setter(setter)
	{
		combo_box_list = gcnew List<combo_box_property^>();
		
		Int32 index = -1;
		for each(String^ str in values)
			combo_box_list->Add(gcnew combo_box_property(++index, str));
		
		spec->Tag = combo_box_list;
	}

property_combo_box::~property_combo_box	()
{
	this->!property_combo_box				();
}

property_combo_box::!property_combo_box	()
{
}

ref class match_item_closure
{
public:
	match_item_closure(String^ str):
		value(str) { }

private:
	String^ value;

public:
	Boolean match_item(combo_box_property^ obj)
	{
		return obj->StringValue == value;
	}
};

Object^		property_combo_box::get_value	()
{
	String^ str = m_getter();
	combo_box_property^ return_combo;
	return_combo = combo_box_list->Find(
		gcnew Predicate<combo_box_property^>( gcnew match_item_closure( str ),
		&match_item_closure::match_item)
	);
	return return_combo;
}

void		property_combo_box::set_value	(Object^ object)
{
	String^ str = dynamic_cast<String^>(object);
	if(str != nullptr)
	{
		m_setter	( str );
		return;
	}
	
	combo_box_property^ prop = safe_cast<combo_box_property^>(object);
	if(prop->StringValue == String::Empty)
		prop = combo_box_list[0];
	m_setter	( prop->StringValue );
}
