////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_file_item.h"
#include "sound_file_item_property.h"
#include "sound_item_factory.h"
#include "sound_item_editor_panel.h"

namespace xray
{
	namespace editor
	{
		void sound_file_item::in_constructor							()
		{
			this->Height = 6;
			
		}

		void sound_file_item::m_split_container_Panel2_ControlAdded		(System::Object^ , ControlEventArgs^ e)
		{
			this->Height += e->Control->Height;
			e->Control->SizeChanged += gcnew EventHandler(this, &sound_file_item::m_split_container_Panel2_ControlSizeChanged);
		}

		void sound_file_item::m_split_container_Panel2_ControlRemoved		(System::Object^ , ControlEventArgs^  e)
		{
			this->Height -= e->Control->Height;
		}

		sound_item_editor_panel^		sound_file_item::parent_editor::get		()
		{
			return m_parent_editor;
		}

		void							sound_file_item::parent_editor::set		(sound_item_editor_panel^ value)
		{	
			m_parent_editor = value;
		}

		String^							sound_file_item::sound_name::get		()
		{
			return m_file_name_label->Text;
		}

		void							sound_file_item::sound_name::set		(String^ value)
		{
			m_file_name_label->Text = value;
		}

		void sound_file_item::m_split_container_Panel2_ControlSizeChanged	(System::Object^ , EventArgs^ )
		{
			Int32 height=6;

			for each(Control^ ctrl in m_split_container->Panel2->Controls)
			{
				height += ctrl->Height;
			}
			this->Height = height;
		}

		void sound_file_item::m_delete_button_Click							(System::Object^ , System::EventArgs^ )
		{
			m_parent_editor->selected_item->sounds->Remove(m_selected_sound);
			Parent->Controls->Remove(this);
		}

		sound_item_struct::sound^		sound_file_item::selected_sound::get		()
		{
			return m_selected_sound;
		}

		void							sound_file_item::selected_sound::set		(sound_item_struct::sound^ value)
		{
			m_selected_sound					= value;

			this->Height						= 6;
			properties->Clear					();
			properties->Owner->SuspendLayout	();

			for each(sound_item_struct::sound_property^ property in m_selected_sound->properties)
			{
				sound_file_item_property^ prop		= sound_item_factory::CreateProperty(this);
				properties->Add						(prop);
				properties->SetChildIndex			(prop, 0);

				prop->selected_property				= property;
			}

			sound_file_item_property^ prop		= sound_item_factory::CreateProperty(this);
			properties->Add						(prop);
			properties->SetChildIndex			(prop, 0);

			properties->Owner->ResumeLayout		(false);
			properties->Owner->PerformLayout	();
		}
		
	}//namespace editor
}//namespace xray