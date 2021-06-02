////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_editors_factory.h"
#include "texture_document_factory.h"
#include "texture_document_factory.h"

namespace xray {
namespace editor {

resource_document_factory^ resource_editors_factory::get_document_factory(resource_selector::resource_type type)
{
	switch(type)
	{
	case resource_selector::resource_type::Texture:
		return gcnew texture_document_factory();
		break;
	default:
		throw gcnew ArgumentException("The resource type do not match any cases.");
		break;
	}
}

controls::tree_view_source^	resource_editors_factory::get_resources_source(resource_selector::resource_type type)
{
	return get_resources_source(type, nullptr);
}

controls::tree_view_source^	resource_editors_factory::get_resources_source(resource_selector::resource_type type, Object^ filter)
{
	switch(type)
	{
	case resource_selector::resource_type::Texture:
		return (filter == nullptr)?gcnew texture_editor_source():gcnew texture_editor_source(filter);
		break;
	default:
		throw gcnew ArgumentException("The resource type do not match any cases.");
		break;
	}
}

} // namespace editor
} // namespace xray