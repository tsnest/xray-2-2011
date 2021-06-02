#include "pch.h"
#include "inplace_combo_box.h"

typedef ComboBox::ObjectCollection ObjectCollection;

namespace xray
{
	namespace editor
	{
		ObjectCollection^		inplace_combo_box::Items::get							()
		{
			return m_combo_box->Items;
		}
			
		Object^					inplace_combo_box::SelectedItem::get					()
		{
			return m_combo_box->SelectedItem;
		}

		void					inplace_combo_box::SelectedItem::set					(Object^ value)
		{
			m_combo_box->SelectedItem = value;
		}

		Int32					inplace_combo_box::SelectedIndex::get					()
		{
			return m_combo_box->SelectedIndex;
		}
		void					inplace_combo_box::SelectedIndex::set					(Int32 index)
		{
			m_combo_box->SelectedIndex = index;
		}

		String^					inplace_combo_box::SelectedText::get					()
		{
			return m_combo_box->SelectedItem->ToString();
		}

		void					inplace_combo_box::m_combo_box_SelectedIndexChanged		(System::Object^ , System::EventArgs^  )
		{
			this->m_label->Text				= this->m_combo_box->SelectedItem->ToString();
			
			SelectedIndexChanged(this, EventArgs::Empty);
		}
		
		void					inplace_combo_box::m_label_Click						(System::Object^ , System::EventArgs^  )
		{
			this->m_combo_box->Text			= this->m_label->Text;
			this->m_label->Visible			= false;
			this->m_combo_box->Visible		= true;
			this->m_combo_box->DroppedDown	= true;
		}
		
		void					inplace_combo_box::m_combo_box_DropDownClosed			(System::Object^ , System::EventArgs^  )
		{
			this->m_label->Visible			= true;
			this->m_combo_box->Visible		= false;
		}
	}//namespace editor
}//namespace xray