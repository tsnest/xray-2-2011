////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_AABB_OBJECT_H_INCLUDED
#define COLLIDERS_AABB_OBJECT_H_INCLUDED

#include <xray/collision/object.h>
#include "colliders_aabb_base.h"

namespace xray {
namespace collision {

class object;
struct oct_node;

namespace colliders {

class aabb_object : public aabb_base {
public:
						aabb_object		( query_type const query_type, loose_oct_tree const& tree, math::aabb const& aabb, objects_type& objects );
						aabb_object		( query_type const query_type, loose_oct_tree const& tree, math::aabb const& aabb, triangles_type& triangles );
	inline	bool		result			( ) const { return m_result; }

private:
			void		add_objects		( non_null<collision::oct_node const>::ptr node ) const;
			void		add_triangles	( non_null<oct_node const>::ptr const node ) const;
			void		query			( non_null<collision::oct_node const>::ptr node, float3 const& aabb_center, float const aabb_extents ) const;
			void		process			( );

private:
	typedef aabb_base	super;

private:
	objects_type* const		m_objects;
	triangles_type* const	m_triangles;
	loose_oct_tree const&	m_tree;
	query_type				m_query_type;
}; // class aabb_object

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_AABB_OBJECT_H_INCLUDED