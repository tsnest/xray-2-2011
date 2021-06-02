////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "utils.h"

void fill_dds_header( dds_header& header, ATI_TC_DWORD width, ATI_TC_DWORD height, ATI_TC_DWORD size, ATI_TC_DWORD pitch, ATI_TC_FORMAT format, bool is_cubemap, bool is_volume_texture, u32 mip_map_count )
{
	header.signature	= MAKEFOURCC('D', 'D', 'S', ' ');
	
	DDSD2& ddsd = header.header;
	
	memset(&ddsd, 0, sizeof(DDSD2));
	ddsd.dwSize = sizeof(DDSD2);
	ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT|DDSD_MIPMAPCOUNT|DDSD_LINEARSIZE|DDSD_PITCH;
	
	if (is_volume_texture)
		ddsd.dwFlags |=DDSD_DEPTH;
	
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	ddsd.dwMipMapCount = mip_map_count;
	
	// TODO: depth, not height
	if (is_volume_texture)
		ddsd.dwDepth = height;
	
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
	
	if (is_cubemap)
		ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALLFACES;
	
	if (is_volume_texture)
		ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_VOLUME;
	
	ddsd.ddpfPixelFormat.dwFourCC = GetFourCC(format);
	if(ddsd.ddpfPixelFormat.dwFourCC)
	{
	  ddsd.dwLinearSize = size;
	  ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
	  //if(IsDXT5SwizzledFormat(texture.format))
	  if(IsDXT5SwizzledFormat(format))
	  {
		 ddsd.ddpfPixelFormat.dwPrivateFormatBitCount = ddsd.ddpfPixelFormat.dwFourCC;
		 ddsd.ddpfPixelFormat.dwFourCC = FOURCC_DXT5;
	  }
	}
	else
	{
	  //switch(texture.format)
	  switch(format)
	  {
	  case ATI_TC_FORMAT_ARGB_8888:
		 ddsd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
		 ddsd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
		 ddsd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
		 ddsd.lPitch = pitch;
		 ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
		 ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_RGB;
		 ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
		 break;

	  case ATI_TC_FORMAT_RGB_888:
		 ddsd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
		 ddsd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
		 ddsd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
		 ddsd.lPitch = pitch;
		 ddsd.ddpfPixelFormat.dwRGBBitCount = 24;
		 ddsd.ddpfPixelFormat.dwFlags=DDPF_RGB;
		 break;

	  case ATI_TC_FORMAT_RG_8:
		 ddsd.ddpfPixelFormat.dwRBitMask = 0x0000ff00;
		 ddsd.ddpfPixelFormat.dwGBitMask = 0x000000ff;
		 ddsd.lPitch = pitch;
		 ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		 ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_LUMINANCE;
		 ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
		 break;

	  case ATI_TC_FORMAT_R_8:
		 ddsd.ddpfPixelFormat.dwRBitMask = 0x000000ff;
		 ddsd.lPitch = pitch;
		 ddsd.ddpfPixelFormat.dwRGBBitCount = 8;
		 ddsd.ddpfPixelFormat.dwFlags= DDPF_LUMINANCE;
		 break;

	  case ATI_TC_FORMAT_ARGB_2101010:
		 ddsd.ddpfPixelFormat.dwRBitMask = 0x000003ff;
		 ddsd.ddpfPixelFormat.dwGBitMask = 0x000ffc00;
		 ddsd.ddpfPixelFormat.dwBBitMask = 0x3ff00000;
		 ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xc0000000;
		 ddsd.lPitch = pitch;
		 ddsd.ddpfPixelFormat.dwRGBBitCount = 32;
		 ddsd.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_RGB;
		 break;

	  case ATI_TC_FORMAT_ARGB_16:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A16B16G16R16;
		 break;

	  case ATI_TC_FORMAT_RG_16:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G16R16;
		 break;

	  case ATI_TC_FORMAT_R_16:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_L16;
		 break;

	  case ATI_TC_FORMAT_ARGB_16F:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A16B16G16R16F;
		 break;

	  case ATI_TC_FORMAT_RG_16F:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G16R16F;
		 break;

	  case ATI_TC_FORMAT_R_16F:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_R16F;
		 break;

	  case ATI_TC_FORMAT_ARGB_32F:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_A32B32G32R32F;
		 break;

	  case ATI_TC_FORMAT_RG_32F:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_G32R32F;
		 break;

	  case ATI_TC_FORMAT_R_32F:
		 ddsd.dwLinearSize = size;
		 ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
		 ddsd.ddpfPixelFormat.dwFourCC = D3DFMT_R32F;
		 break;

	  default:
		 //assert(0);
		 break;
	  }
	}
}

