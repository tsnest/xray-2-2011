////////////////////////////////////////////////////////////////////////////
//	Created		: 17.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mode_strip_button.h"

namespace xray{
namespace editor{
namespace controls{


Int32			mode_strip_button::current_mode::get			( )
{
	return m_mode; 
}

void			mode_strip_button::current_mode::set			( Int32 value )
{
	if( value >= modes->Count )
		throw gcnew ArgumentException( "such mode does not exist in modes collection" );

	m_context_menu_items[ m_mode ]->Checked = false;
	m_mode			= value; 
	m_context_menu_items[ m_mode ]->Checked = true;
	if( image_list != nullptr )
		Image		= image_list->Images[value + image_offset];
	if( m_modes != nullptr && m_modes->Count > 0 )
	{
		Text = m_modes[m_mode];
	}
}

List<String^>^	mode_strip_button::modes::get					( )
{
	return m_modes; 
}

void			mode_strip_button::modes::set					( List<String^>^ value )
{
	if( m_image_list != nullptr && m_image_list->Images->Count < value->Count + image_offset )
		throw gcnew ArgumentException("image list does not contain images for all " + value->Count + " modes.");

	m_modes					= value; 
	create_context_menu		( );

	if (m_mode < m_modes->Count)
		Text = m_modes[m_mode];
	else if (m_modes->Count)
		Text = m_modes[0];
	else Text = "";
}

String^			mode_strip_button::current_mode_name::get		( )
{
	return modes[ current_mode ];
}

ImageList^		mode_strip_button::image_list::get				( )
{
	return m_image_list;
}

void			mode_strip_button::image_list::set				( ImageList^ value )
{
	if( m_modes != nullptr && value->Images->Count < m_modes->Count )
		throw gcnew ArgumentException("image list must contains at last " + m_modes->Count + image_offset + " images.");

	m_image_list = value;
	Image = m_image_list->Images[m_mode + image_offset];
}

void			mode_strip_button::create_context_menu			( )									
{
	m_context_menu_items = gcnew array<MenuItem^>( modes->Count );

	for( int i = 0; i < m_modes->Count ; ++i )
	{
		m_context_menu_items[i]			= gcnew MenuItem( m_modes[i], gcnew EventHandler( this, &mode_strip_button::context_item_click ) );
		m_context_menu_items[i]->Tag	= i;
	}
	
	if (current_mode < m_modes->Count)
		m_context_menu_items[ current_mode ]->Checked	= true;
	else
		m_context_menu_items[ 0 ]->Checked	= true;
	
	m_context_menu						= gcnew System::Windows::Forms::ContextMenu( m_context_menu_items );
	if( image_list != nullptr )
		Image							= image_list->Images[image_offset];
}

void			mode_strip_button::context_item_click			( Object^ sender, EventArgs^ e )		
{
	current_mode = (Int32)((MenuItem^)sender)->Tag;
	super::OnClick	( e );
	on_click		( );
}

void			mode_strip_button::OnMouseUp					( MouseEventArgs^ e)
{
	if( e->Button == System::Windows::Forms::MouseButtons::Right )
	{
		m_context_menu->Show( Parent, System::Drawing::Point( Bounds.Location.X + e->Location.X, Bounds.Location.Y + e->Location.Y ) );
	}
	else if( e->Button == System::Windows::Forms::MouseButtons::Left && can_iterate_modes )
	{
		current_mode	= ( current_mode + 1 ) % modes->Count;
		super::OnClick	( e );
		on_click		( );
		return;
	}
	super::OnMouseUp( e );
}

void			mode_strip_button::OnClick						( EventArgs^ e )
{	

}

} // namespace controls
} // namespace editor
} // namespace xray