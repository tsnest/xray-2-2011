////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_files_view_panel.h"
#include "raw_file_property_struct.h"
#include <xray/editor/base/managed_delegate.h>
#include "sound_files_source.h"
#include "sound_options_struct.h"
#include "sound_editor.h"
#include "sound_file_struct.h"
#include "editor_world.h"
#include "graph_view.h"

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;

using xray::editor::sound_files_view_panel;
using xray::editor::controls::tree_view;
using xray::editor::controls::scalable_scroll_bar;

namespace xray
{
	namespace editor
	{
		void	sound_files_view_panel::in_constructor()
		{
			SuspendLayout			( );

			m_sound_files_tree_view->source						= gcnew sound_files_source();
			m_sound_files_tree_view->refresh					();

			m_rms_graph											= gcnew graph_view();
			m_rms_graph->Dock									= DockStyle::Fill;
			m_rms_graph_panel->Controls->Add					(m_rms_graph);
			m_rms_graph_panel->Controls->SetChildIndex			(m_rms_graph, 1);

			m_rms_scroll										= gcnew scalable_scroll_bar();
			m_rms_scroll->Height								= 22;
			m_rms_scroll->Dock									= DockStyle::Bottom;
			m_rms_scroll->scroll_max							= m_rms_graph->max_graph_x;
			m_rms_scroll->scroll_min							= m_rms_graph->min_graph_x;
			m_rms_scroll->scroller_position						= m_rms_graph->min_graph_x;
			m_rms_scroll->scroller_width						= m_rms_scroll->scroll_max-m_rms_scroll->scroll_min;
			m_rms_scroll->scroller_moved						+= gcnew EventHandler<scroller_event_args^>(this, &sound_files_view_panel::rms_scroll_changed);
			m_rms_scroll->on_draw_background					+= gcnew PaintEventHandler(this, &sound_files_view_panel::rms_scroll_draw_background);
			m_rms_graph_panel->Controls->Add					(m_rms_scroll);

			ResumeLayout			( false );
		}

		void	sound_files_view_panel::raw_files_tree_view_after_select			(System::Object^  , System::Windows::Forms::TreeViewEventArgs^  e)
		{
			//unselect options
			m_rms_graph->set_graph_points		(nullptr);
			m_property_grid->SelectedObject		= nullptr;

			m_selected_file_label->Text			= "file: "+e->Node->FullPath;
			m_loading_label->Visible			= true;

			//if user pick file
			if(safe_cast<controls::tree_node^>(e->Node)->m_node_type == tree_node_type::single_item)
			{
				//if options not been loaded yet than load it from .options file and store into Node->Tag	
				if(e->Node->Tag == (nullptr))
				{
					sound_file_struct^ sound_file = gcnew sound_file_struct(e->Node->FullPath);

					sound_file->load(
						gcnew Action<sound_file_struct^>(this, &sound_files_view_panel::load_complete),
						gcnew Action<raw_file_property_struct^>(this, &sound_files_view_panel::raw_options_load_complete)
					);

					Dictionary<String^, sound_file_struct^>^ sound_files = safe_cast<sound_editor^>(ParentForm)->m_sound_files;
					if(!sound_files->ContainsKey(e->Node->FullPath))
						sound_files->Add(e->Node->FullPath, sound_file);

					e->Node->Tag = sound_file;
				}
				//else (options has been loaded) show options saved in Node->Tag
				else
				{
					sound_file_struct^ sound_struct = safe_cast<sound_file_struct^>(e->Node->Tag);
					if(sound_struct->m_is_loaded)
					{
						raw_file_selected(this, gcnew sound_file_selection_event_args(sound_struct));
						m_rms_graph->set_graph_points	(sound_struct->m_sound_options->rms_data, sound_struct->m_sound_options->rms_discretization);
						m_rms_scroll->set_scrollbar		(
							m_rms_graph->min_graph_x,
							m_rms_graph->max_graph_x,
							m_rms_graph->min_graph_x,
							m_rms_graph->max_graph_x-m_rms_graph->min_graph_x
						);
						m_loading_label->Visible		= false;
						//m_property_grid->SelectedObject = sound_struct->m_raw_options;
					}
				}
			}
		}