u32* cook_dds_body_from_fb(	 u32&						out_body_size,
							 malloc_function_ptr_type	allocator, 
							 free_function_ptr_type		deallocator, 
							 ATI_TC_CompressOptions		options, 
							 ATI_TC_FORMAT				dest_format, 
							 FIBITMAP*					fb, 
							 s32						cubemap_face )
{
	const FREE_IMAGE_TYPE image_type	= FreeImage_GetImageType(fb);
	const u32 src_pitch					= FreeImage_GetPitch(fb);
	const BYTE *src_bits				= FreeImage_GetBits(fb);
	const u32 width						= FreeImage_GetWidth(fb);
	const u32 height					= FreeImage_GetHeight(fb);
	const u32 bpp						= FreeImage_GetBPP( fb );

	ATI_TC_Texture source_texture;
	ATI_TC_Texture dest_texture;
	
	if (image_type==FIT_RGB16 || image_type==FIT_RGBA16)
	{
		
		u32 tex_width  = width,
			tex_height = height;
		
		if (cubemap_face>=0)
			tex_width = tex_height;
		
		source_texture.dwSize		= sizeof(source_texture);
		source_texture.dwWidth		= tex_width;
		source_texture.dwHeight		= tex_height;
		source_texture.dwPitch		= 0;
		source_texture.format		= ATI_TC_FORMAT_ARGB_16;
		
		u32 const data_size			= source_texture.dwWidth * source_texture.dwHeight * sizeof(u64);
		
		source_texture.dwDataSize	= data_size;
		source_texture.pData		= (ATI_TC_BYTE*)(*allocator)(data_size, false);
				
		for (u32 rows=0; rows<height; ++rows ) 
		{
			const FIRGB16 *src_pixel_rgb   = (FIRGB16*)src_bits;
			const FIRGBA16 *src_pixel_rgba = (FIRGBA16*)src_bits;
			
			for(u32 cols=0; cols<width; ++cols)
			{
				WORD r, g, b, a;

				if (image_type==FIT_RGB16)
				{
					r = src_pixel_rgb[cols].red;
					g = src_pixel_rgb[cols].green;
					b = src_pixel_rgb[cols].blue;
					a = 0xFFFF;
				}
				else
				{
					r = src_pixel_rgba[cols].red;
					g = src_pixel_rgba[cols].green;
					b = src_pixel_rgba[cols].blue;
					a = src_pixel_rgba[cols].alpha;
				}
				setBGRA_16( *((u64*)source_texture.pData + rows*source_texture.dwWidth + cols), r, g, b, a);
			}
			src_bits += src_pitch;
		}	
		
		dest_texture.dwSize			= sizeof(dest_texture);
		dest_texture.dwWidth		= source_texture.dwWidth;
		dest_texture.dwHeight		= source_texture.dwHeight;
		dest_texture.dwPitch		= 0;
		dest_texture.format			= dest_format;
		dest_texture.dwDataSize		= ATI_TC_CalculateBufferSize(&dest_texture);
		dest_texture.pData			= (ATI_TC_BYTE*)((u8*)(*allocator)(dest_texture.dwDataSize, true));
		ATI_TC_ConvertTexture		( &source_texture, &dest_texture, &options, NULL, NULL, NULL );
		
		(*deallocator)				( source_texture.pData );
		out_body_size				= dest_texture.dwDataSize;
		
		return (u32*)dest_texture.pData;
	}

	
	u32 tex_width  = width;
	u32 tex_height = height;
	
	if (cubemap_face>=0)
		tex_width = tex_height;
	
	source_texture.dwSize		= sizeof(source_texture);
	source_texture.dwWidth		= tex_width;
	source_texture.dwHeight		= tex_height;


	if(bpp==32)	
		source_texture.format		= ATI_TC_FORMAT_ARGB_8888;
	else
	if(bpp==24)
		source_texture.format		= ATI_TC_FORMAT_RGB_888;
	else
	{
		//R_ASSERT("unsupported image format ");
		return NULL; // unsupprted
	}

	u32 const bytes_per_pixel	= (bpp/8);
	u32 const additional_size	= 16;

	bool data_allocated = false;
	FIBITMAP* cube_face = NULL;
	
	// just alpha needed()
	if(dest_format==ATI_TC_FORMAT_R_8)
	{
		u32 const data_size			= source_texture.dwWidth * source_texture.dwHeight * 1;
		dest_texture.dwDataSize		= data_size;
		dest_texture.pData			= (ATI_TC_BYTE*)(*allocator)(data_size, false);
		
		FIBITMAP* ach				= FreeImage_GetChannel(fb, FICC_ALPHA);
		const BYTE* a_bits			= FreeImage_GetBits(ach);
		memcpy						(dest_texture.pData, a_bits, data_size);
		FreeImage_Unload			(ach);
		out_body_size				= data_size;
		return						(u32*)dest_texture.pData;
	}else
	{
		u32 const data_size			= source_texture.dwWidth * source_texture.dwHeight * bytes_per_pixel + additional_size;
		source_texture.dwDataSize	= data_size;
		if(cubemap_face==-1)
		{
			source_texture.pData	= (ATI_TC_BYTE*)src_bits;
			source_texture.dwPitch	= src_pitch;
		}else
		{
			int offset_x				= cubemap_face*tex_width;
			cube_face					= FreeImage_Copy(	fb, 
															offset_x, 
															0, 
															offset_x+tex_width, 
															tex_height);

			source_texture.pData		= (ATI_TC_BYTE*)FreeImage_GetBits(cube_face);
			source_texture.dwPitch		= FreeImage_GetPitch(cube_face);
		}
	}
	
	dest_texture.dwSize			= sizeof(dest_texture);
	dest_texture.dwWidth		= source_texture.dwWidth;
	dest_texture.dwHeight		= source_texture.dwHeight;
	dest_texture.dwPitch		= 0;
	dest_texture.format			= dest_format;
	dest_texture.dwDataSize		= ATI_TC_CalculateBufferSize(&dest_texture);
	dest_texture.pData			= (ATI_TC_BYTE*)((u8*)(*allocator)(dest_texture.dwDataSize, true));

	if(	dest_texture.dwWidth <=16 && dest_texture.dwHeight <=16)
		options.bDisableMultiThreading = TRUE;

	options.bDXT1UseAlpha		= (bpp==32);
	options.nAlphaThreshold		= 128;

	//bool solid_block = (source_texture.dwPitch == source_texture.dwWidth*bytes_per_pixel);
	//if(solid_block && source_texture.format==ATI_TC_FORMAT_ARGB_8888 &&
	//	dest_texture.dwWidth >4 &&
	//	dest_texture.dwHeight >4)
	//{
	//	if(dest_texture.format==ATI_TC_FORMAT_DXT1)
	//	{
	//	#ifndef _WIN64
	//		extreme_dxt::CompressImageDXT1( source_texture.pData, 
	//										dest_texture.pData, 
	//										dest_texture.dwWidth, 
	//										dest_texture.dwHeight );
	//	#else
	//		int outputBytes;
	//		realtime_dxt::CompressImageDXT1( source_texture.pData, 
	//										dest_texture.pData, 
	//										dest_texture.dwWidth, 
	//										dest_texture.dwHeight, 
	//										outputBytes );
	//	#endif
	//		++count_dxt1;
	//	}else
	//	if(dest_texture.format==ATI_TC_FORMAT_DXT5)
	//	{
	//	#ifndef _WIN64
	//		extreme_dxt::CompressImageDXT5( source_texture.pData, 
	//										dest_texture.pData, 
	//										dest_texture.dwWidth, 
	//										dest_texture.dwHeight );
	//	#else
	//		int outputBytes;
	//		realtime_dxt::CompressImageDXT5( source_texture.pData, 
	//										dest_texture.pData, 
	//										dest_texture.dwWidth, 
	//										dest_texture.dwHeight, 
	//										outputBytes );
	//	#endif
	//		++count_dxt5;
	//	}
	//}else
	{
		ATI_TC_ConvertTexture		( &source_texture, &dest_texture, &options, NULL, NULL, NULL );
	}
	
	if(data_allocated)
		(*deallocator)			( source_texture.pData );

	out_body_size				= dest_texture.dwDataSize;

	if(cube_face)
		FreeImage_Unload		( cube_face );

	return (u32*)dest_texture.pData;
}

