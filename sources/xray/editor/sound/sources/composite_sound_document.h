////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef COMPOSITE_SOUND_DOCUMENT_H_INCLUDED
#define COMPOSITE_SOUND_DOCUMENT_H_INCLUDED

#include "sound_editor.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using xray::editor_base::command_engine;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;

namespace xray {
namespace sound_editor {

	ref class sound_editor;
	ref class sound_object_wrapper;
	ref class composite_sound_wrapper;
	ref class composite_sound_item;
	ref class sound_collection_item;

	public ref class composite_sound_document: public xray::editor::controls::document_base
	{
		typedef xray::editor::controls::document_base super;
		typedef List<composite_sound_item^> items_list;
	public:
								composite_sound_document		(sound_editor^ ed);
								~composite_sound_document		();
		virtual	void			save							() override;
		virtual	void			load							() override;
		virtual	void			undo							() override;
		virtual	void			redo							() override;
		virtual	void			copy							(bool del) override;
		virtual	void			paste							() override;
		virtual	void			select_all						() override;
		virtual	void			del								() override;
				sound_editor^	get_editor						() {return m_editor;};
				void			on_property_changed				(Object^, value_changed_event_args^ e);
				void			add_items_to_collection			(List<String^>^ names_list, tree_node^ dest_node, sound_object_type t);
				void			add_item_to_collection			(tree_node^ dest, composite_sound_item^ item);
				void			remove_item_from_collection		(composite_sound_item^ item);
				tree_view^		get_tree_view					() {return m_tree_view;};
	composite_sound_wrapper^	get_sound						() {return m_sound;};
				tree_node^		get_node_by_tag					(TreeNodeCollection^ root, composite_sound_item^ tag);

	private:
				void			on_resources_loaded				(Object^, EventArgs^);
				void			fill_tree_view					(tree_node^ root, List<sound_collection_item^>^ itms);
				void			on_document_activated			(Object^, EventArgs^);
				void			on_drag_over					(Object^, DragEventArgs^ e);
				void			on_drag_drop					(Object^, DragEventArgs^ e);
				void			on_selection_changed			(Object^, xray::editor::controls::tree_view_selection_event_args^);
				void			menu_opened						(Object^, EventArgs^);
				void			remove_click					(Object^, EventArgs^);
				void			add_collection_click			(Object^, EventArgs^);

	private:
		System::ComponentModel::Container^	components;
		sound_editor^						m_editor;
		command_engine^						m_command_engine;
		composite_sound_wrapper^			m_sound;
		String^								m_name;
		SaveFileDialog^						m_save_file_dialog;
		System::Windows::Forms::ContextMenuStrip^ m_context_menu;
		ToolStripMenuItem^					removeToolStripMenuItem;
		ToolStripMenuItem^					addCollectionToolStripMenuItem;
		tree_view^							m_tree_view;
		ImageList^							m_tree_view_image_list;
		bool								m_first_save;
	}; // ref class composite_sound_document
} // namespace sound_editor
} // namespace xray 
#endif // #ifndef COMPOSITE_SOUND_DOCUMENT_H_INCLUDED