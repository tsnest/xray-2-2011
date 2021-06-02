////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_JOB_H_INCLUDED
#define OBJECT_JOB_H_INCLUDED

#include "object_logic.h"

using namespace xray::editor::wpf_controls::hypergraph;
using namespace System;
using namespace System::Collections::Generic;

namespace xray {
namespace editor {


interface class i_job_resource;
ref class tool_scripting;
ref class job_behaviour_base;
ref class logic_event;

public delegate void queried_behaviour_loaded	( job_behaviour_base^ behaviour );
public ref struct query_behaviour
{
public:
	System::String^				behaviour_name;
	queried_behaviour_loaded^	delegate_on_loaded;
};


typedef xray::editor::wpf_controls::logic_view::logic_entity_view	wpf_logic_entity_view;
typedef System::Collections::Generic::List< job_behaviour_base^ >	behaviours_list;
typedef xray::editor::wpf_controls::hypergraph::properties_node		wpf_hypergpaph_node;
typedef xray::editor::wpf_controls::property_container				wpf_property_container;
typedef xray::editor::wpf_controls::property_descriptor				wpf_property_descriptor;
typedef System::Collections::Generic::List<query_behaviour^>		query_behaviours_list;



public ref class object_job : public object_logic
{
	typedef object_logic		super;
public:
	object_job				( tool_scripting^ t );
	virtual ~object_job		( );

	virtual void			load_props				( configs::lua_config_value const& t )	override;
	virtual void			save					( configs::lua_config_value t )			override;
	virtual void			render					( ) override;
	virtual void			on_selected				( bool bselected )						override;
	virtual void			on_before_manual_delete	( )	override;
	virtual void			show_logic				( ) override;
	
	void					initialize_logic_view	( );
	void					clear_logic_view		( );
	void					add_new_behaviour		( );
	void					remove_behaviour		( System::String^ behaviour_id );
	void					remove_link				( link^ lnk );


	virtual bool			is_job					() override { return true; }

	void					on_link_creating		(System::Object^, link_event_args^ e);
	void					select_resource_button_clicked	();

	void					on_job_resource_loaded	( project_item_base^ object );
	void					query_logic_behaviour	( System::String^ behaviour_name, queried_behaviour_loaded^ d );

	void					on_hypergraph_selecting	( System::Object^, selection_event_args^ e );


	void					logic_hypergraph_context_menu_opening	( );
	void					logic_hypergraph_menu_item_click		( System::Object^, menu_event_args^ args );
	void					on_logic_view_property_changed			( System::Object^, System::ComponentModel::PropertyChangedEventArgs^ e );

	bool					has_static_resource						( );

	System::Collections::Generic::List<System::String^>^ get_parent_scene_events_list	( );

	void					return_queried_behaviours				( );

	List<logic_event^>^		get_connected_logic_events();

				
private:
	project_item_base^		m_job_resource;		
	behaviours_list^		m_behaviours_list;
	wpf_logic_entity_view^	m_logic_view;
	query_behaviours_list^	m_behaviours_query;
	System::String^			m_start_behaviour_name;
	System::String^			m_start_event_name;
	System::String^			m_job_type;
	System::String^			m_static_resource_name;
	float					m_logic_view_height;

	Windows::Controls::MenuItem^	m_set_start_behaviour_item;
	Windows::Controls::MenuItem^	m_add_new_behaviour_item;		
	Windows::Controls::MenuItem^	m_delete_selected_item;	

private:
	property float		logic_view_height
	{
		void			set( float value ){ m_logic_view_height = value; }
		float			get() { return ( m_logic_view != nullptr ) ? (float)m_logic_view->logic_view_height : m_logic_view_height; }
	}

public:
	property String^	job_type
	{
		System::String^ get(){ return m_job_type; }
		void			set( System::String^ value );
	}
	property String^	start_behaviour_name
	{
		System::String^ get(){ return m_start_behaviour_name; }
		void			set( System::String^ value );
	}

	property String^	start_event_name
	{
		System::String^ get(){ return m_start_event_name; }
		void			set( System::String^ value ){ m_start_event_name = value; }
	}

	property behaviours_list^	behaviours
	{	
		behaviours_list^		get( ){ return m_behaviours_list;}
	}
	property wpf_logic_entity_view^	logic_view
	{
		wpf_logic_entity_view^		get( ) { return m_logic_view; }
	}

	property project_item_base^		selected_job_resource
	{
		project_item_base^			get( ){ return m_job_resource; }
	}





}; // class object_npc


} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_JOB_H_INCLUDED
