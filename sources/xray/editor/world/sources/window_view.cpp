#include "pch.h"
#include "window_view.h"
#include "window_ide.h"
#include "editor_world.h"
#include "resources.h"
#include "gui_binder.h"

#pragma managed(push,off)
#include <xray/editor/world/engine.h>
#pragma managed(pop)

using namespace xray::editor;
using namespace System::Drawing;

using System::Windows::Forms::ToolStrip;
using System::Windows::Forms::ToolStripPanel;


void window_view::in_constructor						( )
{
	m_mouse_start				= Drawing::Point(0, 0);
	m_mouse_real				= Drawing::Point(0, 0);

	m_default_clip				= System::Drawing::Rectangle( 0 ,0 , 0, 0);
	m_current_cursor			= enum_editor_cursors::enum_editor_cursors_default;
	m_infinite_mode				= false;
	m_top_toolstrip_panel		= gcnew ToolStripPanel;
	top_panel->SuspendLayout	();
	top_panel->Controls->Add	(m_top_toolstrip_panel);
	m_top_toolstrip_panel->Dock = System::Windows::Forms::DockStyle::Fill;
	top_panel->ResumeLayout		();
}

System::Void window_view::window_view_Load(System::Object^, System::EventArgs^)
{
	HINSTANCE hInstance		= (HINSTANCE)System::Runtime::InteropServices::Marshal::GetHINSTANCE	(
								System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0] ).ToInt64();
	m_tranform_cursor		= 
		gcnew System::Windows::Forms::Cursor(	(IntPtr)LoadCursor(
														hInstance,
														MAKEINTRESOURCE(IDC_TRANSFORM_CURSOR)
												)
											);
	//enum_editor_cursors_default, enum_editor_cursors_cross, enum_editor_cursors_dragger);
	cursors.Add( enum_editor_cursors::enum_editor_cursors_default,	Cursors::Default );
	cursors.Add( enum_editor_cursors::enum_editor_cursors_cross,	Cursors::Cross );
	cursors.Add( enum_editor_cursors::enum_editor_cursors_dragger,	m_tranform_cursor );

// Resize window to make the view of 800x600 size.
// 	LOG_INFO ( "window_view_SizeChanged(): width = %d, height = %d", ViewPanel->ClientRectangle.Width, ViewPanel->ClientRectangle.Height); 	
// 
// 	int asdx = ( 800 - ViewPanel->ClientRectangle.Width );
// 	int asdy = ( 600 - ViewPanel->ClientRectangle.Height );
// 
// 	this->Width +=  asdx;
// 	this->Height +=  asdy;
}
System::Void window_view::window_view_Paint				(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS( sender, e );

	if ( m_ide->in_idle( ) )
		return;

	m_ide->get_editor_world().tick	( );
}

System::Void window_view::window_view_Activated			()
{

	if ( !Parent )
		return;

	while ( ShowCursor( TRUE ) <= 0 );
}

System::Void window_view::window_view_Deactivate		()
{
	if (!Parent)
		return;

	while ( ShowCursor(TRUE) <= 0 );

	unclip_cursor				( );
	m_input_key_holder.reset	( );
}


System::Void window_view::window_view_LocationChanged	(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS( sender, e );

	if (!Parent)
		return;

	if( !m_ide->get_editor_world().editor_mode() )
	{
		clip_cursor				( );

		while ( ShowCursor(FALSE) >= 0 );
	}

}

System::Void window_view::window_view_SizeChanged (System::Object^, System::EventArgs^)
{
// 	if ( !components )
// 		return;

	m_ide->get_editor_world().engine().on_resize	( );

//	LOG_INFO ( "window_view_SizeChanged(): width = %d, height = %d", ViewPanel->ClientRectangle.Width, ViewPanel->ClientRectangle.Height); 

// 	if( !EditButton->Checked)
// 	{
// 		clip_cursor				( );
// 
// 		while ( ShowCursor(FALSE) >= 0 );
// 		//::Cursor::Hide();
// 	}
}

void window_view::clip_cursor ( )
{
	if( m_default_clip.Width == 0)
	{
		using System::Drawing::Rectangle;
		Rectangle					view = ViewPanel->RectangleToScreen(ViewPanel->ClientRectangle);
		m_default_clip				= ::Cursor::Clip;
		::Cursor::Clip				= view;
	}
}
void window_view::unclip_cursor ( )
{
	if( m_default_clip.Width > 0)
	{
		::Cursor::Clip = m_default_clip;

		m_default_clip.Width = 0;
		m_default_clip.Height = 0;
	}
}

System::IntPtr window_view::draw_handle					( )
{
	return						( ViewPanel->Handle );
}

void window_view::on_idle								( )
{
	if ( m_ide->in_idle( ) )
		return;

	m_ide->get_editor_world().tick	( );
}

