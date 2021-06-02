////////////////////////////////////////////////////////////////////////////
//	Created		: 22.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef POLY_CURVE_H_INCLUDED
#define POLY_CURVE_H_INCLUDED

#include "bi_spline_data.h"

namespace xray {

namespace configs {
	class lua_config_value;
} // namespace configs


namespace animation {

template<class Point_type, u32 Dimension >
struct poly_curve_order3_domain
{
	typedef			Point_type	point_type;

	static const	u8			dimension = Dimension;//order + 1
	static const	u8			coeff_number = 4;//order + 1


	inline void		evaluate	( float t, Point_type &p )const;

	Point_type		m_coeff		[coeff_number];

};

template<class Point_type, u32 Dimension  >
void	read		( poly_curve_order3_domain<Point_type, Dimension > &d, configs::lua_config_value const& config );

template<class Point_type, u32 Dimension  >
void	write		( const poly_curve_order3_domain<Point_type, Dimension > &d, configs::lua_config_value const& config );

template<class DomainType >
class poly_curve
{
	typedef		typename DomainType::point_type	point_type;

public:
							poly_curve			( ): m_previous_domain( 0 ){};

public:
inline	void				evaluate 			( float t, point_type &p )const;
inline	point_type			value				( float t ) const ;

public:
		void				create				( const bi_spline_data<point_type> &cv, float param_scale );
		void				read				( configs::lua_config_value const& config );
		void				write				( configs::lua_config_value const& config )const;

		void				init_first_knote	( float t );
		void				add_domain			( const DomainType &d, float interval );

public:
inline	float				min_param			()const{ return  m_knots[0]; }
inline	float				max_param			()const{ return  *(m_knots.end()-1); }
inline	u32					num_points			()const{ return  m_domains.size(); }

private:
inline	u32					domain				( float t )const;

private:
	vector<DomainType>	m_domains;
	vector<float>		m_knots;
	mutable u32			m_previous_domain;

}; // class poly_curve

} // namespace animation
} // namespace xray

#include "poly_curve_inline.h"

#endif // #ifndef POLY_CURVE_H_INCLUDED