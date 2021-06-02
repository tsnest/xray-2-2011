////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_DYNAMIC_ENUM_H_INCLUDED
#define PROPERTY_DYNAMIC_ENUM_H_INCLUDED

#include "property_string.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace xray::editor::wpf_controls::property_grid_editor_types;
using namespace Flobbster::Windows::Forms;

ref class property_combo_box:public xray::editor::controls::property_value
{
public:
	property_combo_box	(
		string_getter_type^ getter,
		string_setter_type^ setter,
		ArrayList^ values,
		PropertySpec^ spec
	);
	virtual					~property_combo_box();
							!property_combo_box();
	virtual Object^			get_value		();
	virtual void			set_value		(Object ^object);

private:
	string_getter_type^		m_getter;
	string_setter_type^		m_setter;

private:
	List<combo_box_property^>^ combo_box_list;

}; // class property_dynamic_enum

#endif // #ifndef PROPERTY_DYNAMIC_ENUM_H_INCLUDED
