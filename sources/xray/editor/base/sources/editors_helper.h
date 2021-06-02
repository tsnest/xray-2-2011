////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EDITORS_HELPER_H_INCLUDED
#define EDITORS_HELPER_H_INCLUDED

using namespace System;

using System::Windows::Forms::Form;

namespace xray
{
	namespace editor_base
	{
		public ref class editors_helper
		{
		public:
			static		Boolean		is_active_form_belong_to( Form^ form );

		}; // class editors_helper
	} // namespace editor_base
} // namespace xray

#endif // #ifndef EDITORS_HELPER_H_INCLUDED