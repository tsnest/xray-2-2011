////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_translation.h"
#include "transform_control_object.h"
#include <xray/editor/base/collision_object_types.h>
#include "scene_view_panel.h"

#pragma managed( push, off )
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/geometry_instance.h>
#include <xray/collision/geometry.h>
#include <xray/collision/collision_object.h>
#pragma managed( pop )

namespace xray{
namespace editor_base{

void transform_control_translation::execute_input_step_snap( )
{
	float3 mouse_origin;
	float3 mouse_direction;
	
	m_helper->get_mouse_ray( mouse_origin, mouse_direction );

	float3 curr_point;

	if(!m_plane_mode)
	{
		float3	saved_origin	= get_matrix().c.xyz();
		float3	curr_axis		= get_axis_vector( m_active_axis );

		if( !ray_nearest_point( saved_origin, curr_axis, mouse_origin, mouse_direction, curr_point ) ) 
			return;
	}else
	{
		if( !plane_ray_intersect( *m_mouse_drag_start, get_axis_vector( m_active_plane ), mouse_origin, mouse_direction, curr_point ) )
			return;
	}

	float3 drag_vector = (curr_point - *m_mouse_drag_start);

	if( math::is_zero(drag_vector.length()) )
		return;

	float const step = editor_base::scene_view_panel::m_grid_step;

	float3 quot			= drag_vector;
	quot.x				= (float)math::floor(quot.x/step);
	quot.y				= (float)math::floor(quot.y/step);
	quot.z				= (float)math::floor(quot.z/step);

	float3 rem			= (drag_vector-quot);
	if(rem.x>(step/2.0f)) ++quot.x;
	if(rem.y>(step/2.0f)) ++quot.y;
	if(rem.z>(step/2.0f)) ++quot.z;

	drag_vector			= quot;
	drag_vector			*= step;

	drag_vector			-= (*m_step_snap_rem);

	if(!m_plane_mode)
		drag_vector		*= get_axis_vector( m_active_axis );

	*m_result			= math::create_translation( drag_vector );
}

void transform_control_translation::execute_input_no_snap( )
{
	float3 drag_vector = this->get_drag_vector();
	if( math::is_zero(drag_vector.length()) )
		return;

	if ( m_helper->m_object->translation_ignore_rotation() )
		*m_result = math::create_translation( drag_vector );
	else 
	{
		float4x4 inverted;
		inverted.try_invert( get_matrix() );
		R_ASSERT( inverted.valid() );
		drag_vector = inverted.transform_direction( drag_vector );	
		*m_result = math::create_translation(  drag_vector );
	}
}

typedef System::Collections::Generic::List<collision_object_wrapper> collision_objects_list;

struct collision_predicate
{
	
	collision_predicate( collision_objects_list^ list ):m_object_list(list),
														m_triangle_result(NULL, 0, 0.0f),
														m_object_result(NULL,0){}
	
	bool object_in_list( collision::ray_triangle_result const& r)
	{
		R_ASSERT(m_triangle_result.object==NULL);
		for(int i=0; i<m_object_list->Count; ++i)
			if(m_object_list->default[i].m_collision_object == r.object)
			{
				m_triangle_result = r;
				return true;
			}
		return false;
	}

	bool object_not_in_list( collision::ray_triangle_result const& r)
	{
		R_ASSERT(m_triangle_result.object==NULL);
		for(int i=0; i<m_object_list->Count; ++i)
		{
			collision::object* co = m_object_list->default[i].m_collision_object;
			if(co == r.object)
				return false;
		}
		m_triangle_result = r;
		return true;
	}

	bool object_in_list( collision::ray_object_result const& r)
	{
		R_ASSERT(m_triangle_result.object==NULL);
		for(int i=0; i<m_object_list->Count; ++i)
			if(m_object_list->default[i].m_collision_object == r.object)
			{
				m_object_result = r;
				return true;
			}
		return false;
	}
	bool object_not_in_list( collision::ray_object_result const& r)
	{
		R_ASSERT(m_triangle_result.object==NULL);
		for(int i=0; i<m_object_list->Count; ++i)
		{
			collision::object* co = m_object_list->default[i].m_collision_object;
			if(co == r.object)
				return false;
		}
		m_object_result = r;
		return true;
	}

