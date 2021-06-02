////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_COLLISION_OBJECT_H_INCLUDED
#define EDITOR_COLLISION_OBJECT_H_INCLUDED

#include <xray/collision/object.h>
#include <xray/collision/geometry_instance.h>

#include "transform_control.h"

namespace xray{

namespace collision {
	struct engine;
	struct geometry;
} // namespace collision

namespace editor{
namespace detail{


class editor_colision_object :	/*public collision::geometry_instance,*/ public collision::object
{
public:
					editor_colision_object	(	math::float4x4 const& matrix, 
												collision::geometry const* geometry, 
												transform_control* owner_control, 
												enum_transform_axis axis );

	virtual			~editor_colision_object	( );

public:
	virtual	bool		aabb_query			( xray::collision::query_type query_type, math::aabb const& aabb, xray::collision::Results& results ) const;	
	virtual	bool		frustum_query		( xray::collision::query_type query_type, math::frustum const& frustum, xray::collision::Results& results ) const;
	virtual	bool		ray_query			(	xray::collision::query_type query_type, 
												math::float3 const& origin, 
												math::float3 const& direction, 
												float max_distance, 
												float& distance, 
												xray::collision::Results& results, 
												xray::collision::Predicate const& predicate ) const;

	virtual	bool			aabb_test		( xray::collision::query_type query_type, math::aabb const& aabb ) const;	
	virtual	bool			frustum_test	( xray::collision::query_type query_type, math::frustum const& frustum ) const;	
	virtual	bool			ray_test		( xray::collision::query_type query_type, math::float3 const& origin, math::float3 const& direction, float max_distance ) const;	

	virtual	void		render				( render::base::world& world ) const;
	virtual	xray::collision::geometry const* get_object_geometry	( ) const;


	virtual	non_null<xray::collision::geometry const>::ptr	get_geometry	( ) const;

// 	virtual	bool							aabb_query		( object const* object, math::aabb const& aabb, collision:: xray::collision::Results& results ) const;
// 	virtual	bool							frustum_query	( object const* object, math::frustum const& frustum, collision:: xray::collision::Results& results ) const;
// 	virtual	bool							ray_query		( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, collision:: xray::collision::Results& results, collision::Predicate const& predicate ) const;

	virtual	void							set_matrix		( math::float4x4 const& matrix );
	virtual	math::float4x4 const&			get_matrix		( ) const;

	virtual	void							set_matrix_original		( math::float4x4 const& matrix );
	virtual	math::float4x4 const&			get_matrix_original		( ) const;

public:
			transform_control*				get_owner			( ) const;
			math::float3					get_center			( ) const;
			math::float3					get_radius			( ) const;
			enum_transform_axis				get_axis			( ) const;

			void							set_priority		( u32 priority );
			u32								get_priority		( ) const;

private:
			void			modify			( xray::math::aabb& aabb, math::float4x4 const& transform );

private:

	xray::collision::geometry const*	m_geometry;
	transform_control*		m_owner;

	math::float4x4			m_transform_original;
	math::float4x4			m_transform_general;
	math::float4x4			m_transform;
	math::float4x4			m_transform_invert;

	enum_transform_axis		m_axis;
	math::aabb				m_transformed_aabb;
	u32						m_priority;

}; // class editor_collision_object

}// namespace detail
}// namespace editor
}// namespace xray

#endif // #ifndef EDITOR_COLLISION_OBJECT_H_INCLUDED