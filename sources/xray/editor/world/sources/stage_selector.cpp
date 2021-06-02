////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_selector.h"
#include "stage_selector_source.h"
#include "stage_selector_item_comparator.h"

namespace xray {
namespace editor {

void stage_selector::effect_selector_Load( Object^, System::EventArgs^ )
{
	stage_selector_source^ source			= gcnew stage_selector_source(m_tree_view);
	source->existing_items					= existing_items;
	m_tree_view->source						= source;
	m_tree_view->items_loaded				+= gcnew System::EventHandler(this, &stage_selector::effect_items_loaded);
	m_tree_view->NodeMouseDoubleClick		+= gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &stage_selector::m_tree_view_NodeMouseDoubleClick);
	source->refresh							();
}

void stage_selector::effect_items_loaded( Object^, System::EventArgs^ )
{
	int count						= m_tree_view->nodes->Count;
	resources::request*	arr			= XRAY_NEW_ARRAY_IMPL(*::xray::editor::g_allocator, resources::request, count );
	unmanaged_string** str_arr		= XRAY_NEW_ARRAY_IMPL(*::xray::editor::g_allocator, unmanaged_string*, count );
	for(int i = 0; i < count; ++i)
	{
		arr[i].id	= resources::lua_config_class;
		str_arr[i]	= NEW(unmanaged_string)("resources/stages/"+m_tree_view->nodes[i]->FullPath+".stage");
		arr[i].path	= str_arr[i]->c_str();
	}

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &stage_selector::stages_loaded), g_allocator);

	resources::query_resources(
		arr, 
		count, 
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
	);
	
	for(int i = 0; i < count; ++i)
	{
		DELETE(str_arr[i]);
	}
	XRAY_DELETE_ARRAY_IMPL(*::xray::editor::g_allocator, str_arr);
	XRAY_DELETE_ARRAY_IMPL(*::xray::editor::g_allocator, arr);
	

	if(!DesignMode && selected_item != nullptr && selected_item != "")
	{
		m_tree_view->track_active_node(selected_item);
	}
}

void stage_selector::stages_loaded( resources::queries_result& result )
{
	int count = result.size();

	for( int i = 0; i < count; ++i )
	{
		if( result[i].is_successful() )
		{
			configs::lua_config_ptr config			= static_cast_resource_ptr<configs::lua_config_ptr>( result[i].get_unmanaged_resource() );
			System::String^ node_name						= gcnew System::String( config->get_file_name() );
			int index								= node_name->IndexOf( "stages" ) + 7;
			int length								= node_name->LastIndexOf( '.' ) - index;
			node_name								= node_name->Substring( index, length );
			m_tree_view->get_node(node_name)->Tag	= (int)config->get_root()["parameters"]["order_id"];
		}
	}

	m_tree_view->TreeViewNodeSorter = gcnew stage_selector_item_comparator();
	m_tree_view->Sort();
}

void stage_selector::m_tree_view_NodeMouseDoubleClick( Object^, TreeNodeMouseClickEventArgs^ )
{
	ok_button_Click(nullptr, nullptr);
}

void stage_selector::ok_button_Click( Object^  ,System::EventArgs^ )
{
	if( m_tree_view->selected_node )
	{
		selected_item		= m_tree_view->selected_node->FullPath;
		this->DialogResult	= System::Windows::Forms::DialogResult::OK;
	}
}

void stage_selector::m_cancel_button_Click( Object^, System::EventArgs^ )
{
	Close();
}

} // namespace editor
} // namespace xray


