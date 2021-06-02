////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROJECT_H_INCLUDED
#define PROJECT_H_INCLUDED

#include "project_items.h"

namespace xray {
namespace editor {

ref class project_tab;
ref class editor_cell_manager;
ref class link_storage;
ref class property_connection_editor;
ref class missing_resource_registry;
ref class project_item_folder;
ref class object_environment;
ref class object_sky;

typedef project_tab^ project_ui;

ref class link_temporary_data
{
public:
	System::String^ src_name;
	System::String^ src_member;
	object_base^	dst_obj;
	System::String^ dst_member;
};

ref class paste_temporary_data
{
public:
	System::String^ orig_name;
	object_base^	object;
};

public delegate void selection_changed		( );
public delegate void queried_object_loaded	( project_item_base^ obj );


public ref struct query_object
{
public:
	System::String^ full_name;
	queried_object_loaded^ d;
};

typedef System::Collections::Generic::List<link_temporary_data^>	temporary_link_list;
typedef System::Collections::Generic::List<paste_temporary_data^>	temporary_paste_list;
typedef System::Collections::Generic::List<query_object^>			query_objects_list;

public ref class project
{

public:
					project					( level_editor^ le );
					~project				( );
	void			load_project			( System::String^ path );
	void			load_project_config		( configs::lua_config_value const& cfg );
	void			load_items_config		( configs::lua_config_value const& cfg, bool load_ids, bool load_guid );
	void			save_project			( System::String^ path );
	void			save_intermediate		( );
	void			load_settings			( );
	void			save_settings			( );
	
	void			create_new				( );
	void			clear					( );
	bool			empty					( );
	pcstr			session_id				( );
	bool			name_assigned			( );
	
	bool			save_needed				( );
	void			set_changed				( );
	void			set_unchanged			( );
	void			set_modified_caption	( bool const is_modified );	

	System::String^ project_name				( );
	fs_new::virtual_path_string cfg_resource_name			( );
	fs_new::native_path_string	save_project_resources_path	( );
	fs_new::native_path_string	load_project_resources_path	( );
	
	System::String^ project_resources_intermediate_path	( );

	tool_base^		get_tool				( System::String^ tool_name );
	project_ui		ui						( );
	level_editor^	get_level_editor		( ) {return m_level_editor;}

	void			render					( );

	project_item_folder^ add_new_folder		( System::String^ folder_name, project_item_folder^ parent, u32 id );

	project_item_object^	add_new_item	(
						tool_base^ tool,
						System::String^ lib_name,
						float4x4 const& transform,
						System::String^ path,
						u32 set_object_id
					);
	
	void			remove					( System::String^ item_name, bool b_destroy_object );
	void			remove					( project_item_base^ pi, bool b_destroy_object );

	void			move					( System::String^ src_name, System::String^ dst_name );

	void			select_object			( project_item_base^ item, enum_selection_method method );
	void			select_objects			( project_items_list^ items, enum_selection_method method );
	void			select_id				( u32 id, enum_selection_method method );
	void			select_ids				( item_id_list^ objs, enum_selection_method method );
	void			extract_topmost_items	( string_list^ src_names, string_list^% dst_names );

	project_items_list^	get_all_items		( );
	project_items_list^	selection_list		( );
	u32				get_all_items_count		( );

	void			on_object_load			( object_base^ object, configs::lua_config_value const& t );
	void			on_object_save			( object_base^ object, configs::lua_config_value const& t );

//	void			on_project_links_loaded	(resources::queries_result& data);
	void			query_project_item		( System::String^ queried_item_full_name, queried_object_loaded^ d );
	void			process_project_items_query();
	void			on_item_added			( project_item_base^ itm );
	void			on_item_removed			( project_item_base^ itm );
	void			on_item_transform_changed( project_item_base^ itm, math::float3 const& prev_position );

	void			show_connection_editor	( );
	void			show_connection_editor	( object_base^ src, object_base^ dst );
	void			show_object_inspector	( ); // selected objects
	void			show_object_inspector	( project_items_list^ objects );
	project_item_base^	find_by_path		( System::String^ name, bool assert_on_fail );

	void			pin_selection			( );
	bool			get_pin_selection		( );

	bool			is_object_in_ignore_list( u32 id );
	void			set_object_ignore_list	( u32 id, bool value );
	void			export_as_obj			( bool selection_only );
	bool			track_active_item;
protected:
	void			load_project_impl		( );
	void			fill_tree_view			( tree_node_collection^ nodes );

	void			save_impl				( );
//	void			save_links				( System::String^ project_name );
//	void			load_links				( System::String^ project_name );
	void			on_selection_changed	( );

	void			on_config_created		( resources::queries_result& data );
	void			on_project_config_loaded( resources::queries_result& data );
	void			setup_intermediate_path	( execute_delegate^ execute_on_complete );
public:
	project_item_folder^	get_focused_folder( ) {return m_focused_folder ? m_focused_folder : m_root_folder;}
	void			set_focused_folder		( project_item_folder^ g);

	link_storage^	get_link_storage		( );
	void			subscribe_on_selection_changed( selection_changed^ d );
	project_item_folder^	root					( ) { return m_root_folder;}

	map_backgrounds% get_map_backgrounds	( ) {return m_map_backgrounds;}
	missing_resource_registry^				m_missing_resource_registry;
	editor_cell_manager^					m_editor_cell_manager;
	timing::timer*							m_project_loading_timer;

private:
	project_item_folder^					m_focused_folder;
	configs::lua_config_ptr*				m_config;
	project_item_folder^					m_root_folder;
	project_item_object^					m_root_scene;
	resources::mount_ptr*					m_project_mount;

	System::String^							m_project_name;
	System::String^							m_project_intermediate_path;

	level_editor^							m_level_editor;
	project_tab^							m_project_tab;
	project_items_list^						m_selection_list;
	item_id_list							m_ignore_list;

	query_objects_list^						m_objects_query;

	link_storage^							m_link_storage;
	map_backgrounds							m_map_backgrounds;

	event selection_changed^				m_selection_changed;
	temporary_link_list^					m_link_tmp_list;
	temporary_paste_list^					m_paste_tmp_list;
	bool									m_selection_pin;
	bool									m_intermediate_save;
	bool									m_changed;
public:
	property project_item_object^			root_scene
	{
		project_item_object^				get( ){ return m_root_scene; }
		void								set( project_item_object^ scene_item ) { ASSERT( m_root_scene == nullptr, "Root scene has been already set!" ); m_root_scene = scene_item; }
	}
}; // class project

void load_items_list(	project^ p, 
						configs::lua_config_value& cfg, 
						string_list^ loaded_names,
						item_id_list^ loaded_ids,
						bool use_absolute_path,
						bool load_ids,
						bool load_guid );

void extract_topmost_items_int(	project_items_list^ src_list, 
							project_items_list% dst_list );

} // namespace editor
} // namespace xray

#endif // #ifndef PROJECT_H_INCLUDED