////////////////////////////////////////////////////////////////////////////
//	Created		: 10.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_property_panel.h"
#include "raw_file_property_struct.h"
#include "property_holder.h"
#include "object_base.h"
#include "tool_base.h"

using namespace System;
using namespace System::Collections::Generic;

using xray::editor::controls::property_grid;
using xray::editor::sound_property_panel;

namespace xray
{
	namespace editor
	{

		///<summary>
		///Add property grid to form
		///</summary>
		void sound_property_panel::in_constructor()
		{
			m_property_grid			= gcnew property_grid( );
			m_property_grid->Dock	= System::Windows::Forms::DockStyle::Fill;
			m_property_grid->ToolbarVisible = true;
			m_property_grid->PropertySort = PropertySort::NoSort;

			SuspendLayout			( );
			Controls->Add			( m_property_grid );
			ResumeLayout			( false );
		}

		///<summary>
		///Show object properties in property grid
		///</summary>
		///<param name="object">Object form show</param>
		void sound_property_panel::show_properties(Object^ object)
		{
			if(object)
				m_property_grid->SelectedObject		= object;
			else
				m_property_grid->SelectedObject		= nullptr;
		}

		raw_file_property_struct^	sound_property_panel::selected_struct::get()
		{
			return safe_cast<raw_file_property_struct^>(m_property_grid->SelectedObject);
		}
		void						sound_property_panel::selected_struct::set(raw_file_property_struct^ value)
		{
			m_property_grid->SelectedObject = value;
		}
	}//namespace editor
}//namespace xray