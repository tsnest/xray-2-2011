////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_object.h"
#include "project.h"
#include "editor_world.h"
#include "tool_base.h"
#include "level_editor.h"
#include "lua_config_value_editor.h"

#pragma managed( push, off )
#	include <zlib/zlib.h>
#	include <xray/render/engine/model_format.h>
#pragma managed( pop )

#pragma comment(lib, "zlibN.lib")

namespace xray {
namespace editor {

void terrain_node::load_props( configs::lua_config_value const& t )
{
	load_config( t );
}

void terrain_node::load_config( configs::lua_config_value const& t )
{
	// no need to call super::load
	set_library_name	( gcnew System::String(t["lib_name"]) );
	m_cell_size			= t["cell_size"];

	if(!t["textures"].empty())
	{
		m_used_textures.Clear	();
		configs::lua_config_value::iterator it		= t["textures"].begin();
		configs::lua_config_value::iterator it_e	= t["textures"].end();

		for(; it!=it_e; ++it)
			m_used_textures.Add( gcnew System::String(*it));
	}
	
	m_vertices.m_ext_file = t["ext_file"];

	fs_new::virtual_path_string p;
	if(m_vertices.m_ext_file)
	{
		if(t.value_exists("key"))
		{
			math::int2 k	= t["key"];
			m_tmp_key.x		= k.x;
			m_tmp_key.z		= k.y;
		}else
		{
			m_tmp_key.x		= t["px"];
			m_tmp_key.z		= t["pz"];
		}

		p.assignf_with_conversion(	"%s/terrain/%d_%d.terr",
					get_project()->load_project_resources_path().c_str(),
					m_tmp_key.x,
					m_tmp_key.z);

		m_vertices.m_actual_file_name = System::IO::Path::GetFullPath(gcnew System::String(p.c_str()))->Replace("\\", "/");
	}

}

void terrain_node::save_config( configs::lua_config_value t )
{
	set_c_string		(t["lib_name"], get_library_name());
	set_c_string		(t["lib_guid"], get_library_guid().ToString());

	math::int2 k(m_tmp_key.x, m_tmp_key.z);
	t["key"]			= k;
	t["cell_size"]		= m_cell_size;

	t["ext_file"]		= true;
}

void terrain_node::save_vertices( System::String^ file_name, bool runtime_save )
{
	if(m_vertices.m_actual_file_name->Length!=0 && !m_vertices.m_modified)
	{// there are good file
		if(m_vertices.m_actual_file_name==file_name)
		{
			 //do nothing
			return;
		}else
		{// just copy file
			System::String^ prev_filename	= m_vertices.m_actual_file_name;
			System::IO::File::Copy			( m_vertices.m_actual_file_name, file_name, true );
			
			if(m_vertices.m_actual_file_name_is_runtime)
				System::IO::File::Delete	( prev_filename );

			m_vertices.m_actual_file_name				= file_name;
			m_vertices.m_actual_file_name_is_runtime	= runtime_save;
			return;
		}
	}

	bool b_generate =  m_vertices.empty() && !m_vertices.m_ext_file;
	
	if(b_generate)
		m_vertices.generate			( (float)size(), m_cell_size );

	R_ASSERT(!m_vertices.empty());

	memory::writer	contents_writer(g_allocator);

	// row size
	contents_writer.write_u32	( m_vertices.m_dimension+1);

	// physical size
	contents_writer.write_float	( (float)size() );

	// used textures
	R_ASSERT(m_used_textures.Count!=0);
	contents_writer.write_u32	(m_used_textures.Count);

	for each (System::String^ t in m_used_textures)
		contents_writer.write_string (unmanaged_string(t).c_str());

	// transform
	float4x4	tr				= get_transform();
	contents_writer.write_float3	(tr.i.xyz());
	contents_writer.write_float3	(tr.j.xyz());
	contents_writer.write_float3	(tr.k.xyz());
	contents_writer.write_float3	(tr.c.xyz());
	{
		vectora<render::terrain_data>	vvertices(g_allocator);
		math::rectangle<math::uint2>	rect;
		rect.min.set(0,0);
		rect.max.set(65,65);

		m_vertices.export_vertices		( rect, vvertices );

		contents_writer.write			( &vvertices[0], vvertices.size()* sizeof(vvertices[0]) );
	}

// compress it!
    z_stream			c_stream;
    c_stream.zalloc		= (alloc_func)0;
    c_stream.zfree		= (free_func)0;
    c_stream.opaque		= (voidpf)0;

	int err = deflateInit(&c_stream, Z_BEST_COMPRESSION);
	R_ASSERT(err==Z_OK);
	u32 len				= contents_writer.size();

	u8* const compressed_buff	= (u8*)XRAY_MALLOC_IMPL(debug::g_mt_allocator, len, "compressed terrain" );
	c_stream.next_in	= contents_writer.pointer();
    c_stream.next_out	= compressed_buff;

    c_stream.avail_in = c_stream.avail_out = len; // force all buffer
    while (c_stream.total_in != len && c_stream.total_out < len) 
	{
        err = deflate(&c_stream, Z_FINISH); //Z_NO_FLUSH);
		if (err == Z_STREAM_END) 
			break;        

		R_ASSERT(err==Z_OK, "deflate");
    }
    err			= deflateEnd(&c_stream);
    R_ASSERT	(err==Z_OK, "deflateEnd");


	unmanaged_string fn(file_name);

	memory::writer				w( g_allocator );
	w.open_chunk				( render::model_chunk_terrain_data_compressed1 );
	w.write						( compressed_buff, c_stream.total_out );
	w.close_chunk				( );
	XRAY_FREE_IMPL				( debug::g_mt_allocator, compressed_buff );
	w.save_to					( fn.c_str(), true );

	if(m_vertices.m_actual_file_name_is_runtime && 
		m_vertices.m_actual_file_name->Length!=0 &&
		m_vertices.m_actual_file_name != file_name )
	{
		System::IO::File::Delete	( m_vertices.m_actual_file_name );
	}

	m_vertices.m_modified					= false;
	m_vertices.m_actual_file_name			= file_name;
	m_vertices.m_actual_file_name_is_runtime = runtime_save;
	m_vertices.m_ext_file					= true;

	if(b_generate)
		m_vertices.clear();
}

void terrain_node::load_vertices_from_reader( memory::reader& F )
{
	m_vertices.m_dimension		= F.r_u32() - 1;
	float phys_size				= F.r_float();
	u32 tex_count				= F.r_u32();

	XRAY_UNREFERENCED_PARAMETERS(phys_size);

	m_used_textures.Clear		();
	for(u32 tex_idx=0; tex_idx<tex_count; ++tex_idx)
		m_used_textures.Add( gcnew System::String(F.r_string()));

	float4x4		tr;
	tr.identity		( );
	tr.i.xyz()		= F.r_float3	( );
	tr.j.xyz()		= F.r_float3	( );
	tr.k.xyz()		= F.r_float3	( );
	tr.c.xyz()		= F.r_float3	( );


	u32 vcount = math::sqr(m_vertices.m_dimension+1);
	m_vertices.m_vertices.Capacity	= vcount;

	render::terrain_data			d;
	for(u32 vidx=0; vidx<vcount; ++vidx)
	{
		ASSERT				(!F.eof());
		F.r					(&d, sizeof(d), sizeof(d));

		terrain_vertex		v;

		v.height			= d.height;
		v.t0_infl			= d.alpha0;
		v.t1_infl			= d.alpha1;
		v.t2_infl			= d.alpha2;
		v.t0_index			= d.tex_id0;
		v.t1_index			= d.tex_id1;
		v.t2_index			= d.tex_id2;
		v.vertex_color		= d.color;
	
		m_vertices.m_vertices.Add( v );
	}
}

//bool generate_for_all = false;

void terrain_node::on_vertices_ready( resources::queries_result& data )
{
	if( !m_needed_quality_ )
		return;

	if(!data.is_successful())
	{
//		if(!generate_for_all)
		{
			/*System::Windows::Forms::DialogResult res =*/ owner_tool()->get_level_editor()->ShowMessageBox( 
//				System::String::Format("terrain node file [terrain/{0}_{1}.terr] not found. Generate default?", m_tmp_key.x, m_tmp_key.z), 
				System::String::Format("terrain node file [terrain/{0}_{1}.terr] not found.", m_tmp_key.x, m_tmp_key.z), 
				System::Windows::Forms::MessageBoxButtons::OK, 
				System::Windows::Forms::MessageBoxIcon::Error);
		
//			generate_for_all = (res==System::Windows::Forms::DialogResult::OK);
		}
		return;
		//if(generate_for_all)
		//{
		//	load_defaults		( );
		//	query_render_model	( );
		//	return;
		//}else
		//	return;
	}

	resources::pinned_ptr_const<u8> pdata	(data[ 0 ].get_managed_resource( ));

	memory::chunk_reader		chunk( pdata.c_ptr(), pdata.size(), memory::chunk_reader::chunk_type_sequential );

	if(chunk.chunk_exists(render::model_chunk_terrain_data_compressed1))
	{
		memory::reader CF	= chunk.open_reader( render::model_chunk_terrain_data_compressed1 );

		z_stream				d_stream;
		d_stream.zalloc			= (alloc_func)0;
		d_stream.zfree			= (free_func)0;
		d_stream.opaque			= (voidpf)0;

		d_stream.next_in		= (u8*)CF.pointer();
		d_stream.avail_in		= CF.length();
		
		int   err				= inflateInit(&d_stream);
		R_ASSERT				(err==Z_OK);

		u32 uncompressed_max_size = 100*1024;

		u8* uncompressed_buffer	= (u8*)XRAY_MALLOC_IMPL(debug::g_mt_allocator, uncompressed_max_size, "uncompressed terrain");
		d_stream.next_out		= uncompressed_buffer;
		d_stream.avail_out		= uncompressed_max_size;

        err = inflate			(&d_stream, Z_FINISH);
        R_ASSERT				(err == Z_STREAM_END);
		err						= inflateEnd(&d_stream);
        R_ASSERT				(err == Z_OK);
//.		R_ASSERT(d_stream.total_out==uncompressed_max_size);
		memory::reader F( uncompressed_buffer, d_stream.total_out );
		load_vertices_from_reader( F ); // old (uncompressed) format
		XRAY_FREE_IMPL(debug::g_mt_allocator, uncompressed_buffer);
	}else
	{
		memory::reader F		= chunk.open_reader( render::model_chunk_terrain_data );
		load_vertices_from_reader( F ); // old (uncompressed) format
		m_vertices.m_modified	= true; // force to save in new format!
	}

	query_render_model		( );
}
} // namespace editor
} // namespace xray
