#include "pch.h"
#include "resource_loading_dialog.h"
#include "tool_base.h"

namespace xray{
namespace editor{

void resource_loading_dialog::select_button_Click(System::Object^, System::EventArgs^)
{

	System::String^ selected	= nullptr;
	if( editor_base::resource_chooser::choose(	m_tool->name(), 
												m_missing_resource_name, 
												nullptr, 
												selected,
												true)
		)
	{
		selected_resource_name	= selected;
		m_dialog_result			= ::DialogResult::OK;
		Hide					( );
	}
}

void resource_loading_dialog::ignore_button_Click(System::Object^, System::EventArgs^)
{
	m_dialog_result			= ::DialogResult::Cancel;
	Hide					( );
}

void resource_loading_dialog::init_labels( System::String^ label1, System::String^ label2 )
{
	message1_label->Text = label1;
	message2_label->Text = label2;
}

subst_resource_result resource_loading_dialog::do_dialog( tool_base^ tool, System::String^ missing_resource_name )
{
	m_tool					= tool;
	m_missing_resource_name	= missing_resource_name;

	ShowDialog	( );

	if(m_dialog_result == ::DialogResult::OK)
	{
		if (btn_this_only->Checked)
			return subst_resource_result::subst_one;
		else
		if (btn_this_name->Checked)
			return subst_resource_result::subst_all_by_name;
		else
		if (btn_all->Checked)
			return subst_resource_result::subst_all_any_name;
		else
			UNREACHABLE_CODE();
	}else
	{
		if (btn_this_only->Checked)
			return subst_resource_result::ignore_one;
		else
		if (btn_this_name->Checked)
			return subst_resource_result::ignore_all_by_name;
		else
		if (btn_all->Checked)
			return subst_resource_result::ignore_all_any_name;
		else
			UNREACHABLE_CODE();
	}
	return subst_resource_result::ignore_one;
}

} // namespace editor
} // namespace xray