////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_ACTION_INLINE_H_INCLUDED
#define ANIMATION_ACTION_INLINE_H_INCLUDED

#include <xray/tasks_system.h>
#include <xray/tasks_thread_tls.h>
#include <xray/fs/device_utils.h>

namespace xray {
namespace rtp {
			
extern	bool g_transform_depends_from_previous_action;
extern	bool g_force_recalculate_action_transforms;
extern	bool g_save_cached_data;

template< class space_params, class world_space_params, class action >
inline animation_action<space_params,world_space_params,action>::animation_action( 
	action_base<action> &ab,  
	animation::i_animation_action  const *anim ):
	super( ab ),
	m_animation_action( anim ),
	m_samples_per_weight(u32(-1)),
	m_depend_on_previous_action( g_transform_depends_from_previous_action ),
	m_has_motion_data( false ),
	m_motion_data_query_returned( false ),
	m_weights( 0 ),
	m_weights_count( u32(-1) ),
	m_weights_combinations_count( u32(-1) )
{
	
	
}

template< class space_params, class world_space_params, class action >
inline animation_action<space_params,world_space_params,action>::animation_action( 
	xray::rtp::action_base<action> &ab
	):
	super( ab ),
	m_animation_action( 0 ),
	m_samples_per_weight(u32(-1)),
	m_depend_on_previous_action( g_transform_depends_from_previous_action ),
	m_has_motion_data( false ),
	m_motion_data_query_returned( false ),
	m_weights( 0 ),
	m_weights_count( u32(-1) ),
	m_weights_combinations_count( u32(-1) )
{
	
}

template< class space_params, class world_space_params, class action >
inline animation_action<space_params,world_space_params,action>::~animation_action()
{
	ASSERT( m_weights );
	FREE( m_weights );
}


template< class space_params, class world_space_params, class action >
inline		u32			animation_action<space_params,world_space_params,action>::discrete_param_value	( )const 
{  
	u32 res = (u32)gait(); ASSERT( res < space_params::discrete_dimension_size );
	return res;
}

template< class space_params, class world_space_params, class action >
inline float	animation_action<space_params,world_space_params,action>::duration			()const
{
	return m_animation_action->duration();
}

template< class space_params, class world_space_params, class action >
inline pcstr	animation_action<space_params,world_space_params,action>::name( )const 
{
	ASSERT( m_animation_action );
	return m_animation_action->name();
}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::construct( )
{
	super::construct( );
	calc_samples_per_weight	( );
	calculate_weights_combinations_count();
	generate_all_weights_combinations( );
}



template< class space_params, class world_space_params, class action >
fs_new::virtual_path_string		animation_action<space_params,world_space_params,action>::cached_data_file_path( ) const
{

	R_ASSERT( m_animation_action );
	pcstr path_animation_action = m_animation_action->name();
	fs_new::virtual_path_string directory;
	fs_new::get_path_without_last_item( &directory, path_animation_action );
	
	fs_new::virtual_path_string	name;	
	fs_new::file_name_with_no_extension_from_path( &name, path_animation_action );
	
	fs_new::virtual_path_string	out_path;
	out_path.assignf	("%s/motion_data/%s_motion_data.lua", directory.c_str(), name.c_str());
	return out_path;
}


template< class space_params, class world_space_params, class action >
void	animation_action<space_params,world_space_params,action>::on_cached_data_loaded	( resources::queries_result& data )
{
	m_motion_data_query_returned = true;
	
	if( !data.is_successful() )
	{
		m_cached_data_loaded_callback( );
		return;
	}
	m_has_motion_data = true;
	
	LOG_INFO( "on_cached_data_loaded " );

	xray::configs::binary_config_ptr config	= static_cast_resource_ptr<xray::configs::binary_config_ptr>(data[0].get_unmanaged_resource());

	xray::configs::binary_config_value const cfg= config->get_root()["data"];
	load_cached_data( cfg );

	m_cached_data_loaded_callback( );
}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::cached_data_query( const boost::function< void ( ) >	 &cb  )
{
	m_cached_data_loaded_callback = cb;
	m_has_motion_data = false;
	m_motion_data_query_returned = false;

	fs_new::virtual_path_string const & path = cached_data_file_path( );

	LOG_INFO( "cached_data_query: %s ", path.c_str() );
	
	resources::request	resources[]	= 
	{
		{ path.c_str(),					resources::binary_config_class },
	
	};

	xray::resources::query_resources(
		resources,
		array_size(resources),
		boost::bind( &animation_action<space_params,world_space_params,action>::on_cached_data_loaded, this, _1 ),
		g_allocator
	);

}

#ifndef MASTER_GOLD

template< class space_params, class world_space_params, class action >
void	animation_action<space_params,world_space_params,action>::save_cached_data( configs::lua_config_value cfg ) const
{
	cfg["setings"]["depend_on_previous_action"] = m_depend_on_previous_action;
}

template< class space_params, class world_space_params, class action >
void	animation_action<space_params,world_space_params,action>::save_cached_data( ) const
{


	xray::configs::lua_config_ptr cfg		= xray::configs::create_lua_config();
	xray::configs::lua_config_value v_cfg	=(*cfg)["data"];

	save_cached_data( v_cfg );

	fs_new::virtual_path_string const & path = cached_data_file_path( );

	cfg->save_as	( path.c_str(), configs::target_sources );
}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::save( xray::configs::lua_config_value &cfg )const
{
	super::save( cfg );

	ASSERT( m_animation_action );

	cfg["group_id"] = m_animation_action->get_id( );
	cfg["samples_per_weight"] = m_samples_per_weight;
	
	if( g_save_cached_data )
			save_cached_data();
}

#endif // #ifndef MASTER_GOLD

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::load_cached_data( const configs::binary_config_value &cfg )
{
	m_depend_on_previous_action = cfg["setings"]["depend_on_previous_action"];
}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::load( const xray::configs::binary_config_value &cfg )
{
	super::load( cfg );

	u32 id					= cfg["group_id"];
	animation::i_animation_controller_set *set = super::get_action_base().global_params().m_set;
	ASSERT( set );
	m_animation_action = set->action( id );
	//samples_per_weight = u32( floor( exp( log( float( max_samples ) )/float( anim_count() -1 ) ) ) );
	//calc_samples_per_weight( );
	m_samples_per_weight = cfg["samples_per_weight"];

}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::load_config( )
{
	m_animation_action->config() = 0;
}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::calc_samples_per_weight	( )
{
	if( anim_count() == 1 )
		m_samples_per_weight = max_samples;
	else
	{
		R_ASSERT( anim_count() > 1 );
		m_samples_per_weight = u32( 
						( 
							exp(  log(
										float( max_samples ) )/float( anim_count() - 1 ) 
									) 
									
						) 
				);
	}

}



template< class space_params, class world_space_params, class action >
inline u32		animation_action<space_params,world_space_params,action>::weights_combinations_count	( )const
{
	ASSERT( m_animation_action );
	ASSERT( m_weights_combinations_count != u32(-1) );
	//return math::pow( m_samples_per_weight  , anim_count() - 1 );
	return m_weights_combinations_count;//calculate_weights_combinations_count(  anim_count(), m_samples_per_weight );
}

template< class space_params, class world_space_params, class action >
inline u32		animation_action<space_params,world_space_params,action>::anim_count( )const
{
	ASSERT( m_animation_action );
	return m_animation_action->animations_number();
}

void generate_weights( buffer_vector< float > &weights, u32 animations_count, u32 samples_per_animation, u32 id );

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::get_weights( u32 blend, buffer_vector< float > &weights )const
{


	const u32 anim_cnt = anim_count( ) ;
	ASSERT( weights.capacity() >= anim_cnt );
	
	ASSERT( blend < weights_combinations_count	( ) );
	weights.resize( anim_cnt );

	ASSERT( blend < m_weights_combinations_count );
	ASSERT(  blend * anim_cnt < m_weights_count );



	for( u32 i = 0; i < anim_cnt; ++i )
	{
		weights[ i ]  = m_weights[ blend * anim_cnt + i ];
		ASSERT( math::valid( weights[ i ] ) );
		ASSERT( weights[ i ] <= 1.f );
		ASSERT( weights[ i ] >= 0.f );
	}

#ifdef	DEBUG
	float sum = 0.f;
	for( u32 i = 0; i < anim_cnt; ++i )
		sum += weights[ i ];
	ASSERT( math::is_similar( sum, 1.f ) );
#endif
	

	//generate_weights( weights, anim_cnt, m_samples_per_weight, blend );

/*
	u32		bn	= blend;
	float	sum = 0;
	const u32 d = m_samples_per_weight ;
	const u32 lev_0 = blend % m_samples_per_weight;
	float weight0 = 

	for( u32 i = 0; i < anim_cnt; ++i )
	{
		u32 b = bn % d ;//samples_per_weight;
		bn = ( bn - b ) /  m_samples_per_weight;
		float fb = float( b )/float( d - 1 ) * m_animation_action->max_weight(i) ; //anim_count - 1
		weights[i] = fb ;
		sum += fb;
	}
	float clmp = sum; 
	math::clamp( clmp, 0.f,  1.f   );
	weights[ anim_cnt - 1 ] = 1.f - clmp;

	sum +=weights[ anim_cnt - 1 ];
	
	R_ASSERT( !math::is_zero( sum ) );

	{
		const float  isum = 1.f/sum;
#if 0
		pcstr lg = 0;
#endif
		for( u32 i = 0; i < anim_cnt; ++i )
		{
			weights[i] *= isum;
			R_ASSERT( math::valid( weights[i] ) );
			R_ASSERT( weights[i] >= 0.f && weights[i] <= 1.f );
#if 0
			string256 sweights;
			sprintf( sweights, "w%d: %f; ", i, weights[i] );
			STR_JOINA	( lg, lg, sweights ); 
#endif

		}
#if 0
			LOG_INFO( "action :%d,  %s ", id(), lg );
#endif
	}
*/

}

inline void	weight_mean( buffer_vector< float > &weights, const buffer_vector< float > &weights0, const buffer_vector< float > &weights1, float f )
{

	ASSERT( weights1.size() == weights0.size() );
	ASSERT( f >= 0 && f <=1 );

	const u32 sz = weights0.size();
	weights.resize( sz );
	
	float f1 = 1.f - f;
	for( u32 i = 0; i < sz; ++i )
		weights[i] = weights0[i] * f + weights1[i] * f1;

}

template< class space_params, class world_space_params, class action >
inline float4x4	animation_action<space_params,world_space_params,action>::calculate_transform	( u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float > const &from_weights )const
{
	XRAY_UNREFERENCED_PARAMETERS( blend );
	
	ASSERT( from_action_id!=u32(-1) );

	ASSERT( super::get_action_base().get( from_action_id ) );
	u32 from_animation_group_id = super::get_action_base().get( from_action_id )->animation_action_id();
	
	float4x4 transform;
	m_animation_action->get_displacement_transform( transform, weights, from_animation_group_id, from_weights );
	return transform;
}

template< class space_params, class world_space_params, class action >
float4x4	animation_action<space_params,world_space_params,action>::calculate_transform( const action& af, u32 from_sample, u32 blend_sample  )const
{
	const u32 anim_cnt	=	anim_count();

	buffer_vector<float>	weights( ALLOCA( sizeof( float ) * anim_cnt ), anim_cnt );

	get_weights( blend_sample, weights );
	
	buffer_vector<float>	from_weights( ALLOCA( sizeof( float ) * af.anim_count() ),  af.anim_count() );
	af.get_weights( from_sample, from_weights );

	return calculate_transform( blend_sample, weights, af.id(), from_weights );
}

template< class space_params, class world_space_params, class action >
void	animation_action<space_params,world_space_params,action>::cache_blend_transform	( u32 id_transform, u32 blend, buffer_vector< float > const &weights, u32 from_action_id, buffer_vector< float > const &from_weights )
{
	on_cache_transform( id_transform, calculate_transform( blend, weights, from_action_id, from_weights ) );
}

template< class space_params, class world_space_params, class action >
void	animation_action<space_params,world_space_params,action>::on_cache_transform		( u32 id_transform, float4x4 const &transform )
{
	XRAY_UNREFERENCED_PARAMETERS( id_transform, &transform );
}

template< class space_params, class world_space_params, class action >
inline u32 animation_action<space_params,world_space_params,action>::cached_transform_id(  u32 blend_id, u32 from_action_id,  u32 from_blend_id )const
{
	XRAY_UNREFERENCED_PARAMETERS( from_action_id,  from_blend_id  );

if ( !m_depend_on_previous_action )
		return blend_id;

	return blend_id *  transforms_per_blend( ) +  this->get_action_base().blend_sample_id_starts(  from_action_id ) + from_blend_id;

}

template< class space_params, class world_space_params, class action >
inline	u32	animation_action<space_params,world_space_params,action>::blend_transform_ids_start	(  u32 blend_id )const
{
	return blend_id *  transforms_per_blend( );
}

template< class space_params, class world_space_params, class action >
inline	u32	animation_action<space_params,world_space_params,action>::transforms_per_blend	( )const
{
	if ( !m_depend_on_previous_action )
		return 1;
	return this->get_action_base().total_blend_samples( );
}

