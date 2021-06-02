////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_RESOURCE_H_INCLUDED
#define TEXTURE_RESOURCE_H_INCLUDED

#include "resource_base.h"
#include "texture_document.h"
#include "texture_editor_source.h"
#include "texture_options.h"

namespace xray
{
	namespace editor
	{
		ref class texture_resource : resource_base
		{
		public:
			virtual controls::document_base^	get_new_document			(controls::document_editor_base^ editor){return gcnew texture_document(editor);}
			virtual controls::tree_view_source^ get_resources_source		() {return gcnew texture_editor_source();}
			virtual String^						get_view_panel_name			() {return "Textures";}
			virtual String^						get_properties_panel_name	() {return "Texture Properties";}
			virtual String^						get_editor_name				() {return "Texture";}
		}; // class texture_resource
	} // namespace editor
} // namespace xray

#endif // #ifndef TEXTURE_RESOURCE_H_INCLUDED