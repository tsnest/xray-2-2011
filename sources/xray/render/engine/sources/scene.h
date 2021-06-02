////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_SCENE_H_INCLUDED
#define XRAY_RENDER_ENGINE_SCENE_H_INCLUDED

#include <xray/particle/engine.h>
#include <xray/render/engine/vertex_colored.h>
#include <xray/render/engine/base_classes.h>
#include "render_model.h"
#include "decal_instance.h"

namespace xray {

namespace collision {
	struct space_partitioning_tree;
} // namespace collision

namespace particle {
	struct world;
	typedef	resources::resource_ptr<
		world,
		resources::unmanaged_intrusive_base
	> world_ptr;

	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_instance_ptr;
} // namespace particle

namespace render {

struct material_effects_instance;
typedef	resources::resource_ptr<material_effects_instance, resources::unmanaged_intrusive_base> material_effects_instance_ptr;

typedef vectora< vertex_colored >		debug_vertices_type;
typedef vectora< u16 >					debug_indices_type;

class terrain_render_model_instance;
typedef	resources::resource_ptr <
	terrain_render_model_instance,
	resources::unmanaged_intrusive_base
> terrain_render_model_instance_ptr;

class speedtree_instance;
typedef	resources::resource_ptr<
	speedtree_instance,
	resources::unmanaged_intrusive_base
> speedtree_instance_ptr;

struct light_props;
class render_model_instance;
class lights_db;
class terrain;
struct editor_renderer_configuration;
struct speedtree_forest;
class grass_world;

class scene : public base_scene {
public:
								scene					( editor_renderer_configuration const& renderer_configuration );
	virtual						~scene					( );
			
			void		add_speedtree_instance			( speedtree_instance_ptr v, math::float4x4 const& transform, bool populate_forest );
			void		remove_speedtree_instance		( speedtree_instance_ptr v, bool populate_forest );
			void		set_speedtree_instance_transform( speedtree_instance_ptr v, math::float4x4 const& transform, bool populate_forest );
			void		populate_speedtree_forest		( );

			void		add_model						( render_model_instance_impl_ptr v);
			void		modify_model					( render_model_instance_impl_ptr v);
			void		remove_model					( render_model_instance_impl_ptr v);
	
			void		add_light						( u32 id, render::light_props const& props);
			void		update_light					( u32 id, render::light_props const& props);
			void		remove_light					( u32 id);

			void		add_decal						( u32 id, render::decal_properties const& properties);
			void		update_decal					( u32 id, render::decal_properties const& properties);
			void		remove_decal					( u32 id);
	
			void		select_models					( float4x4 const & mat_vp, render_surface_instances& selection );
			void		select_terrain_cells			( float4x4 const & mat_vp, render::vector< terrain_render_model_instance_ptr > & cells);
			void		update_models					(  );

			void		select_particle_system_instance	( particle::particle_system_instance_ptr const& instance, bool is_selected);
			void		select_speedtree_instance		( speedtree_instance_ptr const& instance, bool is_selected);
			void		select_model					( render_model_instance_impl_ptr const& instance, bool is_selected);

public:
			void 				draw_lines				( debug_vertices_type const& vertices, debug_indices_type const& indices );
			void 				draw_triangles			( debug_vertices_type const& vertices, debug_indices_type const& indices );
			void				flush					( boost::function< void ( bool ) > const& on_draw_scene );

//.	inline	model_manager&		model_manager			( )	{ return *m_models; }
	inline	lights_db const&	lights					( )	{ return *m_lights; }
	inline	terrain*			terrain					( )	{ return m_terrain.c_ptr(); }
			particle::world*	particle_world			( );
	inline  speedtree_forest*	get_speedtree_forest	( ) { return m_speedtree_forest; }
			// Set scene slow motion.
			void				set_slomo				( float time_multiplier );
			float				get_slomo				( ) const { return m_scene_slomo; }
			
			grass_world*		get_grass				( ) const { return m_grass; }
			
			// TODO: move to environment			
			void				set_sky_material		(material_effects_instance_ptr const& in_material) { m_sky_material = in_material; }
			material_effects_instance_ptr const& get_sky_material() const { return m_sky_material; }
			
private:
	friend	class				scene_cook;
	
private:
			void 				update_lines			( u32 add_count );
			void 				render_lines			( bool covering_effect = false );
			void 				update_triangles		( u32 add_count );
			void 				render_triangles		( bool covering_effect = false );

private:
	struct particle_engine :
		public particle::engine,
		private boost::noncopyable
	{
		inline	void					set_particle_world				( particle::world& particle_world )
		{
			m_particle_world			= &particle_world;
		}

		virtual	particle::render_particle_emitter_instance*	create_render_emitter_instance	(
											particle::particle_emitter_instance& particle_emitter_instance,
											particle::particle_list_type const& particle_list,
											particle::billboard_parameters* billboard_parameters,
											particle::beamtrail_parameters* beamtrail_parameters,
											particle::enum_particle_locked_axis locked_axis,
											particle::enum_particle_screen_alignment screen_alignment,
											math::float4x4 const& transform,
											math::float4 const& instance_color
										);
		virtual	void					destroy							( particle::render_particle_emitter_instance*& instance );
		particle::world*				m_particle_world;
	}; // struct particle_engine

private:
	typedef vector< render::vertex_colored >					vertices_type;
	typedef vector< u16 >										indices_type;
	typedef vector< particle::particle_system_instance_ptr >	particle_system_instances_type;
	typedef vector< speedtree_instance_ptr >					speedtree_instances_type;
	
public:

typedef xray::intrusive_list< 
	decal_instance, 
	decal_instance*, 
	&decal_instance::m_next, 
	threading::single_threading_policy, 
	size_policy> decal_instance_list_type;
	
	inline decal_instance_list_type&			decals						() { return m_decals; }
	inline collision::space_partitioning_tree&	decals_tree					() { return *m_decals_tree; }
	
private:
	material_effects_instance_ptr		m_sky_material;
	
	particle_engine						m_particle_engine;

	vertices_type						m_line_vertices;
	indices_type						m_line_indices;

	vertices_type						m_triangle_vertices;
	indices_type						m_triangle_indices;

	render_model_instances				m_selected_models;
	particle_system_instances_type		m_particle_system_instances;
	speedtree_instances_type			m_speedtree_instances;
	
	decal_instance_list_type			m_decals;
	collision::space_partitioning_tree*	m_decals_tree;	
	collision::space_partitioning_tree*	m_models_tree;
	
//.	unique_ptr< render::model_manager >	m_models;
	render_model_instances				m_render_model_instances;
	unique_ptr< render::lights_db >		m_lights;
	unique_ptr< render::terrain >		m_terrain;
	particle::world_ptr					m_particle_world;
	render::speedtree_forest*			m_speedtree_forest;
	grass_world*						m_grass;
	float								m_scene_slomo;
}; // class scene


} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_SCENE_H_INCLUDED