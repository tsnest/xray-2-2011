////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "dependency_editor_float_bool.h"

using namespace System;

namespace xray
{
	namespace editor
	{
		namespace controls
		{
			void dependency_editor_float_bool::in_constructor()
			{
				m_text_box->TextChanged				+= gcnew EventHandler(this, &dependency_editor_float_bool::values_processor);
				m_combo_box->SelectedIndexChanged	+= gcnew EventHandler(this, &dependency_editor_float_bool::values_processor);
			}

			list_of_float^	dependency_editor_float_bool::values::get		()
			{
				return gcnew list_of_float(gcnew array<Single>(2){safe_cast<float>(m_combo_box->SelectedIndex), convert_to_single(m_text_box->Text)});
			}

			void			dependency_editor_float_bool::values::set		(list_of_float^ values)
			{
				if(values->Count < 2)
					return;

				m_combo_box->SelectedIndex	= safe_cast<Int32>(values[0]);
				m_text_box->Text			= values[1].ToString();
			}
		}//namespace controls
	}//namespace editor
}//namespace xray