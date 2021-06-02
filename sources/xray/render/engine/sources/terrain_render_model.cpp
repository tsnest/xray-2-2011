////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_render_model.h"
#include "terrain.h"
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/effect_manager.h>
#include "effect_terrain_new.h"
#include <xray/render/engine/model_format.h>
#include <zlib/zlib.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/destroy_data_helper.h>
#include <xray/render/core/res_effect.h>
#include "material.h"

#ifdef _DLL
#	pragma comment(lib, "zlibN.lib")
#else // #ifdef _DLL
#	pragma comment(lib, "zlib.lib")
#endif // #ifdef _DLL

namespace xray {
namespace render {


terrain_render_model::terrain_render_model()
:m_heightfield( NULL )
{

}

terrain_render_model::~terrain_render_model()
{
	if(m_effect.c_ptr()==NULL)
		m_effect_loader->query_rejected = true;

	collision::delete_geometry_instance			( render::g_allocator, m_collision_geom );
	physics::bt_collision_shape* s				= m_bt_collision_shape.c_ptr();

	m_bt_collision_shape						= NULL;

	physics::destroy_shape						( *render::g_allocator, s );

	DELETE_ARRAY								( m_heightfield );

}

void terrain_render_model::load_from_reader( memory::reader& F, render::terrain& terrain )
{
	u32 vertex_row_size			= F.r_u32();
	float physical_size			= F.r_float();
	u32 tex_count				= F.r_u32();

	R_ASSERT(tex_count!=0);
	m_textures.resize		(tex_count);

	for(u32 tidx = 0; tidx<tex_count; ++tidx)  
	{
		pcstr texture_name	= F.r_string();
		int id_tex			= terrain.m_texture_pool.add_texture(texture_name);
		m_textures[tidx]	= id_tex;
	} 

	float4x4					transform;
	transform.identity			();
	transform.i.xyz()			= F.r_float3();
	transform.j.xyz()			= F.r_float3();
	transform.k.xyz()			= F.r_float3();
	transform.c.xyz()			= F.r_float3();

	m_transform					= transform;

	u32	vertex_size				= terrain_vertex_size;

	u32 vertices_size_x			= vertex_row_size;
	u32 vertices_size_y			= vertex_row_size;

	m_render_geometry.vertex_count	= vertices_size_x*vertices_size_y;

	m_render_geometry.primitive_count	= 2*(vertices_size_x-1)*(vertices_size_y-1);
	m_render_geometry.index_count		= m_render_geometry.primitive_count*3;

	void* tmp_buffer			= ALLOCA( math::max( vertex_size*m_render_geometry.vertex_count, u32(m_render_geometry.index_count*sizeof(u16)) ));

	math::aabb bbox(math::create_zero_aabb());

//	extend_bounding_box			( (render::terrain_data*)F.pointer(), m_render_geometry.vertex_count, physical_size, vertex_row_size, transform, bbox);
	extend_bounding_box			( (render::terrain_data*)F.pointer(), m_render_geometry.vertex_count, physical_size, vertex_row_size, float4x4().identity(), bbox);
	m_aabbox					= bbox;

	m_heightfield			= NEW_ARRAY(float, m_render_geometry.vertex_count);
	u16* heightmap_buffer	= static_cast<terrain::heightmap_element*>( ALLOCA( sizeof( heightmap_element)*m_render_geometry.vertex_count));
	u32* color_buffer		= static_cast<terrain::color_element*>( ALLOCA( sizeof( color_element)*m_render_geometry.vertex_count));
	u32* params_buffer		= static_cast<terrain::params_element*>( ALLOCA( sizeof( params_element)*m_render_geometry.vertex_count));
	u32* tex_ids_buffer		= static_cast<terrain::tex_ids_element*>( ALLOCA( sizeof( tex_ids_element)*m_render_geometry.vertex_count));
	u32* tc_shift_buffer	= static_cast<terrain::tc_shift_element*>( ALLOCA( sizeof( tc_shift_element)*m_render_geometry.vertex_count));

	// Filling vertices here 
	fill_vertices				(	tmp_buffer, 
									m_render_geometry.vertex_count, 
									0, 
									vertex_row_size, 
									physical_size, 
									transform, 
									(render::terrain_data*)F.pointer(), 
									m_textures,
									m_heightfield);

	fill_textures			( (render::terrain_data*)F.pointer(), 
								vertices_size_x, 
								vertices_size_y, 
// 								0, 
// 								0, 
// 								physical_size, 
								m_textures, 
								heightmap_buffer,
								sizeof( heightmap_element)*vertices_size_x,
								color_buffer,
								sizeof( color_element)*vertices_size_x,
								params_buffer,
								sizeof( params_element)*vertices_size_x,
								tex_ids_buffer,
								sizeof( tex_ids_element)*vertices_size_x,
								tc_shift_buffer,
								sizeof( tc_shift_element)*vertices_size_x
								);


	create_maps		( math::uint2( vertices_size_x, vertices_size_y), heightmap_buffer, color_buffer, params_buffer, tex_ids_buffer, tc_shift_buffer);


	m_collision_geom				= &*collision::new_terrain_geometry_instance(render::g_allocator, float4x4().identity(), physical_size, vertex_row_size, m_heightfield, false  );
	//m_collision_geom->set_no_delete	( );
	m_bt_collision_shape			= physics::create_terrain_shape(*render::g_allocator, 
																	m_heightfield, 
																	vertex_row_size, 
																	m_aabbox.min.y, 
																	m_aabbox.max.y );

	m_bt_collision_shape->set_no_delete	( );

	untyped_buffer_ptr vb			= resource_manager::ref().create_buffer ( m_render_geometry.vertex_count*vertex_size, tmp_buffer, enum_buffer_type_vertex, true);

	m_vb						= &*vb;

	//Filling indices here
	fill_indexes				( math::uint2(vertex_row_size,vertex_row_size), (u16*)tmp_buffer);

	untyped_buffer_ptr ib		= resource_manager::ref().create_buffer( 
			m_render_geometry.index_count*sizeof(u16), 
			tmp_buffer, 
			enum_buffer_type_index, 
			false);

//	DELETE_ARRAY				(tmp_buffer);

	ref_declaration decl_ptr	= resource_manager::ref().create_declaration	( terrain_vertex_declaration );
	m_render_geometry.geom		= resource_manager::ref().create_geometry		( &*decl_ptr, terrain_vertex_size, *vb, *ib);
	
	
	pcvoid mem					= ALLOCA( 1 * Kb );
	effect_options_descriptor	desc(mem, 1 * Kb);
	desc["texture_tile_diffuse"]  = terrain.m_texture_pool.texture_diffuse();
	desc["texture_tile_nmap"]	  = terrain.m_texture_pool.texture_nmap();
	desc["texture_tile_spec"]	  = terrain.m_texture_pool.texture_spec();
	desc["height_texture"]		= m_height_texture;
	desc["color_texture"]		= m_color_texture;
	desc["params_texture"]		= m_params_texture;
	desc["tex_ids_texture"]		= m_tex_ids_texture;
	desc["tc_shift_texture"]	= m_tc_shift_texture;
	
	m_effect_loader				= NEW(effect_loader)(&m_effect);
	effect_manager::ref().create_effect<effect_terrain_NEW>(m_effect_loader, desc);
	
	m_vertex_row_size			= vertex_row_size;
	m_physical_size				= physical_size;
}

res_effect* terrain_render_model::effect()		
{ 
	return m_effect.c_ptr() ? &*m_effect : 0;
}

void terrain_render_model::load( memory::chunk_reader& chunk, render::terrain& terrain )
{
	// temporara solunion for support compressed and uncompressed terrain raw file
	if(chunk.chunk_exists(model_chunk_terrain_data_compressed1))
	{
		memory::reader CF	= chunk.open_reader( model_chunk_terrain_data_compressed1 );

		z_stream				d_stream;
		d_stream.zalloc			= (alloc_func)0;
		d_stream.zfree			= (free_func)0;
		d_stream.opaque			= (voidpf)0;

		d_stream.next_in		= (u8*)CF.pointer();
		d_stream.avail_in		= CF.length();
		
		int   err				= inflateInit(&d_stream);
		R_ASSERT				(err==Z_OK);

		u32 uncompressed_max_size = 100*1024;

		u8* uncompressed_buffer	= (u8*)XRAY_MALLOC_IMPL(g_allocator, uncompressed_max_size, "uncompressed terrain");
		d_stream.next_out		= uncompressed_buffer;
		d_stream.avail_out		= uncompressed_max_size;

        err = inflate(&d_stream, Z_FINISH);
        R_ASSERT				(err == Z_STREAM_END);
		err						= inflateEnd(&d_stream);
        R_ASSERT				(err == Z_OK);
//		R_ASSERT(d_stream.total_out==uncompressed_max_size);
		memory::reader F		( uncompressed_buffer, d_stream.total_out );
		load_from_reader		( F, terrain ); // old (uncompressed) format
		XRAY_FREE_IMPL(g_allocator, uncompressed_buffer);
	}else
	{
		memory::reader F = chunk.open_reader( model_chunk_terrain_data );
		load_from_reader		( F, terrain ); // old (uncompressed) format
	}
}

void terrain_render_model::create_maps	( math::uint2 size, 
										 heightmap_element * heightmap_buffer, 
										 color_element * color_buffer, 
										 params_element * params_buffer, 
										 tex_ids_element * tex_ids_buffer, 
										 tc_shift_element * tc_shift_buffer)
{
	static u32 s_unique_ind = 0;
	fixed_string<64>	tmp_name;
	D3D_SUBRESOURCE_DATA	initial_data;

	initial_data.pSysMem			= heightmap_buffer;
	initial_data.SysMemPitch		= sizeof(heightmap_element)*size.x;
	initial_data.SysMemSlicePitch	= initial_data.SysMemPitch*size.y;
	tmp_name.assignf( "terr_height_%d", s_unique_ind );
	m_height_texture	=	resource_manager::ref().create_texture2d( tmp_name.c_str(), size.x, size.y, &initial_data, terrain::s_height_texture_format, D3D_USAGE_DEFAULT, 1);

	initial_data.pSysMem			= color_buffer;
	initial_data.SysMemPitch		= sizeof(color_element)*size.x;
	initial_data.SysMemSlicePitch	= initial_data.SysMemPitch*size.y;
	tmp_name.assignf( "terr_color_%d", s_unique_ind );
	m_color_texture		=	resource_manager::ref().create_texture2d( tmp_name.c_str(), size.x, size.y, &initial_data, terrain::s_color_texture_format, D3D_USAGE_DEFAULT, 1);

	initial_data.pSysMem			= params_buffer;
	initial_data.SysMemPitch		= sizeof(params_element)*size.x;
	initial_data.SysMemSlicePitch	= initial_data.SysMemPitch*size.y;
	tmp_name.assignf( "terr_params_%d", s_unique_ind );
	m_params_texture	=	resource_manager::ref().create_texture2d( tmp_name.c_str(), size.x, size.y, &initial_data, terrain::s_params_texture_format, D3D_USAGE_DEFAULT, 1);

	initial_data.pSysMem			= tex_ids_buffer;
	initial_data.SysMemPitch		= sizeof(tex_ids_element)*size.x;
	initial_data.SysMemSlicePitch	= initial_data.SysMemPitch*size.y;
	tmp_name.assignf( "terr_tex_ids_%d", s_unique_ind );
	m_tex_ids_texture	=	resource_manager::ref().create_texture2d( tmp_name.c_str(), size.x, size.y, &initial_data, terrain::s_tex_ids_texture_format, D3D_USAGE_DEFAULT, 1);

	initial_data.pSysMem			= tc_shift_buffer;
	initial_data.SysMemPitch		= sizeof(tc_shift_element)*size.x;
	initial_data.SysMemSlicePitch	= initial_data.SysMemPitch*size.y;
	tmp_name.assignf( "terr_tc_shift_%d", s_unique_ind );
	m_tc_shift_texture	=	resource_manager::ref().create_texture2d( tmp_name.c_str(), size.x, size.y, &initial_data, terrain::s_tc_shift_texture_format, D3D_USAGE_DEFAULT, 1);

	++s_unique_ind;
}

} // namespace render 
} // namespace xray 