static u32 get_memory_size_for_mipmaps(	ATI_TC_FORMAT const format, 
										u32 size_x, 
										u32 size_y, 
										u32 mip_map_count)
{
	// Calc memory size for mip-maps.
	ATI_TC_Texture					temp_texture;
	temp_texture.dwSize				= sizeof(temp_texture);
	temp_texture.dwWidth			= size_x;
	temp_texture.dwHeight			= size_y;
	temp_texture.dwPitch			= 0;
	temp_texture.format				= format;
	
	u32 result = 0;
	
	for (u32 i=0; i<mip_map_count; i++)
	{
		if (temp_texture.dwWidth<4) temp_texture.dwWidth	= 4;
		if (temp_texture.dwHeight<4)temp_texture.dwHeight	= 4;
		
		result						+= ATI_TC_CalculateBufferSize(&temp_texture);
		
		temp_texture.dwWidth		/= 2;
		temp_texture.dwHeight		/= 2;
	}

	return result;
}

nvtt::Format get_nv_texture_format( pcstr name )
{
	if ( 0==strcmp(name,"A8") ) 
		return								nvtt::Format_DXT5;//kA8
	else if ( 0==strcmp(name,"DXT1") ) 
		return								nvtt::Format_DXT1;
	else if ( 0==strcmp(name,"DXT1a") ) 
		return								nvtt::Format_DXT1a;
	else if ( 0==strcmp(name,"DXT3") ) 
		return								nvtt::Format_DXT3;
	else if ( 0==strcmp(name,"DXT5") )
		return								nvtt::Format_DXT5;
	else if ( 0==strcmp(name,"NORMAL_MAP") ) 
		return								nvtt::Format_DXT5n;
	else if ( 0==strcmp(name,"UNCOMPRESSED") ) 
		return								nvtt::Format_RGBA;
	
	return									nvtt::Format_DXT1;
}


