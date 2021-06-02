#include "pch.h"
#include "wpf_controls_page.h"

namespace xray {
namespace editor_base {

void		wpf_controls_page::in_constructor						( )						
{
	lock_radio_button->Checked		= xray::editor::wpf_controls::settings::curve_editor_lock_zoom;
	unlock_radio_button->Checked	= !xray::editor::wpf_controls::settings::curve_editor_lock_zoom;
	m_hierarchical_item_indent_text_box->Text = xray::editor::wpf_controls::settings::hierarchical_item_indent.ToString( );
}
void		wpf_controls_page::unlock_radio_button_checked_changed	( Object^ , EventArgs^ )
{
	m_curve_editor_lock_zoom	= lock_radio_button->Checked;
	m_changes_made				= true;
}

void		wpf_controls_page::hierarchical_item_indent_changed		( Object^ , EventArgs^ )
{
	Double new_item_indext;
	if( Double::TryParse( m_hierarchical_item_indent_text_box->Text, new_item_indext ) )
	{
		m_hierarchical_item_indent		= new_item_indext;
		m_changes_made					= true;
	}
}

void		wpf_controls_page::activate_page		( )
{
	if( !m_initialized )
	{
		m_curve_editor_lock_zoom		= xray::editor::wpf_controls::settings::curve_editor_lock_zoom;
		m_hierarchical_item_indent		= xray::editor::wpf_controls::settings::hierarchical_item_indent;

		lock_radio_button->Checked		= m_curve_editor_lock_zoom;
		unlock_radio_button->Checked	= !m_curve_editor_lock_zoom;

		m_hierarchical_item_indent_text_box->Text = m_hierarchical_item_indent.ToString( );

		m_initialized = true;
	}
}
void		wpf_controls_page::deactivate_page		( )
{
}
bool		wpf_controls_page::accept_changes		( )
{
	m_changes_made	= false;
	m_initialized	= false;

	xray::editor::wpf_controls::settings::curve_editor_lock_zoom	= m_curve_editor_lock_zoom;
	xray::editor::wpf_controls::settings::hierarchical_item_indent	= m_hierarchical_item_indent;

	return true;
}
void		wpf_controls_page::cancel_changes		( )
{
	m_changes_made	= false;
	m_initialized	= false;
}
bool		wpf_controls_page::changed				( )
{
	return m_changes_made;
}
Control^	wpf_controls_page::get_control			( )
{ 
	return this; 
}

} // namespace editor_base
} // namespace xray