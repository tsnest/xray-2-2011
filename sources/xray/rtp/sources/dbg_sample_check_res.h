////////////////////////////////////////////////////////////////////////////
//	Created		: 09.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DBG_SAMPLE_CHECK_RES_H_INCLUDED
#define DBG_SAMPLE_CHECK_RES_H_INCLUDED

#include "action_value.h"

namespace xray {
namespace rtp {



template< class action >
struct samples_res;
template< class action >
struct samples_res
{
	typedef	action														action_type;
	typedef	typename action::space_param_type							space_param_type;
	typedef	training_regression_tree< space_param_type >				regression_tree_type;
	typedef	typename regression_tree_type::training_sample_type			training_sample_type;
	typedef	action_value< regression_tree_type, space_param_type >		action_value_type;

				samples_res		( );
	inline bool compare			( const samples_res<action> &a )const;

	u32	  id;

	u32	  subset;
	float nu;
	float value;
				
	const action_type *a;
	const action_type *a_to;
	space_param_type param;		//  res->rnd //. add previous blending state to samples?
	space_param_type to_param;
	training_sample_type* p;
	float diff;
	float cur_sum_sq_residual;
	float sq_diff;
	float prev_sum_sq_residual;

	u32 rnd_seed;
	u32 rnd_count;

	float state_reward;
	float reward;

	float	max_v;
	float	transition_reward;
	float	core_ret;
	u32		blend_id;

	float	state_rew_a1;	
	float	state_rew_a2;	
	float	state_rew_res;



}; // struct samples_res

} // namespace rtp
} // namespace xray

#endif // #ifndef DBG_SAMPLE_CHECK_RES_H_INCLUDED