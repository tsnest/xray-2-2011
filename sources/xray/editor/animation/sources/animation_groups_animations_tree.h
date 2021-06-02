////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUPS_ANIMATIONS_TREE_H_INCLUDED
#define ANIMATION_GROUPS_ANIMATIONS_TREE_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace xray::editor::controls;

namespace xray {
namespace animation_editor {
	public ref class animation_groups_animations_tree : public xray::editor::controls::file_view_panel_base
	{
		typedef xray::editor::controls::file_view_panel_base super;
	public:
						animation_groups_animations_tree	(xray::editor::controls::document_editor_base^ d);
						~animation_groups_animations_tree	()	{};
		virtual	void	track_active_item					(System::String^ path) override;
		tree_node^		get_node							(System::String^ path);

		property 		String^								last_selected_node_name;

	protected:
		virtual	void	on_tree_mouse_down					(System::Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
		virtual	void	on_tree_mouse_move					(System::Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
	
	private:
				void	on_tree_node_mouse_click			(System::Object^, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e);

	private:
		System::Windows::Forms::ImageList^			tree_view_image_list;
		System::Windows::Forms::ContextMenuStrip^	m_context_menu;
	}; // ref class animation_groups_animations_tree
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_GROUPS_ANIMATIONS_TREE_H_INCLUDED
