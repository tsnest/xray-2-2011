////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRAINING_VALUE_FUNCTION_H_INCLUDED
#define TRAINING_VALUE_FUNCTION_H_INCLUDED

#include "value_function.h"
#include "training_regression_tree.h"
#include "trajectory.h"

#include <xray/threading_mutex.h>

namespace xray {
namespace rtp {



struct update_action_sets_task_data;
template< class action >
struct generate_trajectory_result;
struct update_sets_result;

template< class action >
class training_value_function:
	public value_function< training_regression_tree< training_node< typename action::space_param_type::tree_space_param_type > >, action >
{
	typedef value_function< training_regression_tree< training_node< typename action::space_param_type::tree_space_param_type > >, action > super;

	using super::clear;
	using super::m_action_base;
	using super::m_regression_trees;

	using super::separate_dimensions_count;
	using super::discrete_dimension_size;

	using typename super::tree_space_param_type;
	using typename super::separate_reward_type;

	typedef	action																action_type;
	typedef	typename action::space_param_type									space_param_type;
	typedef training_node< typename space_param_type::tree_space_param_type >	training_node_type;
	typedef	training_regression_tree< training_node_type >						regression_tree_type;
	typedef	typename regression_tree_type::training_sample_type					training_sample_type;
	typedef	action_value< training_node_type, space_param_type >				action_value_type;

public:
					training_value_function	( const action_base<action_type>& ab );
		virtual		~training_value_function( );

public:
	void			learn				( );
	bool			samples_step		( );
	float			learn_step			( );
	void			learn_init			( );

	inline u32		steps_samples_count	( ) const { return m_steps_samples; }

public:

#ifndef MASTER_GOLD
	void		save				( xray::configs::lua_config_value cfg, bool training )const;
#endif // MASTER_GOLD

	void		load				( const xray::configs::binary_config_value &cfg );

#ifndef MASTER_GOLD
	void		save_training_sets	( xray::configs::lua_config_value cfg )const;
#endif // MASTER_GOLD

	void		load_training_sets	( const xray::configs::binary_config_value &cfg );

public:
	virtual	void	render			( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const;

private:
		float						generate_samples					( );
		void						generate_trajectory					( const action* start_action, u32 id, math::random32_with_count &r );
		void						generate_trajectories				( generate_trajectory_result< action > *result );
		void						generate_samples_singlethread		( generate_trajectory_result< action > *result );
		void						generate_samples_tasks				( generate_trajectory_result< action > *result );

		u32							generate_samples_iterations_count	( );
virtual	void						init_trees							( );
virtual	void						clear								( );

		void						build_trees							( );
		void						recalculate_trees					( );

		void						update_action_set			( u32 action_id, update_action_sets_task_data* res );
		void						update_action_sets			( update_action_sets_task_data* res );
		float						update_sets					( );

		void						prune						( );
		void						prune_build_tree			( u32 min_samples, u32 id_action, u32 separate_dimension_value,  u32 discrete_dimension_value );
		void						randomize_sets				( );
		float						prune_residual				( );
		//float						prune_residual				( u32 id_action );
		float						prune_sum_residual			( u32 id_action, u32 discrete_dim, u32 &number );
		u32							samples_total				( )const;
		void						clear_training_sets 		( );
		void						clear_trajectories  		( );
		void						add_trajectories_to_sets	( );

		u32							tasks_count					( )const;

		u32							tasks_count_generate_samples( )const;
		u32							tasks_count_update_sets		( )const;

		void						fill_update_action_sets_tasks	( update_action_sets_task_data * data, u32 tasks_num, u32 samples_per_task, update_sets_result *result, u32 result_size, u32 result_granularity )const; 
		void						fill_generate_samples_tasks		( generate_trajectory_result< action > *result, u32 trajectories_cnt, buffer_vector<u32> &rand_actions, u32* random_seeds_buffer )const;

		float						update_sets_tasks			( );
		float						update_sets_single_thread	( );

private:
	rtp::vector< training_set< space_param_type, training_sample_type > >	m_training_sets;

	rtp::vector< trajectory<action_type> >				m_trajectories;
	//math::random32 rnd;

private:
threading::mutex		m_add_samples_mutex;
u32						m_steps_trees;
u32						m_steps_samples;

u32						m_prune_steps;
///////////



/////////////
}; // class training_value_function

} // namespace rtp
} // namespace xray

#include "training_value_function_inline.h"

#endif // #ifndef TRAINING_VALUE_FUNCTION_H_INCLUDED