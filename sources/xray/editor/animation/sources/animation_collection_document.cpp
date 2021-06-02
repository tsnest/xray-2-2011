////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_collection_document.h"
#include "animation_collections_editor.h"
#include "animation_collection_item.h"
#include "animation_collection_document_commands.h"
#include "animation_editor.h"
#include "animation_model.h"
#include "animation_editor_form.h"

#include <xray/animation/api.h>
#include <xray/animation/mixing_expression.h>
#include <xray/animation/i_editor_mixer.h>

using xray::editor::controls::tree_node_type;
using System::Runtime::InteropServices::GCHandle;
using System::Drawing::Point;

namespace xray {
namespace animation_editor {

/////////////////////////////////////				I N I T I A L I Z E					///////////////////////////////////////////

animation_collection_document::animation_collection_document ( animation_collections_editor^ editor ):
	super						( editor->multidocument_base ),
	m_editor					( editor ),
	m_current_time				( 0 ),
	m_collection_node			( nullptr ),
	m_selected_animation_item	( nullptr ),
	m_selected_node				( nullptr ),
	m_loading_node				( nullptr )
{
	in_constructor( );

	m_collection		= nullptr;
}

animation_collection_document::animation_collection_document( animation_collections_editor^ editor, animation_collection_item^ collection ):
	super						( editor->multidocument_base ),
	m_editor					( editor ),
	m_current_time				( 0 ),
	m_collection_node			( nullptr ),
	m_selected_animation_item	( nullptr ),
	m_selected_node				( nullptr ),
	m_loading_node				( nullptr )
{
	in_constructor( );

	m_collection		= collection;
	m_loading_node->Tag = m_collection;
	Name				= m_collection->name;
	Text				= Name;

	resources_loaded	( nullptr, nullptr );

	is_saved					= false;
	m_first_save				= true;
}

void animation_collection_document::in_constructor ( )
{
	m_name				= "new collection";
	m_timer				= NEW(xray::timing::timer)( );
	m_first_save		= true;
	m_command_engine	= gcnew command_engine( 100 );

	SuspendLayout( );
	// m_save_file_dialog
	m_save_file_dialog = gcnew SaveFileDialog();
	m_save_file_dialog->AddExtension = true;
	m_save_file_dialog->DefaultExt = ".anim_collection";
	m_save_file_dialog->Filter = "Animation Collection Files|*.anim_collection";
	m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath( animation_editor::animation_collections_absolute_path );
	m_save_file_dialog->RestoreDirectory = true;
	m_save_file_dialog->SupportMultiDottedExtensions = true;
	m_save_file_dialog->Title = "Save collection";

	m_context_menu					= gcnew System::Windows::Forms::ContextMenuStrip( );
	m_context_menu_add_collection	= gcnew ToolStripMenuItem( );
	m_context_menu_remove			= gcnew ToolStripMenuItem( );

	// m_context_menu
	m_context_menu->SuspendLayout( );
	m_context_menu->Items->AddRange( gcnew cli::array<System::Windows::Forms::ToolStripItem^>( 2 )
		{
			m_context_menu_add_collection,
			m_context_menu_remove,
		});
	m_context_menu->Name					= L"m_context_menu";
	m_context_menu->Size					= System::Drawing::Size(153, 92);
	m_context_menu->Opened					+= gcnew EventHandler( this, &animation_collection_document::menu_opened );

	m_context_menu_remove->Name				= L"m_context_menu_remove";
	m_context_menu_remove->Size				= System::Drawing::Size(152, 22);
	m_context_menu_remove->Text				= L"Remove";
	m_context_menu_remove->Click			+= gcnew System::EventHandler( this, &animation_collection_document::remove_click );

	m_context_menu_add_collection->Name		= L"m_context_menu_add_collection";
	m_context_menu_add_collection->Size		= System::Drawing::Size(152, 22);
	m_context_menu_add_collection->Text		= L"Add new collection";
	m_context_menu_add_collection->Click	+= gcnew System::EventHandler( this, &animation_collection_document::add_collection_click );

	m_context_menu->ResumeLayout( false );

	// m_tree_view_image_list
	xray::editor_base::image_loader	loader;
	m_tree_view_image_list = loader.get_shared_images16( );
	m_tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;

	// m_tree_view
	m_tree_view = gcnew tree_view( );
	m_tree_view->Sorted = false;
	m_tree_view->TreeViewNodeSorter = nullptr;
	m_tree_view->HideSelection = false;
	m_tree_view->AllowDrop = true;
	m_tree_view->ContextMenuStrip = m_context_menu;
	m_tree_view->Dock = System::Windows::Forms::DockStyle::Fill;
	m_tree_view->ImageList = m_tree_view_image_list;
	m_tree_view->is_multiselect = true;
	m_tree_view->Name = L"m_tree_view";
	m_tree_view->TabIndex = 0;
	m_tree_view->DragOver += gcnew DragEventHandler( this, &animation_collection_document::drag_over );
	m_tree_view->DragDrop += gcnew DragEventHandler( this, &animation_collection_document::drag_drop );
	m_tree_view->MouseDown += gcnew MouseEventHandler( this, &animation_collection_document::document_mouse_down );
	m_tree_view->MouseMove += gcnew MouseEventHandler( this, &animation_collection_document::document_mouse_move );
	m_tree_view->selected_items_changed += gcnew EventHandler<xray::editor::controls::tree_view_selection_event_args^>(this, &animation_collection_document::selection_changed);

	m_loading_node = gcnew tree_node( );
	m_loading_node->Text = "loading...";
	m_tree_view->nodes->Add( m_loading_node );

	AllowEndUserDocking = false;
	AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	AutoScaleMode		= System::Windows::Forms::AutoScaleMode::Font;
	ClientSize			= System::Drawing::Size(562, 439);
	Controls->Add		( m_tree_view );
	VisibleChanged		+= gcnew System::EventHandler(this, &animation_collection_document::document_activated);
	ResumeLayout		( false);
	PerformLayout		( );

	m_editor->properties_panel->property_grid_control->property_value_changed += gcnew xray::editor::wpf_controls::property_grid::value_changed_event_handler( this, &animation_collection_document::property_changed );
}

animation_collection_document::~animation_collection_document ( )
{
	m_editor->active_model->animation_end -= gcnew animation_callback( this, &animation_collection_document::animation_ended );

	delete m_command_engine;
	delete m_save_file_dialog;
	delete m_tree_view;

	if ( components )
		delete components;

	if( m_editor->properties_panel->property_grid_control->selected_object == m_collection )
		m_editor->clear_properties( );

	m_editor->properties_panel->property_grid_control->property_value_changed -= gcnew xray::editor::wpf_controls::property_grid::value_changed_event_handler( this, &animation_collection_document::property_changed );

	DELETE( m_timer );
}

/////////////////////////////////////			P U B L I C   M E T H O D S				///////////////////////////////////////////

void animation_collection_document::save ( )
{
	if( is_saved )
		return;
	
	if( !m_collection->can_save( ) )
	{
		MessageBox::Show( 
				"Collection can not be saved. It has an empty interior collections that are not allowed.",
				"Invalid Collection"
				);
		return;
	}

	m_collection->save( );
	m_command_engine->set_saved_flag( );
	is_saved		= true;
	m_first_save	= false;
}

void animation_collection_document::load ( )
{
	m_collection = safe_cast<animation_collection_item^>( m_editor->get_collection( this->Name ) );
	m_loading_node->Tag = m_collection;
	if( m_collection->is_loaded )
		return resources_loaded( nullptr, nullptr );

	m_collection->loaded += gcnew EventHandler( this, &animation_collection_document::resources_loaded );
}

void animation_collection_document::undo ( )
{
	m_command_engine->rollback( -1 );
	is_saved = m_command_engine->saved_flag( );
}

void animation_collection_document::redo ( )
{
	m_command_engine->rollback				( 1 );
	is_saved = m_command_engine->saved_flag	( );
}

void animation_collection_document::copy ( bool del )
{
	XRAY_UNREFERENCED_PARAMETER( del );
}

void animation_collection_document::paste ( )
{

}

void animation_collection_document::select_all ( )
{
	m_tree_view->select_all( );
}

void animation_collection_document::del ( )
{
	remove_click( nullptr, nullptr );
}

void animation_collection_document::insert_items_into_collection ( List<String^>^ items_paths, animation_item_type type )
{
	insert_items_into_collection( items_paths, gcnew hierarchy_item_id( m_collection->items->Count ), type );
}

void animation_collection_document::insert_items_into_collection ( List<String^>^ items_paths, hierarchy_item_id^ destination, animation_item_type type )
{
	is_saved &= !m_command_engine->run( gcnew animation_collection_document_add_items_command( this, destination, items_paths, type ) );
}

void animation_collection_document::reorganize_nodes ( List<tree_node^>^ nodes_list, hierarchy_item_id^ destination )
{
	is_saved &= !m_command_engine->run( gcnew animation_collection_document_reorganize_command( this, destination, nodes_list ) );
}

Boolean animation_collection_document::try_close ( )
{
	if( query_close( ) )
	{
		m_is_in_try_close = true;
		Close( );
		m_is_in_try_close = false;
		
		return true;
	}
	return false;
}

/////////////////////////////////////			I N T E R N A L  M E T H O D S				///////////////////////////////////////////

void animation_collection_document::insert_item_into_collection ( hierarchy_item_id^ destination_id, animation_item^ item )
{
	tree_node^					parent_node		= get_parent_node_by_id( destination_id );
	animation_collection_item^	parent_item		= safe_cast<animation_collection_item^>( parent_node->Tag );

	parent_item->insert_item	( destination_id->last_index, item );
	parent_node->nodes->Clear	( );
	fill_tree_view				( parent_node, parent_item->items );

	parent_node->Expand			( );
}

void animation_collection_document::remove_item_from_collection ( hierarchy_item_id^ id )
{
	tree_node^					node			= get_node_by_id( id );

	tree_node^					parent_node		= safe_cast<tree_node^>( node->Parent );
	animation_collection_item^	parent_item		= safe_cast<animation_collection_item^>( parent_node->Tag );

	parent_node->nodes->RemoveAt( id->last_index );
	parent_item->remove_item_at	( id->last_index );
}

void animation_collection_document::rename_to ( String^ new_name, String^ old_name )
{
	String^ old_path	= animation_editor::animation_collections_absolute_path + "/" + old_name;
	String^ new_path	= animation_editor::animation_collections_absolute_path + "/" + new_name;
	
	m_editor->rename_collection	( old_name, new_name );
	if( System::IO::File::Exists( new_path + animation_collection_item::extension ) )
	{
		if( MessageBox::Show( 
				"File " + new_path + animation_collection_item::extension + " already exists. Do you want to overwrite it?",
				"File already exists",
				MessageBoxButtons::OKCancel, 
				MessageBoxIcon::Asterisk
				) == System::Windows::Forms::DialogResult::OK )
		{
			System::IO::File::Delete	( new_path + animation_collection_item::extension );
			System::IO::File::Move		( old_path + animation_collection_item::extension, new_path + animation_collection_item::extension );
		}
	}
	else
		System::IO::File::Move		( old_path + animation_collection_item::extension, new_path + animation_collection_item::extension );

	this->Text = new_name;
}

void animation_collection_document::update_properties ( )
{
	if( m_editor->properties_panel->property_grid_control->selected_object == m_collection )
		m_editor->properties_panel->property_grid_control->update( );
}

static void update_node_text_recursively( tree_node^ node )
{
	String^ name	= safe_cast<animation_item^>( node->Tag )->name;
	int ind			= name->LastIndexOf( "/" );
	if( ind > 0 )
		name = name->Remove( 0, ind + 1 );

	node->Text = name;

	for each( tree_node^ sub_node in node->nodes )
		update_node_text_recursively( sub_node );
}

void animation_collection_document::update_tree_names ( )
{
	update_node_text_recursively( m_collection_node );
}

void animation_collection_document::update_tree ( tree_node^ node, animation_collection_item^ item )
{
	if( node->Tag == item )
	{
		fill_tree_view( node, item->items );
		return;
	}

	for each( tree_node^ sub_node in node->nodes )
		update_tree( sub_node, item );
}

void animation_collection_document::tick ( )
{
	if( m_selected_animation_item == nullptr )
		return;

	u32 elapsed_ms_delta = m_timer->get_elapsed_msec( );
	m_timer->start( );
	m_current_time += elapsed_ms_delta;

	m_editor->active_model->tick		( m_current_time );
	m_editor->active_model->render		( );

	//vectora<editor_animations_event> events( g_allocator );
	//m_editor->active_model->calculate_animations_events( events );

	//vectora<editor_animations_event>::iterator   current	= events.begin( );
	//vectora<editor_animations_event>::iterator   end		= events.end( );

	//for ( ; current != end; ++current )
	//{
	//	if( current->type == editor_animations_event::finish_animation )
	//	{
	//		void* buffer_memory		= ALLOCA( animation_editor::expression_alloca_buffer_size );
	//		mutable_buffer buffer	( buffer_memory, animation_editor::expression_alloca_buffer_size );
	//		xray::animation::mixing::expression expr	( m_selected_animation_item->emit( buffer ) );

	//		m_editor->active_model->set_target	( expr, m_current_time );
	//		break;
	//	}
	//}
}

tree_node^ animation_collection_document::get_node_by_id ( hierarchy_item_id^ id )
{
	if( id->ids->Length == 0 )
		return m_collection_node;

	tree_node^ node = m_collection_node;

	Int32 count = id->ids->Length;
	for ( Int32 i = 0; i < count; ++i )
		node = safe_cast<tree_node^>( node->nodes[id->ids[i]] );

	return node;
}

tree_node^ animation_collection_document::get_parent_node_by_id ( hierarchy_item_id^ id )
{
	R_ASSERT( id->ids->Length > 0 );

	tree_node^ node = m_collection_node;

	Int32 count = id->ids->Length - 1;
	for ( Int32 i = 0; i < count; ++i )
		node = safe_cast<tree_node^>( node->nodes[id->ids[i]] );

	return node;
}

hierarchy_item_id^ animation_collection_document::get_id_from_node ( tree_node^ node )
{
	Int32 index_array [50];
	Int32 count = 0;

	while( node != m_collection_node )
	{
		index_array[count] = node->Index;
		++count;
		node = safe_cast<tree_node^>( node->Parent );
	}

	array<Int32>^ arr = gcnew array<Int32>( count );

	for ( Int32 i = 0; i < count; ++i )
		arr[i] = index_array[count - i - 1];

	return gcnew hierarchy_item_id( arr );
}

hierarchy_item_id^ animation_collection_document::get_id_for_new_child ( tree_node^ node )
{
	if( node == m_collection_node )
		return gcnew hierarchy_item_id( node->nodes->Count );

	Int32		index_array [50];
	Int32		count = 0;
	tree_node^	tmp_node = node;

	while( tmp_node != m_collection_node )
	{
		index_array[count] = tmp_node->Index;
		++count;
		tmp_node = safe_cast<tree_node^>( tmp_node->Parent );
	}

	array<Int32>^ arr = gcnew array<Int32>( count + 1 );

	for ( Int32 i = 0; i < count; ++i )
		arr[i] = index_array[count - i - 1];

	arr[count] = node->nodes->Count;

	return gcnew hierarchy_item_id( arr );
}

/////////////////////////////////////			P R O T E C T E D   M E T H O D S				///////////////////////////////////////////

void animation_collection_document::on_form_closing ( Object^ , System::Windows::Forms::FormClosingEventArgs^ e )
{
	if( m_is_in_try_close )
		return;

	e->Cancel = !query_close( );
	if( !e->Cancel )
		document_base::m_editor->on_document_closing( this );
}

/////////////////////////////////////			P R I V A T E  M E T H O D S				///////////////////////////////////////////

void animation_collection_document::property_changed ( Object^ , value_changed_event_args^ e )
{
	is_saved &= !m_command_engine->run( gcnew animation_collection_document_change_objects_property_command( this, get_id_from_node( m_selected_node ), e->changed_property->name, e->old_value, e->new_value ) );
}

void animation_collection_document::resources_loaded ( Object^, EventArgs^ )
{
	if( m_collection == nullptr || !m_collection->is_loaded )
		return;
	
	m_collection->loaded -= gcnew EventHandler( this, &animation_collection_document::resources_loaded );

	m_command_engine->clear_stack		( );
	m_command_engine->set_saved_flag	( );
	is_saved					= true;
	m_first_save				= false;
	m_tree_view->nodes->Clear	( );
	m_name						= this->Name;
	int ind						= m_name->LastIndexOf( "/" );
	if( ind > 0 )
		m_name						= m_name->Remove( 0, ind + 1 );

	m_collection_node			= m_tree_view->add_group( m_name, editor_base::folder_closed, editor_base::folder_open );
	m_collection_node->Tag		= m_collection;
	fill_tree_view				( m_collection_node, m_collection->items );
	m_collection_node->Expand	( );
}

void animation_collection_document::fill_tree_view ( tree_node^ root, items_list^ itms )
{
	m_tree_view->deselect_all( );
	root->nodes->Clear( );
	for each( animation_item^ itm in itms )
	{
		String^ name	= itm->name;
		int ind			= name->LastIndexOf( "/" );
		if( ind > 0 )
			name = name->Remove( 0, ind + 1 );
		
		tree_node^ node	= nullptr;
		if( itm->GetType( ) != animation_collection_item::typeid )
		{
			node			= m_tree_view->add_item( root, name, editor_base::node_resource );
			node->Tag		= itm;
		}
		else
		{
			node			= m_tree_view->add_group( root, name, editor_base::folder_closed, editor_base::folder_open );
			node->Tag		= itm;
			fill_tree_view	( node, safe_cast<animation_collection_item^>( itm )->items );
		}

		if( root != m_collection_node && !safe_cast<animation_collection_item^>( root->Tag )->is_internal_collection )
			node->ForeColor = System::Drawing::Color::Gray;
	}
}

void animation_collection_document::document_activated ( Object^, EventArgs^ )
{
	if( Visible )
	{
		m_editor->properties_panel->property_grid_control->property_value_changed += gcnew xray::editor::wpf_controls::property_grid::value_changed_event_handler( this, &animation_collection_document::property_changed );
		m_editor->active_model->reset( );
//		m_editor->clear_properties( );
		selection_changed( nullptr, nullptr );
	}
	else
	{
		m_editor->active_model->animation_end -= gcnew animation_callback( this, &animation_collection_document::animation_ended );
		m_editor->properties_panel->property_grid_control->property_value_changed -= gcnew xray::editor::wpf_controls::property_grid::value_changed_event_handler( this, &animation_collection_document::property_changed );
	}
}

void animation_collection_document::document_mouse_down ( Object^, MouseEventArgs^ e )
{
	m_drag_node = safe_cast<tree_node^>( m_tree_view->GetNodeAt( e->X, e->Y ) );
}

void animation_collection_document::document_mouse_move ( Object^, MouseEventArgs^ e )
{
	u32 count = m_tree_view->selected_nodes->Count;
	if( count > 0 && m_drag_node != nullptr )
	{
		if( ( e->Button & ::MouseButtons::Left ) == ::MouseButtons::Left )
		{
			List<tree_node^>^ list = gcnew List<tree_node^>( );
			for( u32 i = 0; i < count; ++i )
			{
				tree_node^ n = m_tree_view->selected_nodes[i];

				if( n == m_collection_node )
					continue;

				animation_collection_item^ parent_item = safe_cast<animation_collection_item^>( n->Parent->Tag );
				if( parent_item == m_collection || parent_item->is_internal_collection )
					list->Add( n );
			}

			IntPtr handle		= GCHandle::ToIntPtr( GCHandle::Alloc( list ) );
			DataObject^ data	= gcnew DataObject( "reorganize_nodes_list_handle", handle );
			
			if( ( ModifierKeys & Keys::Control ) == Keys::Control )
				m_tree_view->DoDragDrop( data, DragDropEffects::Copy );
			else
				m_tree_view->DoDragDrop( data, DragDropEffects::Move );

			GCHandle::FromIntPtr( IntPtr( handle ) ).Free( );
		}	
	}
}

void animation_collection_document::drag_over ( Object^, DragEventArgs^ e )
{
	if( m_collection_node == nullptr )
	{
		e->Effect = DragDropEffects::None;
		return;
	}

	Point client_mouse_position		= m_tree_view->PointToClient( Point( e->X, e->Y ) );
	tree_node^ dest_node			= safe_cast<tree_node^>( m_tree_view->GetNodeAt( client_mouse_position ) );



	if( dest_node != nullptr )
	{
		if( dest_node->Parent != nullptr &&
			dest_node->Parent != m_collection_node &&
			dot_net::is<animation_collection_item^>( dest_node->Parent->Tag ) &&
			!safe_cast<animation_collection_item^>( dest_node->Parent->Tag )->is_internal_collection )
		{
			e->Effect = DragDropEffects::None;
			return;
		}

		animation_collection_item^ item = dot_net::as<animation_collection_item^>( dest_node->Tag );
		if( item != nullptr && !item->is_internal_collection )
		{
			e->Effect = DragDropEffects::None;
			return;
		}
	}

	if( e->Data->GetDataPresent( "reorganize_nodes_list_handle" ) )
	{
		if( dest_node != nullptr )
		{
			List<tree_node^>^ list			= safe_cast<List<tree_node^>^>( GCHandle::FromIntPtr( safe_cast<IntPtr>( e->Data->GetData( "reorganize_nodes_list_handle" ) ) ).Target );

			animation_collection_item^ item = dot_net::as<animation_collection_item^>( dest_node->Tag );
			if( item != nullptr && item->is_internal_collection && list->Contains( dest_node ) )
			{
				e->Effect = DragDropEffects::None;
				return;
			}

			while( dest_node != m_collection_node )
			{
				if( list->Contains( dest_node->Parent ) )
				{
					e->Effect = DragDropEffects::None;
					return;
				}
				dest_node = safe_cast<tree_node^>( dest_node->Parent );
			}
		}

		e->Effect = e->AllowedEffect;
		return;
	}

	if( e->Data->GetDataPresent( "nodes_list_handle" ) )
	{
		e->Effect = DragDropEffects::Move;
		return;
	}

	e->Effect = DragDropEffects::None;
}

static Int32 comparison( tree_node^ left, tree_node^ right )
{
	u32		left_ids	[32];
	u32		right_ids	[32];
	s32		left_ids_count = -1;
	s32		right_ids_count = -1;

	while( left != nullptr )
	{
		left_ids[++left_ids_count] = left->Index;
		left = safe_cast<tree_node^>( left->Parent );
	}

	while( right != nullptr )
	{
		right_ids[++right_ids_count] = right->Index;
		right = safe_cast<tree_node^>( right->Parent );
	}

	u32 result = 0;

	while( left_ids_count >= 0 && right_ids_count >= 0 )
	{
		result = left_ids[left_ids_count--] - right_ids[right_ids_count--];

		if( result != 0 )
			return result;
	}

	return result;
}

void animation_collection_document::drag_drop ( Object^, DragEventArgs^ e )
{
	if( m_collection == nullptr )
	{
		e->Effect = DragDropEffects::None;
		MessageBox::Show( this, "Save document first, please!", "Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error );
		return;
	}

	Point client_mouse_position		= m_tree_view->PointToClient( Point( e->X, e->Y ) );
	tree_node^ dest_node			= safe_cast<tree_node^>( m_tree_view->GetNodeAt( client_mouse_position ) );

	hierarchy_item_id^ insert_id;

	if( dest_node == nullptr || dest_node == m_collection_node )
		insert_id		= gcnew hierarchy_item_id( m_collection_node->nodes->Count );
	else
	{
		if( xray::editor::wpf_controls::dot_net_helpers::is_type<animation_collection_item^>( dest_node->Tag ) )
		{
			insert_id	= get_id_for_new_child( dest_node );
		}
		else
		{
			insert_id	= get_id_from_node( dest_node );
			if( ( client_mouse_position.Y - dest_node->Bounds.Y ) > dest_node->Bounds.Height / 2 )
				++insert_id->last_index;
		}
	}

	if( e->Data->GetDataPresent( "reorganize_nodes_list_handle" ) )
	{
		List<tree_node^>^ list			= safe_cast<List<tree_node^>^>( GCHandle::FromIntPtr( safe_cast<IntPtr>( e->Data->GetData( "reorganize_nodes_list_handle" ) ) ).Target );

		list->Sort( gcnew System::Comparison<tree_node^>( &comparison ) );

		reorganize_nodes( list, insert_id );

		return;
	}

	if( e->Data->GetDataPresent( "nodes_list_handle" ) )
	{
		tree_node_type tnt = safe_cast<tree_node_type>( e->Data->GetData( "item_type" ) );
		if( tnt != tree_node_type::single_item )
		{
			e->Effect = DragDropEffects::None;
			return;
		}

		List<tree_node^>^ list			= safe_cast<List<tree_node^>^>( GCHandle::FromIntPtr( safe_cast<IntPtr>( e->Data->GetData( "nodes_list_handle" ) ) ).Target );
		List<String^>^ names_list		= gcnew List<String^>( );
		animation_item_type type		= (animation_item_type)(int)e->Data->GetData( "animation_item_type" );

		for each( tree_node^ drag_node in list )
		{
			R_ASSERT( drag_node != nullptr );
			R_ASSERT( drag_node->m_node_type == tree_node_type::single_item );
			
			String^ collection_name				= drag_node->FullPath;

			if( type == animation_item_type::animation_collection_item )
			{
				animation_collection_item^ item		= m_editor->get_collection( collection_name );
				if( item->is_loaded && !item->is_correct )
				{
					MessageBox::Show( 
						"Collection " + collection_name + " is incorrect. It has an empty interior collections that are not allowed.",
						"Invalid Collection"
					);
					return;
				}
			}

			names_list->Add( collection_name );
		}

		insert_items_into_collection( names_list, insert_id, type );

		Focus( );
		return;
	}
	
	e->Effect = DragDropEffects::None;
}

void animation_collection_document::selection_changed ( Object^, xray::editor::controls::tree_view_selection_event_args^ )
{
	if( m_tree_view->selected_nodes->Count == 0 )
		return m_editor->clear_properties( );

	Boolean is_read_only =
		m_tree_view->selected_nodes->Count > 1 ||
		!( xray::editor::wpf_controls::dot_net_helpers::is_type<animation_collection_item^>( m_tree_view->selected_nodes[0]->Tag ) ) ||
		(
			m_tree_view->selected_nodes[0]->Tag != m_collection &&
			!safe_cast<animation_collection_item^>( m_tree_view->selected_nodes[0]->Tag )->is_internal_collection
		);

	List<animation_item^>^ list = gcnew List<animation_item^>( );
	for each( tree_node^ node in m_tree_view->selected_nodes )
	{
		animation_item^ item = safe_cast<animation_item^>( node->Tag );
		
		if( item->is_loaded )
			list->Add( item );
	}

	if( list->Count == 0 )
	{
		m_editor->clear_properties( );
		return;
	}

	if( m_tree_view->selected_nodes->Count != 1 )
		is_read_only = true;

	array<animation_item^>^ arr		= gcnew array<animation_item^>( list->Count );
	list->CopyTo					( arr );
	m_editor->show_properties		( arr, is_read_only );

	if( m_tree_view->selected_nodes->Count == 1 )
	{
		m_selected_animation_item	= list[0];
		m_selected_node				= m_tree_view->selected_nodes[0];

		R_ASSERT( is_read_only || ( !is_read_only && m_selected_node != nullptr ) );

		if( !m_selected_animation_item->can_emit )
		{
			m_editor->active_model->animation_end -= gcnew animation_callback( this, &animation_collection_document::animation_ended );
			m_editor->active_model->reset( );
			return;
		}

		bool is_last_animation;
		void* buffer_memory		= ALLOCA( animation_editor::expression_alloca_buffer_size );
		mutable_buffer buffer	( buffer_memory, animation_editor::expression_alloca_buffer_size );
		xray::animation::mixing::expression expr	( m_selected_animation_item->emit( buffer, is_last_animation ) );

		m_editor->active_model->set_target	( expr, m_current_time );
		m_timer->start( );

		if( xray::editor::wpf_controls::dot_net_helpers::is_type<animation_collection_item^>( m_selected_animation_item ) )
			m_editor->active_model->animation_end += gcnew animation_callback( this, &animation_collection_document::animation_ended );
		else
			m_editor->active_model->animation_end -= gcnew animation_callback( this, &animation_collection_document::animation_ended );
	}
	else
	{
		m_selected_animation_item	= nullptr;
		m_selected_node				= nullptr;
	}

	R_ASSERT( is_read_only || ( !is_read_only && m_selected_node != nullptr ) );
}

void animation_collection_document::menu_opened ( Object^, EventArgs^ )
{
	m_context_menu_add_collection->Visible		= true;
	m_context_menu_remove->Visible				= true;
	if( m_tree_view->selected_nodes->Count == 0 )
		m_context_menu_remove->Visible = false;
	else
	{
		if( m_tree_view->selected_nodes->Count == 1 && m_tree_view->selected_nodes[0] == m_collection_node )
			m_context_menu_remove->Visible = false;

		if(		m_tree_view->selected_nodes[0]->m_node_type == tree_node_type::single_item ||
				(
					m_tree_view->selected_nodes[0] != m_collection_node &&
					!safe_cast<animation_collection_item^>( m_tree_view->selected_nodes[0]->Tag )->is_internal_collection 
				)
			)
			m_context_menu_add_collection->Visible = false;
	}
}

void animation_collection_document::remove_click ( Object^, EventArgs^ )
{
	if( m_tree_view->selected_nodes->Count == 0 )
		return;

	List<tree_node^>^ items_list = gcnew List<tree_node^>( m_tree_view->selected_nodes );
	items_list->Sort( gcnew System::Comparison<tree_node^>( &comparison ) );
	is_saved &= !m_command_engine->run( gcnew animation_collection_document_remove_items_command( this, items_list ) );
}

void animation_collection_document::add_collection_click ( Object^, EventArgs^ )
{
	if( m_collection == nullptr )
	{
		MessageBox::Show( this, "Save document first, please!", "Animation Editor", MessageBoxButtons::OK, MessageBoxIcon::Error );
		return;
	}

	tree_node^ node = m_collection_node;
	if( m_tree_view->selected_nodes->Count != 0 )
		node = m_tree_view->selected_nodes[0];

	is_saved &= !m_command_engine->run( gcnew animation_collection_document_create_collection_command( this, get_id_for_new_child( node ) ) );
}

tree_node^ animation_collection_document::get_node_by_tag ( TreeNodeCollection^ root, animation_item^ tag )
{
	tree_node^ res = nullptr;
	for each( tree_node^ n in root )
	{
		if( n->Tag == tag )
			return n;

		if( n->nodes->Count > 0 )
			res = get_node_by_tag( n->nodes, tag );

		if( res != nullptr )
			return res;
	}

	return nullptr;
}

void animation_collection_document::animation_ended ( animation::skeleton_animation_ptr const& , pcstr , u32 callback_time_in_ms, u8 )
{
	if( !m_selected_animation_item->can_emit )
		return;

	bool is_last_animation;
	void* buffer_memory		= ALLOCA( animation_editor::expression_alloca_buffer_size );
	mutable_buffer buffer	( buffer_memory, animation_editor::expression_alloca_buffer_size );
	xray::animation::mixing::expression expr	( m_selected_animation_item->emit( buffer, is_last_animation ) );

	m_editor->active_model->set_target	( expr, callback_time_in_ms );
	m_timer->start( );
}

Boolean animation_collection_document::query_close ( )
{
	if( is_saved )
		return true;

	if( !m_collection->can_save( ) )
	{
		if( m_first_save )
		{
			if( MessageBox::Show( 
				"Collection " + m_collection->name + " not saved and can not. It has an empty interior collections that are not allowed. Closing will delete it. \n Do you want to close and delete it?",
				"Invalid Collection",
				MessageBoxButtons::OKCancel,
				MessageBoxIcon::Asterisk
				) == System::Windows::Forms::DialogResult::OK )
			{
				String^ path				= animation_editor::animation_collections_absolute_path + "/" + m_collection->name + animation_collection_item::extension;
				
				m_editor->remove_collection	( m_collection->name );
				System::IO::File::Delete	( path );
				m_collection				= nullptr;

				return true;
			}
		}
		else
		{
			if( MessageBox::Show( 
				"Collection " + m_collection->name + " not saved and can not. It has an empty interior collections that are not allowed.\n Do you want to close it without saving?",
				"Invalid Collection",
				MessageBoxButtons::OKCancel,
				MessageBoxIcon::Asterisk
				) == System::Windows::Forms::DialogResult::OK )
			{
				return true;
			}
		}

		return false;
	}

	System::Windows::Forms::DialogResult result = MessageBox::Show( 
		"Collection " + m_collection->name + " not saved. \n Do you want to save it before close?",
		"Saving Collection",
		MessageBoxButtons::YesNoCancel,
		MessageBoxIcon::Asterisk
	);

	if( result == System::Windows::Forms::DialogResult::Yes )
	{
		save	( );
		return	true;
	}
	else if( result == System::Windows::Forms::DialogResult::No )
	{
		return	true;
	}

	return false;
}

} // namespace 
} // namespace xray