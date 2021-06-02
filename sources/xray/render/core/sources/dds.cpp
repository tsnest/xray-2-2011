////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "dds.h"
#include <xray/render/core/res_texture.h>
#include <xray/render/core/utils.h>
#include <xray/render/core/device.h>

#ifndef MAKEFOURCC 

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#endif


namespace xray {
namespace render {

const u32 s_magic_number_dds = 0x20534444;

bool	load_dds_info	( u8 const * buffer, u32 size, dds_info & dds_info)
{
	ZeroMemory( &dds_info, sizeof(dds_info));

	u8 const * dds_ptr = buffer;

	// Check wather the file is DDS
	ASSERT( s_magic_number_dds == *(u32*)dds_ptr, "Only dds format is supported." );
	if( s_magic_number_dds != *(u32*)dds_ptr)
		return false;

	dds_ptr += sizeof(s_magic_number_dds);

	DDS_HEADER*		dds_header	= (DDS_HEADER*)dds_ptr;
	dds_ptr						+=	sizeof(DDS_HEADER);

	DDS_HEADER_DXT10*	dds_header_dx10 = NULL; 

	if( size <= sizeof(DDS_HEADER)+sizeof(DDS_HEADER_DXT10)
		|| dds_header->dwSize != sizeof(DDS_HEADER)
		|| dds_header->ddpf.dwSize != sizeof(DDS_PIXELFORMAT) )
	{
		ASSERT( 0, "Corrupt dds file.");
		return false;
	}

	dds_info.format = DXGI_FORMAT_UNKNOWN;
	dds_info.array_size = 1;

	if( (dds_header->ddpf.dwFlags&DDPF_FOURCC))
	{
		switch( dds_header->ddpf.dwFourCC)
		{
		case MAKEFOURCC('D','X','1','0') :
			{
				dds_header_dx10	=	(DDS_HEADER_DXT10*) dds_ptr;
				dds_ptr			+=	sizeof(DDS_HEADER_DXT10);
				dds_info.format	=	dds_header_dx10->dxgiFormat;
				
				dds_info.array_size			= dds_header_dx10->arraySize;
				dds_info.resource_dimension	= dds_header_dx10->resourceDimension;

				break;
			}

		case MAKEFOURCC('D','X','T','1') :
			{
				dds_info.format		= DXGI_FORMAT_BC1_UNORM;
				break;
			}

		case MAKEFOURCC('D','X','T','3') :
			{
				dds_info.format		= DXGI_FORMAT_BC3_UNORM;
				break;
			}

		case MAKEFOURCC('D','X','T','5') :
			{
				dds_info.format		= DXGI_FORMAT_BC5_UNORM;
				break;
			}

		default:
			{
				ASSERT( 0, "The dds format is not supported.");
				return false;
			}
		}
	}
	else
	{
		ASSERT( 0, "The dds format is not supported.");
		return false;
	}

	if( dds_header->dwCaps2&DDSCAPS2_CUBEMAP || dds_header->dwCaps2&DDSCAPS2_VOLUME)
	{
		ASSERT( 0, "Cube or volume textures are not supported.");
		return false;
	}

	dds_info.width				= dds_header->dwWidth;
	dds_info.height				= dds_header->dwHeight;
	dds_info.mips_count			= dds_header->dwMipMapCount;
	dds_info.resource_dimension = D3D_RESOURCE_DIMENSION_TEXTURE2D;
	dds_info.data_ptr			= dds_ptr;
	dds_info.data_size			= size - sizeof(DWORD) + sizeof(DDS_HEADER) +  (dds_header_dx10 == NULL ? 0 : sizeof(DDS_HEADER_DXT10));

	return true;
}


#if 1

bool	load_dds_to_texture	( dds_info const & dds_info, res_texture* staging_texture, u32 , u32 )
{
	if( dds_info.width != staging_texture->width() || dds_info.height != staging_texture->height())
	{
		ASSERT( "The loading texture size is different than the destination size.");
		return false;
	}

	if( dds_info.mips_count != staging_texture->mips_count())
	{
		ASSERT( "The loading texture's mip levels are different than the destination's!");
		return false;
	}

	u32 surf_width =	dds_info.width;
	u32 surf_height =	dds_info.height;

	u8 const * src_data_ptr = dds_info.data_ptr;
	for( u32 i = 0; i< dds_info.mips_count; ++i)
	{
		u8* dest_buffer = NULL;

		u32 src_row_pitch;
		u32 src_size	= utils::calc_surface_size( surf_width, surf_height, staging_texture->format(), 0, src_row_pitch);

// 		u32 surf_row_pitch;
// 		u32 surf_size	= utils::calc_surface_size( surf_width, surf_height, staging_texture->format(), row_min_pitch, surf_row_pitch);
// 		surf_size		= math::max( surf_size, surf_min_pitch);
		

#if		USE_DX10
		D3D10_MAPPED_TEXTURE2D mapped_resource;
		HRESULT res = ((ID3DTexture2D*)staging_texture->hw_texture())->Map( D3D10CalcSubresource(  i, 0, dds_info.mips_count), D3D_MAP_WRITE, 0, &mapped_resource);
#else
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		HRESULT res = device::ref().d3d_context()->Map( staging_texture->hw_texture(), D3D10CalcSubresource(  i, 0, dds_info.mips_count), D3D_MAP_WRITE, 0, &mapped_resource);
		
// 		ASSERT( mapped_resource.DepthPitch == surf_size);
// 		ASSERT( mapped_resource.RowPitch == surf_row_pitch);
#endif
		dest_buffer = (u8*)mapped_resource.pData;
		CHECK_RESULT( res); 

		if( mapped_resource.RowPitch == src_row_pitch)
		{
			memory::copy( dest_buffer, src_size, src_data_ptr, src_size);
			src_data_ptr += src_size;
		}
		else
		{
			u32 rows = src_size/src_row_pitch;
			for( u32 r = 0; r < rows; ++r)
			{
				u32 size_to_copy = math::min( src_row_pitch, mapped_resource.RowPitch);
				memory::copy( (u8*)dest_buffer + r*mapped_resource.RowPitch, size_to_copy, src_data_ptr, size_to_copy);
				src_data_ptr += src_row_pitch;
			}
		}

		surf_width/=2;
		surf_height/=2;

#if USE_DX10
		((ID3DTexture2D*)staging_texture->hw_texture())->Unmap( D3D10CalcSubresource( i, 0, dds_info.mips_count)); 
#else
		device::ref().d3d_context()->Unmap( staging_texture->hw_texture(), D3D10CalcSubresource( i, 0, dds_info.mips_count)); 
#endif
	}

	return true;
}

#else 

bool	load_dds_to_texture	( dds_info const & dds_info, res_texture* staging_texture, u32 surf_min_pitch, u32 row_min_pitch)
{
	if( dds_info.width != staging_texture->width() || dds_info.height != staging_texture->height())
	{
		ASSERT( "The loading texture size is different than the destination size.");
		return false;
	}

	if( dds_info.mips_count != staging_texture->mips_count())
	{
		ASSERT( "The loading texture's mip levels are different than the destination's!");
		return false;
	}

	u32 surf_width =	dds_info.width;
	u32 surf_height =	dds_info.height;

	u8 const * src_data_ptr = dds_info.data_ptr;
	u8* dest_buffer = NULL;

#if		USE_DX10
	D3D10_MAPPED_TEXTURE2D mapped_resource;
	HRESULT res = ((ID3DTexture2D*)staging_texture->hw_texture())->Map( 0, D3D_MAP_WRITE, 0, &mapped_resource);
#else
	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	HRESULT res = device::ref().d3d_context()->Map( staging_texture->hw_texture(), 0, D3D_MAP_WRITE, 0, &mapped_resource);
#endif
	dest_buffer = (u8*)mapped_resource.pData;
	CHECK_RESULT( res); 

	for( u32 i = 0; i< dds_info.mips_count; ++i)
	{
		u32 src_row_pitch;
		u32 src_size	= utils::calc_surface_size( surf_width, surf_height, staging_texture->format(), 0, src_row_pitch);

		u32 surf_row_pitch;
		u32 surf_size	= utils::calc_surface_size( surf_width, surf_height, staging_texture->format(), row_min_pitch, surf_row_pitch);
		surf_size		= math::max( surf_size, surf_min_pitch);

		if( surf_size == src_size && i != dds_info.mips_count-1)
		{
			memory::copy( dest_buffer, src_size, src_data_ptr, src_size);
			src_data_ptr += src_size;
			dest_buffer += src_size;
		}
		else
		{
			u32 rows = src_size/src_row_pitch;
			for( u32 r = 0; r < rows; ++r)
			{
				u32 size_to_copy = math::min( src_row_pitch, mapped_resource.RowPitch);
				memory::copy( (u8*)dest_buffer + r*surf_row_pitch, size_to_copy, src_data_ptr, size_to_copy);
				src_data_ptr += src_row_pitch;
			}
			dest_buffer += surf_size;
		}

		surf_width/=2;
		surf_height/=2;
	}

#if USE_DX10
	((ID3DTexture2D*)staging_texture->hw_texture())->Unmap(0); 
#else
	device::ref().d3d_context()->Unmap( staging_texture->hw_texture(), 0); 
#endif

	return true;
}

#endif 


} // namespace render
} // namespace xray