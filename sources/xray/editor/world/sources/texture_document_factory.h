////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_DOCUMENT_FACTORY_H_INCLUDED
#define TEXTURE_DOCUMENT_FACTORY_H_INCLUDED

#include "resource_document_factory.h"
#include "texture_document.h"

namespace xray
{
	namespace editor
	{
		ref class texture_document_factory : resource_document_factory
		{
		public:
			virtual controls::document_base^	new_document			(controls::document_editor_base^ editor){return gcnew texture_document(editor);}
		}; // class texture_resource
	} // namespace editor
} // namespace xray

#endif // #ifndef TEXTURE_DOCUMENT_FACTORY_H_INCLUDED