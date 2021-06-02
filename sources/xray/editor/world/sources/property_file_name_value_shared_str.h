////////////////////////////////////////////////////////////////////////////
//	Created 	: 19.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_FILE_NAME_VALUE_SHARED_STR_HPP_INCLUDED
#define PROPERTY_FILE_NAME_VALUE_SHARED_STR_HPP_INCLUDED

#include "property_string_shared_str.h"
#include "property_file_name_value_base.h"

public ref class property_file_name_value_shared_str :
	public property_string_shared_str,
	public property_file_name_value_base
{
private:
	typedef property_string_shared_str						inherited;

public:
							property_file_name_value_shared_str	(
								xray::editor::engine* engine,
								xray::shared_string& value,
								System::String^	DefaultExt_,
								System::String^	Filter_,
								System::String^	InitialDirectory_,
								System::String^	Title_,
								bool			remove_extension
							);

	virtual	System::String^	default_extension					();
	virtual	System::String^	filter								();
	virtual	System::String^	initial_directory					();
	virtual	System::String^	title								();
	virtual	bool			remove_extension					();

public:
	System::String^			DefaultExt;
	System::String^			Filter;
	System::String^			InitialDirectory;
	System::String^			Title;
	bool					m_remove_extension;
}; // ref class property_file_name_value_shared_str

#endif // ifndef PROPERTY_FILE_NAME_VALUE_SHARED_STR_HPP_INCLUDED