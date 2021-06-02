#include "pch.h"
#include "sound_item_editor_panel.h"
#include "inplace_combo_box.h"
#include "sound_file_item.h"

namespace xray
{
	namespace editor
	{

		void					sound_item_editor_panel::in_constructor						()
		{
		}

		void					sound_item_editor_panel::save_tool_strip_button_Click			(System::Object^ , System::EventArgs^ )
		{
			if(selected_item != nullptr)
				selected_item->save();
		}

		sound_item_struct^		sound_item_editor_panel::selected_item::get					()
		{
			return m_selected_item;
		}
		void					sound_item_editor_panel::selected_item::set					(sound_item_struct^ value)
		{
			m_selected_item = value;

			m_main_panel->SuspendLayout();

			m_main_panel->Controls->Clear();
			for each(sound_item_struct::sound^ sound in m_selected_item->sounds)
			{
				sound_file_item^ item						= gcnew sound_file_item();
				item->sound_name							= sound->name;
				item->Dock									= DockStyle::Top;
				item->Width									= m_main_panel->ClientSize.Width;
				item->parent_editor							= this;
				item->selected_sound						= sound;

				this->m_main_panel->Controls->Add			(item);
				this->m_main_panel->Controls->SetChildIndex	(item, 0);
			}

			m_main_panel->ResumeLayout(true);
		}
		void					sound_item_editor_panel::m_main_panel_drag_over				(System::Object^ , System::Windows::Forms::DragEventArgs^  e)
		{
			// Determine whether string data exists in the drop data. If not, then
			// the drop effect reflects that the drop cannot occur.
			if (  !e->Data->GetDataPresent( System::String::typeid ) )
			{
				e->Effect = DragDropEffects::None;
				//DropLocationLabel->Text = "None - no string data.";
				return;
			}
			
			e->Effect = DragDropEffects::Copy;
		}

		void					sound_item_editor_panel::m_main_panel_drag_drop				(System::Object^ , System::Windows::Forms::DragEventArgs^  e)
		{
			// Ensure that the list item index is contained in the data.
			if (m_selected_item != nullptr && e->Data->GetDataPresent( System::String::typeid ) )
			{
				String^ item_name = safe_cast<String^>(e->Data->GetData( System::String::typeid ));

				// Perform drag-and-drop, depending upon the effect.
				if ( e->Effect == DragDropEffects::Copy)
				{
					sound_file_item^ item						= gcnew sound_file_item();
					item->sound_name							= item_name;

					sound_item_struct::sound^ snd				= gcnew sound_item_struct::sound();
					snd->name									= item_name;
					m_selected_item->sounds->Add				(snd);
					item->parent_editor							= this;
					item->selected_sound						= snd;

					item->Dock									= DockStyle::Top;
					this->m_main_panel->Controls->Add			(item);
					this->m_main_panel->Controls->SetChildIndex	(item, 0);
				}
			}
			else
				e->Effect = DragDropEffects::None;

			// Reset the label text.
			//DropLocationLabel->Text = "None";
		}

	}//namespace editor
}//namespace xray