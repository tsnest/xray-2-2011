////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "dependency_editor_bool_float.h"

namespace xray
{
	namespace editor
	{
		namespace controls
		{
			void dependency_editor_bool_float::in_constructor()
			{
				m_text_box_1->TextChanged += gcnew EventHandler(this, &dependency_editor_bool_float::values_processor);
				m_text_box_2->TextChanged += gcnew EventHandler(this, &dependency_editor_bool_float::values_processor);
			}

			list_of_float^	dependency_editor_bool_float::values::get		()
			{
				return gcnew list_of_float(gcnew array<Single>(2){convert_to_single(m_text_box_1->Text), Single::Parse(m_text_box_2->Text)});
			}

			void			dependency_editor_bool_float::values::set		(list_of_float^ values)
			{
				if(values->Count < 2)
					return;

				m_text_box_1->Text = values[0].ToString();
				m_text_box_2->Text = values[1].ToString();
			}
		}//namespace controls
	}
}