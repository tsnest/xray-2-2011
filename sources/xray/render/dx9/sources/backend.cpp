////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "backend.h"

namespace xray {
namespace render {


const D3DRENDERSTATETYPE backend::m_dx_render_states[rs_count] = {
	D3DRS_STENCILENABLE,
	D3DRS_STENCILFUNC,
	D3DRS_STENCILREF,
	D3DRS_STENCILMASK,
	D3DRS_STENCILWRITEMASK,
	D3DRS_STENCILFAIL,
	D3DRS_STENCILPASS,
	D3DRS_STENCILZFAIL,
	D3DRS_COLORWRITEENABLE,
	D3DRS_CULLMODE,
	D3DRS_ZENABLE,
	D3DRS_ALPHAREF,
};

void backend::on_device_create(ID3DDevice* device)
{
	m_device = device;
	m_device->AddRef();
	m_device->GetRenderTarget(0, &m_base_rt);
	m_device->GetDepthStencilSurface(&m_base_zb);
	m_states.resize(rs_count);

	vertex.create();
	index.create();
}

void backend::on_device_destroy()
{
	safe_release(m_base_rt);
	safe_release(m_base_zb);
	safe_release(m_device);
	m_device = 0;

	vertex.destroy();
	index.destroy();
}

void backend::flush_c_cache()
{
	ID3DDevice* device = hw_wrapper::get_ref().device();
	if (m_a_pixel.is_dirty())
	{
		constant_array::float_cache&	f_cache	= m_a_pixel.m_float_cache;

		if (f_cache.lo() <= 32) //. hack??????????????????????????????????????
		{		
			u32 count = f_cache.dirty_size();

			if (count)
			{
				count = (count>31) ? 31 : count;
				//PGO		(Msg("PGO:P_CONST:%d",count));
				R_CHK(device->SetPixelShaderConstantF(f_cache.lo(), (float*)f_cache.access_dirty(), count));
				f_cache.flush	();
			}
		}
	}

	if (m_a_vertex.is_dirty())
	{
		constant_array::float_cache&	f_cache	= m_a_vertex.m_float_cache;
		u32 count = f_cache.dirty_size();

		if (count)
		{
#ifdef DEBUG
			u32 nreg = hw_wrapper::get_ref().get_caps().geometry.registers;
			if (f_cache.hi() > nreg)
			{
				FATAL("Internal error setting VS-constants: overflow\nregs[%d],hi[%d]",	nreg, f_cache.hi());
			}
			//PGO		(Msg("PGO:V_CONST:%d",count));
#endif
			R_CHK(device->SetVertexShaderConstantF(f_cache.lo(), (float*)f_cache.access_dirty(), count));
		}

		f_cache.flush();
	}
}

} // namespace render 
} // namespace xray 
