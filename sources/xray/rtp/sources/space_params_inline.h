////////////////////////////////////////////////////////////////////////////
//	Created		: 31.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SPACE_PARAMS_INLINE_H_INCLUDED
#define SPACE_PARAMS_INLINE_H_INCLUDED

#include "rtp_world.h"
#include <xray/configs.h>
#include <xray/math_randoms_generator.h>

namespace xray {
namespace rtp {




//template< class space_param_type >
//inline bool in_range( const space_param_type &v, const space_param_type &min, const space_param_type &max )
//{
//	for(u32 i=0; i < space_param_type::dimensions_number; ++i )
//		if( !in_range( v[i], min[i], max[i] ) )
//			return false;
//	return true;
//}


template< class space_param_type >
inline void clamp( space_param_type &v, const space_param_type &min, const space_param_type &max )
{
	for(u32 i=0; i < space_param_type::dimensions_number; ++i )
		if( !in_range( v[i], min[i], max[i] ) )
				math::clamp( v[i], min[i], max[i] );

}



template<class space_param_type>
inline bool in_range( const space_param_type &v )
{
	return v.in_range();
}

template<class space_param_type>
inline bool in_area( const space_param_type &v )
{
	return v.in_area();
}


template<class space_param_type>
inline void clamp_to_area( space_param_type &v )
{
	v.clamp_to_area();
}


template<class space_param_type>
inline void clamp_to_range( space_param_type &v )
{
	v.clamp_to_range();
}


template < u32 dimension, class FinalType >
void	tree_space_param<dimension, FinalType >::set_random	( tree_space_param<dimension, FinalType > const &min, tree_space_param<dimension, FinalType > const &max, math::random32_with_count &r )
{
	
	for( u32 i = 0; i < dimension; ++i )
	{
		float range = max[i] - min[i];
		params[i] = min[i] + float( random( r ) * range );
	}
}


template< u32 dimension,  class FinalType >
bool	tree_space_param<dimension, FinalType >::similar	( const tree_space_param< dimension, FinalType >& l )const
{
	
	for( u32 i = 0; i < dimension; ++i )
		if( !math::is_similar( params[i], l[i] ,math::epsilon_5 ) )
			return false;
	return true;
}


template< u32 dimension, class FinalType >
bool	tree_space_param<dimension, FinalType >::equal	( const tree_space_param< dimension, FinalType >& l )const
{
	
	for( u32 i = 0; i < dimension; ++i )
		if( params[i] != l[i]  )
			return false;
	return true;
}

#ifndef	MASTER_GOLD
template< u32 dimension, class FinalType >
void	tree_space_param<dimension, FinalType >::save	( xray::configs::lua_config_value cfg )const
{
	for( u32 i = 0; i < dimension; ++i )
		cfg[i] = params[i];
}
#endif // #ifndef MASTER_GOLD

template< u32 dimension, class FinalType >
void	tree_space_param<dimension, FinalType >::load	( const xray::configs::binary_config_value &cfg )
{
	for( u32 i = 0; i < dimension; ++i )
		 params[i] = cfg[i];
}


template< u32 dimension, class FinalType >
inline float tree_space_param<dimension, FinalType >::operator [] ( u32 d ) const
{
	ASSERT( d < dimension );
	return params[ d ];
}

template< u32 dimension, class FinalType >
float &tree_space_param<dimension, FinalType >::operator []	( u32 d )
{
	ASSERT( d < dimension );
	return params[ d ];
}

template< u32 dimension, class FinalType >
inline bool tree_space_param<dimension, FinalType >::in_range( tree_space_param<dimension, FinalType > const& min, tree_space_param<dimension, FinalType > const& max ) const
{

	for(u32 i=0; i < dimensions_number; ++i )
		if( ! rtp::in_range( (*this)[i], min[i], max[i] ) )
			return false;
	return true;

	//return rtp::in_range( *this, final_type::min, final_type::max );
}

template< u32 dimension, class FinalType >
inline bool tree_space_param<dimension, FinalType >::in_area ( )const
{
	//return rtp::in_range( *this, final_type::area_min, final_type::area_max );
	for(u32 i=0; i < dimensions_number; ++i )
		if( ! rtp::in_range( (*this)[i], area_min[i], area_max[i] ) )
			return false;
	return true;
}

template< u32 dimension, class FinalType >
inline void tree_space_param<dimension, FinalType >::clamp_to_range	( tree_space_param<dimension, FinalType > const &min, tree_space_param<dimension, FinalType > const &max )
{
	clamp<self_type>( *this, min, max );
}

template< u32 dimension, class FinalType >
inline void tree_space_param<dimension, FinalType >::clamp_to_area(  )
{
	clamp<self_type>( *this, area_min, area_max );
}

template< u32 separate_dimensions, u32 discrete_size, class TreeParamType, class FinalType >
inline bool space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::in_range(  ) const
{
		for(u32 i = 0; i < separate_dimensions; ++ i)
		if( !separate_params( i ).in_range( min.separate_params( i ), max.separate_params( i ) ) )
			return false;
	return true;
}

template< u32 separate_dimensions,  u32 discrete_size, class TreeParamType, class FinalType >
inline bool space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::in_area(  ) const
{
	for(u32 i = 0; i < separate_dimensions; ++ i)
		if( !separate_params( i ).in_area( ) )
			return false;
	return true;
}

template< u32 separate_dimensions, u32 discrete_size, class TreeParamType, class FinalType >
inline void space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::clamp_to_range	(  )
{
	for(u32 i = 0; i < separate_dimensions; ++ i)
		separate_params( i ).clamp_to_range( min.separate_params( i ), max.separate_params( i ) );
}

template< u32 separate_dimensions, u32 discrete_size, class TreeParamType, class FinalType >
inline void space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::clamp_to_area(  )
{
	for(u32 i = 0; i < separate_dimensions; ++ i)
		separate_params( i ).clamp_to_area( );
}


template< u32 separate_dimensions, u32 discrete_size, class TreeParamType, class FinalType >
void	space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::set_random	( math::random32_with_count &r )
{
	for(u32 i = 0; i < separate_dimensions; ++ i)
		separate_params( i ).set_random( min.separate_params( i ), max.separate_params( i ), r );
	
	m_discrete_dimension_value = r.random( discrete_dimension_size );
}

template< u32 separate_dimensions, u32 discrete_size, class TreeParamType, class FinalType >
bool	space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::equal			( const space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>& l )const
{
		if( discrete_value	( ) != l.discrete_value	( ) )
			return false;
		for(u32 i = 0; i < separate_dimensions; ++ i)
			if( !separate_params( i ).equal( l.separate_params( i ) ) )
				return false;
		return true;
}

template< u32 separate_dimensions, u32 discrete_size, class TreeParamType, class FinalType >
bool	space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::similar			( const space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>& l )const
{
		
		if( discrete_value	( ) != l.discrete_value	( ) )
			return false;

		for(u32 i = 0; i < separate_dimensions; ++ i)
			if( !separate_params( i ).similar( l.separate_params( i ) ) )
				return false;
		return true;
}
	


#ifndef MASTER_GOLD
template< u32 separate_dimensions, u32 discrete_size, class TreeParamType, class FinalType >
void	space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::save			( xray::configs::lua_config_value cfg )const
{
		for(u32 i = 0; i < separate_dimensions; ++ i)
			separate_params( i ).save( cfg[i] );
}
#endif //MASTER_GOLD

template< u32 separate_dimensions, u32 discrete_size, class TreeParamType, class FinalType >
void					space_param<separate_dimensions, discrete_size, TreeParamType, FinalType>::load			( const xray::configs::binary_config_value &cfg )
{
	for(u32 i = 0; i < separate_dimensions; ++ i)
		separate_params( i ).load( cfg[i] );
}

template<u32 separate_dimensions>
inline	separate_reward<separate_dimensions>::separate_reward()
{
	for( u32 i = 0; i < separate_dimensions ; ++i )
		value[i] = xray::memory::uninitialized_value<float>();
}

template<u32 separate_dimensions>
inline	float& separate_reward<separate_dimensions>:: operator [] ( u32 i )
{
	ASSERT( i < separate_dimensions );
	return value[i];
}

template<u32 separate_dimensions>
inline	float separate_reward<separate_dimensions>:: operator	[] ( u32 i )const
{
	ASSERT( i < separate_dimensions );
	return value[i];
}

template<u32 separate_dimensions>
inline separate_reward<separate_dimensions>	separate_reward<separate_dimensions>::operator + ( const separate_reward& right )const
{
	separate_reward<separate_dimensions> res;
	for( u32 i = 0; i < separate_dimensions ; ++i )
		res[ i ] = (*this)[i] + right[i];
	return res;
}

template<u32 separate_dimensions>
inline	separate_reward<separate_dimensions> separate_reward<separate_dimensions>::operator - ( const separate_reward& right )const
{
	separate_reward<separate_dimensions> res;
	for( u32 i = 0; i < separate_dimensions ; ++i )
		res[ i ] = (*this)[i] - right[i];
	return res;
}

template<u32 separate_dimensions>
inline	separate_reward<separate_dimensions> separate_reward<separate_dimensions>::operator * ( float v )const
{
	separate_reward<separate_dimensions> res;
	for( u32 i = 0; i < separate_dimensions ; ++i )
		res[ i ] = (*this)[i] * v;
	return res;
}

template<u32 separate_dimensions>
inline	float separate_reward<separate_dimensions>::sum_reward()const
{
	float sum = 0;
	for( u32 i = 0; i < separate_dimensions ; ++i )
		sum += (*this)[i];
	return sum;
}

template<u32 separate_dimensions>
inline	float separate_reward<separate_dimensions>::square_sum_reward()const
{
	float sum = 0;
	for( u32 i = 0; i < separate_dimensions ; ++i )
		sum += ( (*this)[i] * (*this)[i] );
	return sum;
}

template<u32 separate_dimensions>
inline	separate_reward<separate_dimensions>&	separate_reward<separate_dimensions>::set( float v )
{
	for( u32 i = 0; i< separate_dimensions; ++i )
		(*this)[i] = v;
	return *this;
}

template<u32 separate_dimensions>
inline	bool	separate_reward<separate_dimensions>::valid( )
{
	
	for( u32 i = 0; i < separate_dimensions ; ++i )
		if( !math::valid( (*this)[i] ) )
			return false;

	return true;
}

//template<u32 separate_dimensions>
//inline	float separate_reward<separate_dimensions>::max_reward()const
//{
//	float max = -math::infinity;
//	for( u32 i = 0; i < separate_dimensions ; ++i )
//		max = math::max( max,  (*this)[i] );
//	ASSERT( math::valid(max) );
//	return max;
//
//}

} // namespace rtp
} // namespace xray

#endif // #ifndef SPACE_PARAMS_INLINE_H_INCLUDED