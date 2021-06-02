////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_file_item_property.h"
#include "sound_file_item_dependency.h"
#include "inplace_combo_box.h"
#include "sound_item_factory.h"
#include "dependency_editor.h"
#include "sound_file_item.h"

using namespace xray::editor::controls;

typedef ComboBox::ObjectCollection ObjectCollection;
typedef xray::editor::controls::dependency_editor dependency_editor;

namespace xray
{
	namespace editor
	{
		static Boolean loading = false;

		void				sound_file_item_property::in_constructor								()
		{
			this->Height = 27;
			m_properties_combo_box								= gcnew inplace_combo_box();
			this->m_split_container_inner->Panel2->Controls->Add		(m_properties_combo_box);
			this->m_properties_combo_box->SelectedIndexChanged	+= gcnew EventHandler<EventArgs^>(this, &sound_file_item_property::combo_box_selected_index_changed);

			this->m_properties_combo_box->Dock					= DockStyle::Top;
			this->Dock											= DockStyle::Top;
		}

		ObjectCollection^	sound_file_item_property::combobox_items::get							()
		{
			return m_properties_combo_box->Items;
		}

		void				sound_file_item_property::m_split_container_Panel2_ControlAdded			(Object^  , ControlEventArgs^  e)
		{
			this->Height += e->Control->Height;
		}

		void				sound_file_item_property::m_split_container_Panel2_ControlRemoved		(Object^  , ControlEventArgs^  e)
		{
			this->Height -= e->Control->Height;
		}

		void				sound_file_item_property::combo_box_selected_index_changed				(Object^ , EventArgs^ )
		{
			if(loading)
				return;

			//if it's a new property
			if(selected_property == nullptr)
			{
				//add new property to struct and bind it to this
				sound_item_struct::sound_property^ prop				= gcnew sound_item_struct::sound_property();
				prop->type											= selected_combobox_item->property_type;
				parent_sound->selected_sound->properties->Add		(prop);
				selected_property									= prop;

				//create empty dependency
				Control^ ctrl							= safe_cast<UserControl^>(sound_item_factory::CreateProperty(parent_sound));
				parent_sound->properties->Add			(ctrl);
				parent_sound->properties->SetChildIndex	(ctrl, 0);
			}
			//if property changes
			else
			{
				m_selected_property->type				= selected_combobox_item->property_type;
				for each (sound_file_item_dependency^ dep in dependencies)
				{
					if(dep->selected_combobox_item == nullptr)
						continue;
					dep->check_editor_type();
				}
			}
		}

		sound_item_struct::sound_property^	sound_file_item_property::selected_property::get		()
		{
			return m_selected_property;
		}

		void								sound_file_item_property::selected_property::set(sound_item_struct::sound_property^ value)
		{
			m_selected_property			= value;
			m_delete_button->Enabled	= true;
			this->Height				= 6;
			dependencies->Clear			();

			loading						= true;
			m_properties_combo_box->SelectedIndex = safe_cast<int>( m_selected_property->type);
			loading						= false;

			dependencies->Owner->SuspendLayout();

			for each(sound_item_struct::property_dependency^ dep in m_selected_property->dependencies)
			{
				sound_file_item_dependency^ dependency	= sound_item_factory::CreateDependency(this);
				dependencies->Add						(dependency);
				dependencies->SetChildIndex				(dependency, 0);
				dependency->selected_dependency			= dep;
			}

			//create empty dependency
			Control^ ctrl								= safe_cast<UserControl^>(sound_item_factory::CreateDependency(this));
			dependencies->Add							(ctrl);
			dependencies->SetChildIndex					(ctrl, 0);

			dependencies->Owner->ResumeLayout			(false);
			dependencies->Owner->PerformLayout			();

			if(this->Height < 24)
				this->Height = 24;
		}

		void								sound_file_item_property::m_delete_button_Click						(Object^ , EventArgs^ )
		{
			parent_sound->selected_sound->properties->Remove	(m_selected_property);
			parent_sound->properties->Remove					(this);
		}
		
		sound_file_item^					sound_file_item_property::parent_sound::get				()
		{
			return m_parent_sound;
		}

		void								sound_file_item_property::parent_sound::set(sound_file_item^ value)
		{
			m_parent_sound = value;
		}

		sound_item_property_struct^			sound_file_item_property::selected_combobox_item::get	()
		{
			return safe_cast<sound_item_property_struct^>(m_properties_combo_box->SelectedItem);
		}

		Int32								sound_file_item_property::selected_combobox_index::get	()
		{
			return m_properties_combo_box->SelectedIndex;
		}
		void								sound_file_item_property::selected_combobox_index::set	(Int32 value)
		{
			m_properties_combo_box->SelectedIndex = value;
		}
	}
}