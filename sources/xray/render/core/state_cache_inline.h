///////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STATE_CACHE_INLINE_H_INCLUDED
#define STATE_CACHE_INLINE_H_INCLUDED

namespace xray {
namespace render {

namespace state_utils {
	bool	operator==( const D3D_RASTERIZER_DESC &desc1, const D3D_RASTERIZER_DESC &desc2);
	bool	operator==( const D3D_DEPTH_STENCIL_DESC &desc1, const D3D_DEPTH_STENCIL_DESC &desc2);
	bool	operator==( const D3D_BLEND_DESC &desc1, const D3D_BLEND_DESC &desc2);
	bool	operator==( const D3D_SAMPLER_DESC &desc1, const D3D_SAMPLER_DESC &desc2);

	u32		get_hash( const D3D_RASTERIZER_DESC &desc);
	u32		get_hash( const D3D_DEPTH_STENCIL_DESC &desc);
	u32		get_hash( const D3D_BLEND_DESC &desc);
	u32		get_hash( const D3D_SAMPLER_DESC &desc);
} // namespace state_utils

using state_utils::operator==;

template <class IDeviceState, class state_desc>
IDeviceState* state_cache<IDeviceState, state_desc>::get_state( state_desc const & desc )
{
	IDeviceState*	res;

	//state_utils::check_validity(desc);

	u32 crc = state_utils::get_hash(desc);

	res = find( desc, crc);

	if (!res)
	{
		state_record rec;
		rec.crc = crc;
		create_state( desc, &rec.state);
		res = rec.state;
		states.push_back(rec);
	}

	return res;
}

template <class IDeviceState, class state_desc>
IDeviceState* state_cache<IDeviceState, state_desc>::find( const state_desc& desc, u32 CRC )
{

	// --Porting to DX10_
	// Here maybe needed faster search
    u32 res = 0xffffffff;
	for (u32 i=0; i<states.size(); ++i)
	{
		if (states[i].crc== CRC)
		{
			state_desc	desc_candidate;
			states[i].state->GetDesc(&desc_candidate);
			if (desc_candidate==desc)
			{
                res = i;
				break;
			}
			//else
			//{
			//	ASSERT(0);
			//}
		}
	}

	if (res!=0xffffffff)
		return states[res].state;
	else
		return NULL;
    /*
	if (i!=states.size())
		return states[i].m_pState;
	else
		return NULL;
        */
}

} // namespace render
} // namespace xray

#endif // #ifndef STATE_CACHE_INLINE_H_INCLUDED
