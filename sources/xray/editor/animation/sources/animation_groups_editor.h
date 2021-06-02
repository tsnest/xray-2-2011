////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUPS_EDITOR_H_INCLUDED
#define ANIMATION_GROUPS_EDITOR_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace WeifenLuo::WinFormsUI::Docking;
using namespace xray::editor::controls;

namespace xray {
namespace animation_editor {

	ref class animation_editor;
	ref class animation_groups_toolbar;
	ref class animation_groups_controllers_tree;
	ref class animation_groups_animations_tree;

	public ref class animation_groups_editor : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
								animation_groups_editor					(animation_editor^ ed);
								~animation_groups_editor				();
		void					tick									();
		void					clear_resources							();
		bool					close_query								();
		animation_editor^		get_animation_editor					() {return m_animation_editor;};
		document_base^			on_document_creating					();
		IDockContent^			find_dock_content						(System::String^ persist_string);
		bool					predicate_learn							();
		bool					predicate_save_scene					();
		bool					predicate_save_scene_as					();
		void					save_document_as						();
		void					select_model							();
		void					learn_controller						();
		void					show_animations							();
		void					collapse_all_groups						();

		property xray::editor::controls::document_editor_base^ multidocument_base
		{
			xray::editor::controls::document_editor_base^ get() {return m_multidocument_base;}
		}
		property animation_groups_toolbar^ toolbar
		{
			animation_groups_toolbar^ get() {return m_toolbar;};
		};
		property animation_groups_controllers_tree^ controllers_tree
		{	
			animation_groups_controllers_tree^ get() {return m_controllers_tree;}
		}
		property animation_groups_animations_tree^ animations_tree
		{	
			animation_groups_animations_tree^ get() {return m_animations_tree;}
		}

	private:
		void					view_on_drop_down_opening				(System::Object^, System::EventArgs^);
		void					show_toolbar							(System::Object^, System::EventArgs^);
		void					show_controllers_tree					(System::Object^, System::EventArgs^);
		void					show_animations_tree					(System::Object^, System::EventArgs^);
		void					InitializeComponent						(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->AllowEndUserDocking = false;
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(
				WeifenLuo::WinFormsUI::Docking::DockAreas::Document);
			this->HideOnClose = true;
			this->Name = L"animation_groups_editor";
			this->Padding = System::Windows::Forms::Padding(0);
			this->Size = System::Drawing::Size(300,300);
			this->Text = L"Animation groups";
        }

	private:
		System::ComponentModel::Container^			components;
		animation_editor^							m_animation_editor;
		document_editor_base^						m_multidocument_base;
		animation_groups_toolbar^					m_toolbar;
		animation_groups_controllers_tree^			m_controllers_tree;
		animation_groups_animations_tree^			m_animations_tree;

	private:
		MenuStrip^									m_menu;
		ToolStripMenuItem^							m_menu_file;
		ToolStripMenuItem^							m_menu_edit;
		ToolStripMenuItem^							m_menu_view;
		ToolStripMenuItem^							m_menu_view_toolbar;
		ToolStripMenuItem^							m_menu_view_controllers_tree;
		ToolStripMenuItem^							m_menu_view_animations_tree;

	public:
		ToolStripMenuItem^							m_menu_file_new;
		ToolStripMenuItem^							m_menu_file_save;
		ToolStripMenuItem^							m_menu_file_save_as;
		ToolStripMenuItem^							m_menu_file_select_model;
		ToolStripMenuItem^							m_menu_file_show_animations;
		ToolStripMenuItem^							m_menu_file_learn;
		ToolStripMenuItem^							m_menu_edit_undo;
		ToolStripMenuItem^							m_menu_edit_redo;
		ToolStripMenuItem^							m_menu_edit_cut;
		ToolStripMenuItem^							m_menu_edit_copy;
		ToolStripMenuItem^							m_menu_edit_paste;
		ToolStripMenuItem^							m_menu_edit_select_all;
		ToolStripMenuItem^							m_menu_edit_del;
		ToolStripMenuItem^							m_menu_edit_collapse_all;
	};

	public enum class animation_groups_drag_drop_operation: int
	{
		animation_group_node = 0,
		animations_tree_files = 1,
		animations_tree_folder = 2,
	};
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_GROUPS_EDITOR_H_INCLUDED