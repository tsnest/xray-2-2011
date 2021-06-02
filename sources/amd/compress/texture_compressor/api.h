////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef AMD_COMPRESS_TEXTURE_COMPRESSOR_API_H_INCLUDED
#define AMD_COMPRESS_TEXTURE_COMPRESSOR_API_H_INCLUDED

#include <xray/os_include.h>
#include <amd/compress/ati_compress.h>

#ifndef XRAY_TEXTURE_COMPRESSOR_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_TEXTURE_COMPRESSOR_API			extern
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_TEXTURE_COMPRESSOR_BUILDING
#			define XRAY_TEXTURE_COMPRESSOR_API		XRAY_DLL_EXPORT
#		else // #ifdef XRAY_TEXTURE_COMPRESSOR_BUILDING
#			define XRAY_TEXTURE_COMPRESSOR_API		XRAY_DLL_IMPORT
#		endif // #ifdef XRAY_TEXTURE_COMPRESSOR_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_TEXTURE_COMPRESSOR_API

namespace xray {
namespace texture_compressor {

typedef void* (*malloc_function_ptr_type)( u32 size, bool temp_data );
typedef void (*free_function_ptr_type)( pvoid buffer );

typedef void (*outputlog_function_ptr_type)( pcstr string );

XRAY_TEXTURE_COMPRESSOR_API	void set_log_function( outputlog_function_ptr_type );

enum compression_type{
	compression_fast,
	compression_highquality,
}; // enum compression_type

#pragma pack(push,1)
struct TgaHeader {
	u8	id_length;
	u8	colormap_type;
	u8	image_type;
	u16	colormap_index;
	u16	colormap_length;
	u8	colormap_size;
	u16	x_origin;
	u16	y_origin;
	u16	width;
	u16	height;
	u8	pixel_size;
	u8	flags;

	enum { Size = 18 };		//const static int SIZE = 18;
};
#pragma pack(pop)

enum texture_dimension
{
	texture_1D		= 0,
	texture_2D,
	texture_3D,
	texture_cube,
	texture_2D_array,
};

XRAY_TEXTURE_COMPRESSOR_API	u8*				create_texture			(
												u32& result_buffer_size,
												bool only_calculate_destination_size,
												u8* opt_destination_buffer, // can be NULL
												u32 opt_destination_buffer_size,
												malloc_function_ptr_type allocator,
												free_function_ptr_type deallocator,
												u8 const* const tga_file_data,
												pcstr format,
												compression_type const quality,
												u32 const tga_file_data_size,
												bool generate_mip_maps,
												texture_dimension texture_dim,
												bool is_srgb
											);

//XRAY_TEXTURE_COMPRESSOR_API	ATI_TC_FORMAT	parse_format			( pcstr format_string );

XRAY_TEXTURE_COMPRESSOR_API u32*			load_tga				( 
												malloc_function_ptr_type allocator, 
												free_function_ptr_type deallocator, 
												u8 const* buffer, 
												ATI_TC_Texture& texture
											);


} // namespace texture_compressor
} // namespace xray

#endif // #ifndef AMD_COMPRESS_TEXTURE_COMPRESSOR_API_H_INCLUDED