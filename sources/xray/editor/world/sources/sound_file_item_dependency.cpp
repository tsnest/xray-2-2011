#include "pch.h"
#include "sound_file_item_dependency.h"
#include "inplace_combo_box.h"
#include "sound_item_factory.h"
#include "dependency_editor.h"
#include "sound_item_struct.h"
#include "sound_file_item_property.h"

typedef xray::editor::sound_item_struct::property_dependency property_dependency;
typedef xray::editor::controls::dependency_editor dependency_editor;

namespace xray
{
	namespace editor
	{
		static Boolean loading = false;

		#pragma region | Properties | 

		ObjectCollection^				sound_file_item_dependency::combobox_items::get					()
		{
			return m_dependencies_combo_box->Items;
		}

		Int32							sound_file_item_dependency::selected_combobox_index::get		()
		{
			return m_dependencies_combo_box->SelectedIndex;
		}

		void							sound_file_item_dependency::selected_combobox_index::set(Int32 value)
		{
			m_dependencies_combo_box->SelectedIndex = value;
		}
		sound_item_dependency_struct^	sound_file_item_dependency::selected_combobox_item::get			()
		{
			return safe_cast<sound_item_dependency_struct^>(m_dependencies_combo_box->SelectedItem);
		}

		Control^						sound_file_item_dependency::editor_container::get				()
		{
			return m_split_container->Panel2;
		}

		list_of_float^					sound_file_item_dependency::values::get							()
		{
			return gcnew list_of_float(safe_cast<controls::dependency_editor^>( m_split_container->Controls[0])->selected_values);
		}

		void							sound_file_item_dependency::values::set							(list_of_float^)
		{
			//safe_cast<controls::dependency_editor^>( m_split_container->Controls[0])->SetValues(value->ToArray());
		}

		sound_file_item_property^		sound_file_item_dependency::parent_property::get				()
		{
			return m_parent_property;
		}
		void							sound_file_item_dependency::parent_property::set				(sound_file_item_property^ value)
		{
			m_parent_property = value;
		}

		property_dependency^			sound_file_item_dependency::selected_dependency::get			()
		{
			return m_selected_dependency;
		}

		void							sound_file_item_dependency::selected_dependency::set			(property_dependency^ value)
		{
			m_selected_dependency					= value;
			m_delete_button->Enabled				= true;

			Type^ prop_type;
			Type^ depend_type;

			loading									= true;
			m_dependencies_combo_box->SelectedIndex = safe_cast<int>( m_selected_dependency->type);
			loading									= false;

			depend_type								= selected_combobox_item->type;
			prop_type								= safe_cast<sound_item_property_struct^>(m_parent_property->selected_combobox_item)->type;

			dependency_editor^ editor				= sound_item_factory::CreateDependencyEditor(depend_type, prop_type);
			editor->m_loading						= true;
			editor->selected_values					= m_selected_dependency->values;
			editor->m_loading						= false;

			editor_container->Controls->Clear		();
			editor_container->Controls->Add			(editor);
			editor_container->Controls[0]->Dock		= DockStyle::Fill;
		}

		#pragma endregion

		#pragma region |  Methods   | 

		void							sound_file_item_dependency::in_constructor						()
		{
			m_dependencies_combo_box								= gcnew inplace_combo_box();
			this->m_split_container_inner->Panel2->Controls->Add	(m_dependencies_combo_box);

			this->m_dependencies_combo_box->SelectedIndexChanged	+=	gcnew EventHandler<EventArgs^>(this, &sound_file_item_dependency::combobox_selected_index_changed);

			this->m_dependencies_combo_box->Dock					= DockStyle::Top;
			this->Dock												= DockStyle::Top;
		}

		void							sound_file_item_dependency::combobox_selected_index_changed	(Object^ , EventArgs^ e)
		{
			if(m_parent_property->selected_combobox_item == nullptr || loading)
				return;

			//if it's a new property
			if(m_selected_dependency == nullptr)
			{
				//add new property to struct and bind it to this
				sound_item_struct::property_dependency^ dep				= gcnew sound_item_struct::property_dependency();
				dep->type												= selected_combobox_item->dependency_type;
				parent_property->selected_property->dependencies->Add	(dep);
				selected_dependency										= dep;

				//create empty dependency
				Control^ ctrl											= safe_cast<UserControl^>(sound_item_factory::CreateDependency(parent_property));
				parent_property->dependencies->Add						(ctrl);
				parent_property->dependencies->SetChildIndex			(ctrl, 0);
			}
			//is dependency changes
			else
			{ 
				Type^ prop_type;
				Type^ depend_type;

				depend_type		= selected_combobox_item->type;
				prop_type		= safe_cast<sound_item_property_struct^>(m_parent_property->selected_combobox_item)->type;
				m_selected_dependency->values->Clear();

				dependency_editor^ editor				= sound_item_factory::CreateDependencyEditor(depend_type, prop_type);
				editor->selected_values					= m_selected_dependency->values;

				m_selected_dependency->type				= selected_combobox_item->dependency_type;

				editor_container->SuspendLayout			();
				editor_container->Controls->Clear		();
				editor_container->Controls->Add			(editor);
				editor_container->Controls[0]->Dock		= DockStyle::Fill;
				editor_container->ResumeLayout			(false);
				editor_container->PerformLayout			();
			}
			SelectedIndexChanged(this, e);
		}

		void							sound_file_item_dependency::m_delete_button_Click			(Object^ , EventArgs^ )
		{
			parent_property->selected_property->dependencies->Remove	(m_selected_dependency);
			parent_property->dependencies->Remove						(this);
		}
		
		void							sound_file_item_dependency::check_editor_type				()
		{
			combobox_selected_index_changed(this, EventArgs::Empty);
		}

		#pragma endregion	
	}
}