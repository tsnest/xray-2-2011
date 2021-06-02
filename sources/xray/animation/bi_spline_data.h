////////////////////////////////////////////////////////////////////////////
//	Created		: 18.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_BI_SPLINE_DATA_H_INCLUDED
#define XRAY_ANIMATION_BI_SPLINE_DATA_H_INCLUDED

#include <xray/animation/animation_containers.h>

namespace xray {

namespace configs{
class lua_config_value;
class binary_config_value;
} // namespace configs

namespace animation {

typedef FILE*	stream;

template< typename Point_type >
class bi_spline_data {

public:
				void				add_point		( const std::pair< float, Point_type > &p  ){ data.push_back( p ); }

public:

#ifndef MASTER_GOLD
				void				save			( xray::configs::lua_config_value config  )const ;
#endif // MASTER_GOLD

				void				load			( xray::configs::binary_config_value const& config  );
				void				dump			();
public:
		inline  u32					points_number	( )		 const  { return data.size(); }
		inline	const Point_type	point			( u32 i )const	{ return data[i].second; }
		inline  float				knot			( u32 i )const ;
				bool				compare			( const bi_spline_data & ch_d )const;

		inline 	void				write			( stream &file )const;


private:
	typedef std::pair< float, Point_type >			knot_point;
	vector< knot_point >	data;

	static const	u32		bi_spline_order	= 4;

}; // class bi_spline_data

}  // namespace animation
}  // namespace xray

#include <xray/animation/bi_spline_data_inline.h>

#endif // #ifndef XRAY_ANIMATION_BI_SPLINE_DATA_H_INCLUDED