Point	window_view::get_mouse_position					( )
{
	Control^ control = safe_cast<Control^>( this );

	Point view_center = get_view_center();

	return Point( control->Cursor->Position.X - view_center.X, control->Cursor->Position.Y - view_center.Y );
}

void window_view::set_mouse_position(Point p )
{
	Control^ control = safe_cast<Control^>( this );

	Point view_center = get_view_center();

	Point cp(p.X + view_center.X, p.Y + view_center.Y);
	control->Cursor->Position = cp;
}

Point	window_view::get_view_center					( ) 
{
	Point result(	 this->ViewPanel->DisplayRectangle.Left + this->ViewPanel->DisplayRectangle.Width/2
					,this->ViewPanel->DisplayRectangle.Top +  this->ViewPanel->DisplayRectangle.Height/2 );

	result = this->ViewPanel->PointToScreen( result);

	return result;
}

Size	window_view::get_view_size							( )
{
	return ::Size(	 this->ViewPanel->ClientRectangle.Width, this->ViewPanel->ClientRectangle.Height);
}

System::Void window_view::ViewPanel_MouseMove		(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS( sender, e );

	if( m_infinite_mode )
	{
		Control^ control = safe_cast<Control^>( this );

		Point view_center = get_view_center();

		int delta_x = control->Cursor->Position.X - view_center.X;
		int delta_y = control->Cursor->Position.Y - view_center.Y;

		m_mouse_real.X += delta_x;
		m_mouse_real.Y += delta_y;

		if( delta_x != 0 || delta_y != 0)
		{
			control->Cursor->Position	= view_center;
		}
	}
}

enum_editor_cursors window_view::get_cursor	( )
{
	return m_current_cursor;
}

enum_editor_cursors window_view::set_cursor				( enum_editor_cursors cursor )
{
	enum_editor_cursors result = m_current_cursor;
	m_current_cursor = cursor;
	ViewPanel->Cursor = cursors[cursor];	
	return result;
}	

void window_view::set_infinite_mode			( bool inifinte_mode )
{
	if( inifinte_mode )
	{
		((Control^)(this->ViewPanel))->Capture = true;
		clip_cursor ( );

		while ( ShowCursor(FALSE) >= 0 );
		//::Cursor::Hide();

		Control^ control			= safe_cast<Control^>( this );
		m_mouse_start				= control->Cursor->Position;
		control->Cursor->Position	= get_view_center();
		m_infinite_mode				= true;
	}
	else
	{
		Control^ control = ((Control^)(this->ViewPanel));
		control->Capture = false;

		while ( ShowCursor(TRUE) <= 0 );
		//::Cursor::Show();

		unclip_cursor ( );

		control->Cursor->Position	= m_mouse_start;
//		m_input_key_holder.reset	( );
		m_infinite_mode				= false;
	}
}

extern bool g_allow_sensitivity_process;
void window_view::ViewPanel_MouseEnter(System::Object^  , System::EventArgs^ )
{
	m_mouse_in_view				= true;
	g_allow_sensitivity_process	= true;
}

void window_view::ViewPanel_MouseLeave(System::Object^  , System::EventArgs^ )
{
	m_mouse_in_view				= false;
	g_allow_sensitivity_process	= false;
}

Keys button_to_key(System::Windows::Forms::MouseButtons mouse_button)
{
	switch (mouse_button)
	{
	case MouseButtons::Left:
		return Keys::LButton;
	case MouseButtons::Right:
		return Keys::RButton;
	case MouseButtons::Middle:
		return Keys::MButton;
	case MouseButtons::XButton1:
		return Keys::XButton1;
	case MouseButtons::XButton2:
		return Keys::XButton2;
	default:
		return Keys::None;
	}
}

void window_view::ViewPanel_MouseDown(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	m_input_key_holder.register_mouse_event	( e, true );
}

void window_view::ViewPanel_MouseUp(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	m_input_key_holder.register_mouse_event	( e, false );
}

ToolStrip^ window_view::GetToolStrip(System::String^ name) 
{
	ToolStrip^ result				= nullptr;
	int idx							= m_top_toolstrip_panel->Controls->IndexOfKey(name);
	ASSERT(idx!=-1, "toolstrip not found %s", unmanaged_string(name).c_str());
	result							= safe_cast<ToolStrip^>(m_top_toolstrip_panel->Controls[idx]);
	return							result;
}

ToolStrip^ window_view::CreateToolStrip(System::String^ name, int prio) 
{
	ToolStrip^ result			= nullptr;
	int idx						= m_top_toolstrip_panel->Controls->IndexOfKey(name);
	ASSERT_U					(idx==-1, "toolstrip already exist %", unmanaged_string(name).c_str());

	result						= gcnew ToolStrip;
	result->Name				= name;
	result->Tag					= prio;
	result->ImageScalingSize	= System::Drawing::Size(16,16);
	m_top_toolstrip_panel->Join	(result);

	return						result;
}
