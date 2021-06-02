////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "cylinder_geometry_instance.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/primitives.h>
#include <xray/math_randoms_generator.h>

namespace xray {
namespace collision {

enum cylinder_plane_types_enum
{
	cylinder_plane_type_top_circle,
	cylinder_plane_type_bottom_circle,
	cylinder_plane_type_round_side
};

cylinder_geometry_instance::cylinder_geometry_instance	( float4x4 const& matrix, float radius, float half_length ) :
	m_matrix			( matrix ),
	m_inverted_matrix	( math::invert4x3( matrix ) ),
	m_radius			( radius ),
	m_half_length		( half_length )
{
}

cylinder_geometry_instance::~cylinder_geometry_instance	( )
{
}

bool cylinder_geometry_instance::aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &aabb, &triangles);
	return false; 
}

bool cylinder_geometry_instance::cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &cuboid, &triangles);
	return false; 
}

bool cylinder_geometry_instance::ray_query			( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(object, &origin, &direction, max_distance, distance, &triangles,&predicate );
	return					false;
}

bool cylinder_geometry_instance::aabb_test			( math::aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER		( aabb );
	return true;
}

bool cylinder_geometry_instance::cuboid_test			( math::cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETERS		( &cuboid );
	return true;
}

static void	d_matrix_tmp(  float *out_ode_rotation, float *out_ode_position, const float4x4	&in_m )
{
	//ode_rotation : 4x3 is 12 max index 11 3,7,11 unused

	out_ode_rotation[0] = in_m.i[0]; out_ode_rotation[4] = in_m.i[1]; out_ode_rotation[8] =  in_m.i[2];
	out_ode_rotation[1] = in_m.j[0]; out_ode_rotation[5] = in_m.j[1]; out_ode_rotation[9] =  in_m.j[2];
	out_ode_rotation[2] = in_m.k[0]; out_ode_rotation[6] = in_m.k[1]; out_ode_rotation[10] = in_m.k[2];
	
	out_ode_rotation[3] = out_ode_rotation[7] = out_ode_rotation[11] = 0;

	out_ode_position[0] = in_m.c[0]; out_ode_position[1]=in_m.c[1]; out_ode_position[2]=in_m.c[2]; out_ode_position[3] = 0;
}

