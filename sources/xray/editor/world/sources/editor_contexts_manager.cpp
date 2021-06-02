////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_contexts_manager.h"
#include "logic_editor_panel.h"
#include "editor_world.h"
#include "level_editor.h"

namespace xray {
namespace editor {

bool editor_contexts_manager::context_fit( System::String^ context )
{
	if(context=="Global")
	{	
		return true;
	}
// 	else
// 	if(context=="LogicView")
// 	{
// 		bool is_fit = true;
// 		Form^ frm = Form::ActiveForm;
// 		if( xray::editor::wpf_controls::dot_net_helpers::is_type<WeifenLuo::WinFormsUI::Docking::FloatWindow^>( frm ) )
// 		{
// 			WeifenLuo::WinFormsUI::Docking::FloatWindow^ float_window = (WeifenLuo::WinFormsUI::Docking::FloatWindow^)frm; 
// 			if( float_window->DockPanel->ActiveContent->GetType() != logic_editor_panel::typeid  )
// 				return false;
// 		}
// 		
// 		return true;
//	}
	else if(context=="View")
	{
		bool result = m_editor_world.view_window()->context_fit();
		return result;
	}else
	{
		level_editor^ le		= m_editor_world.get_level_editor( );
		return le->context_fit	( context );
	}
}

List<System::String^>^ editor_contexts_manager::get_contexts_list( )	
{
	List<System::String^>^ result_lst = gcnew List<System::String^>();

	result_lst->Add("Global");
	result_lst->Add("View");
	result_lst->Add("LogicView");
	m_editor_world.get_level_editor()->get_contexts_list( result_lst );
	return result_lst;
}


} // namespace editor
} // namespace xray