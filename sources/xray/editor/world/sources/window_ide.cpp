////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "window_ide.h"
#include "project_tab.h"
#include "tools_tab.h"
#include "project.h"
#include "editor_world.h"
#include "about_dialog.h"
#include "level_editor.h"
#include "ide_log_form.h"

#include <xray/sound/world.h>
#include <xray/sound/world_user.h>

using namespace System::Windows::Forms;
using namespace WeifenLuo::WinFormsUI::Docking;

void install_hooks();
void remove_hooks();

namespace xray{
namespace editor{

gcroot<ide_log_form^> g_log_form = nullptr;

void window_ide::in_constructor	( )
{
	xray::editor_base::input_keys_holder::create();
	m_is_closing				= false;
	g_log_form					= gcnew ide_log_form;

	m_in_idle					= 0;
	Application::Idle			+= gcnew System::EventHandler(this, &window_ide::on_idle);
	
	m_view_window				= gcnew xray::editor_base::scene_view_panel( );
	m_camera_view_panel			= gcnew xray::editor_base::camera_view_panel( );
	m_view_window->DockAreas	= WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
	xray::editor_base::input_keys_holder::ref->subscribe_on_changed( gcnew editor_base::keys_combination_changed(this, &window_ide::key_combination_changed));

	// Initialize options dialog
	m_options_dialog			= gcnew editor_base::options_dialog();

	xray::editor_base::image_loader	loader;
	m_images16x16				= loader.get_shared_images16();

	install_hooks				();
	recentFilesToolStripMenuItem->Tag	= (int)8;
	exitToolStripMenuItem->Tag			= (int)100;
	toolStripMenuItem1->Tag				= (int)99;
}

void window_ide::custom_finalize( )
{
	delete						m_view_window;
	delete						m_camera_view_panel;
	Application::Idle			-= gcnew System::EventHandler(this, &window_ide::on_idle);
	remove_hooks				();
}

void window_ide::load_contents_layout( )
{
	bool first_time					=
		!controls::serializer::deserialize_dock_panel_root(
			this,
			Editor,
			"editor",
			gcnew DeserializeDockContent(this, &window_ide::reload_content),
			false
		);
	
	if ( first_time ) {
		Width				= 800;
		Height				= 600;

		m_view_window->Show			( Editor, WeifenLuo::WinFormsUI::Docking::DockState::Document );
		get_editor_world().set_default_layout	( );

		WindowState			= FormWindowState::Maximized;
	}
}

void window_ide::on_idle		( System::Object ^sender, System::EventArgs ^event_args )
{
	XRAY_UNREFERENCED_PARAMETERS( sender, event_args );

	if ( !m_in_idle++ )
		m_editor_world.on_idle_start( );

	MSG message;
	do {
		if ( m_is_closing )
			return;

		m_editor_world.tick			( );

		if ( m_is_closing )
			return;

		if ( m_is_closing )
			return;

		m_editor_world.on_idle		( );
	}
	while ( !PeekMessage( &message, HWND( 0 ), 0, 0, 0 ) );


	if ( !--m_in_idle )
		m_editor_world.on_idle_end		( );

}

void window_ide::window_ide_FormClosing		(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETER	( sender );

	
	if( !m_is_closing && !m_editor_world.close_query ( ) )
	{
		e->Cancel = true;
		return;
	}

	controls::serializer::serialize_dock_panel_root		( this, Editor, "editor" );
	m_editor_world.on_window_closing					( );

	m_is_closing	= true;
}


void window_ide::window_ide_LocationChanged	(System::Object^ , System::EventArgs^ )
{
//	m_view->window_view_LocationChanged	(sender, e);

	if (WindowState == System::Windows::Forms::FormWindowState::Maximized)
		return;

	if (WindowState == System::Windows::Forms::FormWindowState::Minimized)
		return;
}

void window_ide::window_ide_SizeChanged		(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS( sender, e );

	if (WindowState == System::Windows::Forms::FormWindowState::Maximized)
		return;

	if (WindowState == System::Windows::Forms::FormWindowState::Minimized)
		return;
}

void window_ide::add_window	( WeifenLuo::WinFormsUI::Docking::DockContent^ form, WeifenLuo::WinFormsUI::Docking::DockState dock )
{
	form->Show( Editor, dock);
}

void window_ide::window_ide_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETER	( sender );