static bool cylinder_ray_test	( 	float cylinder_radius,
									float cylinder_half_length,
									math::float3 const& ray_origin, 
									math::float3 const& ray_direction, 
									float max_distance, 
									float& distance )
{
	//(o1) == dCylinderClassUser);
	//(o2) == dRayClass);
	float radius = cylinder_radius;
	//   float lz		= m_half_length* 2.0f;
	float lz2	= cylinder_half_length;

	//const float* R = dGeomGetRotation(o1); // rotation of the cylinder
	//const float* p = dGeomGetPosition(o1); // rotation of the cylinder
	float R[12];
	float p[4];
	d_matrix_tmp(R, p, float4x4().identity());

	float3 start,dir;
	start	= ray_origin;
	dir		= ray_direction;

	float length = max_distance;

	// compute some useful info
	float3 cs,q,r;
	float C,k;
	cs[0] = start[0] - p[0];
	cs[1] = start[1] - p[1];
	cs[2] = start[2] - p[2];

	//   k = dDOT41(R+1, cs.elements);	// position of ray start along cyl axis (Y)
	k = (float3(R[0*4+1], R[1*4+1], R[2*4+1]) | cs);	// position of ray start along cyl axis (Y)

	q[0] = k*R[0*4+1] - cs[0];
	q[1] = k*R[1*4+1] - cs[1];
	q[2] = k*R[2*4+1] - cs[2];

	C = (q | q) - radius*radius;
	// if C < 0 then ray start position within infinite extension of cylinder
	// if ray start position is inside the cylinder
	int inside_cyl=0;
	if (C<0 && !(k<-lz2 || k>lz2)) 
		inside_cyl=1;
	// compute ray collision with infinite cylinder, except for the case where
	// the ray is outside the cylinder but within the infinite cylinder
	// (it that case the ray can only hit endcaps)
	if (!inside_cyl && C < 0) 
	{
		// set k to cap position to check
		if (k < 0) 
			k = -lz2; 
		else 
			k = lz2;
	}else 
	{
		float uv = (float3(R[0*4+1], R[1*4+1], R[2*4+1]) | dir);

		r[0] = uv*R[0*4+1] - dir[0];
		r[1] = uv*R[1*4+1] - dir[1];
		r[2] = uv*R[2*4+1] - dir[2];
		float A = (r | r);
		float B = 2*(q | r);
		k = B*B-4*A*C;
		if (k < 0) 
		{
			// the ray does not intersect the infinite cylinder, but if the ray is
			// inside and parallel to the cylinder axis it may intersect the end
			// caps. set k to cap position to check.
			if (!inside_cyl) 
				return false;

			if (uv < 0) 
				k = -lz2; 
			else 
				k = lz2;
		}else 
		{
			k = math::sqrt(k);
			A = 1.0f / (2*A);
			float alpha = (-B-k)*A;
			if (alpha < 0) 
			{
				alpha = (-B+k)*A;
				if (alpha<0) 
					return false;
			}
			if (alpha>length) 
				return false;
			// the ray intersects the infinite cylinder. check to see if the
			// intersection point is between the caps
			float3 contact_pos, contact_norm;
			contact_pos[0] = start[0] + alpha*dir[0];
			contact_pos[1] = start[1] + alpha*dir[1];
			contact_pos[2] = start[2] + alpha*dir[2];
			q[0] = contact_pos[0] - p[0];
			q[1] = contact_pos[1] - p[1];
			q[2] = contact_pos[2] - p[2];

			//		   k = dDOT14(q.elements, R+1);
			k = (q | float3( R[1+4*0], R[1+4*1], R[1+4*2]) );

			float nsign = inside_cyl ? -1.0f : 1.0f;
			if (k >= -lz2 && k <= lz2) 
			{
				contact_norm[0] = nsign * (contact_pos[0] -
					(p[0] + k*R[0*4+1]));
				contact_norm[1] = nsign * (contact_pos[1] -
					(p[1] + k*R[1*4+1]));
				contact_norm[2] = nsign * (contact_pos[2] -
					(p[2] + k*R[2*4+1]));

				contact_norm.normalize_safe(float3(0,1,0));
				distance = alpha;
				return true;
			}
			// the infinite cylinder intersection point is not between the caps.
			// set k to cap position to check.
			if (k < 0) 
				k = -lz2; 
			else 
				k = lz2;
		}
	}

	math::plane	cap_plane( float3(0,1,0), -k );

	float cap_intersect_dist;
	if( cap_plane.intersect_ray(start, dir, cap_intersect_dist))
	{

		float3 pt_intersect = start + dir*cap_intersect_dist;
		float dist = (pt_intersect-float3(0,k,0)).length();
		if(dist<radius)
		{
			distance = cap_intersect_dist;
			return true;
		}else
		{
			return false;
		}
	}else
		return false;
}

bool cylinder_geometry_instance::ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	float3 const new_origin		= m_inverted_matrix.transform_position(origin);
	float3 const new_direction	= m_inverted_matrix.transform_direction(direction);
	float const new_max_distance= length(m_inverted_matrix.transform_position(origin + direction*max_distance) - new_origin);

	if ( !cylinder_ray_test( m_radius, m_half_length, new_origin, new_direction, new_max_distance, distance ) )
		return					false;

	distance					*= max_distance/new_max_distance;
	return						true;
}

math::aabb cylinder_geometry_instance::get_aabb		( ) const
{
	return	math::create_aabb_min_max (
		float3( -m_radius, -m_half_length, -m_radius ),
		float3( +m_radius, +m_half_length, +m_radius )
	);
}

math::aabb cylinder_geometry_instance::get_geometry_aabb	( ) const
{
	return	math::create_aabb_min_max (
		float3( -m_radius, -m_half_length, -m_radius ),
		float3( +m_radius, +m_half_length, +m_radius )
	);
}

bool cylinder_geometry_instance::is_valid			( ) const
{
	return true;
}

void cylinder_geometry_instance::render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	render			( scene, renderer, m_matrix );
}

void cylinder_geometry_instance::render	( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const
{
	renderer.draw_cylinder	( scene, transform, float3( m_radius, m_half_length, m_radius ), math::color( 255u, 255u, 255u, 255u ) );
}

void cylinder_geometry_instance::enumerate_primitives( enumerate_primitives_callback& cb ) const
{
	cb.enumerate( float4x4().identity(), primitive( cylinder( m_half_length, m_radius ) ) );
}

void cylinder_geometry_instance::enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const
{
	cb.enumerate( transform, primitive( sphere( m_radius ) ) );
}

void cylinder_geometry_instance::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER		( triangles );
	UNREACHABLE_CODE();
}

