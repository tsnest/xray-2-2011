////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_VALUES_STORAGE_H_INCLUDED
#define OBJECT_VALUES_STORAGE_H_INCLUDED

#include "object_base.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

using xray::editor::wpf_controls::control_containers::button;

ref class tool_misc;

public ref class object_values_storage :public object_base
{
	typedef object_base	super;
	typedef System::Collections::Generic::List< System::String^ >	string_values_list;
public:
	object_values_storage	( tool_misc^ tool );
	virtual		void				load_contents			( ) override				{ };
	virtual		void				unload_contents			( bool ) override			{ };
	virtual		void				save					( configs::lua_config_value t ) override;
	virtual		void				load_props				( configs::lua_config_value const& t ) override;
	virtual		property_container^	get_property_container	( ) override;
	void							add_value				( System::String^ value );
	void							remove_value			( System::String^ value );
	System::Collections::Generic::IEnumerable< System::String^>^				get_values_by_type		( System::String^ type );

private:
	tool_misc^		m_tool_misc;

	bool			m_values_table_loaded;

	string_values_list^	m_values_list;

	void	on_edit_vaues_button_clicked( button^ );

public:
	[ CategoryAttribute("general") ]
	property System::String^	selected_value;
	
}; // class object_values_storage

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_VALUES_STORAGE_H_INCLUDED