	if (e->Alt && e->KeyCode == System::Windows::Forms::Keys::Return) 
	{
		bool current	= get_editor_world().editor_mode();
		get_editor_world().editor_mode(!current);
		e->Handled		= true;
		return;
	}
}

void window_ide::key_combination_changed( System::String^ combination )
{
	System::String^ status	= "Keys: ["+combination+"]";
	set_status_label		( 1, status );
}

void window_ide::window_ide_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^)
{
	XRAY_UNREFERENCED_PARAMETER	( sender );
}

System::Collections::Generic::List<Keys>^ window_ide::get_held_keys( )
{
	return xray::editor_base::input_keys_holder::ref->get_held_keys();
}

int	window_ide::get_current_keys_string( System::String^& combination )
{
	return xray::editor_base::input_keys_holder::ref->get_current_pressed_keys( combination );
}

//Point window_ide::get_mouse_pos_infinte( )
//{
//	return m_view->m_mouse_real;
//}
//
//xray::math::int2 window_ide::get_mouse_position_int( )
//{
//	System::Drawing::Point pt = get_mouse_position();
//	return math::int2(pt.X, pt.Y);
//}
//
//System::Drawing::Point window_ide::get_mouse_position( )
//{
//	return m_view->get_mouse_position();
//}
//
//void window_ide::set_mouse_position(System::Drawing::Point value)
//{
//	m_view->set_mouse_position(value);
//}
//
//xray::math::int2 window_ide::get_view_size_int( )
//{
//	System::Drawing::Size pt = m_view->get_view_size();
//	if(pt.Width==0)pt.Width=64;
//	if(pt.Height==0)pt.Height=64;
//	return xray::math::int2(pt.Width, pt.Height);
//}
//
//System::Drawing::Size window_ide::get_view_size( )
//{
//	return m_view->get_view_size();
//}

void window_ide::exitToolStripMenuItem_Click	(System::Object^, System::EventArgs^)
{
	Close						();
}

void window_ide::aboutToolStripMenuItem_Click(System::Object^, System::EventArgs^)
{
	about_dialog^ about			= gcnew about_dialog();
	about->ShowDialog			(this);
}

void window_ide::optionsToolStripMenuItem_Click(System::Object^, System::EventArgs^)
{
	m_editor_world.input_engine()->stop		();
	m_options_dialog->ShowDialog			(this);
	m_editor_world.input_engine()->resume	();
}

void window_ide::set_mouse_sensivity( float sens )
{
	extern float g_mouse_sensitivity;	
	g_mouse_sensitivity		= sens;
}

void window_ide::show_tab(WeifenLuo::WinFormsUI::Docking::DockContent^ tab)
{
	show_tab		( tab, WeifenLuo::WinFormsUI::Docking::DockState::Float );
}

void window_ide::show_tab	(WeifenLuo::WinFormsUI::Docking::DockContent^ tab, WeifenLuo::WinFormsUI::Docking::DockState state)
{
	if ( tab->DockPanel ) {
		tab->Show	(Editor);
		return;
	}

	tab->Show		(Editor, state );
}

void window_ide::create_tool_strip( System::String^ strip_key, int prio )
{
	m_view_window->create_tool_strip( strip_key, prio );
}

void window_ide::add_menu_item( editor_base::gui_binder^ binder, 
								System::String^ key, 
								System::String^ text, 
								int prio )
{
	binder->add_menu_item		( menuStrip, key, text, prio );
}

void window_ide::add_action_menu_item( xray::editor_base::gui_binder^ binder, System::String^ action_name, System::String^ strip_key, int prio )
{
	binder->add_action_menu_item( menuStrip, action_name, strip_key, prio );
}

void window_ide::add_action_button_item( xray::editor_base::gui_binder^ binder, System::String^ action_name, System::String^ strip_key, int prio )
{
	m_view_window->add_action_button_item( binder, action_name, strip_key, prio );
}

void window_ide::add_action_button_item( xray::editor_base::gui_binder^ binder, action^ action, System::String^ strip_key, int prio )
{
	m_view_window->add_action_button_item( binder, action, strip_key, prio );
}

void window_ide::add_button( ToolStripButton^ button, System::String^ strip_key, int prio )
{
	NOT_IMPLEMENTED();
	XRAY_UNREFERENCED_PARAMETERS(button, strip_key, prio);
	//ToolStrip^ strip			= m_view->GetToolStrip(strip_key);
	//strip->Items->Add			( button );
	//button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;
}