ATI_TC_FORMAT get_ati_texture_format( pcstr name )
{
	if ( 0==strcmp(name,"A8") ) 
		return								ATI_TC_FORMAT_R_8;
	else if ( 0==strcmp(name,"DXT1") ) 
		return								ATI_TC_FORMAT_DXT1;
	else if ( 0==strcmp(name,"DXT1a") ) 
		return								ATI_TC_FORMAT_DXT3;
	else if ( 0==strcmp(name,"DXT3") ) 
		return								ATI_TC_FORMAT_DXT3;
	else if ( 0==strcmp(name,"DXT5") )
		return								ATI_TC_FORMAT_DXT5;
	else if ( 0==strcmp(name,"NORMAL_MAP") ) 
		return								ATI_TC_FORMAT_ATI2N_DXT5;
	else if ( 0==strcmp(name,"UNCOMPRESSED") ) 
		return								ATI_TC_FORMAT_ARGB_8888;
	
	return									ATI_TC_FORMAT_DXT1;
}

u32 calc_mip_map_count(u32 width, u32 height)
{
	u32 min_sizeXY			= Min(width,height);
	return					static_cast<u32>(logf(static_cast<float>(min_sizeXY)) / logf(2.0)) + 1;
}

u32 calc_memory_usage( ATI_TC_FORMAT dest_format, bool cubemap, bool is_volume_texture, u32 width, u32 height, u32 mip_map_count )
{
	u32 result			= sizeof(dds_header);

	u32 mips_width		= width;
	u32 mips_height		= height;
	
	if(cubemap)
		mips_width		= height;
	
	u32 num_faces		= is_volume_texture ? height : (cubemap ? 6 : 1);
	
	result				+= num_faces * get_memory_size_for_mipmaps(	dest_format, 
																	mips_width, 
																	mips_height, 
																	mip_map_count);
	
	result				+= sizeof(bool);//sizeof(u16) * 4 * 2;

	return result;
}

