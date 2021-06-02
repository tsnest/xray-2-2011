////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/physics/collision_shapes.h>
#include "bullet_include.h"
#include "heightfield_terrain_shape.h"

namespace xray{
namespace physics{

bt_collision_shape::bt_collision_shape( btCollisionShape* sh/*, float3 const& center_offset*/ )
:m_bt_shape ( sh )
//m_center_offset( center_offset )
{}

bt_collision_shape::~bt_collision_shape( )
{
}

void destroy_shape( memory::base_allocator& allocator, bt_collision_shape* shape )
{
//	R_ASSERT				(&allocator == g_ph_allocator);
	btCollisionShape* sh	= shape->get_bt_shape();
	XRAY_DELETE_IMPL		( allocator, sh );
	XRAY_DELETE_IMPL		( allocator, shape );
}

btCollisionShape* create_bt_primitive( memory::base_allocator& allocator, collision::primitive_type type, float3 const& dim )
{
	R_ASSERT				(&allocator == g_ph_allocator);
	btCollisionShape*	result = NULL;
	switch (type)
	{
	case collision::primitive_sphere :
		{
			btScalar radius = dim.x;
			result = XRAY_NEW_IMPL( allocator, btSphereShape)(radius);
		}break;
	case collision::primitive_box :
		{
			btVector3 half_extents(dim.x, dim.y, dim.z);
			result = XRAY_NEW_IMPL( allocator, btBoxShape)(half_extents);
		}break;
	case collision::primitive_cylinder :
		{
			btVector3 half_extents(dim.x, dim.y, dim.z);
			result = XRAY_NEW_IMPL( allocator, btCylinderShape)(half_extents);
		}break;
	}
	return result;
}

bt_collision_shape* create_primitive_shape( memory::base_allocator& allocator, collision::primitive_type type, float3 const& dim )
{
	R_ASSERT				(&allocator == g_ph_allocator);
	btCollisionShape* bt_shape	= create_bt_primitive( allocator, type, dim );
	bt_collision_shape* result	= XRAY_NEW_IMPL( allocator, bt_collision_shape)( bt_shape );
	return						result;
}

bt_collision_shape* create_compound_shape( memory::base_allocator& allocator, configs::binary_config_value const& config )
{
	R_ASSERT				(&allocator == g_ph_allocator);
	configs::binary_config_value root					= config;
	configs::binary_config_value::const_iterator it		= root.begin();
	configs::binary_config_value::const_iterator it_e	= root.end();
	
	btCompoundShape* bt_shape							= XRAY_NEW_IMPL( allocator, btCompoundShape)();
	for(; it!=it_e; ++it)
	{
		collision::primitive_type type		= (collision::primitive_type)(int)(*it)["type"];
		float3 position						= (*it)["position"];
		float3 rotation						= (*it)["rotation"];
		float3 scale						= (*it)["scale"];

		btCollisionShape* child_shape		= create_bt_primitive( allocator, type, scale );

		float4x4 child_transform			= create_rotation(rotation) * create_translation(position);

		btTransform child_local_transform	= from_xray( child_transform );
		bt_shape->addChildShape				( child_local_transform, child_shape );
	}

	bt_collision_shape* result		= XRAY_NEW_IMPL( allocator, bt_collision_shape)(bt_shape );
	return							result;
}

bt_collision_shape* create_static_triangle_mesh_shape( memory::base_allocator& allocator, float3* vertices, u32* indices, u32 num_vertices, u32 num_indices )
{
	R_ASSERT				(&allocator == g_ph_allocator);
	btIndexedMesh mesh;
	mesh.m_numTriangles				= num_indices/3;
	mesh.m_triangleIndexBase		= (u8*)indices;
	mesh.m_triangleIndexStride		= sizeof(int)*3;
	mesh.m_numVertices				= num_vertices;
	mesh.m_vertexBase				= (u8*)vertices;
	mesh.m_vertexStride				= sizeof(float3);
	mesh.m_indexType				= PHY_INTEGER;

	btTriangleIndexVertexArray* mesh_interface	= XRAY_NEW_IMPL( allocator, btTriangleIndexVertexArray);
	mesh_interface->addIndexedMesh				( mesh );
	btBvhTriangleMeshShape* bt_shape			= XRAY_NEW_IMPL( allocator, btBvhTriangleMeshShape)(mesh_interface, true);
	bt_shape->setLocalScaling		( btVector3(1,1,-1) );
	
	bt_collision_shape* result					= XRAY_NEW_IMPL( allocator, bt_collision_shape)(bt_shape);
	return result;
}

bt_collision_shape* create_dynamic_triangle_mesh_shape( memory::base_allocator& allocator, 
														float3* vertices, 
														u32* indices, 
														u32 num_vertices, 
														u32 num_indices )
{
	R_ASSERT				(&allocator == g_ph_allocator);
	btIndexedMesh mesh;
	mesh.m_numTriangles				= num_indices/3;
	mesh.m_triangleIndexBase		= (u8*)indices;
	mesh.m_triangleIndexStride		= sizeof(int)*3;
	mesh.m_numVertices				= num_vertices;
	mesh.m_vertexBase				= (u8*)vertices;
	mesh.m_vertexStride				= sizeof(float3);
	mesh.m_indexType				= PHY_INTEGER;

	btTriangleIndexVertexArray* mesh_interface = XRAY_NEW_IMPL( allocator, btTriangleIndexVertexArray);
	mesh_interface->addIndexedMesh		( mesh );

	btGImpactMeshShape* bt_shape	= XRAY_NEW_IMPL( allocator, btGImpactMeshShape)(mesh_interface);
	bt_shape->setLocalScaling		( btVector3(1,1,-1) );
	bt_shape->updateBound			( );
	bt_collision_shape* result		= XRAY_NEW_IMPL( allocator, bt_collision_shape)(bt_shape);
	return result;
}

bt_collision_shape* create_terrain_shape( memory::base_allocator& allocator, 
										 float* heighfield, 
										 u32 rowcol,
										 float min_height,
										 float max_height)
{
//	R_ASSERT				(&allocator == g_ph_allocator);
	heightfield_terrain_shape* shape = XRAY_NEW_IMPL(allocator, heightfield_terrain_shape)(rowcol,
																				rowcol,
																				heighfield,
																				min_height, 
																				max_height,
																				true);

	bt_collision_shape* result		= XRAY_NEW_IMPL( allocator, bt_collision_shape)(shape/*, float3(32.0f, min_height+(max_height-min_height)/2.0f, -32.0f)*/ );
	return result;
}

}
}