void window_ide::FileMenuItem_DropDownOpening(System::Object^, System::EventArgs^)
{
	recentFilesToolStripMenuItem->DropDownItems->Clear();
	level_editor^ ed = m_editor_world.get_level_editor();

	if(ed)
	{
		System::Collections::ArrayList recent_list;
		ed->get_recent_list(%recent_list);
		for each(System::String^ filename in recent_list)
		{
			System::Windows::Forms::ToolStripItem^ item = recentFilesToolStripMenuItem->DropDownItems->Add(filename);
			item->Click += gcnew System::EventHandler(this, &window_ide::RecentFileMenuItem_Click);
		}
	}
}

void window_ide::RecentFileMenuItem_Click(System::Object^ sender, System::EventArgs^)
{
	System::Windows::Forms::ToolStripItem^ item		= safe_cast<System::Windows::Forms::ToolStripItem^>(sender);
	m_editor_world.get_level_editor()->load						(item->Text);
}

void window_ide::set_status_label(int idx, System::String^ text)
{
	switch (idx)
	{
	case 0:	
		toolStripStatusLabel0->Text = text;
		break;
	case 1:
		toolStripStatusLabel3->Text = text;
		break;
	case 2:
		toolStripStatusLabel4->Text = text;
		break;
	default:
		UNREACHABLE_CODE();
	}
}

xray::editor_base::options_manager^ window_ide::get_options_manager() 
{
	return m_options_dialog;
}

void window_ide::WndProc(Message% m)
{
	super::WndProc(m);

	if (m.Msg == WM_ACTIVATEAPP)
		OnWM_ActivateApp((int)m.WParam, (int)m.LParam);

    if (m.Msg == WM_ACTIVATE)
		OnWM_Activate((int)m.WParam, (int)m.LParam);
}

void window_ide::OnWM_Activate(WPARAM wParam, LPARAM lParam)
{
	XRAY_UNREFERENCED_PARAMETER		(lParam);
	u16 fActive						= LOWORD(wParam);
	BOOL fMinimized					= (BOOL) HIWORD(wParam);
	bool const active				= ((fActive!=WA_INACTIVE) && (!fMinimized)) ? true : false;

	if (active != m_editor_world.engine().is_application_active())
	{
		if (active)	
		{
			// activate
			m_view_window->window_view_Activated		();
//			m_editor_world.engine().on_application_activate();
		} else
		{
			// deactivate
			m_view_window->window_view_Deactivate		();
//			m_editor_world.engine().on_application_deactivate();
		}
	}
}

void window_ide::OnWM_ActivateApp(WPARAM wParam, LPARAM lParam)
{
	XRAY_UNREFERENCED_PARAMETER		(lParam);
	BOOL active						= (BOOL)wParam;

	if (!!active != m_editor_world.engine().is_application_active())
	{
		if (active)	
		{
			// activate
//			m_view->window_view_Activated		();
			m_editor_world.engine().on_application_activate();
		} else
		{
			// deactivate
//			m_view->window_view_Deactivate		();
			m_editor_world.engine().on_application_deactivate();
		}
	}
}

void window_ide::close						( )
{
	m_is_closing = true;
	Close	();
}

void window_ide::error_message_out( System::String^ msg)
{
	g_log_form->error_message_out	( msg );
	error_message_button->Text		= msg;
	error_message_button->BackColor	= System::Drawing::Color::FromArgb(255, 192, 192);
}

void window_ide::clear_error_messages( )
{
	g_log_form->clear_error_messages( );
	error_message_button->Text		= "";
	error_message_button->BackColor	= System::Drawing::SystemColors::Control;
}

void window_ide::error_message_button_ButtonClick(System::Object^, System::EventArgs^)
{
	if(!g_log_form->Visible)
		g_log_form->Show();
	else
		g_log_form->Hide();
}

void window_ide::clearToolStripMenuItem_Click(System::Object^, System::EventArgs^ )
{
	clear_error_messages();
}

void window_ide::on_form_activated(System::Object^, System::EventArgs^)
{
	if( m_editor_world.editor_mode() && m_editor_world.sound_scene().c_ptr() )
	{
		m_editor_world.engine().get_sound_world().get_editor_world_user().set_active_sound_scene(m_editor_world.sound_scene(), 1000, 0);
		m_editor_world.engine().get_sound_world().get_editor_world_user().set_listener_properties_interlocked
			( m_editor_world.sound_scene(),
			m_view_window->get_inverted_view_matrix().c.xyz(),
			m_view_window->get_inverted_view_matrix().k.xyz(),
			m_view_window->get_inverted_view_matrix().j.xyz());
	}
}

} //namespace editor
} //namespace xray