u8* create_texture_ati_compress(	u32& result_buffer_size,
									bool only_calculate_destination_size,
									u8* opt_destination_buffer, // can be NULL
									u32 opt_destination_buffer_size,
									malloc_function_ptr_type allocator,
									free_function_ptr_type deallocator,
									u8 const* const tga_file_data,
									pcstr fmt,
									u32 const tga_file_data_size,
									bool generate_mip_maps,
									xray::texture_compressor::texture_dimension texture_dim,
									bool highquality,
									bool is_srgb)
{
	ATI_TC_FORMAT dest_format		= get_ati_texture_format(fmt);
	ATI_TC_CompressOptions			options;
	memset							( &options, 0, sizeof(options) );
	options.dwSize					= sizeof(options);
	options.nCompressionSpeed		= highquality ? ATI_TC_Speed_Normal : ATI_TC_Speed_SuperFast;
	options.bDisableMultiThreading	= false;
	
	bool is_cubemap					= texture_dim == xray::texture_compressor::texture_cube;
	bool is_volume_texture			= texture_dim == xray::texture_compressor::texture_3D;
	
	FIBITMAP* original_fb			= get_freeimage_from_memory (tga_file_data,tga_file_data_size);
	if ( !original_fb )
		return 0;
	FreeImage_FlipVertical			( original_fb );

	const FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(original_fb);
	
	// Do normalize only for 16bit textures.
	//bool normalize_color			= (image_type==FIT_RGB16 || image_type==FIT_RGBA16);
	
	u32 width						= FreeImage_GetWidth(original_fb);
	u32 height						= FreeImage_GetHeight(original_fb);
	u32 min_sizeXY					= Min(width,height);
	
	if (!width || !height)
	{
		FreeImage_Unload			(original_fb);
		return 0;
	}
		
// 	WORD out_min_max[8];
// 	out_min_max[0] = out_min_max[1] = out_min_max[2] = out_min_max[3] = 0;
// 	out_min_max[4] = out_min_max[5] = out_min_max[6] = out_min_max[7] = 0xffff;
	
	u32 mip_map_count				= generate_mip_maps ? calc_mip_map_count(width, height)	: 1;

	//if (is_volume_texture && mip_map_count!=1)
	//{
	//	// Volume textures with mip maps not implemented yet!
	//	assert(0);
	//}
	
	FIBITMAP* fb_mips[16];
	fb_mips[0] = original_fb;
	
//	if (normalize_color)
//		FreeImage_Normalize			(fb_mips[0], out_min_max[0], out_min_max[1], out_min_max[2], out_min_max[3], out_min_max[4], out_min_max[5], out_min_max[6], out_min_max[7]);
	
	if ( !only_calculate_destination_size )
	for (u32 i=1; i<mip_map_count; ++i)
	{
		fb_mips[i]					= FreeImage_Rescale(fb_mips[i-1], FreeImage_GetWidth(fb_mips[i-1]) /2, FreeImage_GetHeight(fb_mips[i-1])/2, FILTER_KAISER);
//		if (normalize_color)
//		{
//			WORD out_min_max[8];
//			FreeImage_Normalize		(fb_mips[i], out_min_max[0], out_min_max[1], out_min_max[2], out_min_max[3], out_min_max[4], out_min_max[5], out_min_max[6], out_min_max[7]);
//		}
	}
	
	u32 num_faces = is_volume_texture ? min_sizeXY : (is_cubemap ? 6 : 1);
	
	u32 head_total_size				= sizeof(dds_header);
	
	u32 mips_width  = width,
		mips_height = height;
	
	if (is_cubemap  || is_volume_texture)
		mips_width = mips_height = height;
	
	result_buffer_size				= head_total_size + num_faces * get_memory_size_for_mipmaps(dest_format, mips_width, mips_height, mip_map_count);
	
	// sRGB flag
	result_buffer_size				+= sizeof(bool);
	
	u32 result_buffer_size2			= calc_memory_usage(dest_format, is_cubemap, is_volume_texture, mips_width, mips_height, mip_map_count);
	assert(result_buffer_size2==result_buffer_size);

	if ( only_calculate_destination_size )
	{
		FreeImage_Unload			(original_fb);
		return						0;
	}

	if ( opt_destination_buffer && opt_destination_buffer_size < result_buffer_size )
		return						0;
	
	u8* destination_buffer			= opt_destination_buffer ? 
										(u8*)opt_destination_buffer :
										(u8*)(*allocator)(result_buffer_size, true);
	
	dds_header& header				= *(dds_header*)destination_buffer;
	fill_dds_header					(	header, 
										(is_cubemap || is_volume_texture) ? min_sizeXY : width, 
										height, 
										result_buffer_size - head_total_size, 
										0, 
										dest_format, 
										is_cubemap,
										is_volume_texture,
										mip_map_count );
	
	u8* dds_body					= destination_buffer + head_total_size;
	
	u8* mip_start					= dds_body;
	bool result						= true;

	if (is_volume_texture)
	{
		for (u32 mip_index=0; mip_index<mip_map_count; mip_index++)
		{
			for (u32 face_index=0; face_index<num_faces; face_index++)
			{
				u32 body_size			= 0;
				
				u32* const pixel_buffer	= cook_dds_body_from_fb (	body_size, 
																	allocator, 
																	deallocator, 
																	options, 
																	dest_format, 
																	fb_mips[mip_index], 
																	(num_faces==1) ? -1 : face_index );
				if(pixel_buffer!=NULL)
				{
					memcpy( mip_start, pixel_buffer, body_size );
					mip_start				+= body_size;
					(*deallocator)			( pixel_buffer );
				}else
				{
					result = false;
					break;
				}
			}
			if(!result)
				break;
		}
	}
	else
	{
		for (u32 face_index=0; face_index<num_faces; face_index++)
		{
			for (u32 mip_index=0; mip_index<mip_map_count; mip_index++)
			{
				u32 body_size			= 0;
				
				u32* const pixel_buffer	= cook_dds_body_from_fb (	body_size, 
																	allocator, 
																	deallocator, 
																	options, 
																	dest_format, 
																	fb_mips[mip_index], 
																	(num_faces==1) ? -1 : face_index );
				if(pixel_buffer!=NULL)
				{
					memcpy( mip_start, pixel_buffer, body_size );
					mip_start				+= body_size;
					(*deallocator)			( pixel_buffer );
				}else
				{
					result = false;
					break;
				}
			}
			if(!result)
				break;
		}
	}
	
	if(result)
	{
		// Append sRGB flag
		memcpy(mip_start, &is_srgb, sizeof(bool));
		// Append rescale image parameters.
		//memcpy(mip_start, out_min_max, sizeof(out_min_max));
		
		for (u32 i=1; i<mip_map_count; i++)
			FreeImage_Unload( fb_mips[i] );
		
		FreeImage_Unload		(original_fb);
		return					destination_buffer;
	}else
	{
		FreeImage_Unload		(original_fb);

		if ( !opt_destination_buffer ) // dont free if buffer was given
			(*deallocator)		( destination_buffer );

		return					NULL;
	}
}


