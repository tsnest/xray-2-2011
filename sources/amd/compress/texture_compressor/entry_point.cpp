////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "utils.h"

xray::texture_compressor::outputlog_function_ptr_type	g_log_fn = NULL;

void xray::texture_compressor::set_log_function( xray::texture_compressor::outputlog_function_ptr_type fn )
{
	g_log_fn = fn;
}

void output_log( pcstr string )
{
	g_log_fn(string);
}

u8* create_texture_ati_compress(	u32& result_buffer_size,
									bool only_calculate_destination_size,
									u8* opt_destination_buffer, // can be NULL
									u32 opt_destination_buffer_size,
									malloc_function_ptr_type allocator,
									free_function_ptr_type deallocator,
									u8 const* const tga_file_data,
									pcstr dest_format,
									u32 const tga_file_data_size,
									bool generate_mip_maps,
									xray::texture_compressor::texture_dimension texture_dim,
									bool highquality,
									bool is_srgb);

u8* create_texture_nv_compress(		u32& result_buffer_size,
									bool only_calculate_destination_size,
									u8* opt_destination_buffer, // can be NULL
									u32 opt_destination_buffer_size,
									malloc_function_ptr_type allocator,
									free_function_ptr_type deallocator,
									u8 const* const tga_file_data,
									pcstr dest_format,
									u32 const tga_file_data_size,
									bool generate_mip_maps,
									xray::texture_compressor::texture_dimension texture_dim,
									bool is_srgb);

u8* xray::texture_compressor::create_texture(
	u32& result_buffer_size,
	bool only_calculate_destination_size,
	u8* opt_destination_buffer, // can be NULL
	u32 opt_destination_buffer_size,
	malloc_function_ptr_type allocator,
	free_function_ptr_type deallocator,
	u8 const* const tga_file_data,
	pcstr dest_format,
	xray::texture_compressor::compression_type const quality,
	u32 const tga_file_data_size,
	bool generate_mip_maps,
	texture_dimension texture_dim,
	bool is_srgb)
{
	u8* result = NULL;
	
	bool highquality = (quality==compression_highquality);
	
	bool use_nv_compress = highquality && (0 != strcmp(dest_format,"A8"));
#ifndef WIN64
	use_nv_compress = false;
#endif
	
	if (	quality == compression_fast
		 || (highquality && only_calculate_destination_size)
		 || !use_nv_compress
	)
	{
		result =  create_texture_ati_compress(	result_buffer_size, 
											only_calculate_destination_size,
											opt_destination_buffer,
											opt_destination_buffer_size,
											allocator, 
											deallocator, 
											tga_file_data, 
											dest_format, 
											tga_file_data_size,
											generate_mip_maps,
											texture_dim,
											highquality,
											is_srgb);
	}
	else
	{
		result =  create_texture_nv_compress(	result_buffer_size, 
											only_calculate_destination_size,
											opt_destination_buffer,
											opt_destination_buffer_size,
											allocator, 
											deallocator, 
											tga_file_data, 
											dest_format, 
											tga_file_data_size,
											generate_mip_maps,
											texture_dim,
											is_srgb);
	}
	
	if (only_calculate_destination_size && use_nv_compress)
		result_buffer_size += 512; // nv compressor bug?
	
	return result;
}


