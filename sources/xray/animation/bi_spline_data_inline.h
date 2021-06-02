////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_BI_SPLINE_DATA_INLINE_H_INCLUDED
#define XRAY_ANIMATION_BI_SPLINE_DATA_INLINE_H_INCLUDED

namespace xray {
namespace animation {

#ifndef MASTER_GOLD
template< typename Point_type >
void	bi_spline_data< Point_type >::save( xray::configs::lua_config_value config  )const 
{

	const u32 sz = data.size();
	for (u32 i = 0; i < sz; ++i )
	{
		config[i][0] = data[i].first;
		config[i][1] = data[i].second;
	}

}
#endif // MASTER_GOLD

template< typename Point_type >
bool	bi_spline_data< Point_type >::compare( const bi_spline_data & ch_d )const
{
	const u32 sz = data.size();
	if ( ch_d.points_number() != sz ) 
		return false;
	for (u32 i = 0; i < sz-1; ++i )
		if ( data[i].first != ch_d.knot( i )||
			data[i].second !=  ch_d.point( i ) ) 
		  return false;

	return true;
}

template< typename Point_type >
void	bi_spline_data< Point_type >::load( xray::configs::binary_config_value const& config  )
{
	data.resize( config.size() );
	const u32 sz = data.size();
	for (u32 i = 0; i < sz; ++i )
	{
		data[i].first = config[i][0];
		data[i].second = config[i][1];
	}

}

template< typename Point_type >
inline  float	bi_spline_data< Point_type >::knot( u32 i )const  
{ 
	const u32 size_1 =  data.size() - 1;
	ASSERT( i < size_1 + bi_spline_order );
	if ( i > size_1  )
		return data[ size_1 ].first;
	return data[i].first; 
}

template< typename Point_type >
inline void		bi_spline_data< Point_type >::dump()
{
	const u32 sz = data.size();
	for (u32 i = 0; i < sz; ++i )
	{
		LOG_INFO( "knot: %f, value: %f", data[i].first, data[i].second );
		
		
	}
}

template< typename Point_type >
inline void	bi_spline_data< Point_type >::write( stream &file )const
{
	const u32 size = data.size();
	fwrite( &(size), sizeof(size), 1, file );
	
	fwrite( &(*data.begin()), sizeof( knot_point ) , size, file );
}

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_BI_SPLINE_DATA_INLINE_H_INCLUDED
