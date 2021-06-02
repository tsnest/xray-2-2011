////////////////////////////////////////////////////////////////////////////
//	Created		: 27.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////


namespace xray {
namespace rtp {




template < class training_sample_type >
inline training_sub_set< training_sample_type >::training_sub_set( ) :
	m_min_in_leaf_samples( min_in_leaf_samples_default )
{

}

template <class space_param,  class training_sample_type >
inline void training_set< space_param, training_sample_type >::synchronize			()
{
	for( u32 i = 0; i < subset_count; ++ i )
		m_sub_sets[i].synchronize( );
}


template < class training_sample_type >
inline void training_sub_set< training_sample_type >::synchronize			()
{
	m_samples.insert( m_samples.end(), m_add_samples.begin(), m_add_samples.end() );
	m_p_samples.resize( m_samples.size() );
	typename rtp::vector< training_sample_type >::iterator i, b = m_samples.begin(), e = m_samples.end();

	for( i = b; i != e  ; ++i )
		m_p_samples[ u32( i-b ) ] = &(*i) ;

	m_add_samples.clear();
}


//template <class space_param,  class training_sample_type >
//inline void training_set< space_param, training_sample_type >::add( u32 separate_dim, training_sample_type const& sample )
//{
//	R_ASSERT( sample.first.discrete_value( ) < discrete_dimension_size );	
//	
//	m_sub_sets[ sample.first.discrete_value( ) ].add( sample );
//}

template <class space_param,  class training_sample_type >
inline void	training_set< space_param, training_sample_type >::add(  const space_param_type& param, separate_reward_type const& reward )
{
	R_ASSERT( param.discrete_value( ) < discrete_dimension_size );
	for( u32 i = 0; i < separate_dimensions_count ; ++i )
			subset( i, param.discrete_value( )	).add( training_sample_type( param.separate_params( i ), reward[ i ], param.training_data() ) );
	
	//m_sub_sets[ sample.first.discrete_value( ) ].add( sample );
}

template < class training_sample_type >
inline void training_sub_set< training_sample_type >::add( training_sample_type const& sample )
{
	//m_samples.push_back( sumple );
	m_add_samples.push_back( sample );
}

template <class space_param,  class training_sample_type >
inline void	training_set< space_param, training_sample_type >::randomize( )
{
	for( u32 i = 0; i < subset_count; ++ i )
		m_sub_sets[i].randomize( );
}

template < class training_sample_type >
inline void	training_sub_set< training_sample_type >::randomize( )
{
	std::random_shuffle( m_samples.begin(), m_samples.end(), rtp_world::random() );
}


#ifndef MASTER_GOLD

template <class space_param,  class training_sample_type >
inline void training_set< space_param, training_sample_type >::save( xray::configs::lua_config_value cfg )const
{
	for( u32 i = 0; i < subset_count; ++ i )
		m_sub_sets[i].save(  cfg[i] );
}
#endif // MASTER_GOLD

#ifndef MASTER_GOLD
template <class training_sample_type >
inline void training_sub_set< training_sample_type >::save( xray::configs::lua_config_value cfg )const
{
	cfg["min_samples"] = m_min_in_leaf_samples;
	const u32 num_samples = m_samples.size();
	cfg["num_samples"] = num_samples;
	for( u32 i = 0; i<num_samples; ++i )
	{
		m_samples[i].param.save( cfg["samples"][i]["param"] );
		cfg["samples"][i]["value"] = m_samples[i].reward;
	}
}
#endif // MASTER_GOLD


template <class space_param,  class training_sample_type >
inline void training_set< space_param, training_sample_type >::load( const xray::configs::binary_config_value &cfg )
{
	for( u32 i = 0; i < subset_count; ++ i )
		m_sub_sets[i].load(  cfg[i] );
}


template < class training_sample_type >
inline void training_sub_set< training_sample_type >::load( const xray::configs::binary_config_value &cfg )
{
	m_min_in_leaf_samples = cfg["min_samples"];
	const u32 num_samples = cfg["num_samples"];

	m_samples.resize( num_samples );
	for( u32 i = 0; i<num_samples; ++i )
	{
		m_samples[i].param.load( cfg["samples"][i]["param"] );
		m_samples[i].reward = cfg["samples"][i]["value"];
	}
}




//template < class training_sample_type >
//rtp::vector< training_sample_type >	&training_set< training_sample_type >::samples( u32 discrete_dim )	
//{ 
//	R_ASSERT( discrete_dim < discrete_dimension_size );	
//	return m_sub_sets[discrete_dim].samples(); 
//}
//
//template < class training_sample_type >
//const rtp::vector< training_sample_type >	&training_set< training_sample_type>::samples( u32 discrete_dim )const	
//{ 
//	R_ASSERT( discrete_dim < discrete_dimension_size );	
//	return m_sub_sets[discrete_dim].samples(); 
//}

template <class space_param,  class training_sample_type >
u32	training_set< space_param, training_sample_type >::samples_number		()const
{
	u32 res = 0;
	for( u32 i = 0; i < subset_count; ++ i )
		res += m_sub_sets[i].samples().size();
	return res;
}


template <class space_param,  class training_sample_type >
training_sub_set<training_sample_type>&	training_set< space_param, training_sample_type >::subset( u32 subset_idx  )
{
	R_ASSERT( subset_idx < subset_count );	
	return m_sub_sets[subset_idx]; 
}

template <class space_param,  class training_sample_type >
const training_sub_set<training_sample_type>& training_set< space_param, training_sample_type >::subset( u32 subset_idx  )const
{
	R_ASSERT( subset_idx < subset_count );	
	return m_sub_sets[subset_idx]; 
}

template <class space_param,  class training_sample_type >
training_sub_set<training_sample_type>& training_set< space_param, training_sample_type >::subset( u32 separate_dim, u32 discrete_dim )
{
	return subset( index( separate_dim, discrete_dim ) );
}

template <class space_param,  class training_sample_type >
const training_sub_set<training_sample_type>&  training_set< space_param, training_sample_type >::subset( u32 separate_dim, u32 discrete_dim )const
{
	return subset( index( separate_dim, discrete_dim ) );
}

template <class space_param,  class training_sample_type >
u32	training_set< space_param, training_sample_type >::index( u32 separate_dim, u32 discrete_dim )const
{
	ASSERT( separate_dim < separate_dimensions_count  );
	ASSERT( discrete_dim < discrete_dimension_size );
	
	u32 idx = separate_dim * discrete_dimension_size + discrete_dim;
	ASSERT( idx < subset_count );
	return idx;
}

template <class space_param,  class training_sample_type >
space_param	training_set< space_param, training_sample_type >::get_param( u32 idx, u32 discrete_dim )const
{
	ASSERT( idx < get_samples_count( discrete_dim ) );

	space_param ret;
	ret.discrete_value( ) = discrete_dim;

	for( u32 i = 0; i < separate_dimensions_count ; ++i )
	{
		training_sample_type const & sample =  subset( i, discrete_dim ).samples()[idx];
		ret.separate_params( i ) = sample.param;
		ret.training_data() = sample.space_param_training_data;
	}
	return ret;
}

template <class space_param,  class training_sample_type >
u32	training_set< space_param, training_sample_type >::get_samples_count	( u32 discrete_dim )const
{
	u32 const count = subset( 0, discrete_dim ).samples().size();

#ifdef	DEBUG
	for( u32 i = 1; i < separate_dimensions_count ; ++i )
		if( count != subset( i, discrete_dim ).samples().size() )
		{
			NODEFAULT( );
			return u32(-1);
		}
#endif

	return count;
}

template <class space_param,  class training_sample_type >
void		training_set< space_param, training_sample_type >::set_value	( u32 idx, u32 discrete_dim, separate_reward_type const& v)
{

	ASSERT( idx < get_samples_count( discrete_dim ) );

	for( u32 i = 0; i < separate_dimensions_count ; ++i )
		 subset( i, discrete_dim ).samples()[idx].reward = v[ i ];

}


} // namespace rtp
} // namespace xray