 void generate_weights_count	(
		u32 const samples_count,
		u32 const current_weight_id,
		u32 const current_sum,
		u32& count
	);

 void generate_weights	(
		u32* temp_indices,
		float*& output,
		u32 const animations_count,
		u32 const samples_count,
		u32 const current_weight_id,
		u32 const current_sum
	);


template< class space_params, class world_space_params, class action >
inline	void	animation_action<space_params,world_space_params,action>::generate_all_weights_combinations ( )
{
	
	u32 const animations_count	=  anim_count();
	u32 const samples_count		= m_samples_per_weight; 
	u32 *temp_indices = (u32*) ALLOCA( animations_count * sizeof(32 ) );
	float* weights_temp		= m_weights;
	generate_weights( temp_indices, weights_temp, animations_count, samples_count,  animations_count - 1, 0 );
}

template< class space_params, class world_space_params, class action >
inline	void	animation_action<space_params,world_space_params,action>::calculate_weights_combinations_count ( )
{
	u32 combinations_count = 0;
	u32 const animations_count =  anim_count();
	u32 const samples_count = m_samples_per_weight;
	generate_weights_count	( samples_count, animations_count - 1, 0, combinations_count );
	m_weights_combinations_count = combinations_count;
	m_weights_count = m_weights_combinations_count * animations_count;
	ASSERT( !m_weights );
	m_weights = (float*) MALLOC( m_weights_count * sizeof( float ), "rtp action weights combinations" );
}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::cache_transforms	( )
{
	
	m_depend_on_previous_action = g_transform_depends_from_previous_action;
	
	if( g_force_recalculate_action_transforms )
		m_has_motion_data = false;

	if( m_has_motion_data )
		return;
	
	const u32 blends_count	=	weights_combinations_count( ) ;
	
	for( u32 blend_i =0; blend_i < blends_count; ++blend_i )
			cache_blend_transform_internal( blend_i );
	
	m_has_motion_data = true;
}

struct cache_blend_id_task_range
{
	