struct nv_OutputHandler : public nvtt::OutputHandler
{
	nv_OutputHandler(u8* data, u32 data_size) 
	:m_data_ptr(data), m_data_size(data_size), m_current(0){}

	virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
	{
		(void*)size;
		(void*)width;
		(void*)height;
		(void*)depth;
		(void*)face;
		(void*)miplevel;
	}
	
	virtual bool writeData(const void * data, int size)
	{
		memcpy				(m_data_ptr+m_current, data, size );
		m_current			+= size;
		return				true;
	}

	u8*		m_data_ptr;
	u32		m_data_size;
	u32		m_current;
};

struct nv_ErrorHandler : public nvtt::ErrorHandler
{
	virtual void error(nvtt::Error e)
	{
		output_log( nvtt::errorString(e) );
	}
};

u8* create_texture_nv_compress(		u32& result_buffer_size,
									bool only_calculate_destination_size,
									u8* opt_destination_buffer, // can be NULL
									u32 opt_destination_buffer_size,
									malloc_function_ptr_type allocator,
									free_function_ptr_type deallocator,
									u8 const* const tga_file_data,
									pcstr fmt,
									u32 const tga_file_data_size,
									bool generate_mip_maps,
									xray::texture_compressor::texture_dimension texture_dim,
									bool is_srgb)
{
	nvtt::Format dest_format	= get_nv_texture_format(fmt);
	FIBITMAP* original_fb		= get_freeimage_from_memory (tga_file_data,tga_file_data_size);
	u32 width					= FreeImage_GetWidth( original_fb );
	u32 height					= FreeImage_GetHeight( original_fb );
	const u32 bpp				= FreeImage_GetBPP( original_fb );
	u32 line_pitch				= FreeImage_GetPitch( original_fb );

	bool has_alpha				= (bpp==32);
	
	if(bpp!=32)
	{
		FIBITMAP* fb			= FreeImage_ConvertTo32Bits( original_fb );
		FreeImage_Unload		( original_fb );
		original_fb				= fb;
		line_pitch				= FreeImage_GetPitch( original_fb );
	}

	FreeImage_FlipVertical		( original_fb );
	BYTE* pdata					= FreeImage_GetBits( original_fb );

	bool bcubemap				= (texture_dim==xray::texture_compressor::texture_cube);
	bool bvolume_texture		= (texture_dim==xray::texture_compressor::texture_3D);
	
	if (bvolume_texture)
	{
		// nv_compressor volume texture creating not implemented yet.
		assert(0);
	}
	
	nvtt::CompressionOptions	compression_options;
	nvtt::InputOptions			input_options;

	if(dest_format==nvtt::Format_DXT5n)
	{
		input_options.setNormalMap			( true );
		input_options.setConvertToNormalMap	( false );
		input_options.setGamma				( 1.0f, 1.0f );
		input_options.setNormalizeMipmaps	( true );
	}else// color
	{
		input_options.setNormalMap			( false );
		input_options.setConvertToNormalMap	( false );
		input_options.setGamma				( 2.2f, 2.2f );
		input_options.setNormalizeMipmaps	( false );
	}

	input_options.setFormat					( nvtt::InputFormat_BGRA_8UB );

	if(has_alpha)
		input_options.setAlphaMode			( nvtt::AlphaMode_Transparency );
	else
		input_options.setAlphaMode			( nvtt::AlphaMode_None );

	if(bcubemap)
	{
		input_options.setTextureLayout		( nvtt::TextureType_Cube, height, height, 1 );
		for(u32 face_idx=0; face_idx<6; ++face_idx)
		{
			u32 face_width			= height;
			int offset_x			= face_idx*face_width;

			FIBITMAP* cube_face	= FreeImage_Copy(original_fb, 
												offset_x, 
												0, 
												offset_x+face_width, 
												height);

			BYTE* face_bytes					= FreeImage_GetBits(cube_face);
			input_options.setMipmapData			( face_bytes, height, height, 1, face_idx );

			FreeImage_Unload		( cube_face );
		}
	}else
	if(bvolume_texture) // TODO!
	{
		input_options.setTextureLayout		( nvtt::TextureType_2D, height, height, 1 );
		for(u32 face_idx=0; face_idx<height; ++face_idx)
		{
			u32 face_width			= height;
			int offset_x			= face_idx*face_width;

			FIBITMAP* cube_face	= FreeImage_Copy(original_fb, 
												offset_x, 
												0, 
												offset_x+face_width, 
												height);

			BYTE* face_bytes					= FreeImage_GetBits(cube_face);
			input_options.setMipmapData			( face_bytes, height, height, height, face_idx );

			FreeImage_Unload		( cube_face );
		}
	}else
	{
		input_options.setTextureLayout		( nvtt::TextureType_2D, width, height, 1 );
		input_options.setMipmapData			( pdata, width, height );
	}

	input_options.setMipmapFilter			( nvtt::MipmapFilter_Kaiser );
	input_options.setMipmapGeneration		( generate_mip_maps );

	compression_options.setQuality			( nvtt::Quality_Highest );//Quality_Production );
	compression_options.setFormat			( dest_format );

	nvtt::Compressor						compressor;
	compressor.enableCudaAcceleration		( false );

	u32 output_size						=	512 + compressor.estimateSize(input_options, compression_options) + sizeof(bool);

	if ( only_calculate_destination_size )
	{
		result_buffer_size				=	output_size;
		return								0;
	}

	if ( opt_destination_buffer && opt_destination_buffer_size < result_buffer_size )
		return						0;

	u8* output_data						= opt_destination_buffer ? 
											(u8*)opt_destination_buffer :
											(u8*)(*allocator)(output_size, true);

	nv_OutputHandler					output_handler( output_data, output_size/* - sizeof(bool)*/ );
	nv_ErrorHandler						error_handler;
	nvtt::OutputOptions					output_options;
	output_options.setErrorHandler		( &error_handler );
	output_options.setOutputHandler		( &output_handler );


	if(!compressor.process(input_options, compression_options, output_options) )
	{
		if ( !opt_destination_buffer ) // dont deallocate if the buffer was user-supplied
			(*deallocator)(output_data);
		return NULL;
	}
	
	memcpy						(output_data + output_handler.m_current, &is_srgb, sizeof(bool));
	
	result_buffer_size			= output_handler.m_current;
	FreeImage_Unload			( original_fb );
	return						output_data;

//	nvOptions.bRGBE				= false;
//
//	dds_buffer_to_store			buffer;
//	buffer.current				= 0;
//	u32 mip_count				= generate_mip_maps ? calc_mip_map_count( width, height ) : 1;
//	buffer.size					= calc_memory_usage( nv_to_ati(dest_format), 
//														bcubemap, 
//														width, 
//														height, 
//														mip_count );
//
//	buffer.data					= (u8*)(*allocator)(buffer.size, true);
//	nvOptions.user_data			= &buffer;
//
//	if(bcubemap)
//	{
//		RGBAMipMappedCubeMap	pcubemap;
//		pcubemap.resize			(height, height, 1);
//
//		for(int i=0; i<6; ++i)
//		{
//			RGBAMipMappedImage& face= pcubemap[i];
//			RGBAImage&	face_mip	= face[0];
//			face_mip.SetRGBE		(false);
//			rgba_t* pixels			= face_mip.pixels();
//			u32 face_width			= height;
//			int offset_x			= i*face_width;
//
//			FIBITMAP* cube_face	= FreeImage_Copy(original_fb, 
//												offset_x, 
//												0, 
//												offset_x+face_width, 
//												height);
//
//			BYTE* face_bytes		= FreeImage_GetBits(cube_face);
//			if(bpp==32)
//				memcpy				( pixels, face_bytes, face_width*height*(bpp/8) );
//			else
//			{
//				u8* pixel				= face_bytes;
//				for (u32 k=0; k<face_width*height; k++,pixel+=3)
//					pixels[k].set	( pixel[2], pixel[1], pixel[0], 0 );
//			}
//			FreeImage_Unload		( cube_face );
//		}
//		
//		HRESULT hr	= nvDDS::nvDXTcompress(	pcubemap, 
//											&nvOptions,
//											nv_write_callback );
//	}else
//	{
//		nvDDS::nvDXTcompress		(	pdata, 
//										width, 
//										height, 
//										line_pitch,
//										(bpp==32) ? nvBGRA : nvBGR,
//										&nvOptions,
//										nv_write_callback );
//	}
	
}