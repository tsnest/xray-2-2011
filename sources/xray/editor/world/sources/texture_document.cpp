////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_document.h"
#include "texture_editor.h"
#include "texture_tga_to_argb_cook.h"
#include "texture_editor_resource.h"

using namespace	System::IO;
using namespace	System::Collections::Generic;
using namespace	System::Collections::ObjectModel;

using xray::editor::wpf_controls::property_grid::value_changed_event_args;
using xray::editor::wpf_controls::property_grid::value_changed_event_handler;
using xray::editor::wpf_controls::property_grid::host;
using xray::editor::wpf_controls::property_grid::property_grid_control;

using xray::editor::controls::tree_node;

namespace xray {
namespace editor {


typedef xray::resources::queries_result queries_result;

delegate void binded_query_callback(queries_result&, System::Object^);

ref class query_callback_binder
{
public:
	query_callback_binder(binded_query_callback^ callback, System::Object^ bind_data):
		m_callback(callback), m_bind_data(bind_data){}

	void callback(queries_result& data)
	{
		m_callback(data, m_bind_data);
	}
private:
	binded_query_callback^ m_callback;
	Object^ m_bind_data;
};

void register_texture_cook( )
{
	static texture_tga_to_argb_cook	texture_tga_to_argb_cook_obj;
	register_cook					( &texture_tga_to_argb_cook_obj );
}

void texture_document::in_constructor( )
{
	m_images						= gcnew System::Collections::Generic::Dictionary<int, Bitmap^>();
}

void texture_document::load( )
{
	controls::tree_view^ tree			= m_texture_editor->textures_panel->tree_view;
	u32 selected_count					= tree->selected_nodes->Count;

	bool is_multiselect = (selected_count > 1);

	if(is_multiselect)
	{
		m_picture_box->Image = nullptr;
	}else
	{
		unmanaged_string u_str = unmanaged_string("resources/textures/"+Name+".tga");

		m_is_tga_synk_called = true;

		binded_query_callback^ callback = gcnew binded_query_callback(this, &texture_document::tga_loaded);
		query_result_delegate* q		= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(gcnew query_callback_binder(callback, Name), &query_callback_binder::callback), g_allocator);

		m_tga_request_id = resources::query_resource(
			u_str.c_str(),
			xray::resources::raw_data_class,
			boost::bind(&query_result_delegate::callback, q, _1),
			g_allocator
		);

		m_is_tga_synk_called = false;
	}
}

void texture_document::view_selected_options( )
{
	NOT_IMPLEMENTED();
	//m_texture_editor->selected_resources->Clear();
	//for each( texture_editor_resource^ resource in m_options_to_show )
	//	m_texture_editor->selected_resources->Add( resource );

	//m_texture_editor->properties_panel->property_grid_control->read_only_mode = is_use_by_selector;

	//if(m_options_to_show->Count > 1)
	//{
	//	array<Object^>^ objects = gcnew array<Object^>(m_options_to_show->Count);
	//	
	//	for( int i = 0; i < m_options_to_show->Count; ++i )
	//		objects[i] = m_options_to_show[i]->m_property_container;
	//	
	//	m_texture_editor->properties_panel->property_grid_control->selected_objects = objects;
	//}else
	//{
	//	m_texture_editor->properties_panel->property_grid_control->selected_object = m_options_to_show[0]->m_property_container;
	//}

	//m_texture_editor->properties_panel->property_grid_control->Enabled	= true;
	//m_texture_editor->properties_panel->property_grid_control->update	( );
	//is_saved					= true;
}

void texture_document::save( )
{
	NOT_IMPLEMENTED();
	//for each(texture_editor_resource^ resource in m_options_to_show)
	//	resource->save		();

	is_saved			= true;
}

controls::document_base^ texture_document::create_document( )
{
	return gcnew texture_document(m_editor, m_texture_editor);
}

}//namespace editor
}//namespace xray