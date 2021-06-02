////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_SHADOW_CASCADE_VOLUME_H_INCLUDED
#define XRAY_RENDER_ENGINE_SHADOW_CASCADE_VOLUME_H_INCLUDED

#include "ray.h"

namespace xray {
namespace render {

const u32 LIGHT_CUBOIDSIDEPOLYS_COUNT	= 4;
const u32 LIGHT_CUBOIDVERTICES_COUNT	= 2*LIGHT_CUBOIDSIDEPOLYS_COUNT;

class	shadow_cascade_volume
{
public:
typedef		fixed_vector<math::plane,16>			cascade_volume_planes;

	struct	polygon
	{
		int					points[4];
		math::plane			plane;
	};

	fixed_vector<ray, 8>	view_frustum_rays;
	ray						view_ray;
	ray						light_ray;
	float3					light_cuboid_points	[LIGHT_CUBOIDVERTICES_COUNT];
	polygon					light_cuboid_polys	[LIGHT_CUBOIDSIDEPOLYS_COUNT];

public:

	void				compute_planes	()
	{
		for (u32 it=0; it< LIGHT_CUBOIDSIDEPOLYS_COUNT; it++)
		{
			polygon&	poly	=	light_cuboid_polys[it];

			poly.plane = math::create_plane( light_cuboid_points[poly.points[0]], light_cuboid_points[poly.points[2]], light_cuboid_points[poly.points[1]] );

// #if		DEBUG
// 			float3&		p0	= light_cuboid_points[poly.points[0]];
// 			float3&		p1	= light_cuboid_points[poly.points[1]];
// 			float3&		p2	= light_cuboid_points[poly.points[2]];
// 			float3&		p3	= light_cuboid_points[poly.points[3]];
// 			math::plane	p012;	p012.build(p0,p1,p2);
// 			math::plane	p123;	p123.build(p1,p2,p3);
// 			math::plane	p230;	p230.build(p2,p3,p0);
// 			math::plane	p301;	p301.build(p3,p0,p1);
// 			ASSERT	(p012.normal.similar(p123.normal) && p012.normal.similar(p230.normal) && p012.normal.similar(p301.normal));
// #endif
		}
	}

