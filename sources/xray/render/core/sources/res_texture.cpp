////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_texture.h>
#include "com_utils.h"
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/device.h>

#ifndef MASTER_GOLD
#include <d3dx11tex.h>
#endif // #ifndef MASTER_GOLD

namespace xray {
namespace render {


#define		PRIORITY_HIGH	12
#define		PRIORITY_NORMAL	8
#define		PRIORITY_LOW	4

//void ref_texture::create(LPCSTR name)
//{
//	resource_manager::ref().create_texture(name);
//}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
res_texture::res_texture( bool pool_texture):
m_surface		(NULL),
m_mip_level_cut	(0),
m_desc_valid	(false),
m_sh_res_view	(NULL),
m_pool_texture	(pool_texture),
m_rescale_min	(float4(0.0f,0.0f,0.0f,0.0f)),
m_rescale_max	(float4(1.0f,1.0f,1.0f,1.0f)),
m_is_registered	( false )
{
	ZeroMemory( &m_desc, sizeof(m_desc));

	//pAVI				= NULL;
	//pTheora				= NULL;
	//desc_cache			= 0;
	//seqMSPF				= 0;
	//flags.MemoryUsage	= 0;
	//flags.bLoaded		= false;
	//flags.bUser			= false;
	//flags.seqCycles		= FALSE;
	//m_material			= 1.0f;
	//m_bind = fastdelegate::FastDelegate1<u32>(this, &res_texture::apply_load);
}

res_texture::~res_texture()
{
	safe_release(m_surface);
	safe_release(m_sh_res_view);

	//unload				();
	// release external reference
}

void res_texture::destroy_impl		() const
{
	resource_manager::ref().release( this );
}

void res_texture::save_as(pcstr file_name)
{
	// TODO:
	// Saving flips R and B components...
	
	HRESULT result;
	
#ifndef MASTER_GOLD
#	if USE_DX10
		result = D3DX10SaveTextureToFile(device::ref().d3d_context(), hw_texture(), D3DX10_IFF_DDS, file_name);
#	else // #if USE_DX10
		result = D3DX11SaveTextureToFile(device::ref().d3d_context(), hw_texture(), D3DX11_IFF_DDS, file_name);
#	endif // #if USE_DX10
#else
	XRAY_UNREFERENCED_PARAMETER(file_name);
#endif // #ifndef MASTER_GOLD
	
	(void)&result;
}

void res_texture::desc_update()
{
	m_desc_cache_surface	= m_surface;
	if (m_desc_cache_surface)
	{
		D3D_RESOURCE_DIMENSION	type;
		m_desc_cache_surface->GetType(&type);
		if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type)
		{
			ID3DTexture2D*	T	= (ID3DTexture2D*)m_desc_cache_surface;
			T->GetDesc(&m_desc);
			m_desc_valid = true;
		}
		if (D3D_RESOURCE_DIMENSION_TEXTURE3D == type)
		{
			ID3DTexture3D*	T	= (ID3DTexture3D*)m_desc_cache_surface;
			T->GetDesc(&m_desc_3d);
			m_desc_3d_valid = true;
		}
	}
}


static DXGI_FORMAT get_srgb_format(DXGI_FORMAT format)
{
	switch (format)
	{
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_TYPELESS: 
			return DXGI_FORMAT_BC1_UNORM_SRGB;
		case DXGI_FORMAT_BC2_UNORM: 
		case DXGI_FORMAT_BC2_TYPELESS: 
			return DXGI_FORMAT_BC2_UNORM_SRGB;
		case DXGI_FORMAT_BC3_UNORM: 
		case DXGI_FORMAT_BC3_TYPELESS:
			return DXGI_FORMAT_BC3_UNORM_SRGB;
		case DXGI_FORMAT_BC7_UNORM: 
		case DXGI_FORMAT_BC7_TYPELESS:
			return DXGI_FORMAT_BC7_UNORM_SRGB;
		default:
			return format;
	};
}


void res_texture::set_hw_texture(ID3DBaseTexture* surface, u32 mip_level_cut, bool staging, bool srgb)
{
	if (surface)
		surface->AddRef();

	m_mip_level_cut = mip_level_cut;

	safe_release( m_surface);

	safe_release( m_sh_res_view);

	m_surface = surface;
	
	m_desc_valid = false;
	m_desc_3d_valid = false;
	
	if (m_surface)
	{
		desc_update();

		ASSERT( mip_level_cut < m_desc.MipLevels || !m_desc_valid);

		D3D_RESOURCE_DIMENSION	type;
		m_surface->GetType(&type);
		
		if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type )
		{
			D3D_SHADER_RESOURCE_VIEW_DESC	view_desc;

			if (m_desc.MiscFlags&D3D_RESOURCE_MISC_TEXTURECUBE)
			{
				view_desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
				view_desc.TextureCube.MostDetailedMip = 0;
				view_desc.TextureCube.MipLevels = m_desc.MipLevels;
			}
			else
			{
				if(m_desc.SampleDesc.Count <= 1 )
				{
					if( m_desc.ArraySize > 1)
					{
						view_desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
						view_desc.Texture2DArray.FirstArraySlice = 0;
						view_desc.Texture2DArray.ArraySize = m_desc.ArraySize;
						view_desc.Texture2DArray.MostDetailedMip = 0;
						view_desc.Texture2DArray.MipLevels = m_desc.MipLevels;
					}
					else
					{
						view_desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
						view_desc.Texture2D.MostDetailedMip = 0;
						view_desc.Texture2D.MipLevels = m_desc.MipLevels;
					}
				}
				else
				{
					ASSERT( m_desc.ArraySize == 1, "Multisampler Array textures support are not implemented.");
					view_desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
					view_desc.Texture2D.MostDetailedMip = 0;
					view_desc.Texture2D.MipLevels = m_desc.MipLevels;
				}
			}			
			
			if (srgb)
				view_desc.Format = get_srgb_format(m_desc.Format);
			else
				view_desc.Format = DXGI_FORMAT_UNKNOWN;
			
			switch(m_desc.Format)
			{
			case DXGI_FORMAT_R24G8_TYPELESS:
				view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			case DXGI_FORMAT_R32_TYPELESS:
				view_desc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
			}

			// this would be supported by DX10.1 but is not needed for stalker
			// if( view_desc.Format != DXGI_FORMAT_R24_UNORM_X8_TYPELESS )
			if( !staging && ((m_desc.SampleDesc.Count <= 1)  )/* || (view_desc.Format != DXGI_FORMAT_R24_UNORM_X8_TYPELESS))*/ )
				CHECK_RESULT(device::ref().d3d_device()->CreateShaderResourceView(m_surface, &view_desc, &m_sh_res_view));
			else
				m_sh_res_view = 0;
		}
		else if (D3D_RESOURCE_DIMENSION_TEXTURE3D == type )
		{
			D3D_SHADER_RESOURCE_VIEW_DESC		view_desc;
			view_desc.Format					= m_desc_3d.Format;
			view_desc.ViewDimension				= D3D_SRV_DIMENSION_TEXTURE3D;
			view_desc.Texture3D.MostDetailedMip = 0;
			view_desc.Texture3D.MipLevels		= m_desc_3d.MipLevels;
			
			if (!staging)
				CHECK_RESULT(device::ref().d3d_device()->CreateShaderResourceView(m_surface, NULL, &m_sh_res_view));
			else
				m_sh_res_view = 0;
		}
		else
			CHECK_RESULT(device::ref().d3d_device()->CreateShaderResourceView(m_surface, NULL, &m_sh_res_view));
	}	
}

