////////////////////////////////////////////////////////////////////////////
//	Created		: 27.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "res_rt.h"

namespace xray {
namespace render{ 

u32		btw_lowest_bitmask(u32 x) {return x & ~(x-1);}
bool	btw_is_pow2(u32 v) {return btw_lowest_bitmask(v) == v;}

res_rt::res_rt()
{
	m_surface = NULL;
	m_rt	  = NULL;
	m_width	  = 0;
	m_height  = 0;
	m_format  = D3DFMT_UNKNOWN;
}

res_rt::~res_rt()
{
	destroy();

	// release external reference
	resource_manager::get_ref().delete_rt(this);
}

void res_rt::create(LPCSTR name, u32 w, u32 h,	D3DFORMAT fmt)
{
	if (m_surface)
		return;

	ID3DDevice* device = hw_wrapper::get_ref().device();

	R_ASSERT(device && name && name[0] && w && h);
	m_order	= timing::get_clocks();

	HRESULT		hr;

	m_width = w;
	m_height = h;
	m_format	= fmt;

	// Get caps
	D3DCAPS9	caps;
	xray::memory::zero( &caps, sizeof(caps) );

	R_CHK(device->GetDeviceCaps(&caps));

	// Pow2
	if (!btw_is_pow2(w) || !btw_is_pow2(h))
	{
		if (!hw_wrapper::get_ref().get_caps().raster.b_non_pow2)	return;
	}

	// Check width-and-height of render target surface
	if (w > caps.MaxTextureWidth)
		return;

	if (h > caps.MaxTextureHeight)
		return;

	// Select usage
	u32 usage	= 0;
	if (D3DFMT_D24X8 == fmt)
		usage = D3DUSAGE_DEPTHSTENCIL;
	else if (D3DFMT_D24S8 == fmt)
		usage = D3DUSAGE_DEPTHSTENCIL;
	else if (D3DFMT_D15S1 == fmt)
		usage = D3DUSAGE_DEPTHSTENCIL;
	else if (D3DFMT_D16 == fmt)
		usage = D3DUSAGE_DEPTHSTENCIL;
	else if (D3DFMT_D16_LOCKABLE == fmt)
		usage = D3DUSAGE_DEPTHSTENCIL;
	else if ((D3DFORMAT)MAKEFOURCC('D','F','2','4') == fmt)
		usage = D3DUSAGE_DEPTHSTENCIL;
	else
		usage = D3DUSAGE_RENDERTARGET;

	// Validate render-target usage
	hr = hw_wrapper::get_ref().support(fmt, D3DRTYPE_TEXTURE, usage);

	if (FAILED(hr))
		return;

	// Try to create texture/surface
	resource_manager::get_ref().evict();
	hr = device->CreateTexture(w, h, 1, usage, fmt, D3DPOOL_DEFAULT, &m_surface, NULL);
	
	if (FAILED(hr) || (0==m_surface))
		return;

#ifdef DEBUG
	LOG_INFO("* created RT(%s), %dx%d", name, w, h);
#endif // DEBUG

	R_CHK(m_surface->GetSurfaceLevel(0,&m_rt));
	m_texture = resource_manager::get_ref().create_texture(name);
	m_texture->set_surface(m_surface);
}

void res_rt::destroy()
{
	if (m_texture.c_ptr())
	{
		m_texture->set_surface(0);
		m_texture = NULL;
	}

	safe_release(m_rt);
	log_ref_count(get_name(), m_surface);
	safe_release(m_surface);
}

void res_rt::reset_begin()
{
	destroy();
}

void res_rt::reset_end()
{
	create(get_name(), m_width, m_height, m_format);
}

}// namespace render
}// namespace xray
