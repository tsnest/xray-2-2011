////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SAMPLER_STATE_SIMULATOR_H_INCLUDED
#define SAMPLER_STATE_SIMULATOR_H_INCLUDED

namespace xray {
namespace render {

class effect_compiler;

class sampler_state_descriptor 
{
	friend class effect_compiler;
	friend class resource_manager;
	sampler_state_descriptor	( effect_compiler& compiler):m_effect_compiler		( &compiler)	{}
	
public:

	inline 	sampler_state_descriptor () { reset(); } 
		
	inline 	sampler_state_descriptor &	reset				();

	inline 	sampler_state_descriptor &	set					( D3D_FILTER filter, D3D_TEXTURE_ADDRESS_MODE addr_mode = D3D_TEXTURE_ADDRESS_CLAMP);
	inline 	sampler_state_descriptor &	set_filter			( D3D_FILTER filter);
	inline 	sampler_state_descriptor &	set_address_mode	( D3D_TEXTURE_ADDRESS_MODE addr_u, D3D_TEXTURE_ADDRESS_MODE addr_v, D3D_TEXTURE_ADDRESS_MODE addr_w);
	inline 	sampler_state_descriptor &	set_mip				( float lod_bias = 0.f, float min_lod = 0.f, float max_lod = math::float_max);
	inline 	sampler_state_descriptor &	set_max_anisotropy	( u32 max_anisotropy);
	inline 	sampler_state_descriptor &	set_comparison_function		( D3D_COMPARISON_FUNC func);
	inline 	sampler_state_descriptor &	set_border_color	( float4 color);


	// This from the effect_compiler usage only
	effect_compiler &	end_sampler	();
private:
	effect_compiler *		m_effect_compiler;
	D3D_SAMPLER_DESC		m_desc;
	bool					m_updated;
}; // class sampler_state_descriptor

} // namespace render
} // namespace xray

#include <xray/render/core/sampler_state_descriptor_inline.h>

#endif // #ifndef SAMPLER_STATE_SIMULATOR_H_INCLUDED