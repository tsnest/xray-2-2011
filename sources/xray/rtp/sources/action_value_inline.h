////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_VALUE_INLINE_H_INCLUDED
#define ACTION_VALUE_INLINE_H_INCLUDED



namespace xray {
namespace rtp {

template< class regression_tree, class space_param >
inline action_value_base< regression_tree, space_param >::action_value_base()
{
	for( u32 i = 0; i < separate_dimensions_count ; ++i )
		for( u32 j = 0; j < discrete_dimension_size ; ++j )
			tree( i, j ) = NEW( regression_tree )();
}

template< class regression_tree, class space_param >
inline action_value_base<regression_tree, space_param>::action_value_base( training_set_type &set )
{
	for( u32 i = 0; i < separate_dimensions_count ; ++i )
		for( u32 j = 0; j < discrete_dimension_size ; ++j )
			tree( i, j ) = NEW( regression_tree )( set.subset( i, j ) );
}

template< class regression_tree, class space_param >
inline action_value_base< regression_tree, space_param >::~action_value_base()
{
	for( u32 i = 0; i < separate_dimensions_count ; ++i )
		for( u32 j = 0; j < discrete_dimension_size ; ++j )
			DELETE( tree( i, j )  );
}

template< class regression_tree, class space_param >
inline regression_tree*const&	action_value_base< regression_tree, space_param >::tree			( u32 separate_dim, u32 discrete_dim )const
{ 
	ASSERT( discrete_dim < discrete_dimension_size ); 
	ASSERT( separate_dim < discrete_dimension_size ); 
	return trees[ separate_dim ][ discrete_dim ];
}

template< class regression_tree, class space_param >
inline regression_tree*&		action_value_base< regression_tree, space_param >::tree			( u32 separate_dim, u32 discrete_dim ) 
{ 
	ASSERT( discrete_dim < discrete_dimension_size ); 
	ASSERT( separate_dim < discrete_dimension_size ); 
	return trees[ separate_dim ][ discrete_dim ];
}



//template< class regression_tree >
//float	action_value_base<regression_tree>::update_set		( training_set_type &set )
//{
//
//}

//template< class regression_tree, class space_param >
//inline float	action_value_base< regression_tree, space_param >::reward			(  const space_param_type &params  ) const
//{
//	
//	
//	
//
//	return trees[ params.discrete_value() ]->reward( params );
//}

template< class regression_tree, class space_param >
float	action_value_base< regression_tree, space_param >::reward( u32 separate_dimension,  u32 discrete_dimension, const tree_space_param_type &params  ) const
{
	return  tree( separate_dimension, discrete_dimension )->reward( params );
}

template< class regression_tree, class space_param >
typename action_value_base< regression_tree, space_param >::separate_reward_type action_value_base< regression_tree, space_param >::reward	(  const space_param_type &params  ) const
{
	separate_reward_type ret;
	
	for( u32 i = 0; i < separate_dimensions_count; ++i )
			ret[i] = reward( i, params.discrete_value( ), params.separate_params( i ) );
	
	return ret;
}

//template< class regression_tree, class space_param >
//float	action_value_base< regression_tree, space_param >::reward			(  float value[separate_dimensions_count], const space_param_type &params  ) const
//{
//	float result = 0;
//	for( u32 i = 0; i < separate_dimension; ++i )
//	{
//		value[i] = reward( i, params.separate_params( i ) );
//		result += value[i];
//	}
//	return result;
//	//trees[ params.discrete_value() ]->reward( params );
//}


template< class  space_param > 
inline void	action_value < training_node< typename space_param::tree_space_param_type >, space_param >::build( )
{
	for( u32 i = 0; i < separate_dimensions_count ; ++i )
		for( u32 j = 0; j < discrete_dimension_size ; ++j )
			tree( i, j )->build();
}

template< class  space_param >
inline void	action_value < training_node< typename space_param::tree_space_param_type >, space_param >::recalculate		()
{
	
	for( u32 i = 0; i < separate_dimensions_count ; ++i )
		for( u32 j = 0; j < discrete_dimension_size ; ++j )
			tree( i, j )->recalculate();

}

//template< class regression_tree >
//void action_value_base<regression_tree>::prune_build_tree( u32 min_samples, training_set_type &set )
//{

	//for( u32 i = 0; i < discrete_dimension_size ; ++i )
	//{
	//
	//	training_set<training_sample_type>		&set	= m_training_sets[id_action];
	//	regression_tree_type*					&tree	= m_regression_trees[id_action];

	//	const u32 sz = set.samples().size();
	//	ASSERT( sz >= 3 );
	//	const u32 sz_div_3  = u32( sz / 3 );
	//	const u32 sz_2d3	= sz - sz_div_3;



