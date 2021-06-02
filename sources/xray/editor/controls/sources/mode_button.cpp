////////////////////////////////////////////////////////////////////////////
//	Created		: 13.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mode_button.h"

namespace xray {
namespace editor {
namespace controls {

Int32			mode_button::current_mode::get( )
{
	return m_mode; 
}

void			mode_button::current_mode::set( Int32 value )
{
	if( value >= modes->Count )
		throw gcnew ArgumentException( "such mode does not exist in modes collection" );

	m_context_menu_items[m_mode]->Checked	= false;
	m_mode									= value; 
	m_context_menu_items[m_mode]->Checked	= true;
	ImageIndex								= value;
	super::Text								= modes[m_mode];
}

List<String^>^	mode_button::modes::get( )
{
	return m_modes; 
}

void			mode_button::modes::set( List<String^>^ value )
{
	m_modes					= value; 
	create_context_menu		( );
}

String^			mode_button::current_mode_name::get( )
{
	return modes[ current_mode ];
}

void			mode_button::create_context_menu		( )									
{
	m_context_menu_items = gcnew array<MenuItem^>( modes->Count );

	for( int i = 0; i < m_modes->Count ; ++i )
	{
		m_context_menu_items[i]			= gcnew MenuItem( m_modes[i], gcnew EventHandler( this, &mode_button::context_item_click ) );
		m_context_menu_items[i]->Tag	= i;
	}

	m_context_menu_items[ 0 ]->Checked	= true;
	m_context_menu						= gcnew System::Windows::Forms::ContextMenu( m_context_menu_items );
}

void			mode_button::context_item_click			( Object^ sender, EventArgs^ e )		
{
	current_mode = (Int32)((MenuItem^)sender)->Tag;
	Button::OnClick	( e );
}

void			mode_button::OnMouseUp					( MouseEventArgs^ e)
{
	if( e->Button == System::Windows::Forms::MouseButtons::Right )
	{
		m_context_menu->Show( this, e->Location );
	}
	
	super::OnMouseUp( e );
}

void			mode_button::OnMouseClick				( MouseEventArgs^ e )
{
	if( e->Button == System::Windows::Forms::MouseButtons::Left )
	{
		current_mode	= ( current_mode + 1 ) % modes->Count;
		super::OnClick	( e );
		return;
	}else
		super::OnMouseClick( e );
}

void			mode_button::OnClick					( EventArgs^ e )
{	

}

} // namespace controls
} // namespace editor
} // namespace xray