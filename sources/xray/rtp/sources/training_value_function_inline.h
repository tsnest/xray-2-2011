////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include <xray/tasks_system.h>
#include <xray/tasks_thread_tls.h>

namespace xray {

namespace math {
	void on_thread_spawn();
} // namespace math

namespace rtp {

static const u32 samples_number_pruning_threshhold	= 120000;
static const u32 num_prune_steps					= 0; //2;

//static const u32 num_trajectories					= 30;
extern u32 g_num_trajectories;

extern bool b_learn_trees_frozen;
extern bool	b_learn_prune;

extern	command_line::key	rtp_learn_disable_tasks;

struct task_range
{
	inline task_range(  u32 from_, u32 to_, const math::random32_with_count	&rnd_ ):
	rnd( rnd_ ),
	from( from_ ),
	to( to_ ),
	current( 0 )
	{
	}
	
	math::random32_with_count	rnd;
	u32							from, to, current;

}; // struct task_range

struct update_sets_result;
struct  update_action_sets_task_data:
	public task_range
{
	inline update_action_sets_task_data( u32 from, u32 to, const math::random32_with_count &rnd_, update_sets_result *res, u32 result_granularity_ );

	update_sets_result *result;
	u32					result_granularity;
}; //struct  update_action_sets_task_data

inline update_action_sets_task_data::update_action_sets_task_data( u32 from_, u32 to_, const math::random32_with_count &rnd_, update_sets_result *res, u32 result_granularity_ ):
	task_range( from_, to_, rnd_ ),
	result( res ),
	result_granularity( result_granularity_ )
{
}

template< class action >
struct generate_trajectory_result:
	public task_range
{
	
	generate_trajectory_result( const buffer_vector<u32> *rand_actions, u32  rand_actions_start, u32 from, u32 to, const math::random32_with_count &rnd, u32* random_seeds_buffer ):
		task_range				( from, to, rnd ),
		m_rand_actions			( rand_actions ),
		m_rand_actions_start	( rand_actions_start ),
		m_random_seeds_buffer	( random_seeds_buffer )
		{}


	const buffer_vector<u32>	*m_rand_actions;
	u32							m_rand_actions_start;
	u32							*m_random_seeds_buffer;

}; // struct generate_trajectory_result

template< class action >
training_value_function< action >::training_value_function( const action_base<action_type>& ab ): 
	super( ab ),
	m_steps_trees( 0 ),
	m_steps_samples( 0 ),
	m_prune_steps( 0 )
{
	
}

template< class action >
training_value_function< action >::~training_value_function( )
{
	clear();
}

template< class action >
void training_value_function< action >::init_trees( )
{
	super::clear();
	clear_training_sets();
	const u32 sz = m_action_base.size();
	m_regression_trees.resize( sz, 0 );
	m_training_sets.resize( sz,  training_set<space_param_type, training_sample_type>() );
	for( u32 i = 0; i<sz; ++i )
		m_regression_trees[i] = NEW( action_value_type )( m_training_sets[i] );
		//m_regression_trees.push_back( action_value_type( m_training_sets[i]) );
		
	u32 num_trajectories = g_num_trajectories;
	for( u32 i = 0; i < num_trajectories * sz ; ++i )
		m_trajectories.push_back( trajectory<action_type>( i ) );
	
	m_steps_trees	= 0;
	m_steps_samples	= 0;
	m_prune_steps	= 0;

}

template< class action >
void training_value_function< action >::clear				( )
{
	super::clear( );
	clear_training_sets();
	clear_trajectories  ( );
}

template< class action >
bool	training_value_function<action>::samples_step( )
{
	m_add_samples_mutex.lock();
	
	bool samples = m_steps_samples <= m_steps_trees; 



	if( samples )
	{
		if( !b_learn_trees_frozen )
			generate_samples();
		++m_steps_samples;
	}

	m_add_samples_mutex.unlock();

	return samples;
}

template< class action >
void training_value_function<action>::build_trees( )
{
	
	m_add_samples_mutex.lock();
	
	bool build = m_steps_trees < m_steps_samples ; 

	typename vector< training_set< space_param_type, training_sample_type > >::iterator i = m_training_sets.begin(),
												   e = m_training_sets.end();
	
	for( ; i != e ; ++i )
		( *i ).synchronize();
		
	m_add_samples_mutex.unlock();


	if( !build )
		return;

	const u32 sz = m_regression_trees.size();

	buffer_vector< action_value_type* > building_regression_trees( ALLOCA( sz * sizeof( action_value_type* ) ), sz );

	building_regression_trees.resize( sz, 0 );

	for( u32 i = 0; i<sz; ++i )
	{
		//DELETE( m_regression_trees[i] );
		training_set< space_param_type, training_sample_type >& set = m_training_sets[i];
		set.randomize();
		//m_building_regression_trees[i] = NEW(regression_tree_type)( m_training_sets[i] );
		action_value_type* const value = NEW(action_value_type)( set );
		building_regression_trees[i] = value;
		value->build();
	}

	m_add_samples_mutex.lock();

	for( u32 i = 0; i<sz; ++i )
	{
		DELETE( m_regression_trees[i] );
		
		m_regression_trees[i]=building_regression_trees[i];
		building_regression_trees[i] = 0;
	}

	++m_steps_trees;

	m_add_samples_mutex.unlock();


}

template< class action >
void training_value_function< action >::recalculate_trees( )
{
	const u32 sz = m_regression_trees.size();
	
	for( u32 i = 0; i<sz; ++i )
		m_regression_trees[i]->recalculate();

}

template< class action >
u32		training_value_function< action >::generate_samples_iterations_count	( )
{
	return
		num_trajectories*
			m_action_base.size()*
				max_trajectory_length;
}

inline void set_random_seeds_buffer( u32* random_seeds_buffer, u32 cnt )
{
	ASSERT( random_seeds_buffer );
	for( u32 i = 0; i < cnt ; ++i )
		random_seeds_buffer[i] = rtp_world::random().random( u32(-1) );
}

template< class action >
inline void		training_value_function< action >::fill_generate_samples_tasks	( generate_trajectory_result< action > *result, u32 trajectories_cnt, buffer_vector<u32> &rand_actions, u32* random_seeds_buffer  )const
{
	
	const u32 tasks_num			= tasks_count_generate_samples( );

	
	u32 samples_per_task = trajectories_cnt / tasks_num;
	
	
	math::random32_with_count r = rtp_world::random();

	set_random_seeds_buffer( random_seeds_buffer, trajectories_cnt );

	set_random_buffer( rand_actions, r );


	for( u32 i = 0; i < tasks_num; ++i )
	{
		u32 from	= 	i * samples_per_task;
		u32 to		= 	( i + 1 ) * samples_per_task;
			
		math::random32_with_count rnd( r.random( u32 (-1) ) );

		generate_trajectory_result<action> res( 			&rand_actions,
															0,												
															from,
															to,
															rnd,
															random_seeds_buffer
														 );
		 result[ i ] = res;
	}

	result[ tasks_num-1 ].to = trajectories_cnt;
}


template< class action >
void	training_value_function< action >::generate_trajectories( generate_trajectory_result< action > *result )
{
	ASSERT( result );
	
	const u32 actions_num		= m_action_base.size();

	for( u32 i = result->from; i < result->to ; ++i )
	{
		
		u32 action_id = (*result->m_rand_actions)[ ( i + result->m_rand_actions_start ) % actions_num ];

		const action* current = m_action_base.get( action_id );
		
		 math::random32_with_count rnd( result->m_random_seeds_buffer[ i ] );
		generate_trajectory( current, i, rnd );	
	}

}

template< class action >
void	training_value_function< action >::generate_trajectory( const action* start_action, u32 id, math::random32_with_count &rand )
{
	space_param_type start; start.set_random( rand );
	space_param_type pcurrent = start;

	//
	pcurrent.training_data().previos_action		= start_action;
	pcurrent.training_data().previous_blend_id	= 0;
	//

	u32						current_blend_sumple	= 0; //rnd.random(max_samples);
	const action			*current				= start_action;
	trajectory<action_type> &current_trajectory		= m_trajectories[ id ];
	
	u32 max_trajectory_length = g_max_trajectory_length;
	for( u32 i = 0; i < max_trajectory_length; ++i )
	{
		separate_reward_type r;
		//float v = 0; 
		space_param_type pnext; // =  pcurrent;
		u32 blend(u32(-1));
		//const action *next = pi( *current, current_blend_sumple, pcurrent, blend, pnext, r, v, rand );// r = R(a,a') + discount * V(s') ; v = V( s') ; s' = (next,pnext)
		const action *next = pi( *current, current_blend_sumple, pcurrent, blend, pnext, r, rand );// r = R(a,a') + discount * V(s') ; v = V( s') ; s' = (next,pnext)
		ASSERT( r.valid() );
		//ASSERT( math::valid(v) );
		pnext.discrete_value	( )	= next->discrete_param_value();

		separate_reward_type value = r + state_reward( *current, current_blend_sumple,  pcurrent );
		//
		pcurrent.blend_id() = current_blend_sumple;
		//
		ASSERT( value.valid() );

		if( !in_area( pcurrent )  )
		{
			if( current->remove_out_of_area_trajectory() )
					current_trajectory.clear();

			break;
		}
		current_trajectory.add(  pcurrent, value , current );	

		current_trajectory.set_is_in_target( true );

		if(  current->target( pcurrent ) )
			break;

		current_trajectory.set_is_in_target( false );

		pnext.training_data().previos_action = current;
		pnext.training_data().previous_blend_id	= current_blend_sumple;

		current				 = next;
		
		current_blend_sumple = blend;
		pcurrent			 = pnext;

	}
}

template< class action >
void	training_value_function< action >::generate_samples_singlethread( generate_trajectory_result< action > *result  )
{
		const u32 tasks_num		= tasks_count_generate_samples( );

		for( u32 i = 0; i < tasks_num; ++i )
			generate_trajectories( result + i );
}

template< class action >
u32	training_value_function< action >::tasks_count_generate_samples		( )const
{

	u32 count				= tasks_count( );

	const u32 actions_num	= m_action_base.size();
	u32 num_trajectories	= g_num_trajectories;
	u32 trajectories_cnt	= actions_num * num_trajectories;

	if ( count > trajectories_cnt )
		count				= trajectories_cnt;
		

	return						count;

}


static tasks::task_type *	s_task_types []	=	
				{	tasks::create_new_task_type("update_action_sets_task", 0),
					tasks::create_new_task_type("generate_samples_task", 0),
								};

template< class action >
void	training_value_function< action >::generate_samples_tasks( generate_trajectory_result< action > *result  )
{
	const u32 tasks_cnt		= tasks_count_generate_samples( );

	for(u32 j= 0; j < tasks_cnt; ++j )
		tasks::spawn_task		( boost::bind( &training_value_function< action >::generate_trajectories, this, result + j ), s_task_types[1] );

	tasks::wait_for_all_children			( );
			
}


template< class action >
float	training_value_function< action >::generate_samples( )
{
	
	
	clear_trajectories			( );

	const u32 actions_num				= m_action_base.size();

	buffer_vector<u32> rnd_actions( ALLOCA( sizeof(32) * actions_num ), actions_num, actions_num );
	
	u32 num_trajectories = g_num_trajectories;
	u32 trajectories_cnt = actions_num * num_trajectories;
	
	u32*	random_seeds_buffer ( (u32*) ALLOCA( sizeof(u32) * trajectories_cnt ) );

	const u32 tasks_num			= tasks_count_generate_samples( );

	generate_trajectory_result< action > *result = ( generate_trajectory_result< action > * ) ALLOCA( sizeof(generate_trajectory_result< action >) * tasks_num ) ;
	

	fill_generate_samples_tasks( result, trajectories_cnt, rnd_actions, random_seeds_buffer );
	
	
	if( rtp_learn_disable_tasks )
		generate_samples_singlethread( result );
	else
		generate_samples_tasks( result );
	

	add_trajectories_to_sets();
	return 0;
}

template< class action >
void	training_value_function< action >::render		( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const
{
	u32 const count = m_trajectories.size();
	typename rtp::vector< trajectory<action_type> >::const_iterator i = m_trajectories.begin(), e = m_trajectories.end();
	for ( ; i != e ; ++i )
		(*i).render( scene, renderer, count );
}

template< class action >
void	training_value_function< action >::clear_trajectories  ( )
{
	typename rtp::vector< trajectory<action_type> >::iterator i = m_trajectories.begin(), e = m_trajectories.end();
	for ( ; i != e ; ++i )
		(*i).clear();
}


template< class action >
void	training_value_function< action >::add_trajectories_to_sets  ( )
{
	typename rtp::vector< trajectory<action_type> >::iterator i = m_trajectories.begin(), e = m_trajectories.end();
	for ( ; i != e ; ++i )
		(*i).add_to_sets( m_training_sets );
}

template< class action >
void training_value_function< action >::learn( )
{
	init_trees();
	for( u32 k = 0; k < 1000000; ++k )
		learn_step( );
}

template< class action >
void training_value_function<action>::learn_init( )
{
	if( m_regression_trees.empty() )
		init_trees();
}

template< class action >
u32	training_value_function< action >::samples_total( )const
{
	const u32 size = m_action_base.size();
	u32 total_samples = 0;
	for( u32 j= 0; j < size; ++j )
		total_samples += m_training_sets[j].samples_number();//m_training_sets[j].samples().size();
	return total_samples;

}


template< class action >
void training_value_function< action >::clear_training_sets()
{
//	const u32 size = m_training_sets.size();
//	for(u32 j= 0; j < size; ++j )
//		m_training_sets[j].samples().clear();
	m_training_sets.clear();
}


template< class action >
void training_value_function< action >::update_action_set( u32 action_id, update_action_sets_task_data* res )
{
	
			ASSERT( res );		

			const action &a = *m_action_base.get( action_id );
			training_set< space_param_type, training_sample_type > &set = m_training_sets[ a.id() ];

			//const u32 num_subsets = m_training_sets[ action_id ].subsets_number();	

			for( u32 k=0; k < discrete_dimension_size; ++k )
			{
				//rtp::vector< typename regression_tree_type::training_sample_type >& samples = m_training_sets[ a.id() ].subset( k ).samples();
				
				//typename rtp::vector< typename regression_tree_type::training_sample_type >::iterator i = samples.begin(), e  = samples.end();
				u32 const samples_count = set.get_samples_count( k );

				for( u32 i = 0; i < samples_count; ++i )
				{
					
					if( res->current < res->from )
					{
						++(res->current);
						continue;

					} else if( res->current >= res->to )
						return ;
					
					update_sets_result &r = res->result[ res->current / res->result_granularity ];

					//(*i).second = nu( a, 0, (*i).first, r.rnd );
					space_param_type param = set.get_param( i, k );
					separate_reward_type reward = nu( a, param.blend_id(), param, r.rnd );
					
					set.set_value( i, k, reward );

					//float diff = (*i).second -  value( a, (*i).first ) ;
					separate_reward_type diff = reward -  value( a, param ) ;

					//float sq_diff = diff * diff ;
					float sq_diff = diff.square_sum_reward( ) ;
					if( param.in_range() )
						r.sum += sq_diff;

					++(res->current);


				}
			}

}

template< class action >
void training_value_function< action >::update_action_sets(  update_action_sets_task_data* res )
{
		ASSERT( res );
		const u32 size = m_action_base.size();		
		for(u32 j= 0; j < size; ++j )
				update_action_set( j, res );
}


struct update_sets_result
{
	update_sets_result(	):
			rnd					( rtp_world::random().random( u32(-1) ) ),
			sum					( 0 )
			{}
		
			math::random32_with_count	rnd;
			float						sum;
			
}; // struct update_sets_result


inline float sum_result( update_sets_result * r, u32 size )
{
	float ret = 0.f;
	for( u32 i = 0; i < size; ++i )
		ret+= r[i].sum;
	
	return ret;
}

//const static  u32 s_num_tasks = 50;
extern command_line::key g_learn_task_count;

template< class action >
inline u32 training_value_function< action >::tasks_count( )const
{
	static u32 s_tasks_count	= 0;
	if ( !s_tasks_count ) {
		if ( !g_learn_task_count.is_set_as_number( &s_tasks_count ) )
			s_tasks_count		= 128;
	}

	return s_tasks_count;
}

template< class action >
u32	training_value_function< action >::tasks_count_update_sets( )const
{
	u32 count				= tasks_count( );

	const u32 total_samples		= samples_total();

	if ( count > total_samples )
		count				= total_samples;
		

	return						count;
}


//static const min_granularity = 10;

static const u32 max_result = 50000;

template< class action >
inline void training_value_function< action >::fill_update_action_sets_tasks( update_action_sets_task_data * data, u32 tasks_num, u32 samples_per_task, update_sets_result *result, u32 result_size, u32 result_granularity )const
{
	

	for( u32 i = 0; i < result_size; ++i )
		result[i] = update_sets_result( );
	
	
	
	math::random32_with_count r =math::random32_with_count();
	for( u32 i = 0; i < tasks_num; ++i )
			data[ i ] = update_action_sets_task_data(
							i * samples_per_task, 
					( i + 1 ) * samples_per_task, 
					math::random32_with_count( r.random( u32 (-1) ) ),
					result,
					result_granularity
				);

	const	u32 total_samples		= samples_total();

	data[ tasks_num-1 ].to			= total_samples;

}

template< class action >
float	training_value_function< action >::update_sets_tasks( )
{
	////
	
	const	u32 total_samples		= samples_total();
	const	u32 result_size			= math::min( total_samples, max_result );
	const	u32 tasks_num			= math::min( result_size, tasks_count_update_sets( ) );

	const	u32 result_granularity	= total_samples / result_size + ( ( total_samples % result_size ) == 0 ? 0 : 1 );
	const	u32 samples_per_task	= ( result_size / tasks_num ) * result_granularity;

	////

	update_action_sets_task_data *data		= ( update_action_sets_task_data* ) ALLOCA( sizeof( update_action_sets_task_data ) * tasks_num );
	update_sets_result			 *result	= ( update_sets_result* ) ALLOCA( sizeof( update_sets_result ) * result_size );
	

	fill_update_action_sets_tasks( data, tasks_num, samples_per_task, result, result_size, result_granularity  );

	////

	
	for(u32 j= 0; j < tasks_num; ++j )
		tasks::spawn_task		( boost::bind( &training_value_function< action >::update_action_sets, this,  data + j ), s_task_types[0] );

//
	samples_step();
//
	tasks::wait_for_all_children			( );

	float	sum =  sum_result ( result, result_size ) ;
	 
	return	sum;
}




template< class action >
float	training_value_function< action >::update_sets_single_thread ( )
{
	
	////
	
	const	u32 total_samples		= samples_total();
	const	u32 result_size			= math::min( total_samples, max_result );
	const	u32 tasks_num			= math::min( result_size, tasks_count_update_sets( ) );
	const	u32 result_granularity	= total_samples / result_size + ( ( total_samples % result_size ) == 0 ? 0 : 1 );

	const	u32 samples_per_task	= ( result_size / tasks_num ) * result_granularity;

	////

	update_action_sets_task_data	*data	= ( update_action_sets_task_data* ) ALLOCA( sizeof( update_action_sets_task_data ) * tasks_num );
	update_sets_result				*result	= ( update_sets_result* ) ALLOCA( sizeof( update_sets_result ) * result_size );
	
	fill_update_action_sets_tasks( data, tasks_num, samples_per_task, result, result_size, result_granularity  );

	for(u32 j= 0; j < tasks_num; ++j )
			update_action_sets( data + j );
	
	float sum =  sum_result ( result, result_size ) ;
	 
	return sum;
}

template< class action >
float	training_value_function< action >::update_sets( )
{

	
	u32 total_samples = samples_total();
	
	if( total_samples == 0 )
		return math::float_max;

	float sum = 0;
	
	if( rtp_learn_disable_tasks )
		sum = update_sets_single_thread( );
	else
		sum = update_sets_tasks( );


	return math::sqrt( sum/total_samples  );
}

template<class action>
void training_value_function< action >::randomize_sets( )
{
	const u32 num_actions = m_action_base.size	( );
	for( u32 id_action = 0; id_action <  num_actions; ++id_action )
	{
		m_training_sets[ id_action ].randomize();
	}
}

template<class action>
void training_value_function< action >::prune_build_tree( u32 min_samples, u32 id_action, u32 separate_dimension_value,  u32 discrete_dimension_value )
{

	{
		training_sub_set<training_sample_type>		&sub_set			= m_training_sets[id_action].subset( separate_dimension_value, discrete_dimension_value );
		regression_tree_type*						&tree				= m_regression_trees[id_action]->tree(separate_dimension_value, discrete_dimension_value );
		
		const u32 sz = sub_set.samples().size();
		ASSERT( sz >= 3 );
		const u32 sz_div_3  = u32( sz / 3 );
		const u32 sz_2d3	= sz - sz_div_3;

		buffer_vector<const training_sample_type*>	smaller_set(  ALLOCA( sizeof( training_sample_type ) * sz_2d3 ) , sz_2d3 );
		//set.randomize();
		for( u32 i = 0; i < sz_2d3; ++i )
			smaller_set.push_back( &sub_set.samples()[i] );
		DELETE(tree);
		tree = NEW(regression_tree_type)( min_samples, smaller_set );
		tree->build();
	}
}

template< class action >
void training_value_function< action >::prune( )
{
	//b_prune =true;
	randomize_sets( );

	const u32 num_actions = m_action_base.size( );
	for( u32 id_action = 0; id_action <  num_actions; ++id_action )
	{
		//const u32 trees_count = space_param_type::trees_count();
			//space_param_type::separate_dimensions_count *
			//space_param_type::tree_space_param_type::discrete_dimension_size;
		for( u32 s_dim = 0; s_dim <	separate_dimensions_count; ++s_dim )
			for( u32 d_dim = 0; d_dim < discrete_dimension_size; ++d_dim )
			{
				float min_residual = math::infinity;
				u32 min_samples_best = u32(-1);
				for( u32 min_samples = 1; min_samples <= regression_tree_type::node_type::min_samples_max; ++min_samples )
				{
					prune_build_tree( min_samples, id_action, s_dim, d_dim  );
					float res = prune_residual();//prune_residual(id_action);//prune_residual();
					if( res <= min_residual )
					{
						min_residual		= res;
						min_samples_best	= min_samples;
					}
				}
				training_set<space_param_type, training_sample_type>		&set	= m_training_sets[id_action];
				set.subset( s_dim, d_dim ).min_in_leaf_samples( ) = min_samples_best;
				LOG_DEBUG( "rtp::prune action :%d - min_samples: %d", id_action, min_samples_best );
			}

	}
	clear_training_sets();
	//b_prune =false;
}

template<class action>	
float	training_value_function< action >::prune_sum_residual( u32 id_action, u32 d_dim, u32 &number )
{
		//training_sub_set<training_sample_type>		&set	= m_training_sets[id_action].subset( s_dim, d_dim );
		//regression_tree_type*						&tree	= m_regression_trees[id_action]->tree( d_dim ); 

		training_set< space_param_type, training_sample_type > &set = m_training_sets[id_action];

		//const u32 sz = set.samples().size();
		const u32 sz = set.get_samples_count( d_dim );

		ASSERT( sz >= 3 );

		const u32 sz_div_3  = u32( sz / 3 );
		const u32 sz_2d3	= sz - sz_div_3;
		u32 from = sz_2d3;
		u32 to = sz;
		ASSERT( from < to );
		ASSERT( to <= sz );

		const action *a = m_action_base.get(id_action);
		float sum = 0;
		for( u32 i = from; i < to  ; ++i )
		{

			//separate_reward_type diff = nu( *a, 0, set.samples()[i].first, rtp_world::random() ) - value( *a, set.samples()[i].first ); //.add previous blending state to samples?
			space_param_type p = set.get_param( i, d_dim );
			separate_reward_type diff = nu( *a, p.blend_id(), p , rtp_world::random() ) - value( *a, p ); //.add previous blending state to samples?
			sum += diff.square_sum_reward();
		}
		number += ( to - from );
		return sum;
}

//template< class action >
//float training_value_function< action >::prune_residual( u32 id_action )
//{
//	
//	u32 number = 0;
//	float sum = prune_sum_residual( id_action, number );
//	return math::sqrt( sum/number );
//}

template< class action >
float training_value_function< action >::prune_residual( )
{
	const u32 num_actions = m_action_base.size( );
	float sum = 0;
	u32 number = 0;
	for( u32 id_action = 0; id_action <  num_actions; ++id_action )
	{
		
			//for( u32 s_dim = 0; s_dim <  separate_dimension_size; ++s_dim )
			for( u32 d_dim = 0; d_dim <  discrete_dimension_size; ++d_dim )
			{
				u32 num = 0;
				sum +=prune_sum_residual( id_action, d_dim, num );
				number+=num;
				
			}
	}
	return math::sqrt( sum/number );
}


template< class action >
float			training_value_function< action >::learn_step( )
{

		//if( !b_learn_trees_frozen )
			//generate_samples(  );
		//clear();

		if( !b_learn_trees_frozen )
			build_trees();
		else
			recalculate_trees();

		float residual = update_sets();

		//recalculate_trees(); //?

		if( samples_total() > samples_number_pruning_threshhold * ( m_prune_steps*2 + 1 )  && m_prune_steps < num_prune_steps && b_learn_prune )
		{
			prune();
			++m_prune_steps;
			//b_learn_prune = false;
		}

		return residual;
}

#ifndef MASTER_GOLD
template< class action >
void training_value_function< action >::save( xray::configs::lua_config_value cfg, bool training )const
{
	
	
	if( m_regression_trees.empty() )
		training = false;

	cfg["training"] = training;
	if(!training)
	{
		super::save( cfg, false );
		return;
	}
	
	cfg["prune_steps"] = m_prune_steps;

	save_training_sets( cfg );

	const u32 sz = m_action_base.size();
	for( u32 i = 0; i < sz; ++i )
	{
		//training_save_load_data< space_param_type > data( m_training_sets[i].samples(), *m_regression_trees[i] );
		const training_action_value_save_load_data< space_param_type, training_sample_type > data( m_training_sets[i] );
		m_regression_trees[i]->save( cfg["trees"][i], data );
	}
}
#endif //MASTER_GOLD

template< class action >
void	training_value_function< action >::load( const xray::configs::binary_config_value &cfg )
{
	const bool training = cfg["training"];
	if(!training)
	{
		super::load( cfg );
		return;
	}
	

	clear();
	init_trees();
	

	m_prune_steps = cfg["prune_steps"];

	load_training_sets( cfg );

	const u32 sz = m_action_base.size();

	R_ASSERT( m_regression_trees.size() == sz );

	for( u32 i = 0; i < sz; ++i )
	{
		//training_save_load_data< space_param_type > data( m_training_sets[i].samples(), *m_regression_trees[i] );



		//DELETE( m_regression_trees[i] );

		//m_regression_trees[i] = NEW(regression_tree_type)( m_training_sets[i] ); 

		//m_regression_trees[i].load( cfg["trees"][i], data );
		

		//training_save_load_data< space_param_type > data( m_training_sets[i], *m_regression_trees[i] );

		DELETE( m_regression_trees[i] );
		m_regression_trees[i] = NEW(action_value_type)( m_training_sets[i] ); 

		training_action_value_save_load_data< space_param_type, training_sample_type > data( m_training_sets[i] );

		//training_save_load_data< space_param_type > data( m_training_sets[i] );
		m_regression_trees[i]->load( cfg["trees"][i], data );
		

	}
}

#ifndef MASTER_GOLD
template< class action >
void training_value_function< action >::save_training_sets( xray::configs::lua_config_value cfg )const
{


	xray::configs::lua_config_value cfg_sets = cfg["training_sets"];
	cfg_sets["number"] = m_training_sets.size();
	typename rtp::vector< training_set< space_param_type, training_sample_type> >::const_iterator  i, b  = m_training_sets.begin(),
		e = m_training_sets.end();
	for(i = b;i!=e;++i)
		(*i).save(cfg_sets["sets"][u32(i-b)]);

}

#endif	// MASTER_GOLD

template< class action >
void training_value_function< action >::load_training_sets	( const xray::configs::binary_config_value &cfg )
{
	const xray::configs::binary_config_value cfg_sets = cfg["training_sets"];
	const u32 sz =  (u32)cfg_sets["number"];
	m_training_sets.resize( sz );

	typename rtp::vector< training_set< space_param_type, training_sample_type> >::iterator i, b = m_training_sets.begin(), 
		e = m_training_sets.end();
	for( i = b; i!=e; ++i )
		(*i).load( cfg_sets["sets"][u32(i-b)] );
}

} // namespace rtp
} // namespace xray