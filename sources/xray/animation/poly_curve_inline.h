////////////////////////////////////////////////////////////////////////////
//	Created		: 22.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_ANIMATION_POLY_CURVE_INLINE_H_INCLUDED
#define XRAY_ANIMATION_POLY_CURVE_INLINE_H_INCLUDED

using xray::animation::poly_curve_order3_domain;

namespace xray {
namespace animation {

template< class Point_type, u32 Dimension  >
inline void	poly_curve_order3_domain< Point_type, Dimension >::evaluate( float t, Point_type &p )const
{
	p = t * (t * (t * m_coeff[3] + m_coeff[2]) + m_coeff[1]) + m_coeff[0];
}

template< class Point_type, u32 Dimension  >
inline void	read( poly_curve_order3_domain< Point_type, Dimension > &d, xray::configs::lua_config_value const& config )
{
	for ( u8 i = 0; i < coeff_number; ++i )
		read( d[i] )
}

template< class Point_type, u32 Dimension  >
inline void	write( const poly_curve_order3_domain< Point_type, Dimension > &d, xray::configs::lua_config_value const& config )
{
	for ( u8 i = 0; i < coeff_number; ++i )
		write( d[i] )
}

template< class Point_type, u32 Dimension  > 
inline void dump( const poly_curve_order3_domain< Point_type, Dimension > &d )
{
	for ( u8 i = 0; i <  poly_curve_order3_domain< Point_type, Dimension >::coeff_number; ++i )
		LOG_INFO( " %f ", d.m_coeff[i] );
}


//template< class DomainType >
//inline void	poly_curve< DomainType >::read( xray::configs::lua_config_value const& config )
//{
//
//	{
//		R_ASSERT( config["domains"].size() + 1 ==  config["knots"].size() );
//		
//		domains().resize( config["domains"].size() );
//
//		vector<DomainType>::iterator i,  b = domains().begin( ), e = domains().end() ;
//
//		for ( i = b ; i!= e; ++i )
//			animation::read ( *i, config["domains"][ i - b ] );
//	}
//
//	{
//		knots().resize( config["knots"].size() );
//
//		vector<float>::iterator i,  b = knots().begin( ), e = knots().end() ;
//
//		for ( i = b ; i!= e; ++i )
//			(*i) = config["knots"][ i - b ];
//	}
//
//}



template< class DomainType >
inline void	poly_curve< DomainType >::dump( ) const
{
	{
		vector<DomainType>::const_iterator i , b = domains().begin( ), e = domains().end() ;
		
		for ( i = b ; i!= e; ++i )
			animation::dump ( *i );
	}

	{
		vector<float>::const_iterator i , b = knots().begin( ), e = knots().end() ;
		
		for ( i = b ; i!= e; ++i )
			LOG_INFO( " %f ", *i );
	}

}

template< class DomainType >
void	poly_curve< DomainType >::evaluate( float t, point_type &p, u32 &current_domain )const
{
	u32 i = m_time_channel.domain( t, current_domain );
	m_time_channel.domain( i ).evaluate( t - m_time_channel.knot( i ), p );
}

template< class DomainType >
inline typename DomainType::point_type	poly_curve< DomainType >::cycle_divergence( )const
{
		point_type p0;

		m_time_channel.domain( 0 )
			.evaluate(  min_param( ), p0 );

		point_type p1;
		m_time_channel.domain( m_time_channel.domains_count( ) - 1 )
			.evaluate(  max_param( ), p1 );;

		point_type difference = p1 - p0;
		return difference;
}

template< class DomainType >
inline	bool	poly_curve< DomainType >::is_cyclic( point_type const &epsilon )const
{
	return math::is_zero( cycle_divergence (), epsilon );
}


template<class DomainType >
typename DomainType::point_type poly_curve< DomainType >::value( float t ) const 
{ 
	point_type p; 
	evaluate( t, p, 0 );
	return  p;
}

template< typename Point_type, int Dimension >
inline Point_type point( u32 index, const bi_spline_data< Point_type > &cv )
{
	ASSERT( int(index) >= 0 );
	ASSERT( index < cv.points_number() );
	return cv.point( index );
}

template< class Point_type, u32 Dimension >
void get_spline_params	( const bi_spline_data< Point_type > &cv, u32 index, Point_type point_factors[4] )
{

	float dT[6]						= {
		float(cv.knot(index - 1) - cv.knot(index - 2)),	// delta Tj-1
		float(cv.knot(index + 0) - cv.knot(index - 1)),	// delta Tj
		float(cv.knot(index + 1) - cv.knot(index + 0)),	// delta Tj+1
		float(cv.knot(index + 2) - cv.knot(index + 1)),	// delta Tj+2
		float(cv.knot(index + 3) - cv.knot(index + 2))	// delta Tj+3
	};

	u32 const j = 1;
	float const k0						= 1.f/( dT[j+1]* (dT[j+1] + dT[j]  )* (dT[j+1] + dT[j]   + dT[j-1]) );
	float const k10						= k0;//1.f/( dT[j+1]* (dT[j+1] + dT[j]  )* (dT[j+1] + dT[j]   + dT[j-1]) );
	float const k11						= 1.f/( dT[j+1]* (dT[j+1] + dT[j]  )* (dT[j+1] + dT[j]   + dT[j+2]) );
	float const k12						= 1.f/( dT[j+1]* (dT[j+1] + dT[j+2])* (dT[j+1] + dT[j]   + dT[j+2]) );
	float const k20						= k11;//1.f/( dT[j+1]* (dT[j+1] + dT[j]  )* (dT[j+1] + dT[j]   + dT[j+2]) );
	float const k21						= k12;//1.f/( dT[j+1]* (dT[j+1] + dT[j+2])* (dT[j+1] + dT[j]   + dT[j+2]) );
	float const k22						= 1.f/( dT[j+1]* (dT[j+1] + dT[j+2])* (dT[j+1] + dT[j+2] + dT[j+3]) );
	float const k3						= k22;//1.f/( dT[j+1]* (dT[j+1] + dT[j+2])* (dT[j+1] + dT[j+2] + dT[j+3]) );

	float4x4 factors_matrix;
	math::float4_pod (&factors)[4]		= factors_matrix.lines;
	factors[0]							= float4( k3, -k20-k21-k22, k10+k11+k12, -k0 );
	factors[1]							= float4(
											0,
											k20*(dT[j+1] - 2*dT[j]) + k21*(dT[j+1] + dT[j+2] - dT[j]) + k22*(dT[j+1] + dT[j+2] + dT[j+3]),
											k10*(dT[j-1] + dT[j] -2*dT[j+1]) + k11*(-2*dT[j+1] - dT[j+2] + dT[j]) + k12*(-2*(dT[j+1] + dT[j+2])),
											k0*3*dT[j+1]
										);
	factors[2]							= float4(
											0,
											k20*(2*dT[j]*dT[j+1] - math::sqr(dT[j])) + k21*(dT[j]*(dT[j+1] + dT[j+2])),
											k10*(math::sqr(dT[j+1]) - 2*dT[j+1]*(dT[j-1] + dT[j])) + k11*(-dT[j]*dT[j+1] + (dT[j+1] - dT[j])*(dT[j+1] + dT[j+2])) + k12*(math::sqr(dT[j+1] + dT[j+2])),
											-k0*3*math::sqr(dT[j+1])
										);
	factors[3]							= float4(  0, k20*(dT[j+1]*math::sqr(dT[j])), k10*(math::sqr(dT[j+1])*(dT[j-1] + dT[j])) + k11*(dT[j+1]*dT[j]*(dT[j+1] + dT[j+2])), k0*dT[j+1]*math::sqr(dT[j+1]) );
	float const (&elements)[4][4]		= factors_matrix.elements;

	Point_type	p_j3 = point< Point_type, Dimension >( index-3, cv ), 
				p_j2 = point< Point_type, Dimension >( index-2, cv ), 
				p_j1 = point< Point_type, Dimension >( index-1, cv ),
				p_j0 = point< Point_type, Dimension >( index, cv   );


	point_factors[3] =	elements[0][0]*p_j0 + elements[0][1]*p_j1 + elements[0][2]*p_j2 + elements[0][3]*p_j3;
	point_factors[2] =	elements[1][0]*p_j0 + elements[1][1]*p_j1 + elements[1][2]*p_j2 + elements[1][3]*p_j3;
	point_factors[1] =	elements[2][0]*p_j0 + elements[2][1]*p_j1 + elements[2][2]*p_j2 + elements[2][3]*p_j3;
	point_factors[0] =	elements[3][0]*p_j0 + elements[3][1]*p_j1 + elements[3][2]*p_j2 + elements[3][3]*p_j3;

}

static	const	float	bi_spline_knots_resolution_eps	= 0.0000001f;

template< class DomainType >
u32 poly_knots_count( const bi_spline_data< typename DomainType::point_type > &cv )
{
	u32				knots_cnt		= 0;
	const	int		coeff_number	= DomainType::coeff_number;

	for ( u32 i = coeff_number-1; i < cv.points_number()-1; ++i )
	{
		float n_v		=	cv.knot( i );
		float n_v_next =	cv.knot( i+1 );

		if ( (n_v_next - n_v ) < bi_spline_knots_resolution_eps )
			continue;
		++knots_cnt;
	}
	return knots_cnt + 1;
}

template< class DomainType >
inline size_t poly_curve< DomainType >::count_internal_memory_size	( bi_spline_data<point_type> const &cv )
{
	
	u32 knots_cnt = poly_knots_count< DomainType >( cv );
	
	return time_channel<DomainType>::count_internal_memory_size( knots_cnt ); 

	//return sizeof(DomainType) * ( knots_cnt - 1 ) + sizeof( float ) * knots_cnt;
}




template< class DomainType >
void poly_curve< DomainType >::create_in_place_internals( const bi_spline_data<point_type> &cv, void* memory_buff )
{
	
	m_time_channel.create_in_place( memory_buff, poly_knots_count< DomainType >( cv ) );
	//m_internal_memory_position = size_t ( pbyte( memory_buff ) - pbyte( this ) );

	//m_knots_count = poly_knots_count< DomainType >( cv );
	
	//ASSERT( cv->cuopen );
	//ASSERT( cv.idim == DomainType::dimension );
	//ASSERT( cv.ikind == 1 );
	//ASSERT( cv.ik == DomainType::coeff_number );

	const	int		coeff_number	= DomainType::coeff_number;


	u32 knots_cnt = 0;
	for ( u32 i = coeff_number-1; i < cv.points_number()-1; ++i )
	{
		float n_v		=	cv.knot( i );
		float n_v_next =	cv.knot( i+1 );

		R_ASSERT( knots_cnt <= m_time_channel.knots_count() - 1 );

		if ( ( n_v_next - n_v ) < bi_spline_knots_resolution_eps )
			continue;

		//save_nv = n_v;

		DomainType domain;
		get_spline_params< point_type, DomainType::dimension >( cv, i, domain.m_coeff );
		m_time_channel.domain( knots_cnt ) = domain ;
		m_time_channel.knot( knots_cnt ) = n_v ;
		++knots_cnt;
	}

	R_ASSERT( knots_cnt == m_time_channel.knots_count() -1 );
	m_time_channel.knot( knots_cnt ) = ( cv.knot( cv.points_number()-1 ) );
	R_ASSERT( m_time_channel.knot(  m_time_channel.knots_count() -1 ) > m_time_channel.knot( 0 ) );
}

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_POLY_CURVE_INLINE_H_INCLUDED