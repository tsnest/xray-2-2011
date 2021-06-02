////////////////////////////////////////////////////////////////////////////
//	Created		: 01.06.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EDIT_PORTAL_H_INCLUDED
#define EDIT_PORTAL_H_INCLUDED
#include "triple.h"
#include "bsp_tree_triangle.h"
#include "sector_generator_portal.h"

namespace xray {

namespace collision {
	class triangle_mesh_geometry;
} // namespace collision

namespace model_editor {

ref class unique_name_helper;
ref class edit_object_mesh;
class portal_vertex_collision;
class portal_collision_object;

struct portal_vertex
{
	float3 coord;
	explicit portal_vertex( float3 const& coord_value, bool selected = false )
		:coord( coord_value ), m_selected( selected ) {}
	portal_vertex()
		:m_selected(false){}
	bool is_selected( ) const { return m_selected; }
	void set_selected( bool value ) { m_selected = value; }
private:
	bool m_selected;
};

ref class edit_portal sealed
{
	ref struct selectable_vertex
	{
		Float3 coord;
		property bool selected;
		selectable_vertex ( Float3 c ): 
		coord(c){}
	};
public:
	typedef System::Collections::Generic::List<int> indices_collection;	
	typedef System::Collections::Generic::IList<edit_portal^> portal_ilist;
	enum class vertex_edit_mode { edit_portal_mode_free_vertices, edit_portal_mode_fit_to_plane, edit_portal_mode_fit_to_vertex };

								edit_portal							( edit_object_mesh^ parent, unique_name_helper^ name_helper, portal_ilist^ portals, configs::lua_config_value const& cfg  );
								edit_portal							( edit_object_mesh^ parent, unique_name_helper^ name_helper, portal_ilist^ portals, System::String^ name );
								~edit_portal						( );
	property System::String^ Name
	{
		System::String^			get									( )
		{
			return	m_name;
		}
		void					set									( System::String^ value );
	}
	
	void						set_selected						( bool selected );
	bool						is_selected							( );
	void						save_portal							( configs::lua_config_value& cfg );
	void						render								( render::scene_ptr const& scene, render::debug::renderer& r );
	//float3						get_vertex							( int index );
	void						shift_vertex						( int index, float3 const& shift );
	indices_collection^			get_selected_vertices				( );
	void						on_edit_vertices					( );
	void						on_vertex_touch						( int index );
	bool						is_vertex_selected					( int index );
	u32							get_collisions						( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% collisions );
	math::aabb					get_aabb							( );
	bool						are_vertices_coplanar				( );
	void						make_vertices_coplanar				( );
	math::float4x4 const&		get_transformation					( );
	float3 const&				get_untransformed_vertex			( int index );
	void						add_vertex							();
	bool						delete_selected_vertices			();
	sector_generator_portal		to_sector_generator_portal			();
private:
	void						init_default_vertices				( );
	void						load_portal							( configs::lua_config_value const& cfg );
	void						on_matrix_modified					( cli::array<System::String^>^ /*unused*/ );
	void						on_vertex_modified					( );
	void						on_vertex_end_modify				( );
	void						in_constructor						( );
	void						initialise_plane					( );
	void						insert_vertices_to_collision_tree	( );
	void						remove_vertices_from_collision_tree	( );
	void						update_vertex_collision				( u32 index );
	void						update_vertex_collisions			( );
	void						update_collision_mesh				( );
	void						deselect_other_portals				( );
	typedef vector<float3> coord_vector;
	void						restore_model_and_matrix_from_world	( coord_vector const& world_coords );
	bool						get_two_nearest						( float3 const& point, int& first, int& second );
	bool						delete_unused_vertex				( );
	void						delete_vertex_impl					( u32 index );
	bool						delete_first_selected_vertex		( );
	u32							get_picked_vertex_index				( );
	edit_object_mesh^			m_parent_object; 
	unique_name_helper^			m_unique_name_helper;
	portal_ilist^				m_instances;
	System::String^				m_name;

	typedef vector<portal_vertex>		vertex_vector;
	vertex_vector*						m_vertices;
	typedef triple<u32>					index_triple;
	typedef vector< index_triple >		index_triple_vector; 
	index_triple_vector*				m_indices;
	math::float4x4*						m_model_to_world;
	math::float4x4*						m_world_to_model;
	math::plane*						m_plane;
	typedef vector<portal_vertex_collision*> vertex_collisions;
	vertex_collisions*					m_vertex_collisions;
	collision::geometry*				m_vertex_collision_mesh;
	portal_collision_object*			m_portal_collision;
	collision::geometry*				m_portal_collision_mesh;
	u32									m_picked_vertex_index;
	bool								m_selected;
}; // class edit_portal

} // namespace model_editor
} // namespace xray

#endif // #ifndef EDIT_PORTAL_H_INCLUDED
