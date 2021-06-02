////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TRAJECTORY_H_INCLUDED
#define TRAJECTORY_H_INCLUDED

#include "training_set.h"

namespace xray {

namespace render {
namespace debug {
	class renderer;
} // namespace debug
} // namespace render


namespace rtp {
extern u32 g_max_trajectory_length;

template< class action >
class trajectory {

private:
typedef	action																action_type;
typedef	typename action::space_param_type									space_param_type;
typedef	typename space_param_type::tree_space_param_type					tree_space_param_type;
typedef	training_regression_tree< training_node< tree_space_param_type > >	regression_tree_type;

static const u32	separate_dimensions_count	=	space_param_type::separate_dimensions_count;
static const u32	discrete_dimension_size		=	space_param_type::discrete_dimension_size;

typedef	typename	space_param_type::separate_reward_type	separate_reward_type;

public:
typedef	typename regression_tree_type::training_sample_type	training_sample_type;

private:

struct	item_type
{
	inline item_type( );
	inline item_type( action_type const * a, space_param_type const& par, separate_reward_type const& rew );
	inline item_type( item_type const &other );

	inline item_type&	operator = ( item_type const &other )
	{ 
		const_action = other.const_action;
		param		 = other.param;
		reward		 = other.reward;
		 return *this; 
		//new( this )item_type( other ); return *this; 
	} 

public:
	action_type	const*		const_action;
	space_param_type		param;
	separate_reward_type	reward;
	
};

public:
			trajectory		( u32 self_id );

	//void	add				( const training_sample_type& sample, const action* a );
	void	add				( const space_param_type& sample, separate_reward_type const& rew, const action* a );

	void	add_to_sets		( vector< training_set< space_param_type, training_sample_type > > &sets );
	void	clear			( );
	u32		size			( ) const { return m_count;  }
	void	render			( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer, u32 trajectories_count ) const;
	void	set_is_in_target( bool v ){ m_in_target = v;}

private:
	vector< item_type > m_samples;
	u32					m_count;
	u32					m_id;
	bool				m_in_target;

}; // class trajectory

} // namespace rtp
} // namespace xray

#include "trajectory_inline.h"

#endif // #ifndef TRAJECTORY_H_INCLUDED