math::float3 const* cylinder_geometry_instance::vertices	( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 cylinder_geometry_instance::vertex_count				( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 const* cylinder_geometry_instance::indices			( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

u32 const* cylinder_geometry_instance::indices			( u32 triangle_id ) const
{
	XRAY_UNREFERENCED_PARAMETER ( triangle_id );
	NOT_IMPLEMENTED				( return 0 );
}

u32 cylinder_geometry_instance::index_count				( ) const
{
	NOT_IMPLEMENTED				( return 0 );
}

float cylinder_geometry_instance::get_surface_area		( ) const
{
	return 2 * math::pi * m_radius * ( m_radius + 2 * m_half_length );
}

float3 cylinder_geometry_instance::get_random_surface_point( math::random32& randomizer ) const
{
	float const area1					= math::pi * math::sqr( m_radius );
	float const area2					= 2 * math::pi * m_radius * m_half_length;
	float const total_area				= 2 * area1 + area2;

	typedef fixed_vector< float, 6 >	cylinder_planes_type;
	cylinder_planes_type				cylinder_planes;

	cylinder_planes.push_back			( area1 );
	cylinder_planes.push_back			( cylinder_planes[0] + area1 );
	cylinder_planes.push_back			( cylinder_planes[1] + area2 );

	float random_area					= randomizer.random_f( total_area );
	cylinder_planes_type::iterator iter	= std::lower_bound( cylinder_planes.begin(), cylinder_planes.end(), random_area, std::less<float>() );
	R_ASSERT							( iter != cylinder_planes.end() );

	float const previous_area			= iter == cylinder_planes.begin() ? 0 : (*(iter - 1));
	float const delta_area				= ( random_area - previous_area ) / ( *iter - previous_area );
	R_ASSERT							( delta_area >= 0 );
	float const alpha					= math::pi_x2 - delta_area * math::pi_x2;

	cylinder_plane_types_enum const plane_type = cylinder_plane_types_enum( iter - cylinder_planes.begin() );
	
	float3								result;
	switch ( plane_type )
	{
		case cylinder_plane_type_round_side:
		{
			result.y					= -m_half_length + randomizer.random_f( 2.f * m_half_length );
			result.x					= m_radius * math::cos( alpha );
			result.z					= m_radius * math::sin( alpha );
		}
		break;

		case cylinder_plane_type_top_circle:
		case cylinder_plane_type_bottom_circle:
		{
			result.y					= plane_type == cylinder_plane_type_top_circle ? m_half_length : -m_half_length;
			float const radius_random	= -m_radius + randomizer.random_f( 2.f * m_radius );
			float const r_coefficient	= math::sqrt( math::sqr( m_radius ) - math::sqr( radius_random ) );
			result.x					= r_coefficient * math::cos( alpha );
			result.z					= r_coefficient * math::sin( alpha );
		}
		break;
	}
	
	return								result;
}

float3 cylinder_geometry_instance::get_closest_point_to	( float3 const& point, float4x4 const& origin  ) const
{
	float4x4 transform				= origin * m_matrix;
	float3 center					= transform.c.xyz( );
	float3 y_axis					= transform.j.xyz( );
	float3 top_surface_center		= center + m_half_length * y_axis;
	float3 bottom_surface_center	= center - m_half_length * y_axis;
	float3 height_vector			= bottom_surface_center - top_surface_center;
	float proj_to_y_axis			= ( point - top_surface_center ).dot_product( height_vector ) / height_vector.dot_product( height_vector );
	
	if ( proj_to_y_axis > 0.0f && proj_to_y_axis < 1.0f )
	{
		float3 height_vector_proj_point	= top_surface_center + proj_to_y_axis * height_vector;
		float3 dir						= point - height_vector_proj_point;
		if ( ( dir ).squared_length( ) < m_radius * m_radius )
			return point;

		return							height_vector_proj_point + dir.normalize( ) * m_radius;
	}

	float3 surface_center;
	if ( proj_to_y_axis < 0.0f )
		surface_center			= top_surface_center;
	else
		surface_center			= bottom_surface_center;

	float3 circle_point_dir		= point - surface_center;
	float3 proj					= circle_point_dir.dot_product( y_axis ) * y_axis;
	float3 circle_proj_vec		= circle_point_dir - proj;
	if ( circle_proj_vec.squared_length( ) > m_radius * m_radius )
		return surface_center + m_radius * circle_proj_vec.normalize();
	else
		return surface_center + circle_proj_vec;
}

} // namespace collision
} // namespace xray
