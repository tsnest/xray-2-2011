////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_SAMPLER_STATE_H_INCLUDED
#define RES_SAMPLER_STATE_H_INCLUDED

namespace xray {
namespace render {

typedef ID3DSamplerState			res_sampler_state;

// class res_sampler_state :public res_flagged
// {
// 
// 	friend class resource_manager;
// 	res_sampler_state () : hw_state(NULL)	{	}
// 
// public:
// 	void apply();
// 
// private:
// 
// 	ID3DSamplerState* hw_state;
// 
// }; // class res_sampler_state

typedef res_sampler_state *			ref_sampler_state;
typedef res_sampler_state const *	ref_sampler_state_const;

//typedef intrusive_ptr<res_sampler, resource_intrusive_base, threading::single_threading_policy> ref_sampler;

} // namespace render
} // namespace xray

#endif // #ifndef RES_SAMPLER_STATE_H_INCLUDED