	collision::ray_triangle_result		m_triangle_result;
	collision::ray_object_result		m_object_result;
	gcroot<collision_objects_list^>		m_object_list;
};

bool const_false_predicate( collision::ray_triangle_result const& )
{
	return false;
}

float3 transform_control_translation::get_drag_vector( )
{
	float3 mouse_origin, mouse_direction;
	m_helper->get_mouse_ray( mouse_origin, mouse_direction );

	float3 current_point;
	if(!m_plane_mode)
	{
		float3	saved_origin	= get_matrix().c.xyz();
		float3	curr_axis		= get_matrix().transform_direction( get_axis_vector( m_active_axis ) );

		if( !ray_nearest_point( saved_origin, curr_axis, mouse_origin, mouse_direction, current_point ) ) 
			return float3( 0.0f, 0.0f, 0.0f );
	}
	else
	{
		if( !plane_ray_intersect( *m_mouse_drag_start, get_axis_vector( m_active_plane ), mouse_origin, mouse_direction, current_point ) )
			return float3( 0.0f, 0.0f, 0.0f );
	}

	return ( current_point - *m_mouse_drag_start );
}

void transform_control_translation::execute_input_vf_snap( )
{
	float3 const drag_vector = this->get_drag_vector();
	if( math::is_zero(drag_vector.length()) )
		return;

	float4x4 const cam			= get_inv_view();
	float3 origin_point							= *m_origin_drag_start + drag_vector;

	collision::space_partitioning_tree* tree	= m_helper->m_collision_tree;
	collision_objects_list						self_collision_objects;
	m_helper->m_object->get_collision			( %self_collision_objects );
	collision_predicate							p(%self_collision_objects);

	collision::object_type mask					= collision_object_type_dynamic;

	float3 const ray_origin						= cam.c.xyz();
	float3 const ray_direction					= (origin_point - ray_origin).normalize();
	float const max_distance					= 10000.0f;

	bool bpivot_moving				= (m_helper->m_object->current_mode()==editor_base::transform_control_object::mode::edit_pivot);


	if(m_snap_mode==snap_mode::face_snap)
	{
		collision::ray_objects_type				result_objects(g_allocator);
		bool query_result = tree->ray_query(
				mask,
				ray_origin,
				ray_direction,
				max_distance,
				result_objects,
				(bpivot_moving)?
					collision::objects_predicate_type(&p, &collision_predicate::object_in_list)
					:
					collision::objects_predicate_type(&p, &collision_predicate::object_not_in_list)
			);

		if(!query_result)
			return;

		float3 pick_point		= ray_origin + ray_direction * p.m_object_result.distance;
		float3 const v			= (pick_point - *m_origin_drag_start);
		//*m_result				= math::create_translation( v );
		float4x4 inverted;
		inverted.try_invert( get_matrix() );
		inverted.c.xyz() = float3( 0.0f, 0.0f, 0.0f );
		R_ASSERT( inverted.valid() );
		*m_result				= math::create_translation( inverted.transform_position( v ) );
	}
	else
	{
		collision::ray_triangles_type				result_objects(g_allocator);
		bool query_result = tree->ray_query(
				mask,
				ray_origin,
				ray_direction,
				max_distance,
				result_objects,
				(bpivot_moving)?
					collision::triangles_predicate_type(&p, &collision_predicate::object_in_list)
					:
					collision::triangles_predicate_type(&p, &collision_predicate::object_not_in_list)
			);

		if(!query_result)
			return;


		R_ASSERT(m_snap_mode==snap_mode::vertex_snap);
		collision::collision_object const* gi = dynamic_cast<collision::collision_object const*>(p.m_triangle_result.object);
		if(NULL==gi)
			return;

		float3 pick_point	= ray_origin + ray_direction * p.m_triangle_result.distance;

		u32 triangle_id		= p.m_triangle_result.triangle_id;

		non_null< collision::geometry_instance const >::ptr geom = gi->get_geometry_instance();
		u32 const* indices								= geom->indices(triangle_id);
		float3 verts[3];
		verts[0]			= geom->vertices()[ indices[0] ];
		verts[1]			= geom->vertices()[ indices[1] ];
		verts[2]			= geom->vertices()[ indices[2] ];

		float4x4 const m	= gi->get_matrix();
		verts[0]			= m.transform_position( verts[0] );
		verts[1]			= m.transform_position( verts[1] );
		verts[2]			= m.transform_position( verts[2] );
		float l0 			= (verts[0]-pick_point).squared_length();
		float l1 			= (verts[1]-pick_point).squared_length();
		float l2 			= (verts[2]-pick_point).squared_length();
		if(l0<l1)
		{
			pick_point	= (l0<l2) ? verts[0] : verts[2];
		}else
		{
			pick_point	= (l1<l2) ? verts[1] : verts[2];
		}

		float3 const v			= (pick_point - *m_origin_drag_start);
		//*m_result				= math::create_translation( v );
		float4x4 inverted;
		inverted.try_invert( get_matrix() );
		R_ASSERT( inverted.valid() );
		*m_result				= math::create_translation( inverted.transform_direction( v ) );

	}

}



} //namespace editor
} // namespace xray
