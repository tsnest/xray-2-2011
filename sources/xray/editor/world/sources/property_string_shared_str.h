////////////////////////////////////////////////////////////////////////////
//	Created 	: 19.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_STRING_SHARED_STR_HPP_INCLUDED
#define PROPERTY_STRING_SHARED_STR_HPP_INCLUDED

#include "property_holder_include.h"

namespace editor {
	class engine;
} // namespace editor

public ref class property_string_shared_str : public xray::editor::controls::property_value {
public:
							property_string_shared_str	(xray::editor::engine* engine, xray::shared_string& value);
	virtual					~property_string_shared_str	();
							!property_string_shared_str	();
	virtual System::Object	^get_value					();
	virtual void			set_value					(System::Object ^object);

private:
	xray::editor::engine*			m_engine;
	xray::shared_string*	m_value;
}; // ref class property_string_shared_str

#endif // ifndef PROPERTY_STRING_SHARED_STR_HPP_INCLUDED