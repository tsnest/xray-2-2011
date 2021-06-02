////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "constant_cache.h"

namespace xray {
namespace render{

void constant_cache::flush_cache()
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
