////////////////////////////////////////////////////////////////////////////
//	Created		: 05.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/state_utils.h>
#include <boost/crc.hpp>

namespace xray{
namespace render {
namespace state_utils{


namespace crc 
{
typedef	boost::crc_optimal<32, 0x04C11DB7, 0, 0, true, false>	processor;

inline void process( processor & proc, u8 value)
{
	proc.process_byte( value);
}


inline void process( processor & proc, bool value)
{
	proc.process_byte( value);
}

inline void process( processor & proc, u32 value)
{
	proc.process_byte( (u8)((value) & 0xff));
	proc.process_byte( (u8)((value>>8) & 0xff));
	proc.process_byte( (u8)((value>>16) & 0xff));
	proc.process_byte( (u8)((value>>24) & 0xff));
}

inline void process( processor & proc, int value)
{
	process( proc, (u32)value);
}


inline void process( processor & proc, float value)
{
	process( proc, horrible_cast<float,u32>(value).second);
}

} //namespace crc



void reset( D3D_RASTERIZER_DESC &desc )
{
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode				= D3D_FILL_SOLID;
	desc.CullMode				= D3D_CULL_BACK;
	desc.FrontCounterClockwise	= FALSE;
	desc.DepthBias				= 0;
	desc.DepthBiasClamp			= 0.0f;
	desc.SlopeScaledDepthBias	= 0.0f;
	desc.DepthClipEnable		= TRUE;
	desc.ScissorEnable			= FALSE;
	// --Porting to DX10_
	// 	if( RImplementation.o.dx10_msaa )
	// 		desc.MultisampleEnable	= TRUE;
	// 	else
	desc.MultisampleEnable	= FALSE;
	desc.AntialiasedLineEnable	= FALSE;
}

void reset( D3D_DEPTH_STENCIL_DESC &desc )
{
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable		= TRUE;
	desc.DepthWriteMask		= D3D_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc			= D3D_COMPARISON_LESS;
	desc.StencilEnable		= TRUE;

	// --Porting to DX10_
	// 	if( !RImplementation.o.dx10_msaa )
	// 	{
	// 		desc.StencilReadMask = 0xFF;
	// 		desc.StencilWriteMask = 0xFF;
	// 	}
	// 	else
	{
		desc.StencilReadMask = 0x7F;
		desc.StencilWriteMask = 0x7F;
	}

	desc.FrontFace.StencilFailOp		= D3D_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp	= D3D_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp		= D3D_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc			= D3D_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp			= D3D_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp	= D3D_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp			= D3D_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc			= D3D_COMPARISON_ALWAYS;
}

void reset( D3D_BLEND_DESC &desc )
{
	ZeroMemory(&desc, sizeof(desc));

#if USE_DX10
	desc.AlphaToCoverageEnable	= FALSE;
	desc.SrcBlend				= D3D_BLEND_ONE;
	desc.DestBlend				= D3D_BLEND_ZERO;
	desc.BlendOp				= D3D_BLEND_OP_ADD;
	desc.SrcBlendAlpha			= D3D_BLEND_ONE;
	desc.DestBlendAlpha			= D3D_BLEND_ZERO;
	desc.BlendOpAlpha			= D3D_BLEND_OP_ADD;

	for ( int i=0; i<8; ++i)
	{
		desc.BlendEnable[i]				= FALSE;
		desc.RenderTargetWriteMask[i]	= D3D_COLOR_WRITE_ENABLE_ALL;
	}

#else

	desc.AlphaToCoverageEnable	= FALSE;
	desc.IndependentBlendEnable = FALSE;

	for ( int i=0; i<8; ++i)
	{
		desc.RenderTarget[i].BlendEnable		= FALSE;
		desc.RenderTarget[i].SrcBlend			= D3D_BLEND_ONE;
		desc.RenderTarget[i].DestBlend			= D3D_BLEND_ZERO;
		desc.RenderTarget[i].BlendOp			= D3D_BLEND_OP_ADD;
		desc.RenderTarget[i].SrcBlendAlpha		= D3D_BLEND_ONE;
		desc.RenderTarget[i].DestBlendAlpha		= D3D_BLEND_ZERO;
		desc.RenderTarget[i].BlendOpAlpha		= D3D_BLEND_OP_ADD;
		desc.RenderTarget[i].RenderTargetWriteMask	= D3D_COLOR_WRITE_ENABLE_ALL;
	}
#endif
}

void reset( D3D_SAMPLER_DESC &desc )
{
	ZeroMemory(&desc, sizeof(desc));

	desc.Filter = D3D_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D_TEXTURE_ADDRESS_CLAMP;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D_COMPARISON_NEVER;
	desc.BorderColor[0] = 1.0f;
	desc.BorderColor[1] = 1.0f;
	desc.BorderColor[2] = 1.0f;
	desc.BorderColor[3] = 1.0f;
	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
}

bool operator==(const D3D_RASTERIZER_DESC &desc1, const D3D_RASTERIZER_DESC &desc2)
{
	if (desc1.FillMode != desc2.FillMode) return false;
	if (desc1.CullMode != desc2.CullMode) return false;
	if (desc1.FrontCounterClockwise != desc2.FrontCounterClockwise) return false;
	if (desc1.DepthBias != desc2.DepthBias) return false;
	if (desc1.DepthBiasClamp != desc2.DepthBiasClamp) return false;
	if (desc1.SlopeScaledDepthBias != desc2.SlopeScaledDepthBias) return false;
	if (desc1.DepthClipEnable != desc2.DepthClipEnable) return false;
	if (desc1.ScissorEnable != desc2.ScissorEnable) return false;
	if (desc1.MultisampleEnable != desc2.MultisampleEnable) return false;
	if (desc1.AntialiasedLineEnable != desc2.AntialiasedLineEnable) return false;

	return true;
}

bool operator==(const D3D_DEPTH_STENCIL_DESC &desc1, const D3D_DEPTH_STENCIL_DESC &desc2)
{
	if ( desc1.DepthEnable != desc2.DepthEnable) return false;
	if ( desc1.DepthWriteMask != desc2.DepthWriteMask) return false;
	if ( desc1.DepthFunc != desc2.DepthFunc) return false;
	if ( desc1.StencilEnable != desc2.StencilEnable) return false;
	if ( desc1.StencilReadMask != desc2.StencilReadMask) return false;
	if ( desc1.StencilWriteMask != desc2.StencilWriteMask) return false;

	if ( desc1.FrontFace.StencilFailOp != desc2.FrontFace.StencilFailOp) return false;
	if ( desc1.FrontFace.StencilDepthFailOp != desc2.FrontFace.StencilDepthFailOp) return false;
	if ( desc1.FrontFace.StencilPassOp != desc2.FrontFace.StencilPassOp) return false;
	if ( desc1.FrontFace.StencilFunc != desc2.FrontFace.StencilFunc) return false;

	if ( desc1.BackFace.StencilFailOp != desc2.BackFace.StencilFailOp) return false;
	if ( desc1.BackFace.StencilDepthFailOp != desc2.BackFace.StencilDepthFailOp) return false;
	if ( desc1.BackFace.StencilPassOp != desc2.BackFace.StencilPassOp) return false;
	if ( desc1.BackFace.StencilFunc != desc2.BackFace.StencilFunc) return false;
	
	return true;
}

#ifdef USE_DX11
bool operator==(const D3D_BLEND_DESC &desc1, const D3D_BLEND_DESC &desc2)
{
	if ( desc1.AlphaToCoverageEnable != desc2.AlphaToCoverageEnable) return false;
	if ( desc1.IndependentBlendEnable != desc2.IndependentBlendEnable) return false;
	
	for ( int i=0; i<4/*8*//*It's quick fix for dx11 port because of mispatch of desc*/; ++i)
	{
		if ( desc1.RenderTarget[i].SrcBlend != desc2.RenderTarget[i].SrcBlend) return false;
		if ( desc1.RenderTarget[i].DestBlend != desc2.RenderTarget[i].DestBlend) return false;
		if ( desc1.RenderTarget[i].BlendOp != desc2.RenderTarget[i].BlendOp) return false;
		if ( desc1.RenderTarget[i].SrcBlendAlpha != desc2.RenderTarget[i].SrcBlendAlpha) return false;
		if ( desc1.RenderTarget[i].DestBlendAlpha != desc2.RenderTarget[i].DestBlendAlpha) return false;
		if ( desc1.RenderTarget[i].BlendOpAlpha != desc2.RenderTarget[i].BlendOpAlpha) return false;
		if ( desc1.RenderTarget[i].BlendEnable!= desc2.RenderTarget[i].BlendEnable) return false;
		if ( desc1.RenderTarget[i].RenderTargetWriteMask!= desc2.RenderTarget[i].RenderTargetWriteMask) return false;
	}

	return true;
}
#else
bool operator==(const D3D_BLEND_DESC &desc1, const D3D_BLEND_DESC &desc2)
{
#if	USE_DX10

	if ( desc1.AlphaToCoverageEnable != desc2.AlphaToCoverageEnable) return false;
	if ( desc1.SrcBlend != desc2.SrcBlend) return false;
	if ( desc1.DestBlend != desc2.DestBlend) return false;
	if ( desc1.BlendOp != desc2.BlendOp) return false;
	if ( desc1.SrcBlendAlpha != desc2.SrcBlendAlpha) return false;
	if ( desc1.DestBlendAlpha != desc2.DestBlendAlpha) return false;
	if ( desc1.BlendOpAlpha != desc2.BlendOpAlpha) return false;

	for ( int i=0; i<8; ++i)
	{
		if( desc1.BlendEnable[i]!= desc2.BlendEnable[i]) return false;
		if( desc1.RenderTargetWriteMask[i]!= desc2.RenderTargetWriteMask[i]) return false;
	}

#else

	if ( desc1.AlphaToCoverageEnable != desc2.AlphaToCoverageEnable) return false;
	if ( desc1.IndependentBlendEnable != desc2.IndependentBlendEnable) return false;

	for ( int i=0; i<8; ++i)
	{
		if( desc1.RenderTarget[i].BlendEnable	!= desc2.RenderTarget[i].BlendEnable) return false;
		if( desc1.RenderTarget[i].SrcBlend		!= desc2.RenderTarget[i].SrcBlend) return false;
		if( desc1.RenderTarget[i].DestBlend		!= desc2.RenderTarget[i].DestBlend) return false;
		if( desc1.RenderTarget[i].BlendOp		!= desc2.RenderTarget[i].BlendOp) return false;
		if( desc1.RenderTarget[i].SrcBlendAlpha	!= desc2.RenderTarget[i].SrcBlendAlpha) return false;
		if( desc1.RenderTarget[i].DestBlendAlpha != desc2.RenderTarget[i].DestBlendAlpha) return false;
		if( desc1.RenderTarget[i].BlendOpAlpha	 != desc2.RenderTarget[i].BlendOpAlpha) return false;
		if( desc1.RenderTarget[i].RenderTargetWriteMask != desc2.RenderTarget[i].RenderTargetWriteMask) return false;
	}

#endif

	return true;
}
#endif

bool operator==(const D3D_SAMPLER_DESC &desc1, const D3D_SAMPLER_DESC &desc2)
{
	if( desc1.Filter != desc2.Filter) return false;
	if( desc1.AddressU != desc2.AddressU) return false;
	if( desc1.AddressV != desc2.AddressV) return false;
	if( desc1.AddressW != desc2.AddressW) return false;
	if( desc1.MipLODBias != desc2.MipLODBias) return false;
//	Ignore anisotropy since it's set up automatically by the manager
//	if( desc1.MaxAnisotropy != desc2.MaxAnisotropy) return false;
	if( desc1.ComparisonFunc != desc2.ComparisonFunc) return false;
	if( desc1.BorderColor[0] != desc2.BorderColor[0]) return false;
	if( desc1.BorderColor[1] != desc2.BorderColor[1]) return false;
	if( desc1.BorderColor[2] != desc2.BorderColor[2]) return false;
	if( desc1.BorderColor[3] != desc2.BorderColor[3]) return false;
	if( desc1.MinLOD != desc2.MinLOD) return false;
	if( desc1.MaxLOD != desc2.MaxLOD) return false;

	return true;
}

u32 get_hash( const D3D_RASTERIZER_DESC &desc )
{
	crc::processor	crc_hash;

	crc::process( crc_hash, desc.FillMode);
	crc::process( crc_hash, desc.CullMode);
	crc::process( crc_hash, desc.FrontCounterClockwise);
	crc::process( crc_hash, desc.DepthBias);
	crc::process( crc_hash, desc.DepthBiasClamp);
	crc::process( crc_hash, desc.SlopeScaledDepthBias);
	crc::process( crc_hash, desc.DepthClipEnable);
	crc::process( crc_hash, desc.ScissorEnable);
	crc::process( crc_hash, desc.MultisampleEnable);
	crc::process( crc_hash, desc.AntialiasedLineEnable);

// 	crc::process( crc_hash, desc.FillMode, sizeof(desc.FillMode) );
// 	crc::process( crc_hash, desc.CullMode, sizeof(desc.CullMode) );
// 	crc::process( crc_hash, desc.FrontCounterClockwise, sizeof(desc.FrontCounterClockwise) );
// 	crc::process( crc_hash, desc.DepthBias, sizeof(desc.DepthBias) );
// 	crc::process( crc_hash, desc.DepthBiasClamp, sizeof(desc.DepthBiasClamp) );
// 	crc::process( crc_hash, desc.SlopeScaledDepthBias, sizeof(desc.SlopeScaledDepthBias) );
// 	crc::process( crc_hash, desc.DepthClipEnable, sizeof(desc.DepthClipEnable) );
// 	crc::process( crc_hash, desc.ScissorEnable, sizeof(desc.ScissorEnable) );
// 	crc::process( crc_hash, desc.MultisampleEnable, sizeof(desc.MultisampleEnable) );
// 	crc::process( crc_hash, desc.AntialiasedLineEnable, sizeof(desc.AntialiasedLineEnable) );
	
	return crc_hash.checksum();
}

u32 get_hash( const D3D_DEPTH_STENCIL_DESC &desc )
{
	crc::processor crc_hash;

	crc::process( crc_hash, desc.DepthEnable);
	crc::process( crc_hash, desc.DepthWriteMask);
	crc::process( crc_hash, desc.DepthFunc);
	crc::process( crc_hash, desc.StencilEnable);
	crc::process( crc_hash, desc.StencilReadMask);
	crc::process( crc_hash, desc.StencilWriteMask);

	crc::process( crc_hash, desc.FrontFace.StencilFailOp);
	crc::process( crc_hash, desc.FrontFace.StencilDepthFailOp);
	crc::process( crc_hash, desc.FrontFace.StencilPassOp);
	crc::process( crc_hash, desc.FrontFace.StencilFunc);

	crc::process( crc_hash, desc.BackFace.StencilFailOp);
	crc::process( crc_hash, desc.BackFace.StencilDepthFailOp);
	crc::process( crc_hash, desc.BackFace.StencilPassOp);
	crc::process( crc_hash, desc.BackFace.StencilFunc);

	return crc_hash.checksum();
}

u32 get_hash( const D3D_BLEND_DESC &desc )
{
	crc::processor	crc_hash;

#if USE_DX10

	crc::process( crc_hash, desc.AlphaToCoverageEnable);
	crc::process( crc_hash, desc.SrcBlend);
	crc::process( crc_hash, desc.DestBlend);
	crc::process( crc_hash, desc.BlendOp);
	crc::process( crc_hash, desc.SrcBlendAlpha);
	crc::process( crc_hash, desc.DestBlendAlpha);
	crc::process( crc_hash, desc.BlendOpAlpha);

	for ( int i=0; i<8; ++i)
	{
		crc::process( crc_hash, desc.BlendEnable[i]);
		crc::process( crc_hash, desc.RenderTargetWriteMask[i]);
	}

#else

	crc::process( crc_hash, desc.AlphaToCoverageEnable);
	crc::process( crc_hash, desc.IndependentBlendEnable);

	for ( int i=0; i<8; ++i)
	{
		crc::process( crc_hash, desc.RenderTarget[i].BlendEnable);
		crc::process( crc_hash, desc.RenderTarget[i].SrcBlend);
		crc::process( crc_hash, desc.RenderTarget[i].DestBlend);
		crc::process( crc_hash, desc.RenderTarget[i].BlendOp);
		crc::process( crc_hash, desc.RenderTarget[i].SrcBlendAlpha);
		crc::process( crc_hash, desc.RenderTarget[i].DestBlendAlpha);
		crc::process( crc_hash, desc.RenderTarget[i].BlendOpAlpha);
		crc::process( crc_hash, desc.RenderTarget[i].RenderTargetWriteMask);
	}

#endif
	return crc_hash.checksum();
}


u32 get_hash( const D3D_SAMPLER_DESC &desc )
{
	crc::processor	crc_hash;

	crc::process( crc_hash, desc.Filter);
	crc::process( crc_hash, desc.AddressU);
	crc::process( crc_hash, desc.AddressV);
	crc::process( crc_hash, desc.AddressW);
	crc::process( crc_hash, desc.MipLODBias);
//	Ignore anisotropy since it's set up automatically by the manager
//	crc::process( crc_hash, desc.MaxAnisotropy, sizeof(desc.MaxAnisotropy) );
	crc::process( crc_hash, desc.ComparisonFunc);
	crc::process( crc_hash, desc.BorderColor[0]);
	crc::process( crc_hash, desc.BorderColor[1]);
	crc::process( crc_hash, desc.BorderColor[2]);
	crc::process( crc_hash, desc.BorderColor[3]);
	crc::process( crc_hash, desc.MinLOD);
	crc::process( crc_hash, desc.MaxLOD);

	return crc_hash.checksum();
}

void check_validity(D3D_RASTERIZER_DESC &desc)
{
	UNREFERENCED_PARAMETER (desc);
}

void check_validity(D3D_DEPTH_STENCIL_DESC &desc)
{
	ASSERT( (desc.DepthEnable==0) || (desc.DepthEnable==1));
	ASSERT( (desc.StencilEnable==0) || (desc.StencilEnable==1));

	if (!desc.DepthEnable)
	{
		desc.DepthFunc = D3D_COMPARISON_NEVER;
	}

	if (!desc.StencilEnable)
	{
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;

		desc.FrontFace.StencilFailOp = D3D_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D_COMPARISON_ALWAYS;

		desc.BackFace.StencilFailOp = D3D_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D_COMPARISON_ALWAYS;
	}
}

#if USE_DX10
void check_validity(D3D_BLEND_DESC &desc)
{
	BOOL	bBlendEnable = FALSE;

	for ( int i=0; i<8; ++i)
	{
		ASSERT( (desc.BlendEnable[i]==0) || (desc.BlendEnable[i]==1));
		bBlendEnable |= desc.BlendEnable[i];
	}

	if (!bBlendEnable)
	{
		desc.SrcBlend = D3D_BLEND_ONE;
		desc.DestBlend = D3D_BLEND_ZERO;
		desc.BlendOp = D3D_BLEND_OP_ADD;
		desc.SrcBlendAlpha = D3D_BLEND_ONE;
		desc.DestBlendAlpha = D3D_BLEND_ZERO;
		desc.BlendOpAlpha = D3D_BLEND_OP_ADD;
	}
	else
	{
		switch(desc.SrcBlendAlpha)
		{
		case D3D_BLEND_SRC_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_SRC_ALPHA;
			break;
		case D3D_BLEND_INV_SRC_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_INV_SRC_ALPHA;
			break;
		case D3D_BLEND_DEST_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_DEST_ALPHA;
			break;
		case D3D_BLEND_INV_DEST_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_INV_DEST_ALPHA;
			break;
		case D3D_BLEND_SRC1_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_SRC1_ALPHA;
			break;
		case D3D_BLEND_INV_SRC1_COLOR:
			desc.SrcBlendAlpha = D3D_BLEND_INV_SRC1_ALPHA;
			break;
		}

		switch(desc.DestBlendAlpha)
		{
		case D3D_BLEND_SRC_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_SRC_ALPHA;
			break;
		case D3D_BLEND_INV_SRC_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_INV_SRC_ALPHA;
			break;
		case D3D_BLEND_DEST_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_DEST_ALPHA;
			break;
		case D3D_BLEND_INV_DEST_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_INV_DEST_ALPHA;
			break;
		case D3D_BLEND_SRC1_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_SRC1_ALPHA;
			break;
		case D3D_BLEND_INV_SRC1_COLOR:
			desc.DestBlendAlpha = D3D_BLEND_INV_SRC1_ALPHA;
			break;
		}
	}

}

#else

void check_validity(D3D_BLEND_DESC &desc)
{
	BOOL	bBlendEnable = FALSE;

	for ( int i=0; i<8; ++i)
	{
		ASSERT( (desc.RenderTarget[i].BlendEnable==0) || (desc.RenderTarget[i].BlendEnable==1));
		bBlendEnable |= desc.RenderTarget[i].BlendEnable;
	}

	for ( int i=0; i<8; ++i)
	{
		if (!bBlendEnable)
		{
			desc.RenderTarget[i].SrcBlend = D3D_BLEND_ONE;
			desc.RenderTarget[i].DestBlend = D3D_BLEND_ZERO;
			desc.RenderTarget[i].BlendOp = D3D_BLEND_OP_ADD;
			desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_ONE;
			desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_ZERO;
			desc.RenderTarget[i].BlendOpAlpha = D3D_BLEND_OP_ADD;
		}
		else
		{
			switch(desc.RenderTarget[i].SrcBlendAlpha)
			{
			case D3D_BLEND_SRC_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_SRC_ALPHA;
				break;
			case D3D_BLEND_INV_SRC_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_INV_SRC_ALPHA;
				break;
			case D3D_BLEND_DEST_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_DEST_ALPHA;
				break;
			case D3D_BLEND_INV_DEST_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_INV_DEST_ALPHA;
				break;
			case D3D_BLEND_SRC1_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_SRC1_ALPHA;
				break;
			case D3D_BLEND_INV_SRC1_COLOR:
				desc.RenderTarget[i].SrcBlendAlpha = D3D_BLEND_INV_SRC1_ALPHA;
				break;
			}

			switch(desc.RenderTarget[i].DestBlendAlpha)
			{
			case D3D_BLEND_SRC_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_SRC_ALPHA;
				break;
			case D3D_BLEND_INV_SRC_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_INV_SRC_ALPHA;
				break;
			case D3D_BLEND_DEST_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_DEST_ALPHA;
				break;
			case D3D_BLEND_INV_DEST_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_INV_DEST_ALPHA;
				break;
			case D3D_BLEND_SRC1_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_SRC1_ALPHA;
				break;
			case D3D_BLEND_INV_SRC1_COLOR:
				desc.RenderTarget[i].DestBlendAlpha = D3D_BLEND_INV_SRC1_ALPHA;
				break;
			}
		}
	}
}

#endif

void check_validity(D3D_SAMPLER_DESC &desc)
{
	if (	(desc.AddressU != D3D_TEXTURE_ADDRESS_BORDER)
		 &&	(desc.AddressV != D3D_TEXTURE_ADDRESS_BORDER)
		 &&	(desc.AddressW != D3D_TEXTURE_ADDRESS_BORDER))
	{
		for (int i=0; i<4; ++i)
		{
			desc.BorderColor[i] = 0.0f;
		}
	}

	if (	(desc.Filter != D3D_FILTER_ANISOTROPIC)
		 && (desc.Filter != D3D_FILTER_COMPARISON_ANISOTROPIC))
	{
		desc.MaxAnisotropy = 1;
	}
}

} // namespace state_utils

} // namespace render
} // namespace xray
