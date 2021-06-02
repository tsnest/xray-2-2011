////////////////////////////////////////////////////////////////////////////
//	Created		: 06.06.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "unique_name_helper.h"

namespace xray {
namespace model_editor {

void unique_name_helper::add ( System::String^ name )
{
	m_dictionary.Add( name, 0 );
}

void unique_name_helper::remove ( System::String^ name )
{
	m_dictionary.Remove( name );
}

System::String^ unique_name_helper::get_unique_name ( System::String^ name_template )
{
	int number = 0;
	System::String^ name = nullptr;
	do {
		name = name_template + number.ToString();
		++number;
	} while ( m_dictionary.ContainsKey( name ) );
	m_dictionary.Add( name, 0 );
	return name;
}

bool unique_name_helper::contains ( System::String^ name )
{
	return m_dictionary.ContainsKey( name );
}

void unique_name_helper::clear()
{
	m_dictionary.Clear();
}

} // namespace model_editor
} // namespace xray
