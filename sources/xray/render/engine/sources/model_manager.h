////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_MANAGER_H_INCLUDED
#define MODEL_MANAGER_H_INCLUDED

namespace xray {
namespace render {

class render_model;
class render_surface;

namespace model_factory {

	render_model*			create_render_model		( u16 type );
	render_surface*			create_render_surface	( u16 type );
	void					destroy_render_model	( render_model* v );
	void					destroy_render_surface	( render_surface* v );
	u16						get_model_type			( memory::chunk_reader& reader );
	u16						get_surface_type		( memory::chunk_reader& reader );
} //namespace model_factory


//class renderer;
//class convex_volume;
//class res_declaration;
//class resource_intrusive_base;
//
//typedef intrusive_ptr<
//	res_declaration, 
//	resource_intrusive_base, 
//	threading::single_threading_policy
//>	ref_declaration;
//
//class res_effect;
//typedef	resources::resource_ptr < 
//	res_effect, 
//	resources::unmanaged_intrusive_base
//> ref_effect;
//
//struct tri
//{
//	u32				verts	[3];		// 3*4 = 12b
//	union
//	{
//		u32			dummy;				// 4b
//#pragma warning( push)
//#pragma warning( disable:4201)
//		struct
//		{
//			u32		material:14;		// 
//			u32		suppress_shadows:1;	// 
//			u32		suppress_wm:1;		// 
//			u32		sector:16;			// 
//		};
//#pragma warning( pop)
//	};
//};
//
//class collector
//{
//public:
//	//void			add_face			( const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector	);
//	//void			add_face_D			( const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy);
//	//void			add_face_packed		( const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector, float eps = EPS);
//	void			add_face_packed_d	( const float3& v0, const float3& v1, const float3& v2, u32 dummy, float eps = math::epsilon_5);
// //   void			remove_duplicate_T	();
//	//void			calc_adjacency		( xr_vector<u32>& dest);
//
//	float3*	get_vertices() {return &*m_verts.begin();}
//	u32*	get_indices()	{return &*m_indices.begin();}
//	u32*	get_tri_data() {return &*m_tri_data.begin();}
//
//	u32	vertex_count() {return u32( m_verts.size());}
//	u32	index_count() {return u32( m_indices.size());}
//
//	u32	tri_count() { return u32( m_tri_data.size());}
//	
//	void clear()	{m_verts.clear(); m_indices.clear(); m_tri_data.clear();}
//
//private:
//	u32	vpack( const float3& v, float eps);
//
//private:
//	render::vector<float3>	m_verts;
//	render::vector<u32>		m_indices;
//	render::vector<u32>		m_tri_data;
//};
//
//struct non_copyable {
//					non_copyable	() {}
//private:
//					non_copyable	( const non_copyable &) {}
//					non_copyable	&operator=		( const non_copyable &) {}
//};
//
//struct b_portal
//{
//	u16						sector_front;
//	u16						sector_back;
//	float3					vertices[6];
//	u32						count;
//};
//
//class	portal;
//class	sector;
//
//typedef math::rectangle<float2> box2f;
//
//struct scissor
//{
//	box2f	rect;
//	float	depth;
//};
//
//// Connector
//class	portal
//{
//private:
//	fixed_vector<float3,8>	m_poly;
//	sector					*m_face;
//	sector					*m_back;
//public:
//	math::plane		plane;
//	math::sphere	sphere;
//
//	u32		marker;
//	bool	dual_render;
//
//	void create( float3* verts, int vert_cnt, sector* face, sector* back);
//
//	fixed_vector<float3, 8>& get_poly() {return m_poly;}
//	sector*	back() {return m_back;}
//	sector*	front() {return m_face;}
//	sector*	get_sector( sector* from) {return from==m_face ? m_back : m_face;}
//	sector*	get_sector_facing( const float3& v) {if ( plane.classify( v)>0) return m_face; else return m_back;}
//	sector*	get_sector_back	( const float3& v) {if ( plane.classify( v)>0) return m_back; else return m_face;}
//	//float	distance( const float3 &v) {return abs( plane.classify( v));}
//
//	//portal();
//	//virtual	~portal();
//};
//
//class sector
//{
//public:
////	render_visual*	root() {return m_root;}
////	void			traverse( math::frustum& view_frustum, scissor& rect);
////
////	void create( render::vector<portal*> portals, render_visual* root)
////	{
////		m_portals.swap( portals);
////		m_root = root;
////	}
////
////	void traverse( math::frustum& f/*,	_scissor& R*/);
////	//virtual	~sector();
////
////public:
////	render::vector<convex_volume>	view_volumes;
////	//vector<scissor>			scissors;
////	//scissor					scissor_merged;
////	u32						marker;
////
////public:
////	render_visual*	m_root;			// whole geometry of that sector
////	render::vector<portal*>	m_portals;		//list of portals for this sector
//};
//
//class model_manager
//{
//public:
//			model_manager			( );
//			~model_manager			( );
//
//	void	clear_resources			( );
//
//	void	init					( );
//
//	void	load_sectors			( memory::chunk_reader& reader );
//
////.	void	select_models			( float3 view_pos, float4x4 mat_vp, composite_instances& selection );
////.	void	select_models			( sector* start_sector, float4x4 mat_vp, composite_instances& selection );
//	void	add_model_to_selection	( convex_volume const& frustum, composite_render_model_instance_ptr v );
//
//	//void	select_models			( float4x4 const & mat_vp, composite_instances& selection );
//	void	select_models			( float4x4 const & mat_vp, model_instances& selection);
//
//	// This need to be done optimized for only models which are somehow affect the final picture.
//	void	update_models			( );
//	
//	void	update					( const float3& new_view_point, const float4x4& wvp_mat );
//
//// --Porting to DX10_
//// 	inline ref_geometry	create_geom( u32 vb_id, u32 ib_id);
//// 	inline ref_geometry	create_geom( u32 vb_id, u32 ib_id, res_declaration * decl);
//
//	sector*	detect_sector			( float3 view_point );
//	sector*	detect_sector			( float3 view_point, float3 dir );
//
//	void	traverse				( sector* current, const convex_volume& f/*,	_scissor& R*/);
//	void	select_visible_sectors	( );
//
//	static composite_render_model*	create_composite_model			( u16 type );
//	static void						destroy_mesh		( composite_render_model* v );
//
//	static render_model*			create_model		( u16 type );
//	static void						destroy_submesh		( render_model* v );
//
//	static u16						get_mesh_type		( memory::chunk_reader& reader );
//	static u16						get_submesh_type	( memory::chunk_reader& reader );
//
//	// --Porting to DX10_
//// 	u32				create_vb( u32 size, u32 usage, pcvoid data);
//// 	u32				create_ib( u32 size, u32 usage, pcvoid data);
//
//	//	These functions need to be moved into encapsulated index/vertex buffer classes
//	//	"fragments" must to be sorted by start offset. 
//	// --Porting to DX10_
//// 	void 			update_vb( u32 buffer_id, xray::vectora<xray::render::buffer_fragment> const& fragments);
//// 	void 			update_ib( u32 buffer_id, xray::vectora<xray::render::buffer_fragment> const& fragments);
//
//	portal *		get_portal( u32 idx) {ASSERT( idx<m_portals.size()); return m_portals[idx];}
//	sector *		get_sector( u32 idx) {ASSERT( idx<m_sectors.size()); return m_sectors[idx];}
//
////.	void			add_model	( composite_render_model_instance_ptr v);
////.	void			modify_model( composite_render_model_instance_ptr v);
////.	void			remove_model( composite_render_model_instance_ptr v);
////.	composite_instances&	get_models() { return m_models; }
//
//
//	// This will temporary live here
//	void			set_draw_editor	( bool value);
//	void			set_draw_game	( bool value);
//
//	bool			get_draw_editor	(){ return m_draw_editor;}
//	bool			get_draw_game	(){ return m_draw_game;}
//
//private:
//	typedef render::vector< ref_declaration >			Declarators;
//	typedef render::vector< composite_render_model_instance_ptr >			Visuals;
//
//private:
////	render_queue<key_rq, queue_item, key_rq_sort_desc>	m_rq_static;
//
//	
//	collision::object*					m_collision_object_static;
//	collision::geometry*				m_collision_geom_static;
//	collision::space_partitioning_tree*	m_collision_tree_static;
//
//	collision::object*					m_collision_object_portals;
//	collision::geometry*				m_collision_geom_portals;
//	collision::space_partitioning_tree*	m_collision_tree_portals;
//
//	ref_effect		m_static_shader;
//	ref_effect		m_tree_shader;
//
//	render::vector<composite_render_model_instance_ptr>	m_models_selection;
//
//	render::vector<sector*>				m_selection;
//	sector*								m_start_sector;
//	float3								m_view_point;
//	float4x4							m_mx_form;
//
//	render::vector<portal*>				m_portals;
//	render::vector<sector*>				m_sectors;
//	render::vector<ref_effect>			m_effects;
//	Visuals								m_visuals;
//
////.	composite_instances					m_models;
//// 	composite_instances					m_editor_visuals;
//// 	composite_instances					m_game_visuals;
//
//	Declarators							m_declarators;
//
//	u32									m_marker;
//
//	bool								m_draw_editor;
//	bool								m_draw_game;
//
//	collision::space_partitioning_tree*	m_collision_tree;
//
//// 	collision::space_partitioning_tree*	m_collision_tree_editor;
//// 	collision::space_partitioning_tree*	m_collision_tree_game;
//
//}; // class model_manager
//
} // namespace render 
} // namespace xray 

#endif // #ifndef MODEL_MANAGER_H_INCLUDED