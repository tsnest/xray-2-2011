////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_EDITORS_FACTORY_H_INCLUDED
#define RESOURCE_EDITORS_FACTORY_H_INCLUDED

#include "resource_selector.h"

namespace xray
{
	namespace editor
	{
		interface class resource_base;

		ref class resource_editors_factory
		{
		public:
			static resource_document_factory^	get_document_factory(resource_selector::resource_type type);
			static controls::tree_view_source^	get_resources_source(resource_selector::resource_type type);
			static controls::tree_view_source^	get_resources_source(resource_selector::resource_type type, Object^ filter);
		}; // class resource_editors_factory
	} // namespace editor
} // namespace xray

#endif // #ifndef RESOURCE_EDITORS_FACTORY_H_INCLUDED