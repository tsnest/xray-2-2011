////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "model_manager.h"
#include <xray/render/common/sources/hw_wrapper.h>
#include "scene_render.h"
#include "visual.h"

using xray::render::buffer_fragment;
namespace xray {
namespace render {


enum object_types_enum {
	object_type_geometry	= u32(1<<0),
}; // object_types_enum

u32	collector::vpack(const float3& v, float eps)
{
	vector<float3>::iterator	it  = m_verts.begin(),
								end = m_verts.end();

	for (; it != end; ++it)
		if (it->similar(v, eps)) return u32(it-m_verts.begin());
	
	m_verts.push_back(v);

	return u32(m_verts.size()-1);
}


void collector::add_face_packed_d(
		const float3& v0, const float3& v1, const float3& v2,	// vertices
		u32 dummy, float eps
	)
{
	m_indices.push_back(vpack(v0, eps));
	m_indices.push_back(vpack(v1, eps));
	m_indices.push_back(vpack(v2, eps));
	
	m_tri_data.push_back(dummy);
}

#define FRUSTUM_MAXPLANES	12
#define FRUSTUM_SAFE		(FRUSTUM_MAXPLANES*4)

typedef fixed_vector<float3, FRUSTUM_SAFE>		polygon;

void simplify_poly_aabb(polygon& poly, const math::plane& plane)
{
	float4x4	view, inv;
	float3		from, up, right, y;
	
	from = poly[0];
	if (abs(plane.normal.y) > 0.99f)
		up.set(1, 0, 0);
	else
		up.set(0, 1, 0);

	right = cross_product(y, plane.normal);
	//up.cross_product(plane.normal, right); //this seems excessive!!!!
	view = create_camera_direction(from, plane.normal, /*up*/right);

	// Project and find extents
	float3	min,max;
	min.set(math::float_max, math::float_max, 0);
	max.set(math::float_min, math::float_min, 0);
	for (u32 i=0; i<poly.size(); i++)
	{
		float3 tmp = view.transform(poly[i]);
		min.min(tmp/*.x,tmp.y*/);
		max.max(tmp/*.x,tmp.y*/);
	}

	// Build other 2 points and inverse project
	float3	p1,p2;
	p1.set(min.x, max.y, 0);
	p2.set(max.x, min.y, 0);
	inv = invert4x3(view);
	poly.clear	();

	poly.resize(4);

	poly[0] = inv.transform(min);
	poly[1] = inv.transform(p1);
	poly[2] = inv.transform(max);
	poly[3] = inv.transform(p2);
}

convex_volume create_frustum_from_portal(const polygon& portal, const float3& vPN, const float3& base, const float4x4& full_xform)
{
	XRAY_UNREFERENCED_PARAMETER	( vPN );

	math::plane	p;

	polygon poly(portal.begin(), portal.end());

	if (poly.size() > 6)
	{
		simplify_poly_aabb(poly, p);
	}

	// Check plane orientation relative to viewer
	// and reverse if needed
	if (p.classify(base)<0)
	{
		std::reverse(poly.begin(),poly.end());
	}

	// base creation
	math::plane	clip_planes[6];

	//near clip plane
	clip_planes[0] = math::create_plane_precise(poly[0], poly[1], poly[2]);

	// far clip plane
	float4x4 m = full_xform;
	clip_planes[1].normal.x	= -(m.e03/*_14*/ - m.e02/*_13*/);
	clip_planes[1].normal.y	= -(m.e13/*_24*/ - m.e12/*_23*/);
	clip_planes[1].normal.z	= -(m.e23/*_34*/ - m.e22/*_33*/);
	clip_planes[1].d		= -(m.e33/*_44*/ - m.e32/*_43*/);

	for(u32 i=2; i<6; ++i)
		clip_planes[i] = math::create_plane_precise(base, poly[i-2], poly[i-1]);

	convex_volume result;

	result.create_from_planes(clip_planes, 6);
	return result;//*math::cuboid(clip_planes);*/
}

polygon clip_polygon_against_volume(const convex_volume& f, const polygon& p)
{
	polygon	src;
	polygon	dest(p.begin(), p.end());

	for (u32 i=0; i<src.size(); ++i)
	{
		// cache plane and swap lists
		const math::plane &p = f.planes[i];
		std::swap(src,dest);
		dest.clear();

		// classify all points relative to plane #i
		float	cls	[FRUSTUM_SAFE];
		for (u32 j=0; j<src.size(); j++)
			cls[j]=p.classify(src[j]);

		// clip everything against this plane
		cls[src.size()] = cls[0];
		src.push_back(src[0]);
		float3 d; float denum,t;

		for (u32 j=0; j<src.size()-1; ++j)
		{
			if (src[j].similar(src[j+1], math::epsilon_7))
				continue;

			bool intersect = cls[j]*cls[j+1] < 0;

			if (cls[j] < 0) // optimize using (float & 0x80000000)
			{
				dest.push_back(src[j]);
			}

			if (intersect)
			{
				// segment intersects plane
				d = src[j+1] - src[j];
				denum = p.normal.dot_product(d);
				
				if (denum!=0)
				{
					t = -cls[j]/denum; //VERIFY(t<=1.f && t>=0);
					dest.back() = src[j] + d*t;
					dest.push_back(float3());
				}
			}
			//} else {
			//	// J - outside
			//	if (negative(cls[j+1]))
			//	{
			//		// J+1  - inside
			//		// segment intersects plane
			//		D.sub((*src)[j+1],(*src)[j]);
			//		denum = P.n.dotproduct(D);
			//		if (denum!=0) {
			//			t = -cls[j]/denum; //VERIFY(t<=1.f && t>=0);
			//			dest->last().mad((*src)[j],D,t);
			//			dest->inc();
			//		}
			//	}
			//}
		}

		// here we end up with complete polygon in 'dest' which is inside plane #i
		if (dest.size()<3)
		{
			dest.clear();
			break;
		}
	}

	return dest;
}

void model_manager::traverse(sector* current, const convex_volume& f/*,	_scissor& R*/)
{
	// Register traversal process
	if (m_marker != current->marker)
	{
		current->marker = m_marker;
		m_selection.push_back(current);
		current->view_volumes.clear();
		//r_scissors.clear();
	}

	float4x4 identity;
	identity.identity();

	current->view_volumes.push_back(f);
	//r_scissors.push_back		(R_scissor);

	polygon	s/*,d*/;
	vector<portal*>& portals = current->m_portals;
	for	(u32 i=0; i<portals.size(); i++)
	{
		if (portals[i]->marker == m_marker)
			continue;

		portal* cur_portal = portals[i];

		// early-out sphere
		if (!f.test(cur_portal->sphere))
			continue;

		sector* next = 0;

		// select sector (allow intersecting portals to be finely classified)
		if (cur_portal->dual_render)
		{
			next = cur_portal->get_sector(current);
		}
		else
		{
			next = cur_portal->get_sector_back(m_view_point);
			if (next == current)
				continue;

			if (next == m_start_sector)
				continue;
		}

		//// ssa	(if required)
		//if (portaltraverser.i_options&cportaltraverser::vq_ssa)
		//{
		//	fvector				dir2portal;
		//	dir2portal.sub		(portal->s.p,	portaltraverser.i_vbase);
		//	float r				=	portal->s.r	;
		//	float distsq		=	dir2portal.square_magnitude();
		//	float ssa			=	r*r/distsq;
		//	dir2portal.div		(_sqrt(distsq));
		//	ssa					*=	_abs(portal->p.n.dotproduct(dir2portal));
		//	if (ssa<r_ssadiscard)	continue;

		//	if (portaltraverser.i_options&cportaltraverser::vq_fade)	{
		//		if (ssa<r_ssalod_a)	portaltraverser.fade_portal			(portal,ssa);
		//		if (ssa<r_ssalod_b)	continue							;
		//	}
		//}

		// clip by frustum
		fixed_vector<float3,8>&	p = cur_portal->get_poly();
		s.assign(p.begin(), p.end());/* d.clear();*/
		
		polygon poly = clip_polygon_against_volume(f, s/*, d*/);

		if (poly.empty())
			continue;

		// scissor and optimized hom-testing
		//scissor	= r_scissor;

		//// cull by hom (slower algo)
		//if  (
		//	(portaltraverser.i_options&cportaltraverser::vq_hom) && 
		//	(!rimplementation.hom.visible(*p))
		//	)
		//	continue;

		// create _new_ frustum and recurse
		//math::cuboid	clip;
		convex_volume	clip;
		clip = create_frustum_from_portal(poly, cur_portal->plane.normal, m_view_point, m_mx_form);
		cur_portal->marker = m_marker;
		cur_portal->dual_render = false;
		traverse(next, clip);
	}

	// Search visible portals and go through them
	//sPoly	S,D;
	//for	(u32 I=0; I<m_portals.size(); I++)
	//{
	//	if (m_portals[I]->marker == PortalTraverser.i_marker) continue;

	//	CPortal* PORTAL = m_portals[I];
	//	CSector* pSector;

	//	// Select sector (allow intersecting portals to be finely classified)
	//	if (PORTAL->bDualRender) {
	//		pSector = PORTAL->getSector						(this);
	//	} else {
	//		pSector = PORTAL->getSectorBack					(PortalTraverser.i_vBase);
	//		if (pSector==this)								continue;
	//		if (pSector==PortalTraverser.i_start)			continue;
	//	}

	//	// Early-out sphere
	//	if (!F.testSphere_dirty(PORTAL->S.P,PORTAL->S.R))	continue;

	//	// SSA	(if required)
	//	if (PortalTraverser.i_options&CPortalTraverser::VQ_SSA)
	//	{
	//		Fvector				dir2portal;
	//		dir2portal.sub		(PORTAL->S.P,	PortalTraverser.i_vBase);
	//		float R				=	PORTAL->S.R	;
	//		float distSQ		=	dir2portal.square_magnitude();
	//		float ssa			=	R*R/distSQ;
	//		dir2portal.div		(_sqrt(distSQ));
	//		ssa					*=	_abs(PORTAL->P.n.dotproduct(dir2portal));
	//		if (ssa<r_ssaDISCARD)	continue;

	//		if (PortalTraverser.i_options&CPortalTraverser::VQ_FADE)	{
	//			if (ssa<r_ssaLOD_A)	PortalTraverser.fade_portal			(PORTAL,ssa);
	//			if (ssa<r_ssaLOD_B)	continue							;
	//		}
	//	}

	//	// Clip by frustum
	//	svector<Fvector,8>&	POLY = PORTAL->getPoly();
	//	S.assign			(&*POLY.begin(),POLY.size()); D.clear();
	//	sPoly* P			= F.ClipPoly(S,D);
	//	if (0==P)			continue;

	//	// Scissor and optimized HOM-testing
	//	_scissor			scissor	;
	//	if (PortalTraverser.i_options&CPortalTraverser::VQ_SCISSOR && (!PORTAL->bDualRender))
	//	{
	//		// Build scissor rectangle in projection-space
	//		Fbox2	bb;	bb.invalidate(); float depth = flt_max;
	//		sPoly&	p	= *P;
	//		for		(u32 vit=0; vit<p.size(); vit++)	{
	//			Fvector4	t;	
	//			Fmatrix&	M	= PortalTraverser.i_mXFORM_01;
	//			Fvector&	v	= p[vit];

	//			t.x = v.x*M._11 + v.y*M._21 + v.z*M._31 + M._41;
	//			t.y = v.x*M._12 + v.y*M._22 + v.z*M._32 + M._42;
	//			t.z = v.x*M._13 + v.y*M._23 + v.z*M._33 + M._43;
	//			t.w = v.x*M._14 + v.y*M._24 + v.z*M._34 + M._44;
	//			t.mul	(1.f/t.w);

	//			if (t.x < bb.min.x)	bb.min.x	= t.x; 
	//			if (t.x > bb.max.x) bb.max.x	= t.x;
	//			if (t.y < bb.min.y)	bb.min.y	= t.y; 
	//			if (t.y > bb.max.y) bb.max.y	= t.y;
	//			if (t.z < depth)	depth		= t.z;
	//		}
	//		// Msg	("bb(%s): (%f,%f)-(%f,%f), d=%f", PORTAL->bDualRender?"true":"false",bb.min.x, bb.min.y, bb.max.x, bb.max.y,depth);
	//		if (depth<EPS)	{
	//			scissor	= R_scissor;

	//			// Cull by HOM (slower algo)
	//			if  (
	//				(PortalTraverser.i_options&CPortalTraverser::VQ_HOM) && 
	//				(!RImplementation.HOM.visible(*P))
	//				)	continue;
	//		} else {
	//			// perform intersection (this is just to be sure, it is probably clipped in 3D already)
	//			if (bb.min.x > R_scissor.min.x)	scissor.min.x = bb.min.x; else scissor.min.x = R_scissor.min.x;
	//			if (bb.min.y > R_scissor.min.y)	scissor.min.y = bb.min.y; else scissor.min.y = R_scissor.min.y;
	//			if (bb.max.x < R_scissor.max.x) scissor.max.x = bb.max.x; else scissor.max.x = R_scissor.max.x;
	//			if (bb.max.y < R_scissor.max.y) scissor.max.y = bb.max.y; else scissor.max.y = R_scissor.max.y;
	//			scissor.depth	= depth;

	//			// Msg	("scissor: (%f,%f)-(%f,%f)", scissor.min.x, scissor.min.y, scissor.max.x, scissor.max.y);
	//			// Check if box is non-empty
	//			if (scissor.min.x >= scissor.max.x)	continue;
	//			if (scissor.min.y >= scissor.max.y)	continue;

	//			// Cull by HOM (faster algo)
	//			if  (
	//				(PortalTraverser.i_options&CPortalTraverser::VQ_HOM) && 
	//				(!RImplementation.HOM.visible(scissor,depth))
	//				)	continue;
	//		}
	//	} else {
	//		scissor	= R_scissor;

	//		// Cull by HOM (slower algo)
	//		if  (
	//			(PortalTraverser.i_options&CPortalTraverser::VQ_HOM) && 
	//			(!RImplementation.HOM.visible(*P))
	//			)	continue;
	//	}

	//	// Create _new_ frustum and recurse
	//	CFrustum				Clip;
	//	Clip.CreateFromPortal	(P, PORTAL->P.n, PortalTraverser.i_vBase,PortalTraverser.i_mXFORM);
	//	PORTAL->marker			= PortalTraverser.i_marker;
	//	PORTAL->bDualRender		= FALSE;
	//	pSector->traverse		(Clip,scissor);
	//}
}

void portal::create(float3* verts, int vert_cnt, sector* face, sector* back)
{
	// calc bounding sphere from bounding box
	math::aabb			bb;
	bb.invalidate();
	for (int v = 0; v < vert_cnt; ++v)
		bb.modify		(verts[v]);
	sphere = bb.sphere();

	//init
	m_poly.assign(verts, verts+vert_cnt);
	m_face				= face; 
	m_back				= back;
	marker				= 0xffffffff; 

	//float3				n, t;
	//n.set				(0,0,0);

	//calculate normal averaging polygon normals
	//u32	cnt	= 0;
	//for (int i = 2; i < vert_cnt; ++i)
	//{
	//	t = math::cross_product(m_poly[0]-m_poly[i-1], m_poly[i-1]-m_poly[i]);
	//	float m	= math::magnitude(t);
	//	if (m > epsilon_7)//skip if too small
	//	{
	//		n += t/m;
	//		++cnt;
	//	}
	//}
	//R_ASSERT(cnt && "Invalid portal detected");
	//n /=  float(cnt);
	//plane = create_plane(m_poly[0], n);


	//FIX THIS!!!!. LOOK UP FOR CODE!!!!!!
	plane = create_plane(m_poly[0], m_poly[1], m_poly[2]);
}

class simple_tree_blender: public blender
{
public:
	void compile(blender_compiler& compiler, const blender_compilation_options& options)
	{
		XRAY_UNREFERENCED_PARAMETER	( options );

		shader_defines_list	defines;
		make_defines(defines);

		compiler.begin_technique(/*LEVEL*/);
		compiler.begin_pass("tree", "tree", defines)
				.set_z_test(TRUE)
				.set_alpha_blend(FALSE)
			.end_pass()
		.end_technique();
	}
};
class simple_test_blender: public blender
{
public:
	void compile(blender_compiler& compiler, const blender_compilation_options& options)
	{
		XRAY_UNREFERENCED_PARAMETERS	( options );
		shader_defines_list	defines;
		make_defines(defines);

		compiler.begin_technique(/*LEVEL*/);
		compiler.begin_pass("test", "test", defines)
				.set_z_test(TRUE)
				.set_alpha_blend(FALSE)
			.end_pass()
		.end_technique();
	}
};
enum chunks {
	chunk_shaders			= 2,
	chunk_vertex			= 9,
	chunk_index				= 10,
	chunk_visuals			= 3,
	chunk_portals			= 4,
	chunk_sectors			= 8,
}; // enum chunks

enum sector_chunks
{
	sector_chunk_portals		= 1,
	sector_chunk_root_visual	= 2

};

void model_manager::select_visible_sectors()
{
}

void model_manager::init()
{
	simple_static_blender b_simple_static;
	m_static_shader = shader_manager::get_ref().create_shader(&b_simple_static);
	
	simple_tree_blender b_simple_tree;
	m_tree_shader = shader_manager::get_ref().create_shader(&b_simple_tree);

	simple_test_blender b_simple_test;
	m_test_shader = shader_manager::get_ref().create_shader(&b_simple_test);
}

model_manager::~model_manager()
{
	if ( m_collision_geom_static )
		collision::destroy(m_collision_geom_static);
	
	if ( m_collision_object_static )
		collision::destroy(m_collision_object_static);

	if ( m_collision_tree_static )
		collision::destroy(m_collision_tree_static);

	collision::destroy(m_collision_geom_portals);
	collision::destroy(m_collision_object_portals);
	collision::destroy(m_collision_tree_portals);

	Visuals::iterator			i = m_visuals.begin( );
	Visuals::iterator			e = m_visuals.end( );

	for ( ; i != e; ++i )
		if (*i)
			model_manager::destroy_instance(*i);

	for (u32 i = 0; i < m_sectors.size(); ++i)
	{
		DELETE(m_sectors[i]);
	}

	for (u32 i = 0; i < m_portals.size(); ++i)
	{
		DELETE(m_portals[i]);
	}

	for (u32 i = 0; i < m_vertex_buffers.size(); ++i)
	{
		safe_release(m_vertex_buffers[i]);
	}

	for (u32 i = 0; i < m_index_buffers.size(); ++i)
	{
		safe_release(m_index_buffers[i]);
	}
}

void model_manager::load_vertex_buffers( memory::chunk_reader& reader, u32 const usage )
{
	memory::reader chunk	= reader.open_reader( chunk_vertex );

	u32 const count			= chunk.r_u32( );

	m_declarators.resize	( count );
	m_vertex_buffers.resize	( count );

	for ( u32 i=0 ; i<count ; ++i ) {
		D3DVERTEXELEMENT9* const dcl	= ( D3DVERTEXELEMENT9* ) chunk.pointer( );
		u32 const dcl_len				= D3DXGetDeclLength( dcl ) + 1;
		//R_CHK(device().CreateVertexDeclaration( dcl, &m_declarators[i] ));
		m_declarators[i] = resource_manager::get_ref().create_decl(dcl);
		chunk.advance( dcl_len*sizeof( D3DVERTEXELEMENT9 ) );

		u32 vCount			= chunk.r_u32	();
		u32 vSize			= D3DXGetDeclVertexSize	(dcl,0);
		LOG_INFO			("[Loading VB] %d verts, %d Kb",vCount,(vCount*vSize)/1024);

		// Create and fill
		BYTE* pData			= 0;
		R_CHK				( hw_wrapper::get_ref().device()->CreateVertexBuffer( vCount*vSize, usage, 0, D3DPOOL_MANAGED, &m_vertex_buffers[i], 0) );
		R_CHK				( m_vertex_buffers[i]->Lock( 0, 0, (void**)&pData, 0 ) );
		chunk.r				( pData, vCount*vSize, vCount*vSize );
		R_CHK				( m_vertex_buffers[i]->Unlock( ) );
	}
}

void model_manager::load_index_buffers( memory::chunk_reader& reader, u32 const usage )
{
	memory::reader chunk	= reader.open_reader( chunk_index );
	u32 const count			= chunk.r_u32( );
	m_index_buffers.resize	( count );
	for (u32 i=0; i<count; ++i) {
		u32 const index_count = 2*chunk.r_u32( );
		LOG_INFO			("[Loading IB] %d indices, %d Kb", index_count/2, index_count/1024 );

		// Create and fill
		BYTE* pData			= 0;
		R_CHK				( hw_wrapper::get_ref().device()->CreateIndexBuffer( index_count, usage, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_index_buffers[i], 0 ) );
		R_CHK				( m_index_buffers[i]->Lock( 0, 0, (void**)&pData, 0 ) );
		chunk.r				( pData, index_count, index_count );
		R_CHK				( m_index_buffers[i]->Unlock ( ) );
	}
}

void model_manager::load_visuals( memory::chunk_reader& reader )
{
	memory::chunk_reader chunk = reader.open_chunk_reader( chunk_visuals );
	for ( u32 i=0; ; ++i ) {
		if ( !chunk.chunk_exists( i ) )
			break;

		memory::chunk_reader sub_chunk		= chunk.open_chunk_reader( i );
		u16 type					= /*render::*/model_manager::get_visual_type( sub_chunk );

		/*render::*/render_visual* const visual = /*render::*/model_manager::create_instance( type );
		//ASSERT(visual);
		if(visual)
			visual->load			( sub_chunk );

		m_visuals.push_back			( visual );
	}
}

void model_manager::load_shaders(memory::chunk_reader& reader)
{
	memory::reader chunk = reader.open_reader(chunk_shaders);
	u32 count = chunk.r_u32();
	m_shaders.resize(count);
	for(u32 i=0; i<count; ++i)	// skip first shader as "reserved" one
	{
		fixed_string512	n_sh, n_tlist;

		n_sh = chunk.r_string();
		//chunk.skip_string();
		if (n_sh.empty())
			continue;

		LPSTR begin = n_sh.c_str();
		LPSTR delim	= strchr(begin, '/');
		u32 size = static_cast<u32>(delim - begin);

		n_tlist = n_sh.substr(size+1);
		n_sh = n_sh.substr(0, size);

		utils::fix_path(n_tlist.get_buffer());

		LOG_INFO("LOADING shader: %s, texture %s", n_sh.c_str(), n_tlist.c_str());

		m_shaders[i] = shader_manager::get_ref().create_shader(n_sh.c_str(), n_tlist.c_str());
	}
}

void model_manager::dump_visuals(u32 i, render_visual* visual)
{
	fixed_string<256> off;
	for(u32 ii=0; ii<i; ++ii, off+="\t");

	if (!visual)
	{
		LOG_ERROR("%sVisual not created", off.c_str());
		return;
	}

	Visuals::iterator	begin = m_visuals.begin(),
						end = m_visuals.end(),
						it = std::find(begin, end, visual);
	
	if (it == end)
	{
		LOG_ERROR("%sVisual not registered", off.c_str());
	}
	else
	{
		LOG_DEBUG("%sVisual %d", off.c_str(), it-begin);
	}

	if (visual->type == mt_hierrarchy)
	{
		hierrarchy_visual* container = static_cast_checked<hierrarchy_visual*>(visual);
		
		hierrarchy_visual::childs_it	it  = container->m_childs.begin(),
										end = container->m_childs.end();
		for (; it != end; ++it)
		{
			dump_visuals(i+1, *it);
		}
	}
}

void model_manager::load_sectors( memory::chunk_reader& reader )
{
	memory::reader	portals = reader.open_reader(chunk_portals);
	u32 size = portals.length();
	
	R_ASSERT(size%sizeof(b_portal) == 0);
	
	u32 count = size/sizeof(b_portal);
	m_portals.resize(count);

	for (u32 i = 0; i < count; ++i)
	{
		m_portals[i] = NEW(portal);
	}

	memory::chunk_reader	sectors = reader.open_chunk_reader(chunk_sectors);
	
	u32 idx = 0;
	while (sectors.chunk_exists(idx))
	{
		memory::chunk_reader sector_i = sectors.open_chunk_reader(idx);

		memory::reader p = sector_i.open_reader(sector_chunk_portals);
		u32 count			= p.length()/2;

		vector<portal*> portals;
		portals.reserve(count);
	
		while (count--)
		{
			u16 portal_id = p.r_u16();
			portals.push_back(get_portal(portal_id));
		}
		
		//// Assign visual
		//size	= fs.find_chunk(fsP_Root);	R_ASSERT(size==4);
		//m_root	= (dxRender_Visual*)RImplementation.getVisual	(fs.r_u32());
		
		memory::reader v = sector_i.open_reader(sector_chunk_root_visual);
		
		u32 visual_id = v.r_u32();

		sector*	sect = NEW(sector);
		render_visual* root = get_visual(visual_id);
		sect->create(portals, root);
		m_sectors.push_back(sect);
		
		dump_visuals(0, get_visual(visual_id));
		LOG_DEBUG("Total visuals %d", m_visuals.size());

		++idx;
	}

	if (count)
	{
		collector	portal_geom;
		for (u32 i = 0; i < count; ++i)
		{
			b_portal	p;
			
			portals.r		( &p, sizeof(p), sizeof(p) );
			portal*	prtl	= (portal*)m_portals[i];
			prtl->create(
				p.vertices, p.count,
				get_sector(p.sector_front), get_sector(p.sector_back));

			for (u32 j=2; j<p.count; j++)
				portal_geom.add_face_packed_d(p.vertices[0],p.vertices[j-1],p.vertices[j],i);
		}

		m_collision_geom_portals	= 
			&*collision::create_triangle_mesh_geometry(
				xray::render::g_allocator,
				portal_geom.get_vertices(),
				portal_geom.vertex_count(),
				portal_geom.get_indices(),
				portal_geom.index_count(),
				portal_geom.get_tri_data(),
				portal_geom.tri_count()
			);

		m_collision_object_portals	= &*
			collision::create_geometry_object(
				xray::render::g_allocator,
				object_type_geometry,
				float4x4().identity(),
				m_collision_geom_portals
			);

		m_collision_tree_portals = &*collision::create_space_partitioning_tree(
				xray::render::g_allocator,
				1.f,
				1024
			);

		m_collision_tree_portals->insert	(
			m_collision_object_portals,
			m_collision_object_portals->get_aabb_center(),
			m_collision_object_portals->get_aabb_extents()
		);
	}

	// allocate memory for portals
	//u32 size = fs->find_chunk(fsL_PORTALS); 
	//R_ASSERT(0==size%sizeof(b_portal));
	//u32 count = size/sizeof(b_portal);
	//Portals.resize	(count);
	//for (u32 c=0; c<count; c++)
	//	Portals[c]	= xr_new<CPortal> ();

	//// load sectors
	//IReader* S = fs->open_chunk(fsL_SECTORS);
	//for (u32 i=0; ; i++)
	//{
	//	IReader* P = S->open_chunk(i);
	//	if (0==P) break;

	//	CSector* __S		= xr_new<CSector> ();
	//	__S->load			(*P);
	//	Sectors.push_back	(__S);

	//	P->close();
	//}
	//S->close();

	//// load portals
	//if (count) 
	//{
	//	CDB::Collector	CL;
	//	fs->find_chunk	(fsL_PORTALS);
	//	for (i=0; i<count; i++)
	//	{
	//		b_portal	P;
	//		fs->r		(&P,sizeof(P));
	//		CPortal*	__P	= (CPortal*)Portals[i];
	//		__P->Setup	(P.vertices.begin(),P.vertices.size(),
	//			(CSector*)getSector(P.sector_front),
	//			(CSector*)getSector(P.sector_back));
	//		for (u32 j=2; j<P.vertices.size(); j++)
	//			CL.add_face_packed_D(
	//			P.vertices[0],P.vertices[j-1],P.vertices[j],
	//			u32(i)
	//			);
	//	}
	//	if (CL.getTS()<2)
	//	{
	//		Fvector					v1,v2,v3;
	//		v1.set					(-20000.f,-20000.f,-20000.f);
	//		v2.set					(-20001.f,-20001.f,-20001.f);
	//		v3.set					(-20002.f,-20002.f,-20002.f);
	//		CL.add_face_packed_D	(v1,v2,v3,0);
	//	}

	//	// build portal model
	//	rmPortals = xr_new<CDB::MODEL> ();
	//	rmPortals->build	(CL.getV(),int(CL.getVS()),CL.getT(),int(CL.getTS()));
	//} else {
	//	rmPortals = 0;
	//}

	//pLastSector = 0;
}

extern D3DXMATRIXA16				view_d3d;
extern D3DXMATRIXA16				projection_d3d;

void model_manager::render_static()
{
	if(m_visuals.empty())
		return;

	u16 type = 999;
	Visuals::const_iterator		i = m_visuals.begin( );
	Visuals::const_iterator		e = m_visuals.end( );
	for ( ; i != e; ++i )
		if(*i)
		{
			if((*i)->type!=type)
			{
				if((*i)->type==mt_tree_st)
				{
					m_tree_shader->apply();
				}else
				{
					m_static_shader->apply();
				}

				type = (*i)->type;
			}
			(*i)->render			( );
		}
}

//IRender_Sector* CRender::detectSector(const Fvector& P)
//{
//	IRender_Sector*	S	= NULL;	
//	Fvector			dir; 
//	Sectors_xrc.ray_options		(CDB::OPT_ONLYNEAREST);
//
//	dir.set				(0,-1,0);
//	S					= detectSector(P,dir);
//	if (NULL==S)		
//	{
//		dir.set				(0,1,0);
//		S					= detectSector(P,dir);
//	}
//	return S;
//}
//
//IRender_Sector* CRender::detectSector(const Fvector& P, Fvector& dir)
//{
//	// Portals model
//	int		id1		= -1;
//	float	range1	= 500.f;
//	if (rmPortals)	
//	{
//		Sectors_xrc.ray_query	(rmPortals,P,dir,range1);
//		if (Sectors_xrc.r_count()) {
//			CDB::RESULT *RP1 = Sectors_xrc.r_begin();
//			id1 = RP1->id; range1 = RP1->range; 
//		}
//	}
//
//	// Geometry model
//	int		id2		= -1;
//	float	range2	= range1;
//	Sectors_xrc.ray_query	(g_pGameLevel->ObjectSpace.GetStaticModel(),P,dir,range2);
//	if (Sectors_xrc.r_count()) {
//		CDB::RESULT *RP2 = Sectors_xrc.r_begin();
//		id2 = RP2->id; range2 = RP2->range;
//	}
//
//	// Select ID
//	int ID;
//	if (id1>=0) {
//		if (id2>=0) ID = (range1<=range2+EPS)?id1:id2;	// both was found
//		else ID = id1;									// only id1 found
//	} else if (id2>=0) ID = id2;						// only id2 found
//	else return 0;
//
//	if (ID==id1) {
//		// Take sector, facing to our point from portal
//		CDB::TRI*	pTri	= rmPortals->get_tris() + ID;
//		CPortal*	pPortal	= (CPortal*) Portals[pTri->dummy];
//		return pPortal->getSectorFacing(P);
//	} else {
//		// Take triangle at ID and use it's Sector
//		CDB::TRI*	pTri	= g_pGameLevel->ObjectSpace.GetStaticTris()+ID;
//		return getSector(pTri->sector);
//	}
//}

sector*	model_manager::detect_sector(float3 view_point)
{
	float3	dir; 

	dir.set				(0,-1,0);
	sector* sect = detect_sector(view_point, dir);
	
	if (!sect)		
	{
		dir.set				(0,1,0);
		sect = detect_sector(view_point, dir);
	}
	return sect;
}

bool get_first(collision::ray_triangle_result const&) {return true;}

sector*	model_manager::detect_sector(float3 view_point, float3 dir)
{
	collision::ray_triangles_type	results(xray::render::g_allocator);

	// Portals model
	int		id1		= -1;
	float	range1	= 500.f;
	if (m_collision_tree_portals)	
	{
		//Sectors_xrc.ray_query	(rmPortals,P,dir,range1);
		//if (Sectors_xrc.r_count()) {
		//	CDB::RESULT *RP1 = Sectors_xrc.r_begin();
		//	id1 = RP1->id; range1 = RP1->range; 
		//}

		//Sectors_xrc.box_options	(CDB::OPT_FULL_TEST);
		m_collision_tree_portals->ray_query( object_type_geometry, view_point, dir, range1, results, collision::triangles_predicate_type(&get_first));

		id1 = results[0].triangle_id;
	}
	
	results.clear();

	// Geometry model
	int		id2		= -1;
	float	range2	= range1;
	//Sectors_xrc.ray_query	(g_pGameLevel->ObjectSpace.GetStaticModel(),P,dir,range2);
	//if (Sectors_xrc.r_count()) {
	//	CDB::RESULT *RP2 = Sectors_xrc.r_begin();
	//	id2 = RP2->id; range2 = RP2->range;
	//}

//////////////////////////////////////////////////////////////////////////
// This need to be uncommented to enable collision.
//**********************************************************************
// 	if (m_collision_tree_static->ray_query( object_type_geometry, view_point,
// 							dir, range2, results, collision::triangles_predicate_type(&get_first)))
// 	{
// 		id2 = results[0].triangle_id;
// 	}
	
	R_ASSERT(id2>=-1);

	// Select ID
	int ID;

	if (id1>=0)
	{
		if (id2>=0)
			ID = (range1 <= range2+math::epsilon_5) ? id1 : id2;	// both was found
		else
			ID = id1;								// only id1 found
	}
	else if (id2>=0)
		ID = id2;						// only id2 found
	else
		return 0;

	if (ID==id1)
	{
		// Take sector, facing to our point from portal
		u32	portal_id	= m_collision_geom_portals->get_custom_data(ID);
		portal*	portal	= get_portal(portal_id);
		return portal->get_sector_facing(view_point);
	}
	else
	{
//////////////////////////////////////////////////////////////////////////
// This need to be uncommented to enable collision.
//**********************************************************************
		// Take triangle at ID and use it's Sector
//		tri tri;
//		tri.dummy = m_collision_geom_static->get_custom_data(ID);
//		return get_sector(tri.sector);
		return get_sector( 0 );
	}
}

//void model_manager::select_visuals(render_queue<key_rq, queue_item, key_rq_sort_desc>& rq_static)
//{
//	if (m_visuals.empty())
//		return;
//
//	m_rq_static.swap(rq_static);
//
//	math::frustum f(m_mx_form);
//	float4x4 identity;
//	identity.identity();
//	
//	convex_volume cull_volume;//(f, identity);
//	
//	cull_volume.create_from_matrix(m_mx_form);
//
//	if (m_start_sector)
//	{
//
//		traverse(m_start_sector, cull_volume);
//
//		for (u32 i=0; i<m_selection.size(); ++i)
//		{
//			sector*			sector = m_selection[i];
//			render_visual*	root = sector->root();
//			for (u32 j=0; j<sector->view_volumes.size(); ++j)
//				add_visual_to_rq(sector->view_volumes[j], root);
//		}
//		m_selection.clear();
//	}
//
//	rq_static.swap(m_rq_static);
//}
//

void model_manager::select_visuals(float3 view_pos, float4x4 mat_vp, vector<render_visual*>& selection)
{
	if ( m_visuals.empty() )
		return;

//////////////////////////////////////////////////////////////////////////
// This need to be removed to enable collision.
//**********************************************************************
 	selection = m_visuals;
//**********************************************************************
#if 1
	XRAY_UNREFERENCED_PARAMETERS	( mat_vp, view_pos );
 	return;
#else // #if 1
	m_visuals_selection.swap(selection);

	math::frustum f(mat_vp);
	float4x4 identity;
	identity.identity();
	
	convex_volume cull_volume;//(f, identity);
	cull_volume.create_from_matrix(mat_vp);
	update(view_pos, mat_vp);

	if (m_start_sector)
	{

		traverse(m_start_sector, cull_volume);

		for (u32 i=0; i<m_selection.size(); ++i)
		{
			sector*			sector = m_selection[i];
			render_visual*	root = sector->root();
			for (u32 j=0; j<sector->view_volumes.size(); ++j)
				add_visual_to_selection(sector->view_volumes[j], root);
		}
		m_selection.clear();
	}

	m_visuals_selection.swap(selection);
#endif // #if 1
}

void model_manager::select_visuals(sector* start_sector, float4x4 mat_vp, vector<render_visual*>& selection)
{
	if (m_visuals.empty())
		return;

	m_visuals_selection.swap(selection);

	convex_volume cull_volume;
	cull_volume.create_from_matrix(mat_vp);
	//update(view_pos, mat_vp);

	if (start_sector)
	{

		traverse(start_sector, cull_volume);

		for (u32 i=0; i<m_selection.size(); ++i)
		{
			sector*			sector = m_selection[i];
			render_visual*	root = sector->root();
			for (u32 j=0; j<sector->view_volumes.size(); ++j)
				add_visual_to_selection(sector->view_volumes[j], root);
		}
		m_selection.clear();
	}

	m_visuals_selection.swap(selection);
}

void model_manager::add_visual_to_selection(const convex_volume& frustum, render_visual* visual)
{
	if (!visual)
		return;

	//!!!!!!!!FIX ME!!!!!!!!!!!
	////skip already added visuals
	//if (visual.marker_id == marker_id)
	//	return;

	if (frustum.test(visual->m_sphere) == math::intersection_outside)
		return;

	if (visual->type == mt_hierrarchy)
	{
		hierrarchy_visual* container = static_cast_checked<hierrarchy_visual*>(visual);
		
		hierrarchy_visual::childs_it	it  = container->m_childs.begin(),
										end = container->m_childs.end();
		for (; it != end; ++it)
		{
			add_visual_to_selection(frustum, *it);
		}
	}
	else
	{
		//if (!visual->m_shader)
		//	return;

		//ref_pass pass = visual->m_shader->m_sh_techniques[0]->m_passes[0];
		//key_rq key;
		//key.priority = pass->get_priority();
		//queue_item item = {pass, visual, 0};
		m_visuals_selection.push_back(visual);
		//m_rq_static.add_dip(key, item);
	}
}

//void model_manager::add_visual_to_rq(const convex_volume& frustum, render_visual* visual)
//{
//	if (!visual)
//		return;
//
//	//!!!!!!!!FIX ME!!!!!!!!!!!
//	////skip already added visuals
//	//if (visual.marker_id == marker_id)
//	//	return;
//
//	if (frustum.test(visual->m_sphere) == math::intersection_outside)
//		return;
//
//	if (visual->type == mt_hierrarchy)
//	{
//		hierrarchy_visual* container = static_cast_checked<hierrarchy_visual*>(visual);
//		
//		hierrarchy_visual::childs_it	it  = container->m_childs.begin(),
//										end = container->m_childs.end();
//		for (; it != end; ++it)
//		{
//			add_visual_to_rq(frustum, *it);
//		}
//	}
//	else
//	{
//		if (!visual->m_shader)
//			return;
//
//		ref_pass pass = visual->m_shader->m_sh_techniques[0]->m_passes[0];
//		key_rq key;
//		key.priority = pass->get_priority();
//		queue_item item = {pass, visual, 0};
//		m_rq_static.add_dip(key, item);
//	}
//}
//
void model_manager::update(const float3& new_view_point, const float4x4& wvp_mat)
{
	if (m_visuals.empty())
		return;

	m_mx_form = wvp_mat;
	++m_marker;
	// Detect camera-sector
	if (!m_view_point.similar(new_view_point, math::epsilon_7)) 
	{
		sector* current = detect_sector(new_view_point);
		//if (sector && (sector!=pLastSector))
		//	g_pGamePersistent->OnSectorChanged( translateSector(pSector) );

		if (!current) current = m_start_sector;
		m_start_sector = current;
		m_view_point = new_view_point;
	}

	// Check if camera is too near to some portal - if so force DualRender
	if (m_collision_tree_portals) 
	{
		float eps = 0.1f/*FIX ME!!!!:near plane value!!!!!*/ + math::epsilon_3;
		math::aabb aabb;
		aabb.modify(m_view_point-eps);
		aabb.modify(m_view_point+eps);

		collision::triangles_type	results(xray::render::g_allocator);
		//Sectors_xrc.box_options	(CDB::OPT_FULL_TEST);
		m_collision_tree_portals->aabb_query( object_type_geometry, aabb, results);

		collision::triangles_type::iterator	it  = results.begin(),
										end = results.end();

		for (; it != end; ++it)
		{
			u32 id = dynamic_cast<const collision::geometry_instance*>(it->object)->
				get_geometry()->get_custom_data(it->triangle_id); /*rmPortals->get_tris()[Sectors_xrc.r_begin()[K].id].dummy*/
			//u32 id = it->geometry->get_custom_data(it->triangle_id); /*rmPortals->get_tris()[Sectors_xrc.r_begin()[K].id].dummy*/
			portal*	portal = get_portal(id);
			portal->dual_render	= true;
		}
	}
}

void model_manager::clear_resources	( )
{
	collision::destroy				( m_collision_tree_static );
	m_collision_tree_static			= 0;

	collision::destroy				( m_collision_object_static );
	m_collision_object_static		= 0;

	collision::destroy				( m_collision_geom_static );
	m_collision_geom_static			= 0;
}

void model_manager::add_static( resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals )
{
	XRAY_UNREFERENCED_PARAMETER	( collision );
	u32 const usage				= D3DUSAGE_WRITEONLY | (/**HW.Caps.geometry.bSoftware?D3DUSAGE_SOFTWAREPROCESSING:/**/0);

	typedef memory::chunk_reader	Reader;

	{
		resources::pinned_ptr_const<u8> const buffer( geometry );
		Reader					reader( buffer.c_ptr(), buffer.size(), memory::chunk_reader::chunk_type_sequential );
		load_vertex_buffers		( reader, usage );
		load_index_buffers		( reader, usage );
	}
	{
		resources::pinned_ptr_const<u8> const buffer ( visuals );
		Reader					reader( buffer.c_ptr(), buffer.size(), memory::chunk_reader::chunk_type_sequential );
		if (!FALSE)
		{
			load_shaders			( reader );
		}
		load_visuals			( reader );
		load_sectors			( reader );
	}

//////////////////////////////////////////////////////////////////////////
// This need to be uncommented to enable collision.
//**********************************************************************
// 	m_collision_geom_static		= 
// 		&*collision::create_triangle_mesh_geometry(
// 			xray::render::g_allocator,
// 			collision
// 		);

// 	m_collision_object_static	= &*
// 		collision::create_geometry_object(
// 			xray::render::g_allocator,
// 			object_type_geometry,
// 			float4x4().identity(),
// 			m_collision_geom_static
// 		);

// 	m_collision_tree_static = &*collision::create_space_partitioning_tree(
// 			xray::render::g_allocator,
// 			1.f,
// 			1024
// 		);
// 
// 	m_collision_tree_static->insert	(
// 		m_collision_object_static,
// 		m_collision_object_static->get_aabb_center(),
// 		m_collision_object_static->get_aabb_extents()
// 	);
}

u16 model_manager::get_visual_type(memory::chunk_reader& chunk)
{
	memory::reader	ogf = chunk.open_reader( ogf_chunk_header );
	ogf_header		header;
	ogf.r			(&header, sizeof(header), sizeof(header));
	return			header.type;
}

void model_manager::destroy_instance(render_visual* v)
{
	XRAY_DELETE_IMPL(	*::xray::render::g_allocator, v );
}

render_visual* model_manager::create_instance(u16 type)
{
	render_visual* result	= NULL;
	switch(type)
	{
	case mt_normal:
		result = NEW(simple_visual)();
		break;

	case mt_hierrarchy:
		result = NEW(hierrarchy_visual)();
		break;

	case mt_progressive:
		result = NEW(simple_visual_pm)();
		break;

	case mt_tree_st:
		result = NEW(tree_visual)();
		break;

	case mt_lod:
	case mt_skeleton_rigid:
		return NULL;
		break;

	case mt_terrain_cell:
		result = NEW(terrain_visual)();
		break;

	default:
		NODEFAULT();
	};

	result->type	= type;
	return result;
}

u32 model_manager::create_vb(u32 size, u32 usage, pcvoid src_data)
{//Potencial memory leak here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	u32 result_idx		= m_vertex_buffers.size();
	m_vertex_buffers.resize(m_vertex_buffers.size()+1);
	BYTE* pLockData		= 0;

	R_CHK				( hw_wrapper::get_ref().device()->CreateVertexBuffer( size, usage, 0, D3DPOOL_MANAGED, &m_vertex_buffers[result_idx], 0) );
	R_CHK				( m_vertex_buffers[result_idx]->Lock( 0, 0, (void**)&pLockData, 0 ) );
	memory::copy		( pLockData, size, src_data, size );
	R_CHK				( m_vertex_buffers[result_idx]->Unlock( ) );

	return result_idx;
}

u32 model_manager::create_ib(u32 size, u32 usage, pcvoid src_data)
{//Potencial memory leak here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	u32 result_idx		= m_index_buffers.size();
	m_index_buffers.resize(m_index_buffers.size()+1);
	BYTE* pLockData		= 0;

	R_CHK				( hw_wrapper::get_ref().device()->CreateIndexBuffer( size, usage, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_index_buffers[result_idx], 0 ) );
	R_CHK				( m_index_buffers[result_idx]->Lock( 0, 0, (void**)&pLockData, 0 ) );
	memory::copy		( pLockData, size, src_data, size );
	R_CHK				( m_index_buffers[result_idx]->Unlock ( ) );

	return result_idx;
}

void model_manager::update_vb(u32 buffer_id, xray::vectora<buffer_fragment> const& fragments)
{
	ASSERT( buffer_id < m_vertex_buffers.size() );
	ASSERT( fragments.size() > 0 );

	BYTE* pLockData		= 0;

	// Assume that the fragments are sorted.
	u32 lock_start	= fragments[0].start;
	u32 lock_size	= (fragments[fragments.size()-1].start+fragments[fragments.size()-1].size)-lock_start;
	ASSERT( lock_size > 0 );

	R_CHK				( m_vertex_buffers[buffer_id]->Lock( lock_start, lock_size, (void**)&pLockData, 0 ) );

	vectora<buffer_fragment>::const_iterator	it	= fragments.begin();
	vectora<buffer_fragment>::const_iterator	end	= fragments.end();

	for( ; it != end; ++it)
		memory::copy		( pLockData + it->start - lock_start, it->size, it->buffer, it->size );

	R_CHK				( m_vertex_buffers[buffer_id]->Unlock ( ) );
}

void model_manager::update_ib(u32 buffer_id, xray::vectora<xray::render::buffer_fragment> const& fragments)
{
	ASSERT( buffer_id < m_index_buffers.size() );
	ASSERT( fragments.size() > 0 );

	BYTE* pLockData		= 0;

	// Assume that the fragments are sorted.
	u32 lock_start	= fragments[0].start;
	u32 lock_size	= (fragments[fragments.size()-1].start+fragments[fragments.size()-1].size)-lock_start;
	ASSERT( lock_size > 0 );

	R_CHK				( m_index_buffers[buffer_id]->Lock( lock_start, lock_size, (void**)&pLockData, 0 ) );

	vectora<buffer_fragment>::const_iterator	it	= fragments.begin();
	vectora<buffer_fragment>::const_iterator	end	= fragments.end();

	for( ; it != end; ++it)
		memory::copy		( pLockData + it->start - lock_start, it->size, it->buffer, it->size );

	R_CHK				( m_index_buffers[buffer_id]->Unlock ( ) );
}

IDirect3DVertexBuffer9*		model_manager::get_vbuffer_by_id( u32 id)
{
	ASSERT( id < m_vertex_buffers.size());
	return m_vertex_buffers[id];
}


models_cook::models_cook( )
: unmanaged_cook( resources::visual_class, reuse_true, use_current_thread_id, use_current_thread_id) 
{}
extern bool b_test;

void   models_cook::create_resource (resources::query_result_for_cook &	in_out_query, 
						 	 		 const_buffer						raw_file_data, 
							 		 mutable_buffer						in_out_unmanaged_resource_buffer)
{
	XRAY_UNREFERENCED_PARAMETER		(in_out_unmanaged_resource_buffer);

	memory::chunk_reader chunk		( (pbyte)raw_file_data.c_ptr(), raw_file_data.size(), memory::chunk_reader::chunk_type_sequential );

	u16 type						= model_manager::get_ref().get_visual_type( chunk );

	b_test = true;
	render_visual * const visual 	= model_manager::get_ref().create_instance( type );
	if(visual)
		visual->load				( chunk );

	b_test							= false;

#pragma message(XRAY_TODO("pass correct sizeof, not sizeof of a base class"))
	in_out_query.set_unmanaged_resource	(visual, resources::memory_type_non_cacheable_resource, sizeof(render_visual));
	in_out_query.finish_query		(result_success);
}

void   models_cook::destroy_resource (resources::unmanaged_resource * const res)
{
	render_visual* const visual	= dynamic_cast<render_visual*>(res);
	R_ASSERT						(visual);
	model_manager::destroy_instance(visual);
}

// bool operator < (editor_visual const& a, editor_visual const& b)  
// {
// 	return a.visual < b.visual;
// }

// void model_manager::add_editor_visual	(render_visual* v, float4x4 trans, bool selected)	
// { 
// 	editor_visual				visual_to_add		( v, trans, selected );
// 	Editor_Visuals::iterator	it			=	std::upper_bound ( m_editor_visuals.begin(), m_editor_visuals.end(), visual_to_add );
// 	m_editor_visuals.insert							( it, visual_to_add );
// }

void model_manager::add_editor_visual(u32 id, xray::render::visual_ptr v, float4x4 trans, bool selected, bool system_object, bool beditor_chain )
{
	editor_visual				visual_to_add	( id, v, trans, selected, system_object, beditor_chain );
	
	Editor_Visuals::iterator it	= std::lower_bound( m_editor_visuals.begin(), m_editor_visuals.end(), visual_to_add);
	ASSERT						( (it == m_editor_visuals.end()) || (it->id != id) );
	m_editor_visuals.insert		( it, visual_to_add );
}

void model_manager::update_editor_visual(u32 id, float4x4 trans, bool selected)
{
	editor_visual				tmp_vuisual;
	tmp_vuisual.id				= id;
	Editor_Visuals::iterator	it			=	std::lower_bound( m_editor_visuals.begin(), m_editor_visuals.end(), tmp_vuisual );

	ASSERT( it != m_editor_visuals.end() && it->id == id );
	if( it != m_editor_visuals.end() && it->id == id )
	{
		it->transform	= trans;
		it->selected	= selected;
	}
}

void model_manager::remove_editor_visual(u32 id)	
{ 
	Editor_Visuals::iterator new_end = std::remove	( m_editor_visuals.begin(), m_editor_visuals.end(), id );	
	m_editor_visuals.erase (new_end, m_editor_visuals.end( ) );

	LOG_INFO	( "model_manager::remove_editor_visual(id = %d), m_editor_visuals.size() == %d ", id, m_editor_visuals.size() );
}


} // namespace render 
} // namespace xray 
