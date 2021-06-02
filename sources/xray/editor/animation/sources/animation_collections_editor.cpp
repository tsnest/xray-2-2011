////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_collections_editor.h"
#include "animation_collection_document.h"
#include "animation_files_view_panel.h"
#include "animation_collections_view_panel.h"
#include "animation_editor.h"
#include "animation_collections_document_base.h"
#include "animation_collection_document.h"
#include "animation_collection_item.h"
#include "animation_single_item.h"
#include "animation_model.h"
#include "animation_editor_form.h"

#pragma managed( push, off )
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

using namespace System::IO;

namespace xray {
namespace animation_editor {

using xray::editor::controls::document_editor_base;

animation_collections_editor::animation_collections_editor		( animation_editor^ ed ):
	m_scene		( NEW( render::scene_ptr ) )
{
	m_animation_editor = ed;
	components = gcnew System::ComponentModel::Container();
	AllowEndUserDocking = false;
	Size = System::Drawing::Size(300,300);
	Name = L"collections_editor_panel";
	Text = L"Collections";
	Padding = System::Windows::Forms::Padding(0);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	HideOnClose = true;
	DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(
		WeifenLuo::WinFormsUI::Docking::DockAreas::Document);

	m_instance_properties_panel	= gcnew item_properties_panel_base			( m_multidocument_base );
	m_multidocument_base		= gcnew animation_collections_document_base	( this );
	m_animations_view_panel		= gcnew animation_files_view_panel			( this );
	m_collections_view_panel	= gcnew animation_collections_view_panel	( this );
	
	m_multidocument_base->main_dock_panel->DockBackColor	= System::Drawing::SystemColors::AppWorkspace; 
	m_multidocument_base->creating_new_document				= gcnew xray::editor::controls::document_create_callback(this, &animation_collections_editor::on_document_creating);
	m_multidocument_base->content_reloading					= gcnew xray::editor::controls::content_reload_callback(this, &animation_collections_editor::find_dock_content);

	m_menu							= gcnew MenuStrip( );

	m_menu_file						= gcnew ToolStripMenuItem( );
	m_menu_file_save				= gcnew ToolStripMenuItem( );

	m_menu_edit						= gcnew ToolStripMenuItem( );
	m_menu_edit_undo				= gcnew ToolStripMenuItem( );
	m_menu_edit_redo				= gcnew ToolStripMenuItem( );

	m_menu_view						= gcnew ToolStripMenuItem( );
	m_menu_view_animations_tree		= gcnew ToolStripMenuItem( );
	m_menu_view_collections_tree	= gcnew ToolStripMenuItem( );
	m_menu_view_properties_panel	= gcnew ToolStripMenuItem( );

	m_menu->SuspendLayout( );
	// m_menu
	m_menu->Items->AddRange(gcnew cli::array<ToolStripItem^ >( 3 ) 
		{
			m_menu_file,
			m_menu_edit,
			m_menu_view
		}
	);
	m_menu->Location			= System::Drawing::Point( 0, 0 );
	m_menu->Name				= L"m_menu";
	m_menu->Size				= System::Drawing::Size( 292, 24 );
	m_menu->TabIndex			= 1;
	m_menu->Text				= L"m_menu";

	m_menu_file->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^ >( 1 )
		{
			m_menu_file_save
		} );
	m_menu_file->Name			= L"m_menu_file";
	m_menu_file->Size			= System::Drawing::Size( 197, 22 );
	m_menu_file->Text			= L"&File";

	m_menu_file_save->Name		= L"m_menu_file_save";
	m_menu_file_save->Size		= System::Drawing::Size( 197, 22 );
	m_menu_file_save->Text		= L"&Save";

	m_menu_edit->DropDownItems->AddRange( gcnew cli::array< System::Windows::Forms::ToolStripItem^ >( 2 )
		{
			m_menu_edit_undo,
			m_menu_edit_redo
		} );
	m_menu_edit->Name			= L"m_menu_edit";
	m_menu_edit->Size			= System::Drawing::Size( 197, 22 );
	m_menu_edit->Text			= L"&Edit";

	m_menu_edit_undo->Name		= L"m_menu_edit_undo";
	m_menu_edit_undo->Size		= System::Drawing::Size( 197, 22 );
	m_menu_edit_undo->Text		= L"&Undo";

	m_menu_edit_redo->Name		= L"m_menu_edit_redo";
	m_menu_edit_redo->Size		= System::Drawing::Size( 197, 22);
	m_menu_edit_redo->Text		= L"&Redo";

	m_menu_view->DropDownItems->AddRange( gcnew cli::array<ToolStripItem^ >( 3 ) 
		{
			m_menu_view_collections_tree,
			m_menu_view_animations_tree,
			m_menu_view_properties_panel
		} );
	m_menu_view->Name			= L"m_menu_view";
	m_menu_view->Size			= System::Drawing::Size( 37, 20 );
	m_menu_view->Text			= L"&View";
	m_menu_view->DropDownOpening += gcnew System::EventHandler( this, &animation_collections_editor::view_on_drop_down_opening );

