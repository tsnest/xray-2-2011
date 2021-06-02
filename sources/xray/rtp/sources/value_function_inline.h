////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace rtp {

extern float g_discount; //	= 0.8f;//dicount factor

//static const float discount	= 0.3f;//dicount factor
//static const float discount	= 0.5f;//dicount factor

template< class regression_tree, class action >
inline value_function< regression_tree, action >::value_function( const action_base<action_type>& ab ):
	m_action_base( ab )
{

}

template< class action >
inline   typename action::separate_reward_type	state_reward( const	action &st, u32 blend, const typename action::space_param_type &params  )
{
	
	return st.reward( params, blend );
}

template< class action >
inline separate_reward<action::space_param_type::separate_dimensions_count>	instant_reward(  const	action &afrom, u32 from_blend_id, const	action &ato, const typename action::space_param_type &param  ) 
{
	return afrom.reward( ato ) + state_reward( afrom, param ) ;
}

template< class regression_tree, class action >
inline typename value_function< regression_tree, action >::separate_reward_type	value_function< regression_tree, action >::nu( const action_type &afrom, u32 from_blend_id,  const space_param_type &from_param, math::random32_with_count &rnd ) const
{
	separate_reward_type reward; // = 0
	//float value = 0; 
	space_param_type to ;//=  pcurrent;
	u32 blend(u32(-1));//const action &a_to =
	//*pi( afrom, from_blend_id, from_param, blend, to, reward, value, rnd );// r = R(a,a') + discount * V(s') ; v = V( s') ; s' = (next,pnext)
	*pi( afrom, from_blend_id, from_param, blend, to, reward, rnd );// r = R(a,a') + discount * V(s') ; v = V( s') ; s' = (next,pnext)
	return  reward + state_reward( afrom, from_blend_id,  from_param );
}

template< class regression_tree, class action >
inline typename value_function< regression_tree, action >::separate_reward_type	value_function<regression_tree, action>::value(   const	action_type &ato, const space_param_type &to_param  )const
{
	return  m_regression_trees[ ato.id() ]->reward( to_param ) ;
}

inline void set_random_buffer( buffer_vector<u32> &rnd_buff,  math::random32_with_count &rnd  )
{
	buffer_vector<u32>::iterator i			= rnd_buff.begin();
	buffer_vector<u32>::iterator const e	= rnd_buff.end();
	for( u32 k = 0; i != e; ++i, ++k )
		*i				= k;

	std::random_shuffle	( rnd_buff.begin(), rnd_buff.end(), rnd );
}

static u32 samples_check_core_ret_id = 0;

template< class regression_tree, class action >
typename value_function< regression_tree, action >::separate_reward_type
	value_function< regression_tree, action >::core_pi	(
		action_type const& afrom,
		u32 from_blend_id,
		action_type const& ato,
		u32& blend_id,
		space_param_type const& from_param,
		space_param_type& to_param,
	//	float &scalar_max_v,
		math::random32_with_count &rnd
	) const
{

	u32 num_samples = ato.weights_combinations_count();
	ASSERT( num_samples > 0 );

	buffer_vector<core_pi_result> results( ALLOCA( sizeof(core_pi_result)*num_samples ), num_samples );
	separate_reward<separate_dimensions_count> max_v;
	max_v.set_negative_infinity();
	
	blend_id = u32(-1);//rnd_samples[0];
	for( u32 i = 0; i < num_samples; ++i )
	{
		space_param_type to = from_param;//( from_param.target_position());
		
		//pair!
		ato.run( to, afrom, from_blend_id, i );
		
		//separate_reward<separate_dimensions_count> val = value( ato, to ) ;
		separate_reward<separate_dimensions_count> val = value( ato, to ); 
		//separate_reward<separate_dimensions_count> state = state_reward( ato, i,  to );
		
		//separate_reward<separate_dimensions_count> transition_reward = afrom.transition_reward( from_blend_id, ato, i );
		//float discount = g_discount;
		//separate_reward<separate_dimensions_count> res =  transition_reward +  val * discount;//core_pi( afrom, ato, to_param, v );
		separate_reward<separate_dimensions_count> res = val;
		//separate_reward<separate_dimensions_count> cmp = val; // state + val;
		if( res  < max_v )
			continue;

		if ( res == max_v ) {
			core_pi_result const res = { to, i };
			results.push_back	( res );
			continue;
		}

		max_v = res;
		
		results.resize	( 1 );
		core_pi_result& result = results.front();
		result.to_param = to;
		result.blend_id = i;
	}

	u32 const size = results.size();
	
	u32 const index = size > 1 ? rnd.random( size ) : 0;
	
	core_pi_result& result = results[index];
	to_param = result.to_param;
	blend_id = result.blend_id;

	separate_reward<separate_dimensions_count> transition_reward = afrom.transition_reward( from_blend_id, ato, blend_id );
	
	float discount = g_discount;
	separate_reward<separate_dimensions_count> core_ret =  transition_reward +  max_v * discount;//core_pi( afrom, ato, to_param, v );
	
	return core_ret;
}

