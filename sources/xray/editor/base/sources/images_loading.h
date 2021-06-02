////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef IMAGES_LOADING_H_INCLUDED
#define IMAGES_LOADING_H_INCLUDED

namespace xray {
namespace editor_base {

public ref class image_loader
{
public:
	static System::Windows::Forms::ImageList^ load_images(System::String^ resource_id, System::String^ resource_name, u32 width, u32 should_be_count, System::Reflection::Assembly^ assembly);
	System::Windows::Forms::ImageList^ get_shared_images16();
};

} // namespace editor_base
} // namespace xray
#endif // #ifndef IMAGES9X9_H_INCLUDED