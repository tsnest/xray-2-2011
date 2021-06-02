////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "dependency_editor_float_float.h"
#include "float_curve_editor.h"
#include "float_curve.h"

using namespace System;

using xray::editor::float_curve_event_args;
using System::EventArgs;

namespace xray
{
	namespace editor
	{
		namespace controls
		{
			void			dependency_editor_float_float::in_constructor	()
			{
				m_curve_editor									= gcnew controls::float_curve_editor();
				m_curve_editor->SelectedObject					= gcnew float_curve();
				m_curve_editor->SelectedObject->clear_keys		();
				m_curve_editor->SelectedObject->add_key			(0, 50);
				m_curve_editor->SelectedObject->add_key			(100, 50);
				m_curve_editor->Dock = DockStyle::Fill;
				Controls->Add									(m_curve_editor);

				m_curve_editor->SelectedObject->key_removed		+= gcnew EventHandler<float_curve_event_args^>	(this, &dependency_editor_float_float::curve_changed);
				m_curve_editor->SelectedObject->key_added		+= gcnew EventHandler<float_curve_event_args^>	(this, &dependency_editor_float_float::curve_changed);
				m_curve_editor->SelectedObject->key_altered		+= gcnew EventHandler<float_curve_event_args^>	(this, &dependency_editor_float_float::curve_changed);
				m_curve_editor->SelectedObject->keys_cleared	+= gcnew EventHandler							(this, &dependency_editor_float_float::values_processor);		
			}

			list_of_float^	dependency_editor_float_float::values::get		()
			{
				list_of_float^ retval = gcnew list_of_float();
				for each(float_curve_pair^ pair in m_curve_editor->SelectedObject->keys)
				{
					retval->Add(pair->time);
					retval->Add(pair->value);
				}
				return retval;
			}

			void			dependency_editor_float_float::values::set		(list_of_float^ values)
			{
				m_curve_editor->SelectedObject->clear_keys();

				int max_val = safe_cast<int>(values->Count/2);

				for(int i=0; i<max_val; ++i)
				{
					m_curve_editor->SelectedObject->add_key(values[i*2], values[i*2+1]);
				}
			}

			void			dependency_editor_float_float::curve_changed(Object^ , float_curve_event_args^ )
			{
				values_processor(this, EventArgs::Empty);
			}
		}//namespace controls
	}//namespace editor
}//namespace xray