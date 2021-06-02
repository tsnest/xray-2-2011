////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_SCENE_H_INCLUDED
#define OBJECT_SCENE_H_INCLUDED

#include "object_logic.h"

using namespace System;
using System::String;
using xray::editor::wpf_controls::property_container;
using xray::editor::wpf_controls::control_containers::button;
using namespace xray::editor::wpf_controls::hypergraph;

namespace xray {
	namespace editor {

using System::Windows::Controls::Button;

ref class tool_scripting;
ref class object_job;
ref class object_scene;
ref class logic_event;
ref class logic_editor_panel;
ref class logic_scene_event_properties_contaqiner;
ref class logic_scene_event_property_holder;


public delegate void on_child_added			( object_logic^ object );

public ref class object_scene : public object_logic 
{
	typedef Collections::Generic::List<logic_scene_event_property_holder^>	event_handlers_list;
	typedef xray::editor::wpf_controls::logic_view::logic_entity_view		wpf_logic_entity_view;
	typedef xray::editor::wpf_controls::hypergraph::properties_node			wpf_hypergpaph_node;
	typedef Collections::Generic::List<object_job^>							jobs_list;
	typedef Collections::Generic::List<object_scene^>						scenes_list;
	typedef Collections::Generic::List<logic_event^>						events_list;
	
	typedef object_logic super;

public:
	object_scene			(tool_scripting^ t, bool is_root_scene );
	virtual ~object_scene();
	
	virtual void			load_props				( configs::lua_config_value const& t )	override;
	virtual void			save					( configs::lua_config_value t )			override;
	virtual void			render					( ) override;
	virtual void			unload_contents			( bool ) override;
	//virtual bool			visible_for_project_tree( ) override { return !m_is_root_scene;}
	virtual bool			get_persistent			( ) override { return m_is_root_scene;}
	virtual void			on_selected				( bool bselected )						override;

	virtual void			on_before_manual_delete	( )	override;
	virtual void			show_logic				( ) override;
	void					initialize				( );

	property_container^		get_properties_container_for_parent_view	( );
	property_container^		get_properties_container_for_own_view		( );

	void					add_event_handler_to_parent_view_clicked	( button^ btn );
	void					add_event_handler_to_own_view_clicked		( button^ btn );
	
	void					remove_event_handler_from_parent_view_clicked	( button^ btn );
	void					remove_event_handler_from_own_view_clicked		( button^ btn );

	void					add_job					( project_item_object^	obj );
	object_logic^			add_child_from_cfg		( System::String^ child_guid, configs::lua_config_value const& cfg );
	void					add_scene				( project_item_object^ obj );
	void					add_event				( logic_event^ event );
	void					add_scene_to_logic_view	( object_scene^ scene, bool is_new_scene );
	void					remove_job				( object_job^	obj );
	void					remove_scene			( object_scene^ obj );
	void					queried_object_loaded	( project_item_base^ obj );
	logic_event^			get_event_by_name		( String^ event_name );	
	void					on_scene_link_creating	( System::Object^, link_event_args^ e );
	void					on_hypergraph_selecting	( System::Object^, selection_event_args^ e );
	void					logic_hypergraph_context_menu_opening	( );
	void					logic_hypergraph_menu_item_click		( System::Object^, menu_event_args^ args );
	void					remove_link				( link^ lnk );

	void					on_logic_view_closing	( System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e );
	
	System::Collections::Generic::IEnumerable<String^>^ get_children_by_type	(String^ children_type);

	System::Collections::Generic::List<String^>^ get_event_names_list	() 
	{	 
		System::Collections::Generic::List<String^>^ ret_lst = safe_cast<System::Collections::Generic::List<String^>^>(get_children_by_type("event"));
		//ret_lst->Insert(0,"#StartScene");
		return ret_lst;
	}

	virtual bool			is_scene				( ) override { return true; }
	
	virtual property project_item_object^	owner_project_item
	{
		virtual void						set( project_item_object^ pi ) override;
	}

private:
	jobs_list^						m_jobs_list;
	scenes_list^					m_sub_scenes_list;
	events_list^					m_events_list;
	event_handlers_list^			m_event_handlers;

	const bool						m_is_root_scene;

	logic_editor_panel^				m_own_logic_view;
	String^							m_finalizing_event_name;

	Windows::Controls::MenuItem^	m_add_new_scene_item;		
	Windows::Controls::MenuItem^	m_delete_selected_item;	

	float							m_scenes_view_height;
	System::Windows::Point			m_scene_node_position;

public:

	property logic_editor_panel^				own_logic_view
	{
		logic_editor_panel^						get(){ return m_own_logic_view; }
	}

	property System::Windows::Point	scene_node_position
	{
		System::Windows::Point		get(){ return ( own_logic_node_in_parent_view != nullptr ) ? own_logic_node_in_parent_view->position : m_scene_node_position ; }	
		void						set( System::Windows::Point value ) { m_scene_node_position = value; }
	}
	property float					scenes_view_height
	{
		float						get( );
	}
	property wpf_hypergpaph_node^	own_logic_node_in_parent_view;
	property wpf_hypergpaph_node^	own_logic_node_in_own_view;

	property jobs_list^				jobs
	{
		jobs_list^					get(){return m_jobs_list;};
	}

	property events_list^			events
	{
		events_list^				get(){return m_events_list;};
	}

	property scenes_list^			scenes
	{
		scenes_list^				get(){return m_sub_scenes_list;};
	}

	property event_handlers_list^	event_handlers
	{
		event_handlers_list^		get(){ return m_event_handlers; }
	}

	[DisplayNameAttribute("Finalizing Event")]
	property String^				finalizing_event_name
	{
		System::String^ get( ){
			return m_finalizing_event_name; 
		}
		void			set( System::String^ value )
		{
			m_finalizing_event_name = value; 
		}
	}
	

public:
	event on_child_added^	on_child_added;

}; // class object_scene


} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_SCENE_H_INCLUDED