void* res_texture::map2D	( D3D_MAP mode, u32 mip_level, u32& row_pitch)
{
	if( m_surface && m_desc_valid)
	{
		D3D_RESOURCE_DIMENSION	type;
		m_surface->GetType(&type);
		if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type)
		{
#if USE_DX10
			D3D10_MAPPED_TEXTURE2D buffer;
			((ID3DTexture2D*)m_surface)->Map( D3D10CalcSubresource( mip_level, 0, m_desc.MipLevels), mode, 0, &buffer);

			row_pitch = buffer.RowPitch;
			return buffer.pData;
#else
			D3D11_MAPPED_SUBRESOURCE mapped_res;
			device::ref().d3d_context()->Map( m_surface, D3D10CalcSubresource( mip_level, 0, m_desc.MipLevels), mode, 0, &mapped_res);

			row_pitch = mapped_res.RowPitch;
			return mapped_res.pData;
#endif
			
		}
	}
	ASSERT( 0, "The texture couldn't be mapped.");
	return NULL;
}
void res_texture::unmap2D			( u32 mip_level)
{
#if USE_DX10
	((ID3DTexture2D*)m_surface)->Unmap( D3D10CalcSubresource( mip_level, 0, m_desc.MipLevels));
#else
	device::ref().d3d_context()->Unmap( m_surface, D3D10CalcSubresource( mip_level, 0, m_desc.MipLevels));
#endif
}


