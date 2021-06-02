////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_items_view_panel.h"
#include "sound_items_source.h"
#include "sound_item_struct.h"
#include "sound_editor.h"

using namespace System;
using namespace System::IO;

namespace xray
{
	namespace editor
	{
		void	sound_items_view_panel::in_constructor								()
		{
			m_sound_items_tree_view->source = gcnew sound_items_source();
			m_sound_items_tree_view->refresh();
		}

		void	sound_items_view_panel::sound_items_tree_view_after_select			(System::Object^  , System::Windows::Forms::TreeViewEventArgs^  e)
		{
			//if user pick file
			if(safe_cast<tree_node^>(e->Node)->m_node_type == tree_node_type::single_item)
			{
				//if options not been loaded yet than load it from .options file and store into Node->Tag	
				if(e->Node->Tag == (nullptr))
				{
					sound_item_struct^ sound_file = gcnew sound_item_struct(e->Node->FullPath);

					sound_file->load(gcnew Action<sound_item_struct^>(this, &sound_items_view_panel::load_complete));

					Collections::Generic::Dictionary<String^, sound_item_struct^>^ sound_files = safe_cast<sound_editor^>(ParentForm)->m_sound_items;
					if(!sound_files->ContainsKey(e->Node->FullPath))
						sound_files->Add(e->Node->FullPath, sound_file);

					e->Node->Tag = sound_file;
				}
				//else (options has been loaded) show options saved in Node->Tag
				else
				{
					sound_item_selected(this, gcnew sound_item_selection_event_args(safe_cast<sound_item_struct^>(e->Node->Tag)));
				}
			}
		}

		void	sound_items_view_panel::load_complete								(sound_item_struct^ loaded_struct)
		{
			if(!loaded_struct->loaded)
				loaded_struct->save();
		
			sound_item_selected(this, gcnew sound_item_selection_event_args(loaded_struct));
		}

		void	sound_items_view_panel::m_new_sound_item_tool_strip_button_Click	(System::Object^  , System::EventArgs^  )
		{
			tree_node^	new_node						= gcnew tree_node("Enter a Name");
							new_node->Tag					= nullptr;
							new_node->ContextMenuStrip		= m_sound_items_tree_view->SelectedNode->ContextMenuStrip;
							new_node->m_node_type			= tree_node_type::single_item;
							new_node->ImageIndex			= 2;
							new_node->SelectedImageIndex	= 2;

			m_sound_items_tree_view->nodes->Add(new_node);
			
			new_node->BeginEdit();
		}

		void	sound_items_view_panel::toolStripButton1_Click						(System::Object^ , System::EventArgs^ )
		{
			if(m_sound_items_tree_view->SelectedNode->Tag == nullptr)
				return;
			safe_cast<sound_item_struct^>(m_sound_items_tree_view->SelectedNode->Tag)->save();
		}

		void	sound_items_view_panel::sound_items_tree_view_file_create			(System::Object^ , tree_view_event_args^ )
		{
			TreeNodeCollection^ nodes;

			if(safe_cast<tree_node^>(m_sound_items_tree_view->SelectedNode)->m_node_type == tree_node_type::single_item)
				nodes = (m_sound_items_tree_view->SelectedNode->Parent == nullptr)
					?m_sound_items_tree_view->nodes
					:m_sound_items_tree_view->SelectedNode->Parent->Nodes;
			else
				nodes = m_sound_items_tree_view->SelectedNode->Nodes;
			
			tree_node^	new_node						= gcnew tree_node("Enter a Name");
							new_node->Tag					= nullptr;
							new_node->ContextMenuStrip		= m_sound_items_tree_view->SelectedNode->ContextMenuStrip;
							new_node->m_node_type			= tree_node_type::single_item;
							new_node->ImageIndex			= 2;
							new_node->SelectedImageIndex	= 2;

			nodes->Add(new_node);
			
			if(new_node->Parent != nullptr)
				new_node->Parent->Expand();
			new_node->BeginEdit();
		}

		void	sound_items_view_panel::sound_items_tree_view_folder_create			(System::Object^ , tree_view_event_args^ )
		{
			TreeNodeCollection^ nodes;

			if(safe_cast<tree_node^>(m_sound_items_tree_view->SelectedNode)->m_node_type == tree_node_type::single_item)
				nodes = (m_sound_items_tree_view->SelectedNode->Parent == nullptr)
					?m_sound_items_tree_view->nodes
					:m_sound_items_tree_view->SelectedNode->Parent->Nodes;
			else
				nodes = m_sound_items_tree_view->SelectedNode->Nodes;
			
			tree_node^	new_node						= gcnew tree_node("Enter a Name");
							new_node->Tag					= nullptr;
							new_node->ContextMenuStrip		= m_sound_items_tree_view->SelectedNode->ContextMenuStrip;
							new_node->m_node_type			= tree_node_type::group_item;
							new_node->ImageIndex			= 0;
							new_node->SelectedImageIndex	= 0;

			nodes->Add(new_node);
			
			if(new_node->Parent != nullptr)
				new_node->Parent->Expand();
			new_node->BeginEdit();
		}