	//	buffer_vector<const training_sample_type*>	smaller_set(  ALLOCA( sizeof( training_sample_type ) * sz_2d3 ) , sz_2d3 );
	//	//set.randomize();
	//	for( u32 i = 0; i < sz_2d3; ++i )
	//		smaller_set.push_back( &set.samples()[i] );
	//	DELETE(tree);
	//	tree = NEW(regression_tree_type)( min_samples, smaller_set );
	//	tree->build();
	//}

//}
#ifndef MASTER_GOLD
template< class  space_param >
inline void	action_value < training_node< typename space_param::tree_space_param_type >, space_param >::save		( xray::configs::lua_config_value cfg,  const action_value_save_load_data< space_param_type, training_sample_type >& data )const
{
	const training_action_value_save_load_data<space_param_type, training_sample_type>* pdata = data.get_training_save_load_data() ;

	if( !pdata )
	{
		super::save( cfg, data );
		return;
	}
	
	R_ASSERT( pdata );


	const training_set< space_param_type, training_sample_type >	*set = pdata->set();
	R_ASSERT( set );	


	for( u32 i = 0; i < separate_dimensions_count; ++i )
		for( u32 j = 0; j < discrete_dimension_size; ++j )
		{
			training_save_load_data< tree_space_param_type > d( set->subset( i, j ).samples(), *tree( i, j ) );
			tree( i, j )->save( cfg["trees"][i][j], d );
		}

}

#endif // MASTER_GOLD

#ifndef MASTER_GOLD
template< class regression_tree, class space_param >
inline void	action_value_base<regression_tree, space_param>::save		( xray::configs::lua_config_value cfg, const action_value_save_load_data< space_param_type, training_sample_type >& data  )const
{
	XRAY_UNREFERENCED_PARAMETERS( &data );
	for( u32 i = 0; i < space_param::separate_dimensions_count; ++i )
		for( u32 j = 0; j < discrete_dimension_size ; ++j )
		{
			node_save_load_data< tree_space_param_type > d;
			tree( i, j )->save( cfg["trees"][i][j], d );
		}

}
#endif //MASTER_GOLD

template< class  space_param >
inline void	action_value < training_node< typename space_param::tree_space_param_type >, space_param >::load		( const xray::configs::binary_config_value &cfg, action_value_save_load_data< space_param_type, training_sample_type >& data )
{
	training_action_value_save_load_data<space_param_type, training_sample_type>* pdata = data.get_training_save_load_data() ;
			
	if( !pdata )
	{
		super::load( cfg, data );
		return;
	}

	R_ASSERT( pdata );
	

	training_set< space_param_type, training_sample_type >	*set = pdata->set();
	R_ASSERT( set );


	for( u32 i = 0; i < separate_dimensions_count; ++i )
		for( u32 j = 0; j < discrete_dimension_size; ++j )
		{
			
			regression_tree_type*  &t = tree( i, j );
			DELETE( t );

			t = NEW( regression_tree_type )(  set->subset( i, j ) ); 
			

			training_save_load_data< tree_space_param_type > d( set->subset( i, j ).samples(), *t );

			t->load( cfg["trees"][i][j], d );
			
		}

}

template< class regression_tree, class space_param  >
void	action_value_base< regression_tree, space_param >::load		( const xray::configs::binary_config_value &cfg , action_value_save_load_data< space_param_type, training_sample_type >& data  )
{
	XRAY_UNREFERENCED_PARAMETERS( &data );
	for( u32 i = 0; i < space_param::separate_dimensions_count; ++i )
		for( u32 j = 0; j < discrete_dimension_size; ++j )
		{
			node_save_load_data< tree_space_param_type > d;
			tree( i, j )->load( cfg["trees"][i][j], d );
		}
}

template< class regression_tree, class space_param >
bool	action_value_base< regression_tree, space_param >::compare	( const action_value_base< regression_tree, space_param  > &value ) const
{
	for( u32 i = 0; i < separate_dimensions_count; ++i )
		for( u32 j = 0; j < discrete_dimension_size; ++j )
			if( !value.tree( i, j )->compare( *tree( i, j ) ) )
				return false;

	return true;
}

template< class regression_tree, class space_param  >
inline void	action_value_base< regression_tree, space_param >::render	( xray::render::debug::renderer& renderer, const iaction< space_param_type > &a ) const
{
	XRAY_UNREFERENCED_PARAMETERS( renderer, a );
	
	//for( u32 i = 0; i < discrete_dimension_size ; ++i )
			//trees[i]->render( renderer, a );

	//for( u32 i = 0; i < separate_dimensions_count; ++i )
	//	for( u32 j = 0; j < discrete_dimension_size; ++j )
	//		tree( i, j )->render( renderer );
			
}

} // namespace rtp
} // namespace xray

#endif // #ifndef ACTION_VALUE_INLINE_H_INCLUDED