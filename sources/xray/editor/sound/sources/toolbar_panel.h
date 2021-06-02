////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef TOOLBAR_PANEL_H_INCLUDED
#define TOOLBAR_PANEL_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;

namespace xray {
namespace sound_editor {
	
	ref class sound_editor;
	ref class graph_view;
	ref class single_sound_wrapper;

	public ref class toolbar_panel : public toolbar_panel_base
	{
		typedef toolbar_panel_base super; 
	public:
				toolbar_panel				(sound_editor^ ed);
				~toolbar_panel				();

		void	show_rms					(Object^ obj);
		void	enable_buttons				(bool enable);

	private:
		void	on_play_click				(Object^, EventArgs^);
		void	on_pause_click				(Object^, EventArgs^);
		void	on_stop_click				(Object^, EventArgs^);
		void	rms_scroll_changed			(Object^, scroller_event_args^);
		void	rms_scroll_draw_background	(Object^, PaintEventArgs^ e);

	private:
		System::ComponentModel::Container^	components;
		sound_editor^						m_parent;
		Panel^								m_control_panel;
		Panel^								m_rms_graph_panel;
		scalable_scroll_bar^				m_rms_scroll;
		graph_view^							m_rms_graph;
		Button^								m_button_stop;
		Button^								m_button_pause;
		Button^								m_button_play;
	}; // ref class toolbar_panel
} // namespace sound_editor
} // namespace xray
#endif // #ifndef TOOLBAR_PANEL_H_INCLUDED