////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_FILES_VIEW_PANEL_H_INCLUDED
#define ANIMATION_FILES_VIEW_PANEL_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;

namespace xray
{
	namespace animation_editor
	{
		ref class animation_collections_editor;

		public ref class animation_files_view_panel : public xray::editor::controls::file_view_panel_base
		{
			typedef xray::editor::controls::file_view_panel_base super;
		public:
			animation_files_view_panel( animation_collections_editor^ parent );
			virtual	void	track_active_item				(System::String^ path) override;

		protected:
			~animation_files_view_panel						( ){}
					void	in_constructor					( );
			virtual	void	on_node_double_click			( Object^ sender, TreeNodeMouseClickEventArgs^ e) override;
			virtual	void	on_node_key_down				( Object^ sender, KeyEventArgs^ e) override;
			virtual	void	on_tree_mouse_down				( Object^, MouseEventArgs^ e) override;
			virtual	void	on_tree_mouse_move				( Object^, MouseEventArgs^ e) override;

		private:
			animation_collections_editor^				m_parent;
			ImageList^									m_tree_view_image_list;
			System::Windows::Forms::ContextMenuStrip^	m_context_menu;
	
		}; //ref class animations_tree_view
	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_FILES_VIEW_PANEL_H_INCLUDED