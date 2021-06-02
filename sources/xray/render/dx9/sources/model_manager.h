////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_MANAGER_H_INCLUDED
#define MODEL_MANAGER_H_INCLUDED

#include <xray/collision/api.h>
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/object.h>

#include <xray/render/common/sources/convex_volume.h>
#include <xray/render/base/common_types.h>
#include "editor_visual.h"
#include <xray/render/common/sources/shader_manager.h>
#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

class scene_render;

struct tri
{
	u32				verts	[3];		// 3*4 = 12b
	union
	{
		u32			dummy;				// 4b
#pragma warning(push)
#pragma warning(disable:4201)
		struct
		{
			u32		material:14;		// 
			u32		suppress_shadows:1;	// 
			u32		suppress_wm:1;		// 
			u32		sector:16;			// 
		};
#pragma warning(pop)
	};
};

class collector
{
public:
	//void			add_face			( const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector	);
	//void			add_face_D			( const Fvector& v0, const Fvector& v1, const Fvector& v2, u32 dummy );
	//void			add_face_packed		( const Fvector& v0, const Fvector& v1, const Fvector& v2, u16 material, u16 sector, float eps = EPS );
	void			add_face_packed_d	( const float3& v0, const float3& v1, const float3& v2, u32 dummy, float eps = math::epsilon_5 );
 //   void			remove_duplicate_T	( );
	//void			calc_adjacency		( xr_vector<u32>& dest);

	float3*	get_vertices() {return &*m_verts.begin();}
	u32*	get_indices()	{return &*m_indices.begin();}
	u32*	get_tri_data() {return &*m_tri_data.begin();}

	u32	vertex_count() {return u32(m_verts.size());}
	u32	index_count() {return u32(m_indices.size());}

	u32	tri_count() { return u32(m_tri_data.size());}
	
	void clear()	{m_verts.clear(); m_indices.clear(); m_tri_data.clear();}

private:
	u32	vpack( const float3& v, float eps);

private:
	render::vector<float3>	m_verts;
	render::vector<u32>		m_indices;
	render::vector<u32>		m_tri_data;
};

struct non_copyable {
					non_copyable	() {}
private:
					non_copyable	(const non_copyable &) {}
					non_copyable	&operator=		(const non_copyable &) {}
};

struct b_portal
{
	u16						sector_front;
	u16						sector_back;
	float3					vertices[6];
	u32						count;
};

class	portal;
class	sector;

typedef math::rectangle<float2> box2f;

struct scissor
{
	box2f	rect;
	float	depth;
};

// Connector
class	portal
{
private:
	fixed_vector<float3,8>	m_poly;
	sector					*m_face;
	sector					*m_back;
public:
	math::plane		plane;
	math::sphere	sphere;

	u32		marker;
	bool	dual_render;

	void create(float3* verts, int vert_cnt, sector* face, sector* back);

	fixed_vector<float3, 8>& get_poly() {return m_poly;}
	sector*	back() {return m_back;}
	sector*	front() {return m_face;}
	sector*	get_sector(sector* from) {return from==m_face ? m_back : m_face;}
	sector*	get_sector_facing(const float3& v) {if (plane.classify(v)>0) return m_face; else return m_back;}
	sector*	get_sector_back	(const float3& v) {if (plane.classify(v)>0) return m_back; else return m_face;}
	//float	distance(const float3 &v) {return abs(plane.classify(v));}

	//portal();
	//virtual	~portal();
};

class sector
{
public:
	render_visual*	root() {return m_root;}
	void			traverse(math::frustum& view_frustum, scissor& rect);

	void create(render::vector<portal*> portals, render_visual* root)
	{
		m_portals.swap(portals);
		m_root = root;
	}

	void traverse(math::frustum& f/*,	_scissor& R*/);
	//virtual	~sector();

public:
	render::vector<convex_volume>	view_volumes;
	//render::vector<scissor>			scissors;
	//scissor					scissor_merged;
	u32						marker;

public:
	render_visual*	m_root;			// whole geometry of that sector
	render::vector<portal*>	m_portals;		//list of portals for this sector
};


//Some methods are not reentrant!!!!!
//Need some refactor
class model_manager: public quasi_singleton<model_manager>
{
public:
	model_manager():
		m_view_point(0, 0, 0),
		m_collision_object_static(0),
		m_collision_geom_static(0),
		m_collision_tree_static(0),
		m_collision_object_portals(0),
		m_collision_geom_portals(0),
		m_collision_tree_portals(0),
		m_marker				(u32(-1)),
		m_draw_game				(true),
		m_draw_editor			(true)

	{}

	~model_manager();

	void clear_resources		( );

	void init();

	virtual	void add_static		(resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals);

	void	load_vertex_buffers	( memory::chunk_reader& reader, u32 usage );
	void	load_index_buffers	( memory::chunk_reader& reader, u32 usage );
	void	load_visuals		( memory::chunk_reader& reader );
	void	load_shaders		( memory::chunk_reader& reader );
	void	load_sectors		( memory::chunk_reader& reader );

	void	render_static		();

