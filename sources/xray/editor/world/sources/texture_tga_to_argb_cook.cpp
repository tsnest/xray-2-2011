////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_tga_to_argb_cook.h"
#include <xray/resources_cook_classes.h>
#include <amd/compress/texture_compressor/api.h>
#include <amd/compress/ATI_Compress.h>

using namespace xray::resources;

using		xray::texture_compressor::TgaHeader;

namespace xray {
namespace editor {

static pvoid		buffer_ptr	= NULL;
static u32			size		= 0;

static pvoid		tga_reader_allocate			( u32 size, bool is_additional_buffer )
{
	if(is_additional_buffer)
		return XRAY_MALLOC_IMPL( debug::g_mt_allocator, size, "tempprary for image");

	return buffer_ptr;
}

static void			tga_reader_deallocate			( pvoid buffer )
{
	XRAY_FREE_IMPL( debug::g_mt_allocator, buffer);
}

texture_tga_to_argb_cook::texture_tga_to_argb_cook () 
: inplace_unmanaged_cook(resources::texture_tga_to_argb_class, reuse_false, use_current_thread_id, use_cook_thread_id) {}

void	texture_tga_to_argb_cook::create_resource_in_creation_data	(resources::query_result_for_cook & in_out_query,
																	 mutable_buffer						in_out_unmanaged_resource_buffer)
{
	texture_tga_argb_buffers *	buf_struct	= reinterpret_cast<texture_tga_argb_buffers*>(in_out_unmanaged_resource_buffer.c_ptr());

	if (threading::interlocked_increment(buf_struct->m_need_process_flag) == 1)
	{
		TgaHeader* header				= (TgaHeader*)(buf_struct->m_tga_buffer->c_ptr());
		size							= header->width * header->height * 4;
		buffer_ptr						= buf_struct->m_rgba_buffer;

		ATI_TC_Texture							temp_texture;
		xray::texture_compressor::load_tga		(
			tga_reader_allocate,
			tga_reader_deallocate,
			buf_struct->m_tga_buffer->c_ptr(),
			temp_texture
		);
	}

	in_out_query.set_zero_unmanaged_resource	();
	in_out_query.finish_query		(result_success);
}

} //namespace editor
} // namespace xray
