#include "pch.h"
#include "compound_object_tab.h"
#include "tool_compound.h"
#include "object_compound.h"
#include "level_editor.h"
#include "project.h"
#include "property_container.h"
#include "command_add_library_object.h"
#include "property_holder.h"

using xray::editor::compound_object_tab;
using xray::editor::object_compound;
using xray::editor::object_base;
using xray::editor::tool_compound;
using xray::editor::command_add_library_object;
using xray::editor::controls::property_grid;

void compound_object_tab::in_constructor()
{
	m_property_grid			= gcnew property_grid( );
	m_property_grid->Dock	= System::Windows::Forms::DockStyle::Fill;

	panel->SuspendLayout	( );
	panel->Controls->Add	( m_property_grid );
	panel->ResumeLayout		( false );

}

void compound_object_tab::set_compound_object(object_compound^ object, xray::editor::enum_current_mode mode)
{
	m_mode							= mode;

	m_current_object				= object;
	
	fill_object_props				();
	btn_add_from_selection->Enabled	= (m_mode==mode_new);
	btn_del_selected->Enabled		= (m_mode==mode_new);
}

void compound_object_tab::fill_object_props()
{
	::property_holder*		properties = dynamic_cast< ::property_holder* >( m_current_object->get_property_holder() );
	m_property_grid->SelectedObject	= properties->container( );


	objects_list_box->Items->Clear	();
	for(u32 i=0; i<m_current_object->count(); ++i)
	{
		object_base^ o				= m_current_object->get_object(i);
		objects_list_box->Items->Add(gcnew System::String(o->get_name()), false);
	}
}

System::Void compound_object_tab::on_add_from_selection(System::Object^, System::EventArgs^)
{
	object_list^ list_selected	= gcnew object_list;
	project^ project			= m_tool->get_level_editor()->get_project();
	list_selected->AddRange		(project->selection_list());
	
	project->select_object(nullptr, enum_selection_method_set);
	if(list_selected->Count)
	{
		m_current_object->block_collision_rebuild();
		
		float4x4 t;
		t.identity();
		t.c.xyz() = list_selected[0]->get_position();
		m_current_object->set_transform(t);

		for each (object_base^ o in list_selected)
		{
			if(m_current_object->add_object(o))
			{
				project->remove_item(o->m_owner_project_item, false);
				objects_list_box->Items->Add(gcnew System::String(o->get_name()), false);
			}else
				LOG_INFO("failed to add object %s", o->get_name());
		}
		m_current_object->unblock_collision_rebuild();
	}else
	{
		LOG_INFO("nothing selected");
	}
}

System::Void compound_object_tab::on_ok_button(System::Object^, System::EventArgs^)
{
	if(m_mode==mode_new)
	{
		lib_item^ li					= m_tool->new_lib_item(m_current_object->get_name());
		float4x4 m						= m_current_object->get_transform();
		m_current_object->set_transform	(float4x4().identity());
		m_current_object->set_lib_name	(m_current_object->get_name());
		li->m_lib_item_name				= m_current_object->get_name();
		m_current_object->save			(*li->m_config);
		li->m_property_restrictor->save	(*li->m_config);
		m_tool->on_commit_library_item	(li);
		m_tool->save_library			();
		m_tool->get_level_editor()->get_command_engine()->run( gcnew command_add_library_object ( m_tool->get_level_editor(), m_tool, m_current_object->get_name(), m, true ) );
	}
	close_internal						();
}

void compound_object_tab::close_internal()
{
	if(m_mode==mode_edit_library || m_mode==mode_new)
	{
		m_tool->destroy_object(m_current_object);
		m_current_object	= nullptr;
	}

	Hide					();
	m_mode					= mode_none;
}

System::Void compound_object_tab::on_cancel_button(System::Object^, System::EventArgs^)
{
	//implement rollback changes here
	close_internal			();
}

System::Void compound_object_tab::on_remove_selection(System::Object^, System::EventArgs^)
{
	u32 cnt = objects_list_box->Items->Count;
	for(u32 i=0; i<cnt; ++i)
	{
		if(objects_list_box->GetItemChecked(i))
			m_current_object->remove_object(m_current_object->get_object(i), true);
	}
	fill_object_props();
}

System::Void compound_object_tab::on_selected(System::Object^, System::EventArgs^)
{
//	object_base^ o			= m_current_object->get_object(objects_list_box->SelectedIndex);
}

System::Void compound_object_tab::on_properties(System::Object^, System::EventArgs^)
{
	int idx							= objects_list_box->SelectedIndex;
	if(idx!=-1)
	{
		object_list^ l				= gcnew object_list;
		object_base^ o				= m_current_object->get_object(objects_list_box->SelectedIndex);
		l->Add						(o);
		project^ project			= m_tool->get_level_editor()->get_project();
		project->show_object_inspector(l);
	}
}

System::Void compound_object_tab::on_set_pivot(System::Object^, System::EventArgs^)
{
	int idx				= objects_list_box->SelectedIndex;
	if(idx!=-1)
	{
		object_base^ o	= m_current_object->get_object(objects_list_box->SelectedIndex);
		m_current_object->set_pivot(o->get_transform());
	}
}
