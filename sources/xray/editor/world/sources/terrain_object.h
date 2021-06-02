////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_OBJECT_H_INCLUDED
#define TERRAIN_OBJECT_H_INCLUDED

#include "object_base.h"
#include "terrain_quad.h"

#pragma managed( push, off )
#include <xray/render/facade/common_types.h>
#include <xray/render/world.h>
#include <xray/render/facade/model.h>
#include <xray/physics/rigid_body.h>
#pragma managed( pop )

namespace xray {
namespace editor {

ref class tool_terrain;
ref class terrain_core;

public ref class vertices_db
{
public:
					vertices_db				( ):m_modified(false),m_ext_file(false),m_actual_file_name(""),m_actual_file_name_is_runtime(false){}
	terrain_vertex	get						( u32 idx );
	void			set						( terrain_vertex v, u32 idx );
	
	void			generate				( float size, float cell_size );
	void			clear					( );
	void			get_height_bounds		( float% min_height, float% max_height );
	void			export_vertices			( math::rectangle<math::uint2> const& rect, vectora<xray::render::terrain_data>& dest_buffer );
	int				m_dimension;			// quads array

	bool			empty	( );
	vertex_list		m_vertices;
	bool			m_modified;
	bool			m_ext_file;
	System::String^	m_actual_file_name;
	bool			m_actual_file_name_is_runtime;
};

public ref class terrain_node : public object_base
{ 
	typedef object_base		super;
public:
						terrain_node			( tool_base^ t, terrain_core^ core );
	virtual				~terrain_node			( );
	virtual void		render					( )	override;
	virtual void		load_props				( configs::lua_config_value const& t )	override;
	virtual void		save					( configs::lua_config_value t )			override{UNREACHABLE_CODE();}
	virtual	void		set_visible				( bool bvisible)						override;
	virtual	void		set_transform			( float4x4 const& )						override		{};
	virtual bool		get_persistent			( )	override						{return true;}
	virtual bool		get_selectable			( ) override						{ return false;}
	virtual bool		visible_for_project_tree( ) override						{ return false;}
	virtual enum_terrain_interaction get_terrain_interaction( ) override			{return enum_terrain_interaction::tr_free;}


	void				set_transform_internal	( float4x4 const& transform );
	float				get_height_local		( float3 const& position_local );
	int					get_layer_for_paint		( System::String^ texture_name, float3 const& position_local );

			bool		get_quad				( terrain_quad& dest_quad, int x, int y );
			bool		get_quad				( terrain_quad& dest_quad, u16 quad_index );
	inline	void		vertex_xz				( u16 vertex_id, float& _x, float& _z );
	inline	void		vertex_xz				( u16 vertex_id, int& _x_idx, int& _z_idx );
	inline	u16			vertex_id				( int const _x_idx, int const _z_idx );
	inline	bool		get_row_col				( float3 const& position_local, int& x, int& z );

	inline	float		distance_xz_sqr			( u16 vertex_id, float3 const& point );
	inline	float		distance_xz				( u16 vertex_id, float3 const& point ) {return math::sqrt(distance_xz_sqr(vertex_id, point));}
	inline	float3		position_l				( u16 vertex_id );
	inline	float3		position_g				( u16 vertex_id );

			
		terrain_core^	get_terrain_core		( )				{ return m_core; }
	render::terrain_model_ptr	get_model		( )				{ return *m_model; }

	virtual void		load_contents			( )	override;
	virtual void		unload_contents			( bool bdestroy ) override;

			void		initialize_collision	( );
	virtual	void		destroy_collision		( )				override;
			
			void		save_config				( configs::lua_config_value t );
			void		load_config				( configs::lua_config_value const& t );

			void		save_vertices			( System::String^ file_name, bool runtime_save );
			void		copy_from				( terrain_node^ n );

			void		initialize_ph_collision	( );
			void		destroy_ph_collision	( );
protected:

			void		query_vertices			( );
			void		query_render_model		( );

			void		on_vertices_ready		( resources::queries_result& data );
			void		on_render_model_ready	( resources::queries_result& data );

			u16			get_nearest_vertex		( float3 const& position_local );
	inline	bool		is_in_bound				( float3 const& position_local );


			void		load_vertices_from_reader( memory::reader& F );
	terrain_core^					m_core;
	render::terrain_model_ptr*		m_model;

//			float*						m_ph_heightfield;
//	physics::bt_collision_shape_ptr*	m_collision_shape;
	physics::bt_rigid_body*				m_rigid_body;
public:
	void				load_vertices_sync		( );
	void				add_used_texture		( System::String^ );
	string_list			m_used_textures;
	
	terrain_node_key	m_tmp_key;
	vertices_db			m_vertices;

	float				m_min_height;
	float				m_max_height;
	void				get_vertices_l			( float3 const& local_position, float const& radius, modifier_shape shape_type,  vert_id_list^ dest_vert_ids );
	void				get_vertices_g			( float3 const& global_position, float const& radius, modifier_shape shape_type,  vert_id_list^ dest_vert_ids );
	void				get_quads_g				( float3 const& global_position, float const& radius, modifier_shape shape_type,  vert_id_list^ dest_quad_ids );

	float				m_cell_size;
	u32					size					( );
	aabb				get_local_aabb			( );
}; // class terrain_object

public ref class terrain_core : public object_base
{
	typedef object_base				super;

	typedef System::Collections::Generic::KeyValuePair<terrain_node_key, terrain_node^>	key_value_pair;
	typedef System::Collections::Generic::Dictionary<terrain_node_key, terrain_node^>	terrain_nodes_dict;

public:
						terrain_core	( tool_terrain^ tool_terrain, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view );
						~terrain_core	( );

	inline	render::scene_ptr const& scene	( ) { R_ASSERT( m_scene ); return *m_scene; }

	virtual void		load_props		( configs::lua_config_value const& t )	override;
	virtual void		save			( configs::lua_config_value t )			override;
	virtual void		load_defaults	( )										override;
	virtual	void		set_transform	( float4x4 const& transform )			override;
	virtual bool		get_persistent	( )	override							{ return true; }
	virtual bool		get_selectable	( ) override							{ return false;}
	virtual bool		visible_for_project_tree( ) override					{ return false;}
	virtual enum_terrain_interaction get_terrain_interaction( )override			{ return enum_terrain_interaction::tr_free;}
	
	virtual void		load_contents	( )										override	{}
	virtual void		unload_contents	( bool )								override	{}

	virtual	void		set_visible		( bool bvisible )						override;
			void		create_node		( terrain_node_key key );
			void		clear_node		( terrain_node_key key );
			void		destroy_all		( );
			void		update			( );
			float		get_height		( float3 const& p );
	inline terrain_node_key pick_node	( float3 const& p );

			void		select_vertices	( float3 const& point, float const radius, modifier_shape t, key_vert_id_dict^ dest_list );
			void		select_quads	( float3 const& point, float const radius, modifier_shape t, key_vert_id_dict^ dest_list );

			void		add_texture		( System::String^ texture_name );
			void		change_texture	( System::String^ old_texture_name, System::String^ new_texture_name );
			void		do_import		( terrain_import_export_settings^ settings );
			void		do_export		( terrain_import_export_settings^ settings );
			void		sync_visual_vertices( terrain_node^ terrain, vert_id_list^ list );
			void		select_quads	( key_vert_id_dict^ list, enum_selection_method method );

		key_vert_id_dict^ selected_quads( );
			void		copy_node		( terrain_node_key from_key, terrain_node_key to_key );

			void		reset_all_colors ( );
public:
	string_list				m_used_textures;
	terrain_nodes_dict		m_nodes;

	property u32			node_size_; // meters
	
private:
				void	add_node_internal		( terrain_node_key key, terrain_node^ node );
				void	remove_node_internal	( terrain_node_key key );
				void	command_update_cb		( u32 arg );
				bool	update_vertices_impl	( u32 terrain_id );

	update_query_list	m_update_queries;
	item_id_list		m_active_updating_nodes;

	tool_terrain^		m_terrain_tool;
	key_vert_id_dict	m_selected_quad_id_list;

	render::scene_ptr*		m_scene;
	render::scene_view_ptr*	m_scene_view;
}; // ref class terrain_object

} //namespace editor
} //namespace xray

#include "terrain_object_inline.h"

#endif // #ifndef TERRAIN_OBJECT_H_INCLUDED