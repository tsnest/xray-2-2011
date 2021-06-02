////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DELEGATES_H_INCLUDED
#define DELEGATES_H_INCLUDED

#include "document_base.h"

using WeifenLuo::WinFormsUI::Docking::IDockContent;

namespace xray
{
	namespace editor
	{
		namespace controls
		{
			public delegate document_base^	document_create_callback();			
			public delegate IDockContent^	content_reload_callback(String^ content_name);

		} // namespace controls
	} // namespace editor
} // namespace xray

#endif // #ifndef DELEGATES_H_INCLUDED