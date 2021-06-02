#include "pch.h"
#include "select_library_class_form.h"
#include "tool_base.h"
#include "project_items.h"
#include "library_objects_tree.h"
#include "objects_library.h"

namespace xray{
namespace editor{

void select_library_class_form::in_constructor( )
{
	m_library_objects_tree				= ( gcnew library_objects_tree(m_tool) );

	m_library_objects_tree->show_empty_folders		= true;
	m_library_objects_tree->sync_tool_selected_name = false;

	m_library_objects_tree->m_current_path_changed	+= gcnew current_path_changed( this, &select_library_class_form::selected_path_changed );
	m_library_objects_tree->m_double_clicked		+= gcnew double_clicked( this, &select_library_class_form::selected_item_double_clicked );
	
	m_library_objects_tree->Dock		= System::Windows::Forms::DockStyle::Fill;
	panel1->Controls->Add				( m_library_objects_tree );
	name_textBox->Text					= "";
}

void select_library_class_form::selected_item_double_clicked( lib_item_base^ )
{
	if(ok_button->Enabled)
		ok_button_Click( nullptr, nullptr );
}

void select_library_class_form::selected_path_changed( System::String^ new_path, System::String^ prev_path )
{
	XRAY_UNREFERENCED_PARAMETERS	( prev_path );
	name_textBox->Text				= new_path;
}

void select_library_class_form::initialize( System::String^ initial, bool chooser_mode, System::String^ filter )
{
	m_chooser_mode					= chooser_mode;

	name_textBox->Enabled			= !m_chooser_mode;
	ok_button->Enabled				= false;

	m_library_objects_tree->allow_folder_creation	= !chooser_mode;
	m_library_objects_tree->allow_items_reparent	= !chooser_mode;
	m_library_objects_tree->allow_items_rename		= !chooser_mode;
	m_library_objects_tree->allow_items_delete		= !chooser_mode;
	m_library_objects_tree->allow_folder_selection	= !chooser_mode;
	m_library_objects_tree->filter					= filter;

	if( m_library_objects_tree->fill_objects_list( initial ))
	{
		ok_button->Enabled			= true;
		ok_button->Focus			( );
	}
}

void select_library_class_form::name_textBox_TextChanged(System::Object^, System::EventArgs^ )
{
	bool valid_name			= is_valid_object_name( selected_path );
	if(!valid_name)
	{
		ok_button->Enabled	= false;
		return;
	}

	lib_folder^ lf			= m_tool->m_library->get_folder_item_by_full_path( selected_path, false);

	if( lf )
	{// selected path is folder
		ok_button->Enabled	= false;
	}else
	{// new or existing node
		ok_button->Enabled	= true;
	}
}

void select_library_class_form::select_library_class_form_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode==System::Windows::Forms::Keys::Return)
	{
		if(ok_button->Enabled)
			ok_button_Click(sender, e);
	}else
	if(e->KeyCode==System::Windows::Forms::Keys::Escape)
	{
		cancel_button_Click(sender, e);
	}
}

void select_library_class_form::ok_button_Click( System::Object^, System::EventArgs^ )
{
	DialogResult			= System::Windows::Forms::DialogResult::OK;
}

void select_library_class_form::cancel_button_Click( System::Object^, System::EventArgs^ )
{
	DialogResult			= System::Windows::Forms::DialogResult::Cancel;
}

} //namespace editor
} //namespace xray