	cache_blend_id_task_range( u32 action_s, u32 action_e, u32 blend_s, u32 blend_cnt, u32 blend, buffer_vector<float> &w  ): 
		action_start( action_s ), action_end( action_e ), 
		blend_start( blend_s ), blend_count( blend_cnt ), 
		blend_id( blend ), weights( &w )
		{}

	u32 action_start, action_end;
	u32 blend_start,  blend_count;
	u32 blend_id;
	buffer_vector<float> *weights;
}; // struct cache_blend_id_range

static tasks::task_type *	s_catch_samples_task_types []	=	
				{	tasks::create_new_task_type("catch_samples_task", 0),
					
								};

template< class space_params, class world_space_params, class action >
inline	void	animation_action<space_params,world_space_params,action>::cache_blend_transform_internal	( u32 blend_id )
{
	const u32 anim_cnt	=	anim_count();
	buffer_vector<float>	weights( ALLOCA( sizeof( float ) * anim_cnt ), anim_cnt );
	get_weights( blend_id, weights );

	if( !m_depend_on_previous_action )
	{
		u32 from_action_id = this->id() == 0 ?  this->get_action_base().size() - 1 : this->id() -1;
		
		const u32 from_animation_count = this->get_action_base().get( from_action_id )->anim_count();
		buffer_vector<float>	from_weights( ALLOCA( sizeof( float ) * from_animation_count ), from_animation_count );
		this->get_action_base().get( from_action_id )->get_weights( 0, from_weights );
		cache_blend_transform( cached_transform_id( blend_id, u32(-1), u32(-1) ), blend_id, weights, from_action_id, from_weights );
		return;
	}

	u32 const blend_samples_total = transforms_per_blend( );//this->get_action_base().total_blend_samples( );
	u32 const task_count	= 30;
	u32 const task_size = blend_samples_total / task_count; 

	u32 const real_task_count = task_count +  ( ( blend_samples_total % task_count ) == 0 ? 0 : 1 );
	
	buffer_vector<cache_blend_id_task_range>	task_storage( ALLOCA( sizeof( cache_blend_id_task_range ) * real_task_count ), real_task_count );

	u32 const num_actions = this->get_action_base().size();
	u32 sample_cnt = 1;
	u32 total_sample_cnt = 1;
	u32 task_i = 0;
	u32 start_action = 0, start_sample = 0;
	u32 from_action_i = 0;

	for(  from_action_i = 0; from_action_i < num_actions; ++from_action_i )
	{
		ASSERT( this->get_action_base().get( from_action_i ) );
		u32 samples_count = this->get_action_base().get( from_action_i )->weights_combinations_count( );
		
		for( u32 from_blend_i = 0; from_blend_i < samples_count; ++from_blend_i )
		{
				if( sample_cnt == task_size  )
				{
					R_ASSERT( task_i < real_task_count );
					
					cache_blend_id_task_range range( start_action, from_action_i, start_sample,  sample_cnt, blend_id, weights );
					task_storage.push_back( range );
					tasks::spawn_task ( boost::bind( &animation_action<space_params,world_space_params,action>::cache_blend_transform_range, this, boost::ref( task_storage.back() )  ), s_catch_samples_task_types[0] );

					start_action = from_action_i;
					start_sample = from_blend_i + 1;
					sample_cnt = 0;
					++task_i;

				}
				if( task_i == task_count )
						break;
				++sample_cnt;
				++total_sample_cnt;

		}
		if( task_i == task_count )
						break;

	}

	if( blend_samples_total % task_count )
	{		
		R_ASSERT( task_i < real_task_count );
		cache_blend_id_task_range range( start_action, num_actions - 1, start_sample,  blend_samples_total - total_sample_cnt , blend_id, weights );
		task_storage.push_back( range );
		tasks::spawn_task ( boost::bind( &animation_action<space_params,world_space_params,action>::cache_blend_transform_range, this, boost::ref( task_storage.back() ) ), s_catch_samples_task_types[0] );
	}
	
	
	tasks::wait_for_all_children			( );

	//u32 const num_actions = this->get_action_base().size();
	//
	//for( u32 from_action_i = 0; from_action_i < num_actions; ++from_action_i )
	//{
	//	ASSERT( this->get_action_base().get( from_action_i ) );
	//	u32 samples_count = this->get_action_base().get( from_action_i )->weights_combinations_count( );
	//	for( u32 from_blend_i = 0; from_blend_i < samples_count; ++from_blend_i )
	//			cache_blend_transform_internal( blend_id, from_action_i, from_blend_i, weights );
	//}

}

template< class space_params, class world_space_params, class action >
inline	void	animation_action<space_params,world_space_params,action>::cache_blend_transform_range	( cache_blend_id_task_range const& range )
{
	//cache_blend_id_task_range const &range = *prange;
	u32 sample_cnt = 0;
	u32 blend_start = range.blend_start;

	for( u32 from_action_i = range.action_start; from_action_i <= range.action_end; ++from_action_i )
	{
		ASSERT( this->get_action_base().get( from_action_i ) );
		u32 samples_count = this->get_action_base().get( from_action_i )->weights_combinations_count( );
		for( u32 from_blend_i = blend_start; from_blend_i < samples_count; ++from_blend_i )
		{
				cache_blend_transform_internal( range.blend_id, from_action_i, from_blend_i, *range.weights );
				++sample_cnt;
				if( sample_cnt >= range.blend_count )
					return;
		}
		blend_start = 0;
	}
}

template< class space_params, class world_space_params, class action >
inline	void	animation_action<space_params,world_space_params,action>::cache_blend_transform_internal	( u32 blend_id, u32 from_action_id, u32 from_blend_id,   buffer_vector< float > const &weights )
{
			
		const u32 from_animation_count = this->get_action_base().get( from_action_id )->anim_count();
		buffer_vector<float>	from_weights( ALLOCA( sizeof( float ) * from_animation_count ), from_animation_count );
		this->get_action_base().get( from_action_id )->get_weights( from_blend_id, from_weights );

		cache_blend_transform( cached_transform_id( blend_id, from_action_id, from_blend_id )  , blend_id, weights, from_action_id, from_weights );
}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::on_walk_run( world_space_params& wfrom, const space_params& lfrom , const float4x4 &disp, buffer_vector< float > &weights )const
{
	XRAY_UNREFERENCED_PARAMETERS( &weights, &lfrom );

	wfrom.controller_pos = disp * wfrom.controller_pos;//wfrom.target_pos;
}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::run_simulate( world_space_params& wfrom, const space_params& lfrom, const action& afrom, u32 from_sample, u32 blend_sample )const
{
	const u32 lanim_count	=	anim_count();

	buffer_vector<float>	weights( ALLOCA( sizeof( float ) * lanim_count ), lanim_count );
	
	get_weights( blend_sample, weights );


	const u32 from_anim_count = afrom.anim_count();
	buffer_vector<float>	from_weights( ALLOCA( sizeof( float ) * from_anim_count ), from_anim_count );
	afrom.get_weights( from_sample, from_weights );

	float4x4 disp;
	m_animation_action->get_displacement_transform( disp, weights, afrom.animation_action_id(), from_weights );

	on_walk_run( wfrom, lfrom,  disp, weights );
}


template< class space_params, class world_space_params, class action >
inline float	animation_action<space_params,world_space_params,action>::run( world_space_params& wfrom, const space_params& lfrom, const action& afrom, u32 from_sample, u32 blend_sample, u32 step, float time )const
{
	XRAY_UNREFERENCED_PARAMETERS( &lfrom );

	const u32 lanim_count	=	anim_count();

	buffer_vector<float>	weights( ALLOCA( sizeof( float ) * lanim_count ), lanim_count );
	
	get_weights( blend_sample, weights );


	const u32 from_anim_count = afrom.anim_count();
	buffer_vector<float>	from_weights( ALLOCA( sizeof( float ) * from_anim_count ), from_anim_count );
	afrom.get_weights( from_sample, from_weights );

#ifdef RTP_ANIMATION_SIMULATE_MOVEMENT
	float4x4 disp;
	m_animation_action->get_displacement_transform( disp, weights, afrom.animation_action_id(), from_weights );

	on_walk_run( wfrom, lfrom,  disp, weights );
#endif // RTP_ANIMATION_SIMULATE_MOVEMENT

	ASSERT_U( wfrom.animation_controller );
	return m_animation_action->run( wfrom.animation_controller, from_weights, weights, step, time );

}

template< class space_params, class world_space_params, class action >
inline void	animation_action<space_params,world_space_params,action>::weight_mean( buffer_vector< float > &weights, u32 blend[4], float factors[2] )const
{
	const u32 lanim_count	=	anim_count();

	buffer_vector<float>	weights0( ALLOCA( sizeof( float ) * lanim_count ), lanim_count );
	get_weights( blend[0], weights0 );

	buffer_vector<float>	weights1( ALLOCA( sizeof( float ) * lanim_count ), lanim_count );
	get_weights( blend[1], weights1 );

	buffer_vector<float>	mean0( ALLOCA( sizeof( float ) * lanim_count ), lanim_count );

	rtp::weight_mean( mean0, weights0, weights1, factors[0] );

	
	get_weights( blend[2], weights0 );
	get_weights( blend[3], weights1 );

	buffer_vector<float>	mean1( ALLOCA( sizeof( float ) * lanim_count ), lanim_count );

	rtp::weight_mean( mean1, weights0, weights1, factors[1] );

	rtp::weight_mean( weights, mean0, mean1, 0.5f );

}

inline animation_global_actions_params::animation_global_actions_params( xray::animation::i_animation_controller_set *set ):
m_set( set )
{
	//float4x4 m;
	//set->get_target_transform( m );
	//float3 vec( 0, 0, 0 );
	//vec = m.transform_position( vec );
}

} // namespace rtp
} // namespace xray

#endif // #ifndef ANIMATION_ACTION_INLINE_H_INCLUDED