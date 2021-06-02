////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "editor_base.h"
#include "images_loading.h"
#include <xray/editor/base/images/images16x16.h>


#pragma managed( push, off )
#	include <xray/editor/base/api.h>
	xray::editor_base::allocator_type* xray::editor_base::g_allocator = NULL;
#pragma managed( pop )

namespace xray {
namespace editor_base {

System::Windows::Forms::ImageList^ image_loader::load_images(System::String^ resource_id, System::String^ resource_name, u32 width, u32 should_be_count, System::Reflection::Assembly^ assembly)
{
	System::Resources::ResourceManager^ mgr		= gcnew System::Resources::ResourceManager(resource_name, assembly);
	System::Drawing::Bitmap^ strip				= safe_cast<System::Drawing::Bitmap^>(mgr->GetObject(resource_id));
	delete mgr;

	System::Windows::Forms::ImageList^ result	= gcnew System::Windows::Forms::ImageList;
	result->ColorDepth							= System::Windows::Forms::ColorDepth::Depth32Bit;
	result->TransparentColor					= System::Drawing::Color::Transparent;
	result->ImageSize							= System::Drawing::Size( width, strip->Height );
	result->Images->AddStrip					( strip );
	ASSERT										( result->Images->Count == (int)should_be_count );
	XRAY_UNREFERENCED_PARAMETER					( should_be_count );
	return result;
}

System::Windows::Forms::ImageList^ image_loader::get_shared_images16( )
{
	return load_images("images16x16", "editor_base.resources", 16, (int)xray::editor_base::images16x16_count, this->GetType()->Assembly);
}

} // namespace editor_base
} // namespace xray
