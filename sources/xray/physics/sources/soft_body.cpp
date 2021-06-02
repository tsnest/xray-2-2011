////////////////////////////////////////////////////////////////////////////
//	Created		: 04.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/physics/soft_body.h>
#include "bullet_physics_world.h"
#include "bullet_include.h"

namespace xray{
namespace physics{

bt_soft_body_rope* create_soft_body_rope( world* world, rope_construction_info const& construction_info )
{

	bullet_physics_world* w = static_cast_checked<bullet_physics_world*>(world);

	return w->create_soft_body_rope( construction_info );
}

void destroy_soft_body_rope	( world* world, bt_soft_body_rope* body )
{
	bullet_physics_world* w = static_cast_checked<bullet_physics_world*>(world);
	w->destroy_soft_body_rope( body );
}

bt_soft_body_rope::bt_soft_body_rope( btSoftBody* body )
:m_bt_body(body)
{}

bool bt_soft_body_rope::is_active( ) const
{
	return m_bt_body->isActive();
}

u32 bt_soft_body_rope::get_fragments_count( ) const
{
	return m_bt_body->m_links.size();
}

u32 bt_soft_body_rope::get_nodes_count( ) const
{
	return m_bt_body->m_nodes.size();
}

void bt_soft_body_rope::get_fragment( u32 idx, float3& pt0, float3& pt1 ) const
{
	const btSoftBody::Link&	l = m_bt_body->m_links[idx];

	pt0 = from_bullet(l.m_n[0]->m_x);
	pt1 = from_bullet(l.m_n[1]->m_x);
}

void bt_soft_body_rope::get_node( u32 idx, float3& pt ) const
{
	const btSoftBody::Node&	n = m_bt_body->m_nodes[idx];
	pt = from_bullet(n.m_x);
}

rope_construction_info::rope_construction_info()
:	kVCF			(1),
	kDG				(0),
	kLF				(0),
	kDP				(0),
	kPR				(0),
	kVC				(0),
	kDF				(0.2f),
	kMT				(0),
	kCHR			(1),
	kKHR			(0.1f),
	kSHR			(1),
	kAHR			(0.7f),
	timescale		(1),
	iterations		(200),
	margin			(0.1f),
	fragments_count	(10)
{}

}
}