////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ide_log_form.h"

namespace xray{
namespace editor{

void ide_log_form::error_message_out( System::String^ text )
{
	listBox1->Items->Add( text );
}

void ide_log_form::clear_error_messages( )
{
	listBox1->Items->Clear();
}

void ide_log_form::ide_log_form_FormClosing( System::Object^, System::Windows::Forms::FormClosingEventArgs^ e)
{
	e->Cancel	= true;
	Hide		( );
}

}// namespace editor
}//namespace xray

