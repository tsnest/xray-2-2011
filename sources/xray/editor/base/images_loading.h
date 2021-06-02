//-------------------------------------------------------------------------------------------
//	Created		: 04.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
//-------------------------------------------------------------------------------------------

#ifndef IMAGES_LOADING_H_INCLUDED
#define IMAGES_LOADING_H_INCLUDED

namespace xray {
namespace editor_base {

	ref class image_loader
	{
	public:
		static System::Windows::Forms::ImageList^ load_images(System::String^ resource_id,	u32 width, u32 should_be_count, System::Type^ object_type)
		{
			System::Resources::ResourceManager^ mgr = gcnew System::Resources::ResourceManager("editor_base.resources", object_type->Assembly);
			System::Drawing::Bitmap^ strip = safe_cast<System::Drawing::Bitmap^>(mgr->GetObject(resource_id));

			delete mgr;

			System::Windows::Forms::ImageList^ result = gcnew System::Windows::Forms::ImageList();
			result->ColorDepth = ColorDepth::Depth32Bit;
			result->TransparentColor = System::Drawing::Color::Transparent;
			result->ImageSize = System::Drawing::Size(width, strip->Height);
			result->Images->AddStrip(strip);
			ASSERT(result->Images->Count == (int)should_be_count);
			XRAY_UNREFERENCED_PARAMETER(should_be_count);
			return result;
		}
	};
} // namespace editor_base
} // namespace xray
#endif // #ifndef IMAGES9X9_H_INCLUDED