////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.01.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PHYSICS_WORLD_H_INCLUDED
#define PHYSICS_WORLD_H_INCLUDED

#include <xray/physics/world.h>
#include <xray/render/engine/base_classes.h>

class step_object;
class scheduler;
class shell_static;
class temp_storage_holder;
class shell;
namespace xray {

namespace physics {
	struct engine;
	struct shell;
} // namespace physics

namespace collision {
	struct space_partitioning_tree;
	class geometry;
	class geometry_instance;
} // namespace collision

namespace physics {

struct engine;
struct shell;


class physics_world :
	public world,
	private boost::noncopyable
{
public:
					physics_world	( memory::base_allocator& allocator, engine& engine );
	virtual			~physics_world	( );

	virtual	void							tick				( );
	virtual	void							initialize			( ){};

	virtual	void							clear_resources		( );
	virtual	non_null<physics::shell>::ptr	new_physics_shell	( configs::binary_config_value const& cfg );
	virtual	non_null<physics::shell>::ptr	new_static_shell	( configs::binary_config_value const& cfg );

#ifndef MASTER_GOLD
	virtual	non_null<physics::shell>::ptr	new_physics_shell	( configs::lua_config_value const& cfg );
#endif // #ifndef MASTER_GOLD
	
	virtual	non_null<physics::shell>::ptr	new_static_shell	( non_null<collision::geometry_instance>::ptr g_instance );
	virtual	non_null<physics::shell>::ptr	new_physics_shell	( non_null<collision::geometry_instance>::ptr g_instance );

	virtual					void			delete_shell		( non_null<shell>::ptr sh );
							void			deactivate			( ::shell& );
	virtual					void			set_ready			( bool v ) { m_b_ready = v; }

							void			activate_object		( step_object& so );
	virtual					void			set_space			( collision::space_partitioning_tree *space );

	virtual	 collision::space_partitioning_tree* get_collision_tree	( ) const	{ return &(*m_space); }	

public:
	virtual					void			create_test_scene( );
	memory::base_allocator&	get_allocator	( ) { return m_allocator; }

private:
							void			on_create_test_primitives_config_loaded( resources::queries_result& data );

private:
	engine&												m_engine;
	non_null<collision::space_partitioning_tree>::ptr	m_space;
	memory::base_allocator&								m_allocator;
	scheduler*											m_scheduler;
mutable	temp_storage_holder*							m_temp_storage_holder;

public:
		temp_storage_holder&				get_temp_storage_holder	( )const { return *m_temp_storage_holder; }

private:
	template < class ConfigValueType >
	non_null<shell>::ptr					new_shell_impl		( ConfigValueType const& cfg );

	virtual	void							debug_render		( render::scene_ptr const& scene, render::debug::renderer& renderer ) const;

	virtual	void		add_rigid_body		( bt_rigid_body* )	{ UNREACHABLE_CODE(); }
	virtual	void		remove_rigid_body	( bt_rigid_body* )	{ UNREACHABLE_CODE(); }
	virtual	void		add_soft_body		( bt_soft_body_rope* )	{ UNREACHABLE_CODE(); }
	virtual	void		remove_soft_body	( bt_soft_body_rope* )	{ UNREACHABLE_CODE(); }

	virtual closest_ray_result	ray_test	( float3 const& , float3 const& , float const ){return closest_ray_result();}

private:
	shell_static*							debug_new_static_shell				( );
	
	shell_static*							m_shell_static;

/// test primitives;
	shell									*m_test_box;
	shell									*m_test_sphere;
	shell									*m_test_cylinder;
	bool									m_b_ready;

}; // class physics_world

} // namespace physics
} // namespace xray

#endif // #ifndef PHYSICS_WORLD_H_INCLUDED
