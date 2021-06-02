////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace rtp {

#ifndef MASTER_GOLD
template< class space_param_type > 
inline void regression_tree<space_param_type>::save( xray::configs::lua_config_value cfg, const  node_save_load_data<space_param_type>& data  )const
{
	m_root.save( cfg, data );
}
#endif // MASTER_GOLD

template< class space_param_type > 
inline void	regression_tree<space_param_type>::load( const xray::configs::binary_config_value &cfg,  node_save_load_data<space_param_type>& data  )
{
	m_root.load( cfg, data );
}

template< class space_param_type > 
inline node< space_param_type >::node():
			m_split_dimension( u32(-1) ),
			m_split_value( math::QNaN ),
			m_left( 0 ), m_right( 0 ),//, parent( prnt ),
			m_reward_value( 0 )
{
	
}


template< class space_param_type > 
inline node< space_param_type >::node( u32 ):
			m_split_dimension( u32(-1) ),
			m_split_value( math::QNaN ),
			m_left( 0 ), m_right( 0 ),//, parent( prnt ),
			m_reward_value( 0 )
{
	
}

template< class space_param_type > 
inline node< space_param_type >::~node( )
{
	DELETE( m_left );
	DELETE( m_right );
}

template< class space_param_type > 
inline bool	node< space_param_type >::compare( const typename node< space_param_type >::self_type &node ) const
{
	if( is_leaf( )!= node.is_leaf() )
		return false;

	if( !is_leaf( ) )
		return 
			m_split_dimension	==	node.m_split_dimension	&&
			m_split_value		==	node.m_split_value		&&
			left	()->compare( *node.left() )				&& 
			right	()->compare( *node.right() );
	

	return 
		m_reward_value		==	node.m_reward_value;
}

template< class space_param_type > 
inline bool	node< space_param_type >::is_leaf( ) const
{
	//dbg_check_validity	( );
	return !m_left;
}
template< class space_param_type > 
const typename node< space_param_type >::self_type*		node< space_param_type >::left		( ) const
{
	return m_left;
}
template< class space_param_type > 
const typename node< space_param_type >::self_type*		node< space_param_type >::right		( ) const
{
	return m_right;
}

template< class space_param_type >
inline float	node< space_param_type >::reward( const space_param_type &params  ) const
{
	if ( is_leaf( ) )
		return m_reward_value;

	if( params[ m_split_dimension ] < m_split_value ) // <= ?
		return m_left->reward( params );
	else
		return m_right->reward( params );
}

template< class space_param_type >
inline node< space_param_type > * node<space_param_type>::create		(  u32   )
{
	return NEW( node< space_param_type > )(  );
}

#ifndef MASTER_GOLD
template< class space_param_type > 	
void	node< space_param_type >::save( xray::configs::lua_config_value cfg, const  node_save_load_data< space_param_type >& data  )const
{

		cfg["leaf"] = is_leaf();
		if( !is_leaf() )
		{
			if( xray::identity( space_param_type::dimensions_number > 1 ) )
				cfg["split_dimension"] = m_split_dimension;

			cfg["split_value"] = m_split_value;
			m_left	->save( cfg["left"], data );
			m_right	->save( cfg["right"], data );
		} else
			cfg["reward_value"] = m_reward_value;



}
#endif // MASTER_GOLD

template< class space_param_type > 
void	node< space_param_type >::load( const xray::configs::binary_config_value &cfg, node_save_load_data< space_param_type >& data  )
{
		bool leaf = cfg["leaf"];
		if( !leaf )
		{
			if( xray::identity( space_param_type::dimensions_number > 1 ) )
				m_split_dimension	= cfg["split_dimension"] ;
			else
				m_split_dimension	= 0;

			m_split_value		= cfg["split_value"];
			DELETE(m_left);
			DELETE(m_right);
			m_left	= create( level()+1 );
			m_right	= create( level()+1 );
			m_left	->load( cfg["left"], data );
			m_right	->load( cfg["right"], data );
		} else
		{
			ASSERT(!m_left);
			ASSERT(!m_right);
			m_reward_value = cfg["reward_value"];
		}
}

} // namespace rtp
} // namespace xray