void* res_texture::map3D	( D3D_MAP mode, u32 mip_level, u32& row_pitch)
{
	if( m_surface && m_desc_3d_valid)
	{
		D3D_RESOURCE_DIMENSION	type;
		m_surface->GetType(&type);
		if (D3D_RESOURCE_DIMENSION_TEXTURE3D == type)
		{
#if USE_DX10
			D3D10_MAPPED_TEXTURE2D buffer;
			((ID3DTexture3D*)m_surface)->Map( D3D10CalcSubresource( mip_level, 0, m_desc_3d.MipLevels), mode, 0, &buffer);
			
			row_pitch = buffer.RowPitch;
			return buffer.pData;
#else
			D3D11_MAPPED_SUBRESOURCE mapped_res;
			device::ref().d3d_context()->Map( m_surface, D3D10CalcSubresource( mip_level, 0, m_desc_3d.MipLevels), mode, 0, &mapped_res);
			
			row_pitch = mapped_res.RowPitch;
			return mapped_res.pData;
#endif
			
		}
	}
	ASSERT( 0, "The texture couldn't be mapped.");
	return NULL;
}
void res_texture::unmap3D			( u32 mip_level)
{
#if USE_DX10
	((ID3DTexture2D*)m_surface)->Unmap( D3D10CalcSubresource( mip_level, 0, m_desc_3d.MipLevels));
#else
	device::ref().d3d_context()->Unmap( m_surface, D3D10CalcSubresource( mip_level, 0, m_desc_3d.MipLevels));
#endif
}

ID3DBaseTexture* res_texture::hw_texture()
{
// 	if (m_surface)
// 		m_surface->AddRef();

	return m_surface;
}

void res_texture::clone 	( res_texture* other)
{
	safe_release(m_surface);
	safe_release(m_sh_res_view);

	// If you added any member to the class res_texture, than add it to this copy list as well,
	// otherwise just set the new size of the class.
//	COMPILE_ASSERT( sizeof(res_texture) == 92, The_res_texture_class_size_has_changed);

	m_loaded					= other->m_loaded;
	m_user						= other->m_user;
	m_seq_cycles				= other->m_seq_cycles; 
	m_mem_usage					= other->m_mem_usage;
										
#	ifdef	USE_DX10_OLD				
	m_is_loaded_as_staging		= other->m_is_loaded_as_staging;
#	endif	//	USE_DX10		
								
	m_bind						= other->m_bind;
							
	m_surface					= other->m_surface;
	m_desc_cache_surface		= other->m_desc_cache_surface;
	m_sh_res_view				= other->m_sh_res_view;
	m_desc						= other->m_desc;
	m_mip_level_cut				= other->m_mip_level_cut;
	m_desc_valid				= other->m_desc_valid;
	m_pool_texture				= other->m_pool_texture;


	if( m_surface)
		m_surface->AddRef();

	if( m_sh_res_view)
		m_sh_res_view->AddRef();
}

} // namespace render 
} // namespace xray