////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright ( C ) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "gui_binder.h"
#include "input_engine.h"
#include "action.h"

using namespace System;
using namespace Windows::Forms;
using namespace Collections;
using System::Windows::Forms::ImageList;
using xray::editor_base::action;

namespace xray {	
namespace editor_base {

///////////////////////////////////////////			I N I T I A L I Z E			//////////////////////////////////////////////////

gui_binder::gui_binder	( input_engine^ input_engine ):
m_input_engine			( input_engine )
{
	ASSERT( m_input_engine != nullptr );
}

///////////////////////////////////////			S T A T I C   M E T H O D S			///////////////////////////////////////////////

static void add_item_to_collection( ToolStripItemCollection^ collection, ToolStripItem^ item, int item_prio )
{
	item_prio = math::min( item_prio, collection->Count );
	collection->Insert( item_prio, item );
}

static ToolStripMenuItem^ get_parent_menu_item( MenuStrip^ strip, String^ key )
{
	ToolStripMenuItem^ item		= nullptr;
	int idx						= strip->Items->IndexOfKey( key );
	ASSERT						( idx!=-1, "menu item not found %s", unmanaged_string( key ).c_str( ) );
	item						= safe_cast<ToolStripMenuItem^>( strip->Items[idx] );
	return						item;
}

///////////////////////////////////////			P U B L I C   M E T H O D S			///////////////////////////////////////////////

void gui_binder::add_action_menu_item( MenuStrip^ strip, String^ action_name, String^ strip_key, u32 prio )
{
	ToolStripMenuItem^ parent_menu_item = get_parent_menu_item( strip, strip_key );

	ToolStripMenuItem^ menu_item		= gcnew ToolStripMenuItem;
	menu_item->Name						= action_name;
	add_item_to_collection				( parent_menu_item->DropDownItems, menu_item, prio );
	parent_menu_item->DropDownOpening	-= gcnew EventHandler( this, &gui_binder::gui_menu_drop_down );
	parent_menu_item->DropDownOpening	+= gcnew EventHandler( this, &gui_binder::gui_menu_drop_down );
	bind								( menu_item, action_name );
}

void gui_binder::remove_action_menu_item( MenuStrip^ strip, String^ action_name, String^ strip_key )
{
	ToolStripMenuItem^ parent_menu_item = get_parent_menu_item( strip, strip_key );

	int menu_item_index					= parent_menu_item->DropDownItems->IndexOfKey( action_name );
	ToolStripMenuItem^ menu_item		= safe_cast<ToolStripMenuItem^>( parent_menu_item->DropDownItems[menu_item_index] );

	unbind								( menu_item );
	parent_menu_item->DropDownItems->Remove( menu_item );
}

void gui_binder::add_menu_item( 	MenuStrip^ strip, String^ strip_key, String^ text, u32 prio )
{
	int idx						= strip->Items->IndexOfKey( strip_key );
	ASSERT_U					( idx==-1, "menu item already exist %s", unmanaged_string( strip_key ).c_str( ) );

	ToolStripMenuItem^ menu_item= nullptr;
	menu_item					= gcnew ToolStripMenuItem;
	menu_item->Name				= strip_key;
	menu_item->Text				= text;
	add_item_to_collection		( strip->Items, menu_item, prio );
}

void gui_binder::bind( ToolStripMenuItem^ menu_item, String^ action_name )
{
	action^ a = m_input_engine->get_action( action_name );
	if( a == nullptr )
	{
		ASSERT( false );
		return;
	}
	menu_item->Text		= a->name( );
	menu_item->Tag		= a;
	menu_item->Click	+= gcnew EventHandler( this, &gui_binder::gui_control_click );
	menu_item->Paint	+= gcnew PaintEventHandler( this, &gui_binder::gui_menu_item_paint );
	m_menu_items.Add	( menu_item, action_name );
}

void gui_binder::bind ( ToolStripMenuItem^ menu_item, action^ action )
{
	if( action == nullptr )
	{
		ASSERT( false );
		return;
	}
	menu_item->Text		= action->name( );
	menu_item->Tag		= action;
	menu_item->Click	+= gcnew EventHandler( this, &gui_binder::gui_control_click );
	menu_item->Paint	+= gcnew PaintEventHandler( this, &gui_binder::gui_menu_item_paint );
	m_menu_items.Add	( menu_item, action->name( ) );
}

void gui_binder::bind( ToolStripButton^ button, String^ action_name )
{
	action^ a = m_input_engine->get_action( action_name );
	if( a == nullptr )
	{
		ASSERT( false );
		return;
	}

	button->Text			= a->name( );
	button->Tag				= a;
	button->Click += gcnew EventHandler( this, &gui_binder::gui_control_click );

	m_buttons.Add( button, action_name );
}

void gui_binder::bind ( ToolStripButton^ button, action^ action )
{
	if( action == nullptr )
	{
		ASSERT( false );
		return;
	}
	
	button->Text		= action->name( );
	button->Tag			= action;
	button->Click		+= gcnew EventHandler( this, &gui_binder::gui_control_click );

	m_buttons.Add( button, action->name( ) );
}

void gui_binder::unbind( ToolStripMenuItem^ menu_item )
{
	menu_item->Click	-= gcnew EventHandler( this, &gui_binder::gui_control_click );
	menu_item->Paint	-= gcnew PaintEventHandler( this, &gui_binder::gui_menu_item_paint );
	m_menu_items.Remove	( menu_item );
}

void gui_binder::unbind( ToolStripButton^ button )
{
	button->Click -= gcnew EventHandler( this, &gui_binder::gui_control_click );
	m_buttons.Remove( button );
}

void gui_binder::update_menu_item( ToolStripMenuItem^ menu_item )
{
	ASSERT					( m_menu_items.Contains( menu_item ) );
	action^ a				= safe_cast<action^>( menu_item->Tag );

	menu_item->Enabled		= a->enabled( );

	xray::editor_base::checked_state item_check_state	= a->checked_state( );
	menu_item->Checked		= ( item_check_state != xray::editor_base::checked_state::uncheckable );

	if( item_check_state != xray::editor_base::checked_state::uncheckable )
	{
		menu_item->Checked		= true;
		menu_item->CheckState	= ( item_check_state == xray::editor_base::checked_state::checked )	?
									CheckState::Checked : 
									CheckState::Unchecked; 
	}else
		menu_item->Checked		= false;
}

void gui_binder::update_button( ToolStripButton^ button_item )
{
	ASSERT					( m_buttons.Contains( button_item ) );
	action^ a				= safe_cast<action^>( button_item->Tag );
	update_button			( button_item, a );
}

void gui_binder::update_button( ToolStripButton^ button_item, String^ action_name )
{
	XRAY_UNREFERENCED_PARAMETER( action_name );
	action^ a				= safe_cast<action^>( button_item->Tag );

	button_item->Enabled	= a->enabled( );
	xray::editor_base::checked_state item_check_state	= a->checked_state( );
	button_item->Checked	= ( item_check_state == xray::editor_base::checked_state::checked );
}

void gui_binder::update_button ( ToolStripButton^ button_item, action^ action )
{
	button_item->Enabled	= action->enabled( );
	xray::editor_base::checked_state item_check_state	= action->checked_state( );
	button_item->Checked	= ( item_check_state == xray::editor_base::checked_state::checked );
}

void gui_binder::update_items( )
{
	for each ( Collections::DictionaryEntry^ i in m_buttons ) 
	{
		String^ action_name				= safe_cast<String^>			( i->Value );
		ToolStripButton^ button			= safe_cast<ToolStripButton^>	( i->Key );
		update_button					( button, action_name );
	}
}

void gui_binder::gui_menu_drop_down( Object^ sender, EventArgs^ )
{
	ToolStripMenuItem^ parent_item = safe_cast<ToolStripMenuItem^>( sender );
	
	for each ( ToolStripItem^ sub_item in parent_item->DropDownItems )
	{
		if( sub_item->GetType( ) == ToolStripMenuItem::typeid )
		if( m_menu_items.Contains( sub_item ) )
			update_menu_item ( safe_cast<ToolStripMenuItem^>( sub_item ) );
	}
}

Image^ gui_binder::get_image ( String^ key )
{
	if( m_image_key_aliases.ContainsKey( key ) )
		return get_image_internal( m_image_key_aliases[key] );
	else
		return get_image_internal( key );
}

void gui_binder::set_image_list( String^ list_id, ImageList^ list )
{
	m_images[list_id] = list;
}

void gui_binder::register_image( String^ image_list_key, String^ image_key, s32 image_index )
{
	if( get_image( image_key ) )
		return;// already registered
	
	R_ASSERT( m_images.ContainsKey( image_list_key ) );

	for each ( String^ k in m_images[image_list_key]->Images->Keys )
	{
		if( image_index == m_images[image_list_key]->Images->IndexOfKey( k ) )
		{
			m_image_key_aliases.Add( image_key, k );
			return;
		}
	}

	// first 
	m_images[image_list_key]->Images->SetKeyName( image_index, image_key );
}

///////////////////////////////////////			P R I V A T E   M E T H O D S			///////////////////////////////////////////////

void gui_binder::gui_control_click( Object^ sender, EventArgs^ )
{
	 if( m_menu_items.Contains( sender ) )
		 m_input_engine->execute_action		( safe_cast<action^>( safe_cast<ToolStripMenuItem^>( sender )->Tag	 ) );

	 else if( m_buttons.Contains( sender ) )
		 m_input_engine->execute_action		( safe_cast<action^>( safe_cast<ToolStripButton^>( sender )->Tag	 ) );

	 else
		 ASSERT( false, "unregistered control click" );
}

void gui_binder::gui_menu_item_paint( Object^ sender, PaintEventArgs^ )
{
	ToolStripMenuItem^ item			= safe_cast<ToolStripMenuItem^>( sender );
	action^ item_action				= safe_cast<action^>( item->Tag );

	item->Enabled					= item_action->enabled( );
	List<String^>^ shortcuts		= item_action->get_action_keys_list( );
	item->ShortcutKeyDisplayString	= ( shortcuts->Count > 0 ) ? shortcuts[0] : "";
}

Image^ gui_binder::get_image_internal( String^ key )
{
	for each( ImageList^ list in m_images.Values )
	{
		if( list->Images->ContainsKey( key ) )
			return list->Images[key];
	}
	return nullptr;
}

} // namespace editor_base	
} // namespace xray