	// m_menu_view_animations_tree
	m_menu_view_animations_tree->Name	= L"m_menu_view_animations_tree";
	m_menu_view_animations_tree->Size	= System::Drawing::Size(197, 22);
	m_menu_view_animations_tree->Text	= L"&Animations Tree";
	m_menu_view_animations_tree->Click	+= gcnew System::EventHandler( this, &animation_collections_editor::show_animations_tree );

	// m_menu_view_controllers_tree
	m_menu_view_collections_tree->Name	= L"m_menu_view_collections_tree";
	m_menu_view_collections_tree->Size	= System::Drawing::Size(197, 22);
	m_menu_view_collections_tree->Text	= L"&Collections tree";
	m_menu_view_collections_tree->Click	+= gcnew System::EventHandler( this, &animation_collections_editor::show_collections_tree );

	// m_menu_view_properties panel
	m_menu_view_properties_panel->Name	= L"m_menu_view_properties_panel";
	m_menu_view_properties_panel->Size	= System::Drawing::Size(197, 22);
	m_menu_view_properties_panel->Text	= L"&Properties";
	m_menu_view_properties_panel->Click	+= gcnew System::EventHandler( this, &animation_collections_editor::show_properties_panel );

	m_menu->ResumeLayout	( false );
	m_menu->PerformLayout	( );			
	m_multidocument_base->add_menu_items( m_menu );

	Controls->Add( m_multidocument_base );
	bool loaded = m_multidocument_base->load_panels( this );
	if( !loaded )
	{
		m_animations_view_panel	->Show		( m_multidocument_base->main_dock_panel );
		m_collections_view_panel->Show		( m_multidocument_base->main_dock_panel );
		m_instance_properties_panel->Show	( m_multidocument_base->main_dock_panel );
	}
	m_multidocument_base->Dock								= DockStyle::Fill;

