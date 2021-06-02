////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUPS_CONTROLLERS_TREE_H_INCLUDED
#define ANIMATION_GROUPS_CONTROLLERS_TREE_H_INCLUDED

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using System::Windows::Forms::TreeNode;
using namespace System::Drawing;
using xray::editor::controls::tree_node;

namespace xray {
namespace animation_editor {

	ref class animation_groups_editor;

	public ref class animation_groups_controllers_tree: public xray::editor::controls::file_view_panel_base
	{
		typedef xray::editor::controls::file_view_panel_base super;
	public:
						animation_groups_controllers_tree	(animation_groups_editor^ ed);
						~animation_groups_controllers_tree	()	{};
		virtual	void	track_active_item					(System::String^ path) override;

	protected:
		virtual	void	on_node_double_click				(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e) override;

	private:
		System::Windows::Forms::ImageList^	tree_view_image_list;
		animation_groups_editor^			m_groups_editor;
	}; // class animation_groups_controllers_tree
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_GROUPS_CONTROLLERS_TREE_H_INCLUDED