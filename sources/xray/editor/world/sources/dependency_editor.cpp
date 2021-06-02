////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "dependency_editor.h"


namespace xray
{
	namespace editor
	{
		namespace controls
		{
			void dependency_editor::values_processor(Object^ sender, EventArgs^ e)
			{
				XRAY_UNREFERENCED_PARAMETERS(sender,e);

				if ( m_loading )
					return;

				m_selected_values->Clear();
				m_selected_values->AddRange(values);
			}
		}//namespace controls
	}//namespace editor
}//namespace xray