	void	compute_caster_model_fixed(	cascade_volume_planes& dest, float3& translation, float map_size, bool clip_by_view_near, bool align_test0, bool align_test1, bool align_test2)
	{
		(void)&align_test2;
		translation.set( 0.f, 0.f, 0.f );

		if( math::is_zero( 1-abs(view_ray.direction.dot_product( light_ray.direction )), math::epsilon_7) )
			return;
		
		// compute planes for each polygon.
		compute_planes();
		
		for( u32 i = 0; i < LIGHT_CUBOIDSIDEPOLYS_COUNT; i++ )
		{
			ASSERT( light_cuboid_polys[i].plane.classify(light_ray.origin) > 0);
		}

		int	align_planes[2];
		int	align_planes_count = 0;

		// find one or two planes that align to view frustum from behind. 
		for( u32 i = 0; i < LIGHT_CUBOIDSIDEPOLYS_COUNT; i++ )
		{
			float tmp_dot = view_ray.direction.dot_product(light_cuboid_polys[i].plane.normal);
			if( tmp_dot <= math::epsilon_3)
				continue;

			align_planes[align_planes_count] = i;
			++align_planes_count;

			if( align_planes_count == 2 )
				break;
		}


		float3 align_vector; 
		align_vector.set( 0.f, 0.f, 0.f );

		// Align ray points to the align planes.
		if (align_test0)
		for( int p = 0; p < align_planes_count; ++p )
		{
			// Hack !
			float min_dist = 10000;
			for( u32 i = 0; i < view_frustum_rays.size(); ++i )
			{	
				float tmp_dist = 0;
				float3 tmp_point = view_frustum_rays[i].origin;

				tmp_dist = light_cuboid_polys[align_planes[p]].plane.classify( tmp_point );
				min_dist = math::min( tmp_dist, min_dist );
			}

			float3 shift = light_cuboid_polys[align_planes[p]].plane.normal;
			shift		*=	( min_dist );
			align_vector +=		( shift );
		}

		translation += align_vector;

		// Move light ray by the alignment shift.
		light_ray.origin +=	align_vector;

		// Here we can skip this stage us in the next pass we need only normals of planes.
		// in the next translate_light_model call will contain this shift as well.
		// translate_light_model	( align_vector );

		// Reset to reuse.
		align_vector.set( 0.f, 0.f, 0.f );

		// Check if view edges intersect, and push planes................ 
		if (align_test1)
		for( int p = 0; p < align_planes_count; ++p )
		{
			float max_mag = 0;
			for( u32 i = 0; i < view_frustum_rays.size(); ++i )
			{
				float plane_dot_ray = view_frustum_rays[i].direction.dot_product( light_cuboid_polys[align_planes[p]].plane.normal );
				if( plane_dot_ray < 0 )
				{
					float3 per_plane_view;
					per_plane_view		= math::cross_product( light_cuboid_polys[align_planes[p]].plane.normal, view_ray.direction );
					float3 per_view_to_plane;
					per_view_to_plane	= math::cross_product( per_plane_view, view_ray.direction );
					
//					float d  = math::normalize(view_frustum_rays[i].direction).dot_product(math::normalize(per_view_to_plane));
//					float d1 = view_frustum_rays[i].direction.dot_product(per_view_to_plane);
					
					float tmp_mag = -plane_dot_ray / view_frustum_rays[i].direction.dot_product(per_view_to_plane);
					
					max_mag = (max_mag < tmp_mag) ?  tmp_mag : max_mag;
				}
			}

			if( math::is_zero( max_mag ) )
				continue;

			// Not critical.
			//ASSERT ( max_mag <= 1.f );

			float dist = -light_cuboid_polys[align_planes[p]].plane.normal.dot_product( translation );
			align_vector += (light_cuboid_polys[align_planes[p]].plane.normal*(dist*max_mag));
		}
		
		translation	+=			 align_vector;
		light_ray.origin +=		 align_vector;
		translate_light_model	( translation );
		
		// compute culling planes by rays as edges
		for( u32 i = 0; i< view_frustum_rays.size(); ++i )
		{
			float3 tmp_vector;
			tmp_vector	= math::cross_product( view_frustum_rays[i].direction, light_ray.direction );

			// check if the vectors are parallel
			if( math::is_zero( tmp_vector.squared_length(), math::epsilon_5) )
				continue;

			tmp_vector.normalize();
			math::plane tmp_plane  = create_plane_normalized( tmp_vector, view_frustum_rays[i].origin);
			
			float sign = 0;
			if( check_cull_plane_valid( tmp_plane, sign, 5 ) )
			{
				tmp_plane.normal	*= -sign;
				tmp_plane.d			*= -sign;
				dest.push_back( tmp_plane );
			}
		}

		// Compute culling planes by ray points pairs as edges
		if( clip_by_view_near && abs(view_ray.direction.dot_product( light_ray.direction )) < 0.8 )
		{
			float3 perp_light_view, perp_light_to_view;
			perp_light_view		= math::cross_product( view_ray.direction, light_ray.direction );
			perp_light_to_view	= math::cross_product( perp_light_view, light_ray.direction );

			perp_light_to_view.normalize();
			math::plane plane = math::create_plane_normalized( perp_light_to_view, view_ray.origin);

			float max_dist = -1000;
			for( u32 i = 0; i< view_frustum_rays.size(); ++i )
				max_dist = math::max( plane.classify( view_frustum_rays[i].origin ), max_dist);

			for( u32 i = 0; i< view_frustum_rays.size(); ++i )
			{
				float3 origin = view_frustum_rays[i].origin;
				origin += ( view_frustum_rays[i].direction * 5);

				if( plane.classify( origin ) > max_dist)
				{
					max_dist = 0.f;
					break;
				}
			}

			if( max_dist > -1000 )
			{
				plane.d += max_dist;
				dest.push_back(plane);
			}
		}

		for( u32 i = 0; i < LIGHT_CUBOIDSIDEPOLYS_COUNT; i++ )
		{
			dest.push_back( light_cuboid_polys[i].plane );
			dest.back().normal	*= -1;
			dest.back().d		*= -1;
			ASSERT( light_cuboid_polys[i].plane.classify(light_ray.origin) > 0);
		}

		// Compute ray intersection with light model, this is needed for the next cascade to start it's placement.
		for( u32 i = 0; i < view_frustum_rays.size(); ++i )
		{
			float min_dist = 2*map_size;
			for( int p = 0; p < 4; ++p )			
			{
				float dist;
				if( ( light_cuboid_polys[p].plane.normal.dot_product(view_frustum_rays[i].direction) ) > -0.1 )
					dist = map_size;
				else
					light_cuboid_polys[p].plane.intersect_ray( view_frustum_rays[i].origin, view_frustum_rays[i].direction, dist);

				if( dist > math::epsilon_3 && dist < min_dist )
					min_dist = dist;
			}

			view_frustum_rays[i].origin	+=	( view_frustum_rays[i].direction*min_dist );
		}
	}

	bool check_cull_plane_valid( math::plane const &plane, float &sign, float mad_factor = 0.f )
	{
		bool	valid = false;
		bool	oriented = false;
		float	orient = 0;
		for( u32 j = 0; j< view_frustum_rays.size(); ++j )
		{
			float	tmp_dist = 0.f;
			float3	tmp_pt = view_frustum_rays[j].origin;
			tmp_pt	+= (  view_frustum_rays[j].direction * mad_factor );
			tmp_dist = plane.classify( tmp_pt );

			if( math::is_zero(tmp_dist, math::epsilon_3))
				continue;

			if( !oriented )
			{
				orient = tmp_dist > 0.f ? 1.f : -1.f;
				valid = true;
				oriented = true;
				continue;
			}

			if( tmp_dist < 0 && orient < 0 || tmp_dist > 0 && orient > 0) 
				continue;

			valid = false;
			break;
		}
		sign = orient;
		return valid;
	}

	void translate_light_model( float3 translate )
	{
		for( int i = 0; i < LIGHT_CUBOIDSIDEPOLYS_COUNT; ++i )
			light_cuboid_polys[i].plane.d -= translate.dot_product	(light_cuboid_polys[i].plane.normal);
	}
};

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_SHADOW_CASCADE_VOLUME_H_INCLUDED
