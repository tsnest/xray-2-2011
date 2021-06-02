////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SIMULATOR_STATES_H_INCLUDED
#define SIMULATOR_STATES_H_INCLUDED

namespace xray {
namespace render{


class simulator_states
{
public:
	void set_rs(u32 a, u32 b);
	void set_samp(u32 a, u32 b, u32 c);
	bool equal(simulator_states& other);
	void clear();
	//IDirect3DStateBlock9*	record();

#ifdef	USE_DX10
	//void	UpdateState( dx10State &state) const;
	//void	UpdateDesc( D3D10_RASTERIZER_DESC &desc ) const;
	//void	UpdateDesc( D3D10_DEPTH_STENCIL_DESC &desc ) const;
	//void	UpdateDesc( D3D10_BLEND_DESC &desc ) const;
	//void	UpdateDesc( D3D10_SAMPLER_DESC descArray[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT], bool SamplerUsed[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT], int iBaseSamplerIndex ) const;
#endif	//	USE_DX10

public:
	struct state
	{
		u32	type;	// 0=RS, 2=SAMP
		u32	v1, v2, v3;

		inline void set_rs(u32 a, u32 b)
		{
			type	= 0;
			v1	= a;
			v2	= b;
			v3	= 0;
		}

		void set_samp(u32 a, u32 b, u32 c)
		{
			type	= 2;
			v1	= a;
			v2	= b;
			v3	= c;
		}
	};

public:
	render::vector<state>	m_states;
};

} // namespace render
} // namespace xray

#endif // #ifndef SIMULATOR_STATES_H_INCLUDED