		void	sound_items_view_panel::sound_items_tree_view_item_remove			(System::Object^ , tree_view_event_args^ )
		{	
			Boolean is_file		= safe_cast<tree_node^>(m_sound_items_tree_view->SelectedNode)->m_node_type == tree_node_type::single_item;

			String^ message;
			if(is_file)
				message			= "Are You Sure you want to remove this item?";
			else
				message			= "Are You Sure you want to remove this dir with all entires?";

			if(Windows::Forms::MessageBox::Show(message, "Items Deleting", MessageBoxButtons::YesNo) == Windows::Forms::DialogResult::Yes)
			{
				if(is_file)
				{
					File::Delete		(sound_editor::absolute_sound_resources_path+"items/"+m_sound_items_tree_view->SelectedNode->FullPath+".sound_item");
					m_sound_items_tree_view->SelectedNode->Remove();
				}
				else
				{
					Directory::Delete	(sound_editor::absolute_sound_resources_path+"items/"+m_sound_items_tree_view->SelectedNode->FullPath, true);
					m_sound_items_tree_view->SelectedNode->Remove();
				}
			}
		}

		void	sound_items_view_panel::sound_items_tree_view_item_label_edit		(System::Object^ , NodeLabelEditEventArgs^ e)
		{
			Boolean is_file		= safe_cast<tree_node^>(e->Node)->m_node_type == tree_node_type::single_item;
			if(is_file)
			{
				if(e->Node->Tag != nullptr)
				{
					if(e->Label == nullptr)
					{
						e->CancelEdit = true;
						return;
					}
					sound_item_struct^ item_struct	= safe_cast<sound_item_struct^>(e->Node->Tag);
					item_struct->name				= e->Label;

					File::Delete					(sound_editor::absolute_sound_resources_path+"items/"+item_struct->m_file_path+".sound_item");
					item_struct->m_file_path		= Path::GetDirectoryName(item_struct->m_file_path)+"/"+e->Label;
					item_struct->save				();
				}
			}
			else
			{
				if(Directory::Exists(sound_editor::absolute_sound_resources_path+"items/"+e->Node->FullPath))
					Directory::Move(
						sound_editor::absolute_sound_resources_path+"items/"+e->Node->FullPath,
						sound_editor::absolute_sound_resources_path+"items/"+Path::GetDirectoryName(e->Node->FullPath)+"/"+e->Label
						);
				else
					Directory::CreateDirectory(sound_editor::absolute_sound_resources_path+"items/"+Path::GetDirectoryName(e->Node->FullPath)+"/"+e->Label);
			}

			e->Node->Text								= e->Label;
			e->Node->Name								= e->Label;
			m_sound_items_tree_view->SelectedNode		= e->Node;
		}
		
		void	sound_items_view_panel::newItemToolStripMenuItem_Click				(System::Object^ , System::EventArgs^ )
		{
			tree_node^	new_node						= gcnew tree_node("Enter a Name");
							new_node->Tag					= nullptr;
							new_node->ContextMenuStrip		= m_sound_items_tree_view->SelectedNode->ContextMenuStrip;
							new_node->m_node_type			= tree_node_type::single_item;
							new_node->ImageIndex			= 2;
							new_node->SelectedImageIndex	= 2;

			m_sound_items_tree_view->nodes->Add(new_node);
			
			new_node->BeginEdit();
		}
		
		void	sound_items_view_panel::newFolderToolStripMenuItem_Click			(System::Object^ , System::EventArgs^ )
		{
			tree_node^	new_node						= gcnew tree_node("Enter a Name");
							new_node->Tag					= nullptr;
							new_node->ContextMenuStrip		= m_sound_items_tree_view->SelectedNode->ContextMenuStrip;
							new_node->m_node_type			= tree_node_type::group_item;
							new_node->ImageIndex			= 0;
							new_node->SelectedImageIndex	= 0;

			m_sound_items_tree_view->nodes->Add(new_node);
			
			new_node->BeginEdit();
		}
	}//namespace editor
}//namespace xray