		void	sound_files_view_panel::raw_options_load_complete				(raw_file_property_struct^ loaded_struct)
		{
			m_property_grid->SelectedObject = loaded_struct;
		}

		void	sound_files_view_panel::load_complete							(sound_file_struct^ loaded_struct)
		{
			m_loading_label->Visible = false;
			m_rms_graph->set_graph_points(loaded_struct->m_sound_options->rms_data, loaded_struct->m_sound_options->rms_discretization);
			raw_file_selected(this, gcnew sound_file_selection_event_args(loaded_struct));
			m_rms_scroll->set_scrollbar		(
						m_rms_graph->min_graph_x,
						m_rms_graph->max_graph_x,
						m_rms_graph->min_graph_x,
						m_rms_graph->max_graph_x-m_rms_graph->min_graph_x
						);
		}

		void	sound_files_view_panel::raw_files_tree_view_mouse_down			(System::Object^ , System::Windows::Forms::MouseEventArgs^  e)
		{
			m_drag_node = m_sound_files_tree_view->GetNodeAt(e->X, e->Y );
			if ( m_drag_node != nullptr )
			{
				// Remember the point where the mouse down occurred. The DragSize indicates
				// the size that the mouse can move before a drag event should be started.
				System::Drawing::Size dragSize = SystemInformation::DragSize;

				// Create a rectangle using the DragSize, with the mouse position being
				// at the center of the rectangle.
				m_drag_box_from_mouse_down = System::Drawing::Rectangle(Point(e->X - (dragSize.Width / 2),e->Y - (dragSize.Height / 2)),dragSize);
			}
			else
				m_drag_box_from_mouse_down = System::Drawing::Rectangle::Empty;

		}

		void	sound_files_view_panel::raw_files_tree_view_mouse_move			(System::Object^ , System::Windows::Forms::MouseEventArgs^  e)
		{
			if ( (e->Button & ::MouseButtons::Left) == ::MouseButtons::Left )
			{
				// If the mouse moves outside the rectangle, start the drag.
				if ( m_drag_box_from_mouse_down != Rectangle::Empty &&  !m_drag_box_from_mouse_down.Contains( e->X, e->Y ) )
				{                 
					// Proceed with the drag-and-drop, passing in the list item.
					this->m_sound_files_tree_view->DoDragDrop( m_drag_node->FullPath, DragDropEffects::Copy);
				}
			}
		}

		void	sound_files_view_panel::rms_scroll_changed						(System::Object^ , scroller_event_args^ )
		{
			m_rms_graph->min_view_x		= m_rms_scroll->scroller_position;
			m_rms_graph->max_view_x		= m_rms_scroll->scroller_position+m_rms_scroll->scroller_width;
		}

		void	sound_files_view_panel::rms_scroll_draw_background				(System::Object^ , PaintEventArgs^  e)
		{
			m_rms_graph->draw_with_graphics(e->Graphics);
		}

		///<summary>
		///Show object properties in property grid
		///</summary>
		///<param name="object">Object form show</param>
		void	sound_files_view_panel::show_properties(Object^ object)
		{
			if(object)
				m_property_grid->SelectedObject		= object;
			else
				m_property_grid->SelectedObject		= nullptr;
		}

		raw_file_property_struct^	sound_files_view_panel::selected_struct::get()
		{
			return safe_cast<raw_file_property_struct^>(m_property_grid->SelectedObject);
		}
		void						sound_files_view_panel::selected_struct::set(raw_file_property_struct^ value)
		{
			m_property_grid->SelectedObject = value;
		}
	}//namespace editor
}//namespace xray