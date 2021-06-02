////////////////////////////////////////////////////////////////////////////
//	Created		: 22.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include <xray/configs_lua_config.h>


using xray::animation::poly_curve_order3_domain;


namespace xray {
namespace animation {

template< class Point_type, u32 Dimension  >
void	poly_curve_order3_domain< Point_type, Dimension >::evaluate( float t, Point_type &p )const
{
	p = t * (t * (t * m_coeff[3] + m_coeff[2]) + m_coeff[1]) + m_coeff[0];
}

template< class Point_type, u32 Dimension  >
void	read( poly_curve_order3_domain< Point_type, Dimension > &d, xray::configs::lua_config_value const& config )
{
	for( u8 i = 0; i < coeff_number; ++i )
		read( d[i] )
}

template< class Point_type, u32 Dimension  >
void	write( const poly_curve_order3_domain< Point_type, Dimension > &d, xray::configs::lua_config_value const& config )
{
	for( u8 i = 0; i < coeff_number; ++i )
		write( d[i] )
}

template< class DomainType >
inline void	poly_curve< DomainType >::read( xray::configs::lua_config_value const& config )
{

	{
		R_ASSERT( config["domains"].size() + 1 ==  config["knots"].size() );
		
		m_domains.resize( config["domains"].size() );

		vector<DomainType>::iterator i,  b = m_domains.begin( ), e = m_domains.end() ;

		for( i = b ; i!= e; ++i )
			animation::read ( *i, config["domains"][ i - b ] );
	}

	{
		m_knots.resize( config["knots"].size() );

		vector<float>::iterator i,  b = m_knots.begin( ), e = m_knots.end() ;

		for( i = b ; i!= e; ++i )
			(*i) = config["knots"][ i - b ];
	}

}

template< class DomainType >
void	poly_curve< DomainType >::init_first_knote( float t )
{
	
	R_ASSERT( m_domains.empty() );
	R_ASSERT( m_knots.empty() );
	m_knots.push_back( t );

}

template< class DomainType >
void	poly_curve< DomainType >::add_domain( const DomainType &d, float interval )
{
	R_ASSERT( m_knots.size() > 0 );
	R_ASSERT( m_knots.size() + 1 == m_domains.size() );
	R_ASSERT( interval >= 0 );

	m_domains.push_back( d );
	
	m_knots.push_back(  m_knots[ m_knots.size() - 1 ] + interval );
}

template< class DomainType >
inline void	poly_curve< DomainType >::write( xray::configs::lua_config_value const& config )const
{
	
	{
		vector<DomainType>::const_iterator i , b = m_domains.begin( ), e = m_domains.end() ;
		
		for( i = b ; i!= e; ++i )
			animation::write ( *i, config["domains"][ i - b ] );
	}

	{
		vector<float>::const_iterator i , b = m_knots.begin( ), e = m_knots.end() ;
		
		for( i = b ; i!= e; ++i )
			config["knots"][ i - b ] = *i;
	}

}

template< class DomainType >
u32	poly_curve< DomainType >::domain( float t )const
{
	const	u32 sz = m_knots.size();
	math::clamp( t, m_knots[0], m_knots[sz-1] );
	CURE_ASSERT( t >= m_knots[0], t = m_knots[0] );
	CURE_ASSERT( t <= m_knots[sz-1], t = m_knots[sz-1] );
	
	

	for( u32 i= 0; i < sz-1 ; ++i )
	{
		u32 j = (i + m_previous_domain) %(sz-1);
		if( m_knots[j] <= t && m_knots[j+1] >= t )
		{
			m_previous_domain = j;
			return j;
		}
		
	}
	
	NODEFAULT(return u32(-1));
}

template< class DomainType >
void	poly_curve< DomainType >::evaluate( float t, point_type &p )const
{
	u32 i = domain( t );
	m_domains[i].evaluate( t - m_knots[i], p );
}

template<class DomainType >
typename DomainType::point_type poly_curve< DomainType >::value( float t ) const 
{ 
	point_type p; 
	evaluate( t, p );
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

template< class DomainType >
void poly_curve< DomainType >::create( const bi_spline_data< point_type > &cv, float )
{
	m_knots.clear();
	m_domains.clear();

	//ASSERT( cv->cuopen );
	//ASSERT( cv->idim == DomainType::dimension );
	//ASSERT( cv->ikind == 1 );
	//ASSERT( cv->ik == DomainType::coeff_number );

	const	int		coeff_number	= DomainType::coeff_number;
	const	float	eps				= 0.0000001f;
			float	save_nv;//			= -math::infinity;

	for( u32 i = coeff_number-1; i < cv.points_number()-1; ++i )
	{
		float n_v		=	cv.knot( i );
		float n_v_next =	cv.knot( i+1 );

		if( (n_v_next - n_v ) < eps )
			continue;

		save_nv = n_v;

		DomainType domain;
		get_spline_params< point_type, DomainType::dimension >( cv, i, domain.m_coeff );
		m_domains.push_back( domain );
		m_knots.push_back( n_v );
	}

	m_knots.push_back( cv.knot( cv.points_number()-1 ) );

}

} // namespace animation
} // namespace xray