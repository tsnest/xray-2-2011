////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef NAVIGATION_WORLD_H_INCLUDED
#define NAVIGATION_WORLD_H_INCLUDED

#include <xray/ai_navigation/world.h>
#include <xray/ai_navigation/sources/navigation_mesh_types.h>

namespace xray {
namespace render{ 

namespace debug{ 
	class renderer;
} // namespace game
} // namespace render

namespace ai {
namespace navigation {

class graph_generator;

struct engine;

class navigation_world :
	public ai::navigation::world,
	private boost::noncopyable
{
public:
					navigation_world			( engine& engine, render::scene_ptr const& scene, render::debug::renderer& renderer );
	virtual			~navigation_world			( );
	virtual	void	tick						( );
	virtual	void	clear_resources				( );

	virtual void	load_navmesh				( pcstr project_name );
	virtual u32		get_node_id_at				( float3 const& position );
	virtual u32		get_node_id_after_move		( u32 old_node_id, float3 const& old_position, float3 const& new_position );
	virtual bool	find_path					(
						u32 start_node,
						math::float3 const& start_position,
						u32 goal_node,
						math::float3 const& goal_position,
						float agent_radius,
						path_type& path
					);
	virtual	bool	can_stand( 
						float3 const& vertex,
						u32 triangle_id,
						float agent_radius
					);

#ifdef DEBUG
	virtual	void	debug_render				( input::world& input, float3 const& position, float3 const& direction, math::frustum const& frustum );
#endif // #ifdef DEBUG

private:
	inline	graph_generator* get_graph_generator( ) { return m_graph_generator; }
			void	on_binary_config_resource	( xray::resources::queries_result& resources );

private:
#ifndef MASTER_GOLD
	triangles_mesh_type			m_navigation_mesh;
#endif // #ifndef MASTER_GOLD
	graph_generator*			m_graph_generator;
	engine&						m_engine;
	render::debug::renderer&	m_renderer;
	math::float3				m_start_position;
	math::float3				m_goal_position;
	u32							m_start_node;
	u32							m_goal_node;
}; // class navigation_world

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef NAVIGATION_WORLD_H_INCLUDED