template< class regression_tree, class action >
const action*	value_function< regression_tree, action >::pi(
		action_type const& from,
		u32 from_blend_id,
		space_param_type const& from_params,
		u32& blend_id,
		space_param_type& to_params,
		separate_reward_type	&max_reward,
		//float& v,
		math::random32_with_count &rnd

	) const
{
	const u32 num_actions = m_action_base.size();
	ASSERT( num_actions != 0 );

	buffer_vector<pi_result> results( ALLOCA( sizeof(pi_result)*num_actions ), num_actions );

	const action	*ret = m_action_base.get( 0 ); 
	
	//max_reward = core_pi( from, *ret, from_params, to_params );
	//float max_reward =- math::infinity;
//	separate_reward<separate_dimensions_count> max_reward;
	max_reward.set_negative_infinity();
//////
	
//////
	for( u32 i=0; i < num_actions; ++i )
	{
		pi_result result = { space_param_type(), m_action_base.get(i), u32(-1) };

		//separate_reward<separate_dimensions_count> reward = 	core_pi( from, from_blend_id,  *result.ret, result.blend_id, from_params,  result.to_param, result.value, rnd );
		separate_reward<separate_dimensions_count> reward = 	core_pi( from, from_blend_id,  *result.ret, result.blend_id, from_params,  result.to_param, rnd );
		
		//float new_reward = reward.sum_reward();

		if( reward < max_reward )
			continue;


		if( reward == max_reward ) {
			results.push_back	( result );
			continue;
		}
		
		max_reward = reward;
		results.resize( 1 );
		results.front()	= result;

	}

	u32 const size = results.size();
	//u32 const index = size > 1 ? rtp_world::random().random( size ) : 0;
	u32 const index = size > 1 ? rnd.random( size ) : 0;

	pi_result& result = results[index];

	ret = result.ret;
	to_params = result.to_param;
	//v = result.value;
	blend_id = result.blend_id;

	return ret;
}

template< class regression_tree, class action >
inline const action*	value_function< regression_tree, action >::pi( const action_type &from, u32 from_blend_id, const space_param_type &from_param, u32 &blend_id ) const
{
	
	space_param_type to_param;
	//float  value;
	separate_reward<separate_dimensions_count> max_reward;
	//return pi( from, from_blend_id, from_param, blend_id, to_param, max_reward, value, rtp_world::random() );
	return pi( from, from_blend_id, from_param, blend_id, to_param, max_reward, rtp_world::random() );
}

template< class regression_tree, class action >
inline void	value_function< regression_tree, action >::render(  xray::render::scene_ptr const&, xray::render::debug::renderer&  ) const
{

	//XRAY_UNREFERENCED_PARAMETERS( r );
	//typename rtp::vector< action_value_type* > ::const_iterator i, b = m_regression_trees.begin(), e = m_regression_trees.end();
	//for( i = b ; i!=e; ++i )
		//(*i)->render( r, *m_action_base.get(u32(i-b)) );
}

template< class regression_tree, class action >
inline void	value_function< regression_tree, action >::		clear( )
{
	
	
	typename rtp::vector< action_value_type* > ::iterator i = m_regression_trees.begin(), e = m_regression_trees.end();
	for( ; i!=e; ++i )
		DELETE(*i);
	m_regression_trees.clear();

}

template< class regression_tree, class action >
inline void	value_function< regression_tree, action >::init_trees( )
{
	clear();
	const u32 sz = m_action_base.size();
	m_regression_trees.resize( sz, 0 );
	for( u32 i = 0; i<sz; ++i )
		m_regression_trees[i] = NEW(action_value_type)( );
		//m_regression_trees[i] = NEW(regression_tree_type)( );

		
}

#ifndef MASTER_GOLD
template< class regression_tree, class action >
void	value_function< regression_tree, action >::save( xray::configs::lua_config_value cfg, bool )const
{
	bool empty = m_regression_trees.empty();
	cfg["empty"] = empty;

	if( empty )
		return;

	//const u32 sz = m_action_base.size();
	//node_save_load_data<space_param_type> data;
	//for( u32 i = 0; i<sz; ++i )
	//	m_regression_trees[i]->save( cfg["trees"][i], data );
	
	action_value_save_load_data<space_param_type, typename action_value_type::training_sample_type> data;
	const u32 sz = m_action_base.size();
	for( u32 i = 0; i<sz; ++i )
		m_regression_trees[i]->save( cfg["trees"][i], data );

}
#endif //MASTER_GOLD

template< class regression_tree, class action >
void	value_function< regression_tree, action >::load( const xray::configs::binary_config_value &cfg )
{
	clear();
	init_trees();

	if( cfg["empty"] )
		return;

	//const u32 sz = m_action_base.size();
	//node_save_load_data< space_param_type > data;
	//for( u32 i = 0; i<sz; ++i )
	//	m_regression_trees[i]->load( cfg["trees"][i], data );
	
	const u32 sz = m_action_base.size();
	action_value_save_load_data<space_param_type, typename action_value_type::training_sample_type> data;
	for( u32 i = 0; i<sz; ++i )
		m_regression_trees[i]->load( cfg["trees"][i], data );

}

template< class regression_tree, class action >
inline bool value_function< regression_tree, action >::empty( )const
{
	if( m_regression_trees.empty() )
		return true;

	ASSERT( m_regression_trees.size() == m_action_base.size() );

	return false;

}

template< class regression_tree, class action >
bool	value_function< regression_tree, action >::compare ( const typename value_function< regression_tree, action >::self_type& value ) const
{
	const u32 v_sz = value.m_regression_trees.size();
	const u32 sz = m_regression_trees.size();

	if( v_sz != sz )
		return false;

	for( u32 i = 0; i<sz; ++i )
		if( !m_regression_trees[i]->compare( *value.m_regression_trees[i] ) ) 
			return false;
	return true;
}


} // namespace rtp
} // namespace xray