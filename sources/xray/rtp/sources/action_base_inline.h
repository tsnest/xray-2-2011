////////////////////////////////////////////////////////////////////////////
//	Created		: 06.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "animation_grasping_action.h"

namespace xray {
namespace rtp {


template< class action >
action_base<action>::action_base( const global_actions_params< action > &par ): 
	m_global_params( par ), 
	m_total_blend_samples( u32(-1) )
{}


template< class action >
u32		action_base<action>::add		( action* a	)
{
	actions.push_back( a );
	return actions.size() - 1;
}

template< class action >
const	action*	action_base<action>::get 	( u32 id ) const	
{
	ASSERT( id<actions.size() );
	return actions[id];
}

template< class action >
action*	action_base<action>::get 	( u32 id )
{
	ASSERT( id<actions.size() );
	return actions[id];
}

template< class action >
action_base<action>::~action_base()
{
	clear();
}

template< class action >
void action_base<action>::clear()
{
	const u32 sz = size();
	for( u32 i = 0; i<sz; ++i )
		DELETE(actions[i]);
	actions.clear();
}

#ifndef	MASTER_GOLD

template< class action >
void action_base<action>::save	( xray::configs::lua_config_value cfg )const
{
	const u32 sz = actions.size();
	cfg["size"] = sz;
	for(u32 i = 0; i < sz ; ++i )
		actions[i]->save(cfg["actions"][i]);

}

#endif	// MASTER_GOLD

template< class action >
void action_base<action>::load	( const xray::configs::binary_config_value &cfg )
{
	clear();
	
	u32 sz = cfg["size"];
	//actions.resize( sz, 0 );
	for(u32 i = 0; i < sz ; ++i )
	{
		NEW( action_type )(*this);//actions[i] = 
		
		actions[i]->load(cfg["actions"][i]);
		actions[i]->construct( );
		ASSERT( actions[i]->id() == i );
	}
}

template<class action_type>
inline u32 action_blend_starts(  action_base<action_type>  const &base, u32 id )
{
	u32 all_samples_number = 0;
	for( u32 i = 0; i < id; ++i )
			all_samples_number += base.get( i )->weights_combinations_count( );
	return all_samples_number;
}

template< class action >
inline	u32		action_base<action>::blend_sample_id_starts	( u32 action_id )const
{
	return action_blend_starts ( *this, action_id );
}

template< class action >
inline	u32		action_base<action>::total_blend_samples	( )const
{
	if(m_total_blend_samples == u32(-1))
		m_total_blend_samples = blend_sample_id_starts( size() );

		return m_total_blend_samples;
}

} // namespace rtp
} // namespace xray