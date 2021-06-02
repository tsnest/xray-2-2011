////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECTS_LIBRARY_H_INCLUDED
#define OBJECTS_LIBRARY_H_INCLUDED

#include "project_defines.h"

namespace xray {
namespace editor {

ref struct lib_folder ;

public ref struct lib_item_base abstract
{
	System::String^			m_name;
	lib_folder^				m_parent_folder;

	virtual System::String^	full_path_name( ) = 0;
};

public ref struct lib_folder : public lib_item_base
{
	virtual System::String^			full_path_name			( ) override;
};

public ref struct lib_item: public lib_item_base
{
									lib_item				( configs::lua_config_value const& lv );
									lib_item				( );
									~lib_item				( );

	configs::lua_config_value*		m_config;
	virtual System::String^			full_path_name			( ) override;
	System::Guid					m_guid;
	int								m_item_type;
	int								m_image_index;								
};

typedef System::Collections::Generic::List<lib_item^>	library_items;
typedef System::Collections::Generic::List<lib_folder^>	library_folders;


public ref class objects_library 
{
public:
						objects_library				( );
						~objects_library			( );

	lib_folder^ 		add_new_folder				( System::String^ full_name );
	void				delete_folder_item			( lib_folder^ folder );

	lib_item^			add_simple					( System::String^ full_name );
	void				delete_library_item			( lib_item^ item );

	void				on_commit					( lib_item^ lib_item );
	lib_item^			new_lib_item				( System::String^ library_name );
	lib_item^			load_lib_item				( configs::lua_config_value const& t );
	
	lib_item^			get_library_item_by_full_path( System::String^ library_name, bool b_assert );
	lib_item^			get_library_item_by_guid	( System::Guid guid );
	lib_folder^			get_folder_item_by_full_path( System::String^ folder_full_path, bool b_create );
	lib_folder^			get_folder_item				( lib_folder^ parent, System::String^ folder_name );
	library_items^		get_all_library_tems		( );
	lib_item^			extract_library_item		( configs::lua_config_value const& cfg );
	
	void				load						( configs::lua_config* cfg );
	void				save_as						( pcstr filename );
	u32					items_count					( ) { return m_tool_library->Count;}
	void				fill_tree					( System::Windows::Forms::TreeNodeCollection^ root, tool_base^ tool, bool show_empty_folders, System::String^ filter );

	library_items^				m_tool_library;
	library_folders^			m_folders;
	configs::lua_config_ptr*	m_library_cfg;
protected:
	void				process_library_lodaing		( configs::lua_config_value t );

}; // class objects_library

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECTS_LIBRARY_H_INCLUDED