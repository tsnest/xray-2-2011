////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_RESOURCE_DOCUMENT_H_INCLUDED
#define I_RESOURCE_DOCUMENT_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray
{
	namespace editor
	{
		ref struct resource_struct;

		public interface class i_resource_document
		{
		public:
			resources::unmanaged_resource_ptr*		get_new_unmanaged_resource_ptr();
		};//interface class i_resource_editor
	}//namesapce editor
}//namespace editor
#endif // #ifndef I_RESOURCE_DOCUMENT_H_INCLUDED