////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "document_base.h"
#include "document_editor_base.h"

namespace xray {
namespace editor {
namespace controls{

void			document_base::on_form_closing(System::Object^ , System::Windows::Forms::FormClosingEventArgs^ e)
{
	e->Cancel = !check_saved();
	if(!e->Cancel)
		m_editor->on_document_closing(this);
}

void			document_base::on_form_activated(System::Object^ , System::EventArgs^ )
{
	if(this->Visible)
		m_editor->on_document_activated(this);
}

Boolean			document_base::check_saved			()
{	
	if(!is_saved)
	{
		::DialogResult result = System::Windows::Forms::MessageBox::Show(this, "Do you want to save changes in '"+this->Name+"' document?", 
			m_editor->Name, MessageBoxButtons::YesNoCancel, MessageBoxIcon::Exclamation);

		if(result == ::DialogResult::Cancel)
		{
			return false;
		}
		else if(result == ::DialogResult::Yes)
			save();
	}
	return true;
}

} // namespace controls
} // namespace editor
} // namespace xray