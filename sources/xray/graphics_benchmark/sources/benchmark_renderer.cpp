////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "benchmark_renderer.h"
#include "gpu_timer.h"
#include "sphere_primitive.h"
#include "xray/math_randoms_generator.h"

namespace xray { 
namespace graphics_benchmark {

using namespace xray::render;
using namespace xray::math;


// 68
const D3D_INPUT_ELEMENT_DESC F_L[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 24, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 28,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 36,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	2, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 40,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 44,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"BINORMAL",	0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"BINORMAL",	1, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 52,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"BLENDINDICES",0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 56,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"BLENDWEIGHT",	0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 60,	D3D_INPUT_PER_VERTEX_DATA, 0},
};





// 24
const D3D_INPUT_ELEMENT_DESC F_L_24[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
};
// 32
const D3D_INPUT_ELEMENT_DESC F_L_32[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32_FLOAT,	0, 24,	D3D_INPUT_PER_VERTEX_DATA, 0},
};

// 40
const D3D_INPUT_ELEMENT_DESC F_L_40[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,	D3D_INPUT_PER_VERTEX_DATA, 0},
};

// 48
const D3D_INPUT_ELEMENT_DESC F_L_48[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
};

// 64
const D3D_INPUT_ELEMENT_DESC F_L_64[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
};

// 96
const D3D_INPUT_ELEMENT_DESC F_L_96[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 64,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	4, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 80,	D3D_INPUT_PER_VERTEX_DATA, 0},
};

// 128
const D3D_INPUT_ELEMENT_DESC F_L_128[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 64,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	4, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 80,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	5, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 96,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	6, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 112,	D3D_INPUT_PER_VERTEX_DATA, 0},
};

const D3D_INPUT_ELEMENT_DESC vertex_descs[][9] = 
{
	{	//24
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//32
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32_FLOAT,	0, 24,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//40
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//48
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//64
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//96
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 64,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	4, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 80,	D3D_INPUT_PER_VERTEX_DATA, 0},
	},
	{	//128
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	2, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	3, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 64,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	4, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 80,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	5, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 96,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	6, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 112,	D3D_INPUT_PER_VERTEX_DATA, 0},
	}
};


benchmark_renderer::benchmark_renderer()
{
}

benchmark_renderer::~benchmark_renderer()
{

}

void	benchmark_renderer::initialize			( )
{
	m_geom = resource_manager::ref().create_geometry( F_L, sizeof(colored_vertex), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());
	m_low_geom = resource_manager::ref().create_geometry( F_L_24, sizeof(low_colored_vertex), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());


	m_geom_sized[0] = resource_manager::ref().create_geometry( vertex_descs[0], sizeof(colored_vertex_sized<1>), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());
	m_geom_sized[1] = resource_manager::ref().create_geometry( vertex_descs[1], sizeof(colored_vertex_sized<2>), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());
	m_geom_sized[2] = resource_manager::ref().create_geometry( vertex_descs[2], sizeof(colored_vertex_sized<3>), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());
	m_geom_sized[3] = resource_manager::ref().create_geometry( vertex_descs[3], sizeof(colored_vertex_sized<4>), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());
	m_geom_sized[4] = resource_manager::ref().create_geometry( vertex_descs[4], sizeof(colored_vertex_sized<6>), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());
	m_geom_sized[5] = resource_manager::ref().create_geometry( vertex_descs[5], sizeof(colored_vertex_sized<10>), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());
	m_geom_sized[6] = resource_manager::ref().create_geometry( vertex_descs[6], sizeof(colored_vertex_sized<14>), xray::render::backend::ref().vertex.buffer(), xray::render::backend::ref().index.buffer());

	colored_vertex v;

	colored_indices_type				indices;
	colored_vertices_type				vertices;

	xray::math::random32 r(1000);

	v.position = float3(  -1.0f,  1.0f,  0.5f  ); 
	v.color = math::color_rgba(1.0f,0.0f,0.0f,0.1f); 
	v.uv = float2(0.f,0.f);
	for (u32 i=0; i<7; i++)	
		v.colors[i] = math::color_rgba(float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f);
	vertices.push_back(v);

	v.position = float3(   1.0f, 1.0f,  0.5f   );
	v.color = math::color_rgba(0.0f,1.0f,0.3f,0.4f);
	v.uv = float2(1.f,0.f);
	for (u32 i=0; i<7; i++)	
		v.colors[i] = math::color_rgba(float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f);
	vertices.push_back(v);

	v.position = float3(  1.0f,  -1.0f,  0.5f  );
	v.color = math::color_rgba(0.0f,0.0f,1.0f,0.7f);
	v.uv = float2(1.f,1.f);
	for (u32 i=0; i<7; i++)	
		v.colors[i] = math::color_rgba(float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f);
	vertices.push_back(v);

	v.position = float3(  -1.0f,  -1.0f,  0.5f );
	v.color = math::color_rgba(1.0f,1.0f,0.0f,1.0f);
	v.uv = float2(0.f,1.f);
	for (u32 i=0; i<7; i++)	
		v.colors[i] = math::color_rgba(float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f);
	vertices.push_back(v);


	low_colored_vertices_type low_vertices;

	low_colored_vertex lv;

	lv.position = float3(  -1.0f,  1.0f,  0.5f  ); 
	lv.color = math::color_rgba(1.0f,0.0f,0.0f,0.1f); 
	lv.uv = float2(0.f,0.f);
	low_vertices.push_back(lv);

	lv.position = float3(   1.0f, 1.0f,  0.5f   );
	lv.color = math::color_rgba(0.0f,1.0f,0.3f,0.4f);
	lv.uv = float2(1.f,0.f);
	low_vertices.push_back(lv);

	lv.position = float3(  1.0f,  -1.0f,  0.5f  );
	lv.color = math::color_rgba(0.0f,0.0f,1.0f,0.7f);
	lv.uv = float2(1.f,1.f);
	low_vertices.push_back(lv);

	lv.position = float3(  -1.0f,  -1.0f,  0.5f );
	lv.color = math::color_rgba(1.0f,1.0f,0.0f,1.0f);
	lv.uv = float2(0.f,1.f);
	low_vertices.push_back(lv);



	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(0); indices.push_back(2); indices.push_back(3);

	m_quad_mesh_idx = cook_mesh( vertices, indices );

	m_low_quad_mesh_idx = cook_mesh(&low_vertices[0],sizeof(low_colored_vertex),low_vertices.size(),indices);

	m_WVP = backend::ref().register_constant_host( "m_WVP");

	colored_vertices_type s_vertices;
	colored_indices_type  s_indices;

	for (u32 i=0; i<SPHERE_NUMVERTEX; i++)
	{
		colored_vertex v;

		v.position  = sphere_vertices[i];
		v.color		= math::color_rgba(v.position.x,v.position.y,v.position.z,1.0f);
		for (u32 j=0; j<7; j++)	
			v.colors[j] = math::color_rgba(float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f,float(r.random(256))/256.0f);
		v.uv		= float2(v.position.x,v.position.y);
		v.normal	= sphere_vertices[i];

		s_vertices.push_back( v );
	}

	for (u32 i=0; i<SPHERE_NUMFACES*3; i++)
		s_indices.push_back( sphere_faces[i]);

	m_3d_sphere_mesh_idx = cook_mesh( s_vertices, s_indices );
}