	m_collections_items = gcnew Dictionary<String^, animation_collection_item^>( );
	m_single_items		= gcnew Dictionary<String^, animation_single_item^>( );
}

animation_collections_editor::~animation_collections_editor		( )
{
	if ( components )
		delete components;

	DELETE			( m_scene );
	delete			m_model;
}

void animation_collections_editor::tick ( )
{
	if( m_multidocument_base->active_document != nullptr )
		safe_cast<animation_collection_document^>( m_multidocument_base->active_document )->tick( );
}

animation_editor^ animation_collections_editor::get_animation_editor ( )
{
	return m_animation_editor;
}

animation_collection_document^ animation_collections_editor::active_document::get( )
{
	return safe_cast<animation_collection_document^>( m_multidocument_base->active_document );
}

bool animation_collections_editor::close_query					( )
{
	m_multidocument_base->save_panels( this );

	List<document_base^>^ documents = gcnew List<document_base^>( );
	documents->AddRange( m_multidocument_base->opened_documents );

	for each( document_base^ doc in documents )
	{
		if( !safe_cast<animation_collection_document^>( doc )->try_close( ) )
			return false;
	}

	return true;
}

IDockContent^ animation_collections_editor::find_dock_content	( String^ persist_string )
{
	if( persist_string == "xray.animation_editor.animation_files_view_panel" )
		return m_animations_view_panel;
	if( persist_string == "xray.animation_editor.animation_collections_view_panel" )
		return m_collections_view_panel;
	if( persist_string == "xray.editor.controls.item_properties_panel_base" )
		return m_instance_properties_panel;

	return ( nullptr );
}
xray::editor::controls::document_base^ animation_collections_editor::on_document_creating	( )
{
	return gcnew animation_collection_document( this );
}

void animation_collections_editor::view_on_drop_down_opening( System::Object^, System::EventArgs^ )
{
	m_menu_view_collections_tree->Checked = !(m_collections_view_panel->IsHidden);
	m_menu_view_animations_tree->Checked = !(m_animations_view_panel->IsHidden);
}

void animation_collections_editor::show_animations_tree( System::Object^, System::EventArgs^ )
{
	if(m_menu_view_animations_tree->Checked)
		m_animations_view_panel->Hide();
	else
		m_animations_view_panel->Show( m_multidocument_base->main_dock_panel );
}

void animation_collections_editor::show_collections_tree( System::Object^, System::EventArgs^ )
{
	if( m_menu_view_collections_tree->Checked )
		m_collections_view_panel->Hide();
	else
		m_collections_view_panel->Show( m_multidocument_base->main_dock_panel );
}

void animation_collections_editor::show_properties_panel		( Object^, EventArgs^ )
{
	if( m_menu_view_properties_panel->Checked )
		m_instance_properties_panel->Hide();
	else
		m_instance_properties_panel->Show( m_multidocument_base->main_dock_panel );
}

animation_single_item^ animation_collections_editor::get_single		( String^ single_name )
{
	animation_single_item^ result;
	m_single_items->TryGetValue( single_name, result );

	if( result == nullptr )
	{
		animation_single_item^ anim_item		= gcnew animation_single_item( this, single_name );
		anim_item->load							( );
		m_single_items->Add						( single_name, anim_item );

		return anim_item;
	}

	return result;
}

animation_collection_item^ animation_collections_editor::get_collection ( String^ collection_name )
{
	animation_collection_item^ result;
	m_collections_items->TryGetValue( collection_name, result );

	if( result == nullptr )
	{
		animation_collection_item^ anim_item	= gcnew animation_collection_item( this, collection_name );
		anim_item->load							( );
		m_collections_items->Add				( collection_name, anim_item );

		return anim_item;
	}

	return result;
}

animation_collection_item^ animation_collections_editor::new_collection ( String^ collection_name )
{
	R_ASSERT( !m_collections_items->ContainsKey( collection_name ) );
	
	animation_collection_item^ anim_item	= gcnew animation_collection_item( this, collection_name );
	m_collections_items->Add				( collection_name, anim_item );

	animation_collection_document^ doc		= gcnew animation_collection_document( this, anim_item );
	multidocument_base->add_document		( doc );
	doc->is_saved							= false;

	return anim_item;
}

void animation_collections_editor::rename_collection ( String^ old_name, String^ new_name )
{
	animation_collection_item^ item;
	m_collections_items->TryGetValue( old_name, item );

	if( item != nullptr )
	{
		m_collections_items->Remove	( old_name );
		m_collections_items->Add	( new_name, item );
		item->name					= new_name;
	}
}

void animation_collections_editor::remove_collection ( String^ collection_name )
{
	m_collections_items->Remove						( collection_name );
	m_collections_view_panel->tree_view->remove_item( collection_name );
}

animation_collection_item^ animation_collections_editor::find_collection ( String^ collection_name )
{
	animation_collection_item^ item;
	m_collections_items->TryGetValue( collection_name, item );

	return item;
}

void animation_collections_editor::show_properties ( animation_item^ item )
{
	m_instance_properties_panel->show_properties( item );
}

void animation_collections_editor::show_properties ( animation_item^ item, Boolean is_read_only )
{
	m_instance_properties_panel->show_properties( item, is_read_only );
}

void animation_collections_editor::show_properties ( array<animation_item^>^ items, Boolean is_read_only )
{
	m_instance_properties_panel->show_properties( items, is_read_only );
}

void animation_collections_editor::show_properties ( array<animation_item^>^ items )
{
	m_instance_properties_panel->show_properties( items );
}

void animation_collections_editor::clear_properties ( )
{
	m_instance_properties_panel->show_properties( nullptr );
}

void animation_collections_editor::update_properties ( )
{
	m_instance_properties_panel->update_properties( );
}

void animation_collections_editor::scene_created ( xray::render::scene_ptr const& scene )
{
	*m_scene					= scene;
	query_result_delegate* rq	= NEW(query_result_delegate)( gcnew query_result_delegate::Delegate( this, &animation_collections_editor::model_loaded ), g_allocator );
	m_model						= gcnew animation_model( *m_scene, m_animation_editor->get_renderer( ).scene( ), m_animation_editor->get_renderer( ).debug( ), "model_0", animation_editor::models_path, float4x4( ).identity( ), rq );
}

void animation_collections_editor::add_models_to_render ( )
{
	if( m_model != nullptr )
		m_model->add_to_render		( );
}

void animation_collections_editor::remove_models_from_render ( )
{
	if( m_model != nullptr )
		m_model->remove_from_render	( );
}

////////////////////////////////////////		 P R I V A T E				////////////////////////////////////////

bool animation_collections_editor::predicate_active_scene ( )
{
	if( multidocument_base->active_document == nullptr )
		return false;

	return true;
}

bool animation_collections_editor::predicate_save_scene ( )
{
	if( multidocument_base->active_document == nullptr || multidocument_base->active_document->is_saved )
		return false;

	return true;
}

bool animation_collections_editor::predicate_save_all ( )
{
	for each(document_base^ d in multidocument_base->opened_documents)
	{
		if(!d->is_saved)
			return true;
	}

	return false;
}

void animation_collections_editor::model_loaded ( xray::resources::queries_result& result )
{
	if( !result.is_successful( ) )
	{
		LOGI_INFO("animation_collection_editor", "MODEL LOADING FAILED!!!");
		return;
	}

	if( m_animation_editor->form->active_content == this )
		m_model->add_to_render( );
}

} //namespace animation_editor
} //namespace xray