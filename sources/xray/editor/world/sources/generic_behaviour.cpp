////////////////////////////////////////////////////////////////////////////
//	Created		: 20.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "generic_behaviour.h"

namespace xray{
namespace editor{

generic_behaviour::generic_behaviour( System::String^ type )
:m_type( type )
{
	
}

wpf_controls::hypergraph::hypergraph_host^ generic_behaviour::get_behaviour_edit_view()
{
	wpf_controls::hypergraph::hypergraph_host^ result = gcnew wpf_controls::hypergraph::hypergraph_host( );

	wpf_controls::hypergraph::properties_node^ behaviour_properties_node = result->hypergraph->create_properties_node( "behaviour_properties_node" );
	wpf_controls::hypergraph::properties_node^ object_properties_node	 = result->hypergraph->create_properties_node( "object_properties_node" );
	
	behaviour_properties_node->input_link_place_enabled		= false;
	behaviour_properties_node->output_link_place_enabled	= false;
	object_properties_node->input_link_place_enabled		= false;
	object_properties_node->output_link_place_enabled		= false;


	result->hypergraph->link_creating		+= gcnew System::EventHandler< link_event_args^ >( this, &generic_behaviour::on_link_creating);

	//m_add_new_scene_item		= m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->add_context_menu_item( "Add new scene" );
	//m_delete_selected_item		= m_own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->add_context_menu_item( "Delete selected" );

	result->hypergraph->selecting			+= gcnew System::EventHandler<selection_event_args^>( this, &generic_behaviour::on_hypergraph_selecting );

	result->hypergraph->context_menu_opening+= gcnew System::Action(this, &generic_behaviour::on_hypergraph_context_menu_opening);
	result->hypergraph->menu_item_click		+= gcnew System::EventHandler<menu_event_args^>(this, &generic_behaviour::on_hypergraph_menu_item_click);

	return result;
}

}
}