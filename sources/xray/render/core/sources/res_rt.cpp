////////////////////////////////////////////////////////////////////////////
//	Created		: 27.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/device.h>
#include <xray/render/core/res_rt.h>
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render_dx10 { 

u32		btw_lowest_bitmask( u32 x) {return x & ~( x-1);}
bool	btw_is_pow2( u32 v) {return btw_lowest_bitmask( v) == v;}

res_rt::res_rt()
{
	m_surface = NULL;
	m_rt	  = NULL;
	m_width	  = 0;
	m_height  = 0;
	m_format  = DXGI_FORMAT_UNKNOWN;
}

res_rt::~res_rt()
{
	destroy();

}

void res_rt::_free() const
{
	resource_manager::ref().release( const_cast<res_rt*>(this) );
}

void res_rt::create( LPCSTR name, u32 width, u32 height, DXGI_FORMAT format, enum_usage usage)
{
	u32 sample_count = 1;
	if( m_surface)
		return;

	R_ASSERT( device::ref().d3d_context() && name && name[0] && width && height);

	m_order	= timing::get_clocks();

	// Check width-and-height of render target surface
	if ( width > D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)		return;
	if ( height > D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)		return;

	// Select usage
	if( usage == enum_usage_depth_stencil)
	{
		if (	DXGI_FORMAT_R24G8_TYPELESS != format
				|| DXGI_FORMAT_D24_UNORM_S8_UINT != format
				|| DXGI_FORMAT_D16_UNORM != format
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


	resource_manager::ref().evict(); // ??????

	D3D_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.Width		= width;
	desc.Height		= height;
	desc.MipLevels	= 1;
	desc.ArraySize	= 1;
	desc.Format		= format;
	desc.SampleDesc.Count = sample_count;
	desc.Usage		= D3D_USAGE_DEFAULT;
	if( sample_count <= 1 )
		desc.BindFlags = D3D_BIND_SHADER_RESOURCE | ((usage==enum_usage_depth_stencil) ? D3D_BIND_DEPTH_STENCIL : D3D_BIND_RENDER_TARGET);
	else
	{
		desc.BindFlags = ((usage==enum_usage_depth_stencil) ? D3D_BIND_DEPTH_STENCIL : (D3D_BIND_SHADER_RESOURCE | D3D_BIND_RENDER_TARGET));
// 		if( RImplementation.o.dx10_msaa_opt )
// 		{
// 			desc.SampleDesc.Quality = UINT(D3D_STANDARD_MULTISAMPLE_PATTERN);
// 		}
	}

#ifdef USE_DX11
	if (HW.FeatureLevel>=D3D_FEATURE_LEVEL_11_0 && !bUseAsDepth && sample_count == 1 && useUAV )
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
#endif

	R_CHK( device::ref().d3d_device()->CreateTexture2D( &desc, NULL, &m_surface ) );

#ifdef DEBUG
	LOG_INFO( "* created RT( %s), %dx%d", name, width, height);
#endif // DEBUG

	if (usage == enum_usage_depth_stencil)
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

		R_CHK( device::ref().d3d_device()->CreateDepthStencilView( m_surface, &ViewDesc, &m_zrt) );
	}
	else
		R_CHK( device::ref().d3d_device()->CreateRenderTargetView( m_surface, 0, &m_rt ) );

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

	m_texture = resource_manager::ref().create_texture( name);
	m_texture->set_hw_texture( m_surface);
}

void res_rt::destroy()
{
	if ( m_texture.c_ptr())
	{
		m_texture->set_hw_texture( 0);
		m_texture = NULL;
	}

	safe_release( m_rt);
	log_ref_count( get_name(), m_surface);
	safe_release( m_surface);
}

// void res_rt::reset_begin()
// {
// 	destroy();
// }
// 
// void res_rt::reset_end()
// {
// 	create( get_name(), m_width, m_height, m_format, m_usage);
// }

}// namespace render
}// namespace xray