u32 benchmark_renderer::cook_mesh(void* vertices, u32 const vertex_size, u32 const vertex_count, colored_indices_type  const& indices)
{
	mesh msh;

	// Prepare ill vertex buffer.
	void* vbuffer = xray::render::backend::ref().vertex.lock( vertex_count, vertex_size, msh.voffset);
	memory::copy( vbuffer, vertex_count*vertex_size, vertices, vertex_count*vertex_size);
	xray::render::backend::ref().vertex.unlock();

	// Prepare index buffer
	u16* ibuffer = (u16*) xray::render::backend::ref().index.lock( indices.size(), msh.ioffset);
	memory::copy( ibuffer, indices.size()*sizeof(u16), &indices[0], indices.size()*sizeof(u16));
	xray::render::backend::ref().index.unlock();

	msh.index_count = indices.size();

	m_meshes.push_back(msh);

	return m_meshes.size()-1;
}

u32 benchmark_renderer::cook_mesh(colored_vertices_type const& vertices, colored_indices_type  const& indices)
{
	mesh msh;
	
	// Prepare ill vertex buffer.
	colored_vertex* vbuffer = (colored_vertex*) xray::render::backend::ref().vertex.lock( vertices.size(), sizeof(colored_vertex), msh.voffset);
	memory::copy( vbuffer, vertices.size()*sizeof(colored_vertex), &vertices[0], vertices.size()*sizeof(colored_vertex));
	xray::render::backend::ref().vertex.unlock();
	
	// Prepare index buffer
	u16* ibuffer = (u16*) xray::render::backend::ref().index.lock( indices.size(), msh.ioffset);
	memory::copy( ibuffer, indices.size()*sizeof(u16), &indices[0], indices.size()*sizeof(u16));
	xray::render::backend::ref().index.unlock();
	
	msh.index_count = indices.size();
	
	m_meshes.push_back(msh);
	
	return m_meshes.size()-1;
}

void benchmark_renderer::render_cooked_mesh(u32 mesh_index, xray::math::float4x4 const& wvp_matrix)
{
	if (mesh_index >= m_meshes.size())
		return;
	
	m_geom->apply();
	backend::ref().set_vs_constant( m_WVP, wvp_matrix);
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_meshes[mesh_index].index_count, m_meshes[mesh_index].ioffset,m_meshes[mesh_index].voffset );
}

void benchmark_renderer::render_low_fullscreen_quad	( gpu_timer&)
{
	m_low_geom->apply();
	backend::ref().set_vs_constant( m_WVP, xray::math::float4x4().identity());
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_meshes[m_low_quad_mesh_idx].index_count, m_meshes[m_low_quad_mesh_idx].ioffset,m_meshes[m_low_quad_mesh_idx].voffset );
}

void benchmark_renderer::prepare_quad()
{
		
}

void benchmark_renderer::render_fullscreen_quad(gpu_timer&)
{
	render_cooked_mesh( m_quad_mesh_idx );
}

void benchmark_renderer::render_3d_sphere(gpu_timer&, xray::math::float4x4 const& wvp_matrix)
{
	render_cooked_mesh( m_3d_sphere_mesh_idx, wvp_matrix );
}


} // namespace graphics_benchmark
} // namespace xray
