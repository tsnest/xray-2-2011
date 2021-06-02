////////////////////////////////////////////////////////////////////////////
//	Created		: 22.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_POLY_CURVE_H_INCLUDED
#define XRAY_ANIMATION_POLY_CURVE_H_INCLUDED

#include <xray/animation/bi_spline_data.h>
#include <xray/animation/time_channel.h>

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

static	size_t				count_internal_memory_size	( bi_spline_data<point_type> const &cv );
		void				create_in_place_internals	( bi_spline_data<point_type> const &cv, void* memory_buff );


public:
inline	void				evaluate 			( float t, point_type &p, u32 &current_domain )const;
inline	point_type			value				( float t ) const ;

private:
		
public:
		void				dump				() const;

public:
inline	float				min_param			()const{ return  m_time_channel.min_param(); }
inline	float				max_param			()const{ return  m_time_channel.max_param(); }
inline	u32					num_points			()const{ return  m_time_channel.domains_count(); }

inline		bool			is_cyclic			( point_type const &epsilon )const;
inline		point_type		cycle_divergence	( )const;

private:

time_channel<DomainType> m_time_channel;

}; // class poly_curve

} // namespace animation
} // namespace xray

#include <xray/animation/poly_curve_inline.h>

#endif // #ifndef XRAY_ANIMATION_POLY_CURVE_H_INCLUDED