////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef CHANNELS_EDITOR_H_INCLUDED
#define CHANNELS_EDITOR_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace WeifenLuo::WinFormsUI::Docking;

namespace xray {
namespace animation_editor {
	ref class animation_editor;
	ref class channels_layout_panel;
	ref class channels_files_view_panel;

	public ref class channels_editor : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
						channels_editor					(animation_editor^ ed);
						~channels_editor				();
		bool			close_query						();
		IDockContent^	find_dock_content				(System::String^ persist_string);

		xray::editor::controls::document_base^ on_document_creating	();

		property xray::editor::controls::document_editor_base^	multidocument_base
		{	
			xray::editor::controls::document_editor_base^	get() {return m_multidocument_base;}
		}
		property channels_layout_panel^	layout_panel
		{	
			channels_layout_panel^							get() {return m_layout_panel;}
		}
		property channels_files_view_panel^	file_view_panel
		{	
			channels_files_view_panel^						get() {return m_file_view_panel;}
		}

	private:
		animation_editor^								m_animation_editor;
		System::ComponentModel::Container^				components;
		xray::editor::controls::document_editor_base^	m_multidocument_base;
		channels_files_view_panel^						m_file_view_panel;
		channels_layout_panel^							m_layout_panel;
	}; // ref class channels_editor
} // namespace animation_editor
} // namespace xray
#endif // #ifndef CHANNELS_EDITOR_H_INCLUDED