////////////////////////////////////////////////////////////////////////////
//	Created		: 27.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/render_target.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/device.h>
#include "com_utils.h"

#ifndef MASTER_GOLD
#	include <d3dx11tex.h>
#endif // #ifndef MASTER_GOLD

namespace xray {
namespace render { 

u32		btw_lowest_bitmask( u32 x) {return x & ~( x-1);}
bool	btw_is_pow2( u32 v) {return btw_lowest_bitmask( v) == v;}

render_target::render_target() :
	m_is_registered ( false )
{
	m_surface_3d = NULL;
	m_surface = NULL;
	m_rt	  = NULL;
	m_width	  = 0;
	m_height  = 0;
	m_format  = DXGI_FORMAT_UNKNOWN;
}

render_target::~render_target()
{
	destroy();

}

void render_target::destroy_impl() const
{
	resource_manager::ref().release( this );
}

void render_target::save_as	(pcstr file_name)
{
#ifndef MASTER_GOLD
#	if USE_DX10
		if (m_surface)
			D3DX10SaveTextureToFile(device::ref().d3d_context(), m_surface, D3DX11_IFF_DDS, file_name);
		else if (m_surface_3d)
			D3DX10SaveTextureToFile(device::ref().d3d_context(), m_surface_3d, D3DX11_IFF_DDS, file_name);
#	else // #if USE_DX10
		if (m_surface)
			D3DX11SaveTextureToFile(device::ref().d3d_context(), m_surface, D3DX11_IFF_DDS, file_name);
		else if (m_surface_3d)
			D3DX11SaveTextureToFile(device::ref().d3d_context(), m_surface_3d, D3DX11_IFF_DDS, file_name);
#	endif // #if USE_DX10	
#endif // #ifndef MASTER_GOLD
}

void render_target::create_3d( pcstr name, u32 width, u32 height, u32 depth, DXGI_FORMAT format, enum_rt_usage usage, D3D_USAGE memory_usage )
{
	if( m_surface_3d)
		return;
	
	//R_ASSERT( device::ref().d3d_context() && name && name[0] && width && height);
	
	// Select usage
	if( usage == enum_rt_usage_depth_stencil)
	{
		ASSERT(0, "3d depth stencil is not supported.");
		return;
	}
	
	D3D_TEXTURE3D_DESC		desc;
	xray::memory::zero		(&desc, sizeof(desc));
	desc.Width				= width;
	desc.Height				= height;
	desc.Depth				= depth;
	desc.MipLevels			= 1;
	desc.Format				= format;
	desc.Usage				= memory_usage;
	desc.BindFlags			= D3D_BIND_SHADER_RESOURCE | D3D_BIND_RENDER_TARGET;
	
	CHECK_RESULT( device::ref().d3d_device()->CreateTexture3D( &desc, NULL, &m_surface_3d ) );
	
	D3D_RENDER_TARGET_VIEW_DESC				desc_rt;
	xray::memory::zero						(&desc_rt, sizeof(desc_rt));
	desc_rt.Format							= format;
	desc_rt.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE3D;
	desc_rt.Texture3D.MipSlice				= 0;
	desc_rt.Texture3D.FirstWSlice			= 0;
	desc_rt.Texture3D.WSize					= depth;
	
	CHECK_RESULT( device::ref().d3d_device()->CreateRenderTargetView( m_surface_3d, &desc_rt, &m_rt ) );
	
	if (name)
		m_texture = resource_manager::ref().create_texture( name);
	else
	{
		m_texture = NEW( res_texture);
		m_texture->set_name( 0 );
		m_texture->mark_registered();
	}
	m_texture->set_hw_texture( m_surface_3d);
}

void render_target::create( LPCSTR name, u32 width, u32 height, DXGI_FORMAT format, enum_rt_usage usage, ref_texture in_texture, u32 first_array_slice_index)
{
	if( m_surface)
		return;
	
	R_ASSERT( device::ref().d3d_context() && name && name[0] && width && height);
	
	// Select usage
	if( usage == enum_rt_usage_depth_stencil)
	{
		if (   DXGI_FORMAT_R24G8_TYPELESS != format
			&& DXGI_FORMAT_D24_UNORM_S8_UINT != format
			&& DXGI_FORMAT_D16_UNORM != format
			)
		{
			ASSERT(0, "The spedified format is not supported.");
			return;
		}
	}
	
	if (usage == enum_rt_usage_depth_stencil)
	{
		D3D_DEPTH_STENCIL_VIEW_DESC				desc_rt;
		xray::memory::zero						(&desc_rt, sizeof(desc_rt));
		
		desc_rt.Format							= format;
		desc_rt.ViewDimension					= D3D_DSV_DIMENSION_TEXTURE2DARRAY;
		desc_rt.Texture2DArray.MipSlice			= 0;
		desc_rt.Texture2DArray.ArraySize		= 1;
		desc_rt.Texture2DArray.FirstArraySlice	= first_array_slice_index;
		
		switch (desc_rt.Format)
		{
			case DXGI_FORMAT_R24G8_TYPELESS:
				desc_rt.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				break;
			case DXGI_FORMAT_R32_TYPELESS:
				desc_rt.Format = DXGI_FORMAT_D32_FLOAT;
				break;
		}
		
		CHECK_RESULT( device::ref().d3d_device()->CreateDepthStencilView( in_texture->hw_texture(), &desc_rt, &m_zrt) );
	}
	else
	{
		D3D_RENDER_TARGET_VIEW_DESC				desc_rt;
		xray::memory::zero						(&desc_rt, sizeof(desc_rt));
		desc_rt.Format							= in_texture->format();
		desc_rt.ViewDimension					= D3D_RTV_DIMENSION_TEXTURE2DARRAY;
		desc_rt.Texture2DArray.MipSlice			= 0;
		desc_rt.Texture2DArray.ArraySize		= 1;
		desc_rt.Texture2DArray.FirstArraySlice	= first_array_slice_index;
		
		CHECK_RESULT( device::ref().d3d_device()->CreateRenderTargetView( in_texture->hw_texture(), &desc_rt, &m_rt ) );
	}
}

void render_target::create( LPCSTR name, u32 width, u32 height, DXGI_FORMAT format, enum_rt_usage usage, D3D_USAGE memory_usage, u32 sample_count)
{
	//u32 sample_count = 1;
	
	if( m_surface)
		return;

	//R_ASSERT( device::ref().d3d_context() && name && name[0] && width && height);

	m_order	= timing::get_clocks();

	// Check width-and-height of render target surface
	if ( width > D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)		return;
	if ( height > D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)		return;

	// Select usage
	if( usage == enum_rt_usage_depth_stencil)
	{
		if (	DXGI_FORMAT_R24G8_TYPELESS != format
				&& DXGI_FORMAT_D24_UNORM_S8_UINT != format
				&& DXGI_FORMAT_D16_UNORM != format
				// This is a hardware specific format need to check what formats can be used for modern hardware.
				// || ( D3DFORMAT)MAKEFOURCC( 'D','F','2','4') != format
				)
		{
			ASSERT(0, "The spedified format is not supported.");
			return;
		}
	}
	
	m_width		= width;
	m_height	= height;
	m_format	= format;
	m_usage		= usage;
	
	D3D_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width		= width;
	desc.Height		= height;
	desc.MipLevels	= 1;
	desc.ArraySize	= 1;
	desc.Format		= format;
	desc.SampleDesc.Count = sample_count;
	desc.Usage		= memory_usage;
	
	if( sample_count <= 1 )
		desc.BindFlags = D3D_BIND_SHADER_RESOURCE | ((usage==enum_rt_usage_depth_stencil) ? D3D_BIND_DEPTH_STENCIL : D3D_BIND_RENDER_TARGET);
	else
	{
		desc.BindFlags = ((usage==enum_rt_usage_depth_stencil) ? D3D_BIND_DEPTH_STENCIL : (D3D_BIND_SHADER_RESOURCE | D3D_BIND_RENDER_TARGET));
// 		if( RImplementation.o.dx10_msaa_opt )
// 		{
#if USE_DX10
 			desc.SampleDesc.Quality = sample_count * sample_count;//UINT(D3D10_STANDARD_MULTISAMPLE_PATTERN);
#else
		desc.SampleDesc.Quality = sample_count * sample_count;//UINT(D3D11_STANDARD_MULTISAMPLE_PATTERN);
#endif // #ifdef USE_DX10
// 		}
	}

#ifdef USE_DX11
	if (HW.FeatureLevel>=D3D_FEATURE_LEVEL_11_0 && !bUseAsDepth && sample_count == 1 && useUAV )
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
#endif

	CHECK_RESULT( device::ref().d3d_device()->CreateTexture2D( &desc, NULL, &m_surface ) );

#ifdef DEBUG
	LOG_INFO( "* created RT( %s), %dx%d", name, width, height);
#endif // DEBUG

	if (usage == enum_rt_usage_depth_stencil)
	{
		D3D_DEPTH_STENCIL_VIEW_DESC	ViewDesc;
		ZeroMemory( &ViewDesc, sizeof(ViewDesc) );

		ViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		if( sample_count <= 1 )
		{
			ViewDesc.ViewDimension = D3D_DSV_DIMENSION_TEXTURE2D;
		}
		else
		{
			ViewDesc.ViewDimension = D3D_DSV_DIMENSION_TEXTURE2DMS;
			ViewDesc.Texture2DMS.UnusedField_NothingToDefine = 0;
		}

		ViewDesc.Texture2D.MipSlice = 0;
		switch (desc.Format)
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			ViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			ViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			break;
		}

		CHECK_RESULT( device::ref().d3d_device()->CreateDepthStencilView( m_surface, &ViewDesc, &m_zrt) );
	}
	else
	{
		if (sample_count > 1)
		{
			D3D_RENDER_TARGET_VIEW_DESC rtv_desc;
			rtv_desc.Format = format;
			rtv_desc.ViewDimension = D3D_RTV_DIMENSION_TEXTURE2DMS;
			CHECK_RESULT( device::ref().d3d_device()->CreateRenderTargetView( m_surface, &rtv_desc, &m_rt ) );
		}
		else
		{
			CHECK_RESULT( device::ref().d3d_device()->CreateRenderTargetView( m_surface, 0, &m_rt ) );
		}
		
	}

#ifdef USE_DX11
	if (HW.FeatureLevel>=D3D_FEATURE_LEVEL_11_0 && !bUseAsDepth &&  SampleCount == 1 && useUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		ZeroMemory( &UAVDesc, sizeof( D3D11_UNORDERED_ACCESS_VIEW_DESC ) );
		UAVDesc.Format = dx10FMT;
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		UAVDesc.Buffer.FirstElement = 0;
		UAVDesc.Buffer.NumElements = dwWidth * dwHeight;
		CHK_DX( HW.pDevice->CreateUnorderedAccessView( pSurface, &UAVDesc, &pUAView ) );
	}
#endif

	if (name)
		m_texture = resource_manager::ref().create_texture( name);
	else
	{
		m_texture = NEW( res_texture);
		m_texture->set_name( 0 );
		m_texture->mark_registered();
	}
	m_texture->set_hw_texture( m_surface);
}

void render_target::destroy()
{
	if ( m_texture.c_ptr())
	{
		m_texture->set_hw_texture( 0);
		m_texture = NULL;
	}

	safe_release( m_rt);
	
	if (m_surface)
	{
		log_ref_count( name().c_str(), m_surface);
	}

	if (m_surface_3d)
	{
		log_ref_count( name().c_str(), m_surface_3d);
	}
	
	safe_release( m_surface);
	safe_release( m_surface_3d);
}

// void render_target::reset_begin()
// {
// 	destroy();
// }
// 
// void render_target::reset_end()
// {
// 	create( get_name(), m_width, m_height, m_format, m_usage);
// }

}// namespace render
}// namespace xray
