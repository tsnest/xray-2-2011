////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATIONS_VIEW_PANEL_H_INCLUDED
#define ANIMATIONS_VIEW_PANEL_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;
using System::Windows::Forms::TreeNode;

namespace xray {
namespace animation_editor {
	
	ref class animation_lexeme_editor;

	public ref class animations_view_panel : public xray::editor::controls::file_view_panel_base
	{
		typedef xray::editor::controls::file_view_panel_base super;
	public:
		animations_view_panel(animation_lexeme_editor^ ed)
			:xray::editor::controls::file_view_panel_base(nullptr),
			m_editor(ed)
		{
			in_constructor();
			HideOnClose = true;
		}
		property 		TreeNode^						last_selected_node;

		virtual	void	track_active_item				(System::String^ path) override;

	protected:
						~animations_view_panel		()	{};
				void	in_constructor					();
		virtual	void	on_node_double_click			(System::Object^, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e) override;
		virtual	void	on_node_key_down				(System::Object^, System::Windows::Forms::KeyEventArgs^ e) override;
				void	on_tree_node_mouse_click		(System::Object^, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e);

	private:
		System::Windows::Forms::ImageList^			tree_view_image_list;
		System::Windows::Forms::ContextMenuStrip^	m_context_menu;
		animation_lexeme_editor^					m_editor;
	}; // ref class animations_view_panel
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATIONS_VIEW_PANEL_H_INCLUDED