/*
u8* xray::texture_compressor::create_texture	(
		u32& result_buffer_size,
		malloc_function_ptr_type allocator,
		free_function_ptr_type deallocator,
		u8 const* const tga_file_data,
		ATI_TC_FORMAT const format,
		xray::texture_compressor::compression_speed const compression_speed,
		bool enable_multithreading,
		u32 const tga_file_data_size,
		bool generate_mip_maps
)
{
	FreeImage_SetPluginEnabled		(FIF_TARGA, true);
//	TgaHeader* const tga			= (TgaHeader*)tga_file_data;
	FIMEMORYHEADER					fh;
	fh.curpos						= 0;
	fh.data							= (void*)(tga_file_data);
	fh.datalen						= tga_file_data_size;
	fh.delete_me					= false;
	fh.filelen						= tga_file_data_size;
	FIMEMORY fm;
	fm.data							= (void*)&fh;
	FIBITMAP* original_fb			= FreeImage_LoadFromMemory(FIF_TARGA,&fm);
	
	
	bool is_cube_map				= true;
	
	
	u32 mip_map_count				= generate_mip_maps ? (logf(Min(static_cast<float>(source_texture.dwHeight),static_cast<float>(source_texture.dwWidth))) / logf(2.0)) + 1	: 1;
	
	
	ATI_TC_Texture					source_texture;
	u32* const pixel_buffer			= cook_ati_texture_from_fb (allocator,source_texture,original_fb);
	
	
	ATI_TC_CompressOptions			options;
	memset							( &options, 0, sizeof(options) );
	options.dwSize					= sizeof(options);
	options.nCompressionSpeed		= (ATI_TC_Speed)compression_speed;
	options.bDisableMultiThreading	= enable_multithreading ? 0 : 1;
	
	ATI_TC_Texture					dest_texture;
	dest_texture.dwSize				= sizeof(dest_texture);
	dest_texture.dwWidth			= source_texture.dwWidth;
	dest_texture.dwHeight			= source_texture.dwHeight;
	dest_texture.dwPitch			= 0;
	dest_texture.format				= format;
	dest_texture.dwDataSize			= ATI_TC_CalculateBufferSize(&dest_texture);

	u32 offset_to_next_mip = dest_texture.dwDataSize;
	
	dest_texture.dwDataSize += get_memory_size_for_mipmaps(format,source_texture.dwWidth,source_texture.dwHeight,mip_map_count);
	
	result_buffer_size				= sizeof(dds_header) + dest_texture.dwDataSize;

	u8* const destination_buffer	= (u8*)(*allocator)(result_buffer_size, true);
	dds_header& header				= *(dds_header*)destination_buffer;

	fill_dds_header					( header, dest_texture, mip_map_count);

	dest_texture.pData				= (ATI_TC_BYTE*)(destination_buffer + sizeof(dds_header));

	ATI_TC_ConvertTexture			( &source_texture, &dest_texture, &options, NULL, NULL, NULL );

	ATI_TC_Texture mipmap_texture = source_texture;

	FIBITMAP next_fb;
	next_fb.data = original_fb->data;

	for (u32 i=1; i<mip_map_count; i++)
	{
		u32* temp_pixel_buffer = NULL;
		if (mipmap_texture.dwWidth>4 && mipmap_texture.dwHeight>4)
			temp_pixel_buffer = cook_half_size_ati_texture(allocator,mipmap_texture, &next_fb, next_fb);
		
		ATI_TC_Texture					dest_texture2;
		dest_texture2.dwSize			= sizeof(dest_texture2);
		dest_texture2.dwWidth			= mipmap_texture.dwWidth;
		dest_texture2.dwHeight			= mipmap_texture.dwHeight;

		dest_texture2.dwPitch			= 0;
		dest_texture2.format			= format;
		dest_texture2.dwDataSize		= ATI_TC_CalculateBufferSize(&dest_texture2);

		u8* const destination_buffer2	= (u8*)(*allocator)(dest_texture2.dwDataSize, true);
		dest_texture2.pData				= (ATI_TC_BYTE*)destination_buffer2;

		ATI_TC_ConvertTexture			( &mipmap_texture, &dest_texture2, &options, NULL, NULL, NULL );

		memcpy(dest_texture.pData+offset_to_next_mip,dest_texture2.pData,dest_texture2.dwDataSize);

		offset_to_next_mip				+=dest_texture2.dwDataSize;

		if (temp_pixel_buffer)
			(*deallocator)					( temp_pixel_buffer );
	}

	(*deallocator)					( pixel_buffer );

	return							destination_buffer;
}
*/

		/*
		u8 *data = NULL;

		nvCompressionOptions nvOptions;
		nvOptions.mipMapGeneration	= kGenerateMipMaps;
		nvOptions.numMipMapsToWrite = 0;
		nvOptions.mipFilterType		= kMipFilterBox;
		nvOptions.textureType		= kTextureTypeTexture2D;
		nvOptions.textureFormat		= kDXT1;
		nvOptions.quality			= kQualityFastest;
		nvOptions.bRGBE				= false;
		nvOptions.user_data			= &data;

		nvPixelOrder PO = nvPixelOrder::nvBGRA;

		nvDDS::nvDXTcompress(
		source_texture.pData,
		source_texture.dwWidth,
		source_texture.dwHeight,
		source_texture.dwPitch,
		PO,
		&nvOptions,
		nv_allback
		);

		return							data;
		*/


		/*
		ATI_TC_Texture					source_texture;
		u32* const pixel_buffer			= load_tga( allocator, deallocator, tga_file_data, source_texture );
		if ( !pixel_buffer )
		return						0;

		ATI_TC_CompressOptions			options;
		memset							( &options, 0, sizeof(options) );
		options.dwSize					= sizeof(options);
		options.nCompressionSpeed		= (ATI_TC_Speed)compression_speed;
		options.bDisableMultiThreading	= enable_multithreading ? 0 : 1;

		ATI_TC_Texture					dest_texture;
		dest_texture.dwSize				= sizeof(dest_texture);
		dest_texture.dwWidth			= source_texture.dwWidth;
		dest_texture.dwHeight			= source_texture.dwHeight;
		dest_texture.dwPitch			= 0;
		dest_texture.format				= format;
		dest_texture.dwDataSize			= ATI_TC_CalculateBufferSize(&dest_texture);

		result_buffer_size				= sizeof(dds_header) + dest_texture.dwDataSize;
		u8* const destination_buffer	= (u8*)(*allocator)(result_buffer_size, true);
		dds_header& header				= *(dds_header*)destination_buffer;
		fill_dds_header					( header, dest_texture );

		dest_texture.pData				= (ATI_TC_BYTE*)(destination_buffer + sizeof(dds_header));

		ATI_TC_ConvertTexture			( &source_texture, &dest_texture, &options, NULL, NULL, NULL );

		(*deallocator)					( pixel_buffer );

		return							destination_buffer;	
		*/

//static NV_ERROR_CODE nv_allback(const void * Data, size_t NumBytes, const MIPMapData * MipMapData, void* UserData )
//{
//	if (!MipMapData)
//	{
//		return NV_FAIL;
//	}
//	int width = MipMapData->width, 
//		height = MipMapData->height,
//		mip_index = MipMapData->mipLevel;
//	//...
//	return NV_OK;
//}


/*static u32* cook_ati_texture_from_fb(malloc_function_ptr_type allocator, ATI_TC_Texture& out_texture, FIBITMAP* fb, s32 cubemap_face = -1)
{
	u32 tex_width  = FreeImage_GetWidth(fb),
		tex_height = FreeImage_GetHeight(fb);
	
	if (cubemap_face>=0)
		tex_width = tex_height;
	
	out_texture.dwSize		= sizeof(out_texture);
	out_texture.dwWidth		= tex_width;
	out_texture.dwHeight	= tex_height;
	out_texture.dwPitch		= 0;
	out_texture.format		= ATI_TC_FORMAT_ARGB_8888;
	
	u32 const data_size		= out_texture.dwWidth*out_texture.dwHeight*sizeof(u32);
	out_texture.dwDataSize	= data_size;
	out_texture.pData		= (ATI_TC_BYTE*)(*allocator)(data_size, false);
	
	for (u32 j=0; j<out_texture.dwHeight; j++)
		for (u32 i=0; i<out_texture.dwWidth; i++)
		{
			RGBQUAD pix;
			FreeImage_GetPixelColor(fb,i, cubemap_face>=0 ? cubemap_face*tex_width:0 + out_texture.dwHeight-j-1,&pix);
			setBGRA(*((u32*)out_texture.pData + j*out_texture.dwWidth + i),pix.rgbBlue,pix.rgbGreen,pix.rgbRed,pix.rgbReserved);
		}

	return (u32*)out_texture.pData;
}*/