	void	select_visuals(render_queue<key_rq, queue_item, key_rq_sort_desc>& rq_static);
	void	select_visuals(float3 view_pos, float4x4 mat_vp, render::vector<render_visual*>& selection);
	void	select_visuals(sector* start_sector, float4x4 mat_vp, render::vector<render_visual*>& selection);
	void	add_visual_to_selection(const convex_volume& frustum, render_visual* visual);

	void	add_visual_to_rq(const convex_volume& frustum, render_visual* visual);

	void	update(const float3& new_view_point, const float4x4& wvp_mat);

	inline ref_geometry	create_geom(u32 vb_id, u32 ib_id);
	inline ref_geometry	create_geom(u32 vb_id, u32 ib_id, ref_declaration decl);

	sector*	detect_sector(float3 view_point);
	sector*	detect_sector(float3 view_point, float3 dir);

	void	traverse(sector* current, const convex_volume& f/*,	_scissor& R*/);
	void	select_visible_sectors();

	static render_visual*	create_instance(u16 type);
	static void				destroy_instance(render_visual* v);
	static u16				get_visual_type(memory::chunk_reader& reader);

	u32				create_vb(u32 size, u32 usage, pcvoid data);
	u32				create_ib(u32 size, u32 usage, pcvoid data);

	//	These functions need to be moved into encapsulated index/vertex buffer classes
	//	"fragments" must to be sorted by start offset. 
	void 			update_vb(u32 buffer_id, xray::vectora<xray::render::buffer_fragment> const& fragments);
	void 			update_ib(u32 buffer_id, xray::vectora<xray::render::buffer_fragment> const& fragments);

	// This function is temporary!!!
	IDirect3DVertexBuffer9*		get_vbuffer_by_id( u32 id);
	
	render_visual*	get_visual(u32 idx) {ASSERT(idx<m_visuals.size()); return m_visuals[idx];}
	portal*			get_portal(u32 idx) {ASSERT(idx<m_portals.size()); return m_portals[idx];}
	sector*			get_sector(u32 idx) {ASSERT(idx<m_sectors.size()); return m_sectors[idx];}
	ref_shader		get_shader(u32 idx) {ASSERT(idx<m_shaders.size()); return m_shaders[idx];}

public:
	typedef render::vector< editor_visual >				Editor_Visuals;

	void			add_editor_visual	(u32 id, xray::render::visual_ptr v, float4x4 trans, bool selected, bool system_object, bool beditor);
	void			update_editor_visual(u32 id, float4x4 trans, bool selected);
	void			remove_editor_visual(u32 id );
	Editor_Visuals  &	get_editor_visuals( ) { return m_editor_visuals; }

	// This will temporary live here
	void			set_draw_editor	( bool value) { m_draw_editor = value;}
	void			set_draw_game	( bool value) { m_draw_game = value;}

	bool			get_draw_editor	(){ return m_draw_editor;}
	bool			get_draw_game	(){ return m_draw_game;}


private:
	typedef render::vector< ref_declaration >			Declarators;
	typedef render::vector< IDirect3DVertexBuffer9* >	VertexBuffers;
	typedef render::vector< IDirect3DIndexBuffer9* >	IndexBuffers;
	typedef render::vector< render_visual* >			Visuals;

private:
	void dump_visuals(u32 i, render_visual* visual);

private:
	render_queue<key_rq, queue_item, key_rq_sort_desc>	m_rq_static;

	
	collision::object*					m_collision_object_static;
	collision::geometry*				m_collision_geom_static;
	collision::space_partitioning_tree*	m_collision_tree_static;

	collision::object*					m_collision_object_portals;
	collision::geometry*				m_collision_geom_portals;
	collision::space_partitioning_tree*	m_collision_tree_portals;

	ref_shader		m_static_shader;
	ref_shader		m_tree_shader;

	render::vector<render_visual*>	m_visuals_selection;

	render::vector<sector*>		m_selection;
	sector*				m_start_sector;
	float3				m_view_point;
	float4x4			m_mx_form;

	render::vector<portal*>		m_portals;
	render::vector<sector*>		m_sectors;
	render::vector<ref_shader>	m_shaders;
	Visuals				m_visuals;

	Editor_Visuals		m_editor_visuals;

	VertexBuffers	m_vertex_buffers;
	IndexBuffers	m_index_buffers;
	Declarators		m_declarators;

	u32 m_marker;

	bool			m_draw_editor;
	bool			m_draw_game;

public:
	ref_shader		m_test_shader;
}; // class model_manager

struct models_cook : public resources::unmanaged_cook
{
								models_cook	();

	virtual	mutable_buffer		allocate_resource	(resources::query_result_for_cook &	in_query, 
													 const_buffer						raw_file_data, 
													 bool								file_exist)
	{
		XRAY_UNREFERENCED_PARAMETERS				(& in_query, &raw_file_data, file_exist);
		return										mutable_buffer::zero();
	}

	virtual void				deallocate_resource (pvoid) { }

	virtual void				create_resource		(resources::query_result_for_cook &	in_out_query, 
											 	 	const_buffer						raw_file_data, 
												 	mutable_buffer						in_out_unmanaged_resource_buffer); 

	virtual void				destroy_resource	(resources::unmanaged_resource* res);
};

#include "model_manager_inline.h"

} // namespace render 
} // namespace xray 


#endif // #ifndef MODEL_MANAGER_H_INCLUDED