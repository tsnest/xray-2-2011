////////////////////////////////////////////////////////////////////////////
//	Created		: 19.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MAYA_ANIMATION_RAW_BI_SPLINE_H_INCLUDED
#define XRAY_MAYA_ANIMATION_RAW_BI_SPLINE_H_INCLUDED

#include <xray/maya_animation/api.h>
#include <xray/animation/bi_spline_data.h>

struct SISLCurve;

namespace xray {
namespace maya_animation {

template < typename Point_type, int Dimension >
class raw_bi_spline {
public:
	inline			raw_bi_spline		( ) : m_curve( 0 ) { }
					~raw_bi_spline		( );	
public:
			void	evaluate			( float time, Point_type &point )const								;
			void	get_data			( animation::bi_spline_data<Point_type> &data )const				;
			void	build_approximation	( const float* points, u32 number, const Point_type &max_epsilon  )	;

private:
	SISLCurve*		m_curve;
}; // class raw_bi_spline

} // namespace maya_animation
} // namespace xray

#endif // #ifndef XRAY_MAYA_ANIMATION_RAW_BI_SPLINE_H_INCLUDED