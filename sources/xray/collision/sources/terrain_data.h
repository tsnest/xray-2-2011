////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_DATA_H_INCLUDED
#define TERRAIN_DATA_H_INCLUDED

namespace xray {
namespace collision {

struct terrain_data
{
	 
					terrain_data	( float phisical_size, u32 vertex_row_size, const float* heightfield );

			float	quad_size		( )const;	
			float	min_height		( )const;	
			float	max_height		( )const;	
			u32		quad_dimension	( )const;	
	inline	u32		vert_row_size	( )const	{ return m_vertex_row_size; }
	inline	float	physical_size	( )const	{ return m_physical_size; }
			bool	get_row_col		( float3 const& position_local, int& x, int& z ) const;

			bool	ray_test_quad	(	int const y1, int const x1, 
										float3 const& ray_point, 
										float3 const& ray_dir,
										float const max_distance,
										float& range,
										bool log_out ) const;

			float3	position		( u16 vertex_id ) const;
public:
	float				m_physical_size;
	u32					m_vertex_row_size;
	float const* 		m_heightfield;
}; // class terrain_data

} // namespace collision
} // namespace xray

#endif // #ifndef TERRAIN_DATA_H_INCLUDED