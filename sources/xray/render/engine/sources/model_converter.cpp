////////////////////////////////////////////////////////////////////////////
//	Created		: 03.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "model_converter.h"
#include <xray/render/engine/model_format.h>
#include <d3d9types.h>

namespace xray {
namespace render {

bool read_static_surface( memory::reader& vertices_src, memory::reader& indices_src, memory::writer& dst );
bool read_skinned_surface_sw( memory::reader& vertices_src, memory::reader& indices_src, memory::writer& dst );
bool read_skinned_surface_hw( memory::reader& vertices_src, memory::reader& indices_src, memory::writer& dst );

bool convert_surface_to_platform( u16 in_type, memory::reader& vertices_src, memory::reader& indices_src, memory::writer& dst, bool hw_skinning )
{
	mesh_type_enum const type = mesh_type_enum(in_type);
	switch( type )
	{
		case mt_static_submesh:
			return read_static_surface( vertices_src, indices_src, dst );
		case mt_skinned_submesh_1w:
		case mt_skinned_submesh_2w:
		case mt_skinned_submesh_3w:
		case mt_skinned_submesh_4w:
			if(hw_skinning)
				return read_skinned_surface_hw( vertices_src, indices_src, dst );
			else
				return read_skinned_surface_sw( vertices_src, indices_src, dst );
		default:
			NODEFAULT( return true );
	}
}

D3DVERTEXELEMENT9	static_decl[] = // 12+4+4+4+8=32
{
	{0, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },
	{0, 12, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,	0 },
	{0, 16, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TANGENT,	0 },
	{0, 20, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_BINORMAL,	0 },
	{0, 24, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};


bool read_skinned_surface_sw( memory::reader& vertices_src, memory::reader& indices_src, memory::writer& dst )
{
	//vertices
	dst.open_chunk				( model_chunk_vertices );
	dst.write					( vertices_src.data(), vertices_src.length() );
	dst.close_chunk				( );// vertices
	
	// indices
	dst.open_chunk				( model_chunk_indices );
	dst.write					( indices_src.data(), indices_src.length() );
	dst.close_chunk				( ); // indices
	
	return true;
}

bool read_skinned_surface_hw( memory::reader& vertices_src, memory::reader& indices_src, memory::writer& dst )
{
	u32 vert_count				= vertices_src.r_u32();
	
	dst.open_chunk				( model_chunk_vertices );
	dst.write_u32				( vert_count );
	
	vert_boned_4w						src_vertex;
	hardware_4weights_skinning_vertex	dst_vertex;

	for(u32 vert_idx=0; vert_idx<vert_count; ++vert_idx)
	{
		vertices_src.r		( &src_vertex, sizeof(src_vertex), sizeof(src_vertex) );

		dst_vertex.P		= src_vertex.P;

		xray::render::base_basis basis;
		basis.set			( src_vertex.N );
		dst_vertex.N		= math::color_rgba( basis.x, basis.y, basis.z, 127u );


		dst_vertex.uv		= src_vertex.uv;

		std::pair<s16,u8> _24u	= s24_tc_base(src_vertex.uv.x);
		basis.set			( src_vertex.T );
		dst_vertex.T		= math::color_rgba( (u32)basis.x, basis.y, basis.z, _24u.second);

		std::pair<s16,u8> _24v	= s24_tc_base(src_vertex.uv.y);
		basis.set			( src_vertex.B );
		dst_vertex.B		= math::color_rgba( (u32)basis.x, basis.y, basis.z, _24v.second );

		dst_vertex.weights[0]		= src_vertex.w[0];
		dst_vertex.weights[1]		= src_vertex.w[1];
		dst_vertex.weights[2]		= src_vertex.w[2];

		u8 const indices[]	= {
			(u8)src_vertex.m[0],
			(u8)(src_vertex.m[1] == u16(-1) ? src_vertex.m[0] : src_vertex.m[1]),
			(u8)(src_vertex.m[2] == u16(-1) ? src_vertex.m[0] : src_vertex.m[2]),
			(u8)(src_vertex.m[3] == u16(-1) ? src_vertex.m[0] : src_vertex.m[3]),
		};

		dst_vertex.indices[0]		= indices[0];
		dst_vertex.indices[1]		= indices[1];
		dst_vertex.indices[2]		= indices[2];
		dst_vertex.indices[3]		= indices[3];

		dst.write			( &dst_vertex, sizeof(dst_vertex) );
	}
	dst.close_chunk( );// vertices

	// indices
	dst.open_chunk			( model_chunk_indices );
	dst.write				( indices_src.data(), indices_src.length() );
	dst.close_chunk			( ); // indices

	return true;
}

bool read_static_surface( memory::reader& vertices_src, memory::reader& indices_src, memory::writer& dst )
{
	u32 vert_count				= vertices_src.r_u32();
	
	vert_static					v;
	
	dst.open_chunk				( model_chunk_vertices );
	dst.write					( static_decl, sizeof(static_decl) );
	dst.write_u32				( vert_count );
	
	for(u32 vert_idx=0; vert_idx<vert_count; ++vert_idx)
	{
		vertices_src.r		( &v, sizeof(v), sizeof(v) );
		
		base_basis			Basis;

		dst.write_float3	( v.P );

		// Normal
		{
			Basis.set		(v.N);
			u32	uN			= math::color_rgba( Basis.x, Basis.y, Basis.z, 127u );
			dst.write_u32	(uN);
		}

		std::pair<s16,u8> _24u	= s24_tc_base(v.uv.x);
		std::pair<s16,u8> _24v	= s24_tc_base(v.uv.y);

		// Tangent
		{
			Basis.set		(v.T);
			u32	uT			= math::color_rgba( u32(Basis.x), Basis.y, Basis.z, _24u.second);
			dst.write_u32	(uT);
		}

		// Binormal
		{
			Basis.set		(v.B);
			u32	uB			= math::color_rgba( u32(Basis.x), Basis.y, Basis.z, _24v.second);
			dst.write_u32	(uB);
		}

		// TC
		{
			dst.write_float		(v.uv.x);
			dst.write_float		(v.uv.y);
		}
	}
	dst.close_chunk( );// vertices

	// indices
	dst.open_chunk			( model_chunk_indices );
	dst.write				( indices_src.data(), indices_src.length() );
	dst.close_chunk			( ); // indices

	return true;
}


converted_model_cook::converted_model_cook( )
:super(resources::converted_model_class, reuse_true, use_resource_manager_thread_id)
{}

void converted_model_cook::translate_query( resources::query_result_for_cook& parent )
{
#ifndef MASTER_GOLD
	pcstr requested_path				= parent.get_requested_path();
	
	fs_new::virtual_path_string converted_file_path;
	fs_new::virtual_path_string source_file_path;

	converted_file_path.assign_replace	( requested_path, "resources/", "resources.converted/" );
	source_file_path.assign_replace		( requested_path, "/converted_model", "/vertices" );
	resources::request r[]={
		{converted_file_path.c_str(), resources::fs_iterator_class},
		{source_file_path.c_str(), resources::fs_iterator_class}
	};
	resources::query_resources(
		r,
		boost::bind(&converted_model_cook::on_fs_iterators_ready, this, _1, &parent),
		resources::unmanaged_allocator(),
		NULL,
		&parent
		);
#else
	request_converted_file			( &parent );
#endif //#ifndef MASTER_GOLD
}

void converted_model_cook::request_converted_file( resources::query_result_for_cook* parent )
{
	pcstr requested_path				= parent->get_requested_path();
	fs_new::virtual_path_string			file_path;
	file_path.assign_replace			( requested_path, "resources/", "resources.converted/" );

	resources::query_resource(
		file_path.c_str(),
		resources::raw_data_class,
		boost::bind(&converted_model_cook::on_converted_loaded, this, _1, parent ),
		resources::unmanaged_allocator(),
		0,
		parent
		);
}

#ifndef MASTER_GOLD

void converted_model_cook::on_fs_iterators_ready( resources::queries_result& data, resources::query_result_for_cook* parent )
{
	R_ASSERT								( data.is_successful() );
	vfs::vfs_iterator it_converted_model	= data[0].get_result_iterator();
	vfs::vfs_iterator it_source_model		= data[1].get_result_iterator();

	fs_new::physical_path_info const& converted_file_path_info	=	resources::get_physical_path_info(it_converted_model);

	if( !converted_file_path_info.exists() )
	{
		request_convertation			( parent );
		return;
	}

	fs_new::physical_path_info const&	source_file_path_info	=	resources::get_physical_path_info(it_source_model);

	if( !source_file_path_info.exists() )
	{
		parent->finish_query				( resources::query_result_for_user::error_type_file_not_found );
		return;
	}

	if( source_file_path_info.last_time_of_write() > converted_file_path_info.last_time_of_write() )
	{
		request_convertation			( parent );
		return;
	}else
	{
		request_converted_file			( parent );
	}

}

void converted_model_cook::request_convertation( resources::query_result_for_cook* parent )
{
	pcstr requested_path				= parent->get_requested_path();
	
	fs_new::virtual_path_string verts_path;
	verts_path.assign_replace			( requested_path, "/converted_model", "/vertices" );

	fs_new::virtual_path_string inds_path;
	inds_path.assign_replace			( requested_path, "/converted_model", "/indices" );

	fs_new::virtual_path_string properties_path;
	properties_path.assign_replace		( requested_path, "/converted_model", "/export_properties" );

	resources::request r[] = {
		{ verts_path.c_str(), resources::raw_data_class },
		{ inds_path.c_str(), resources::raw_data_class },
		{ properties_path.c_str(), resources::binary_config_class },
	};

	resources::query_resources(
		r,
		boost::bind(&converted_model_cook::on_sources_loaded, this, _1, parent ),
		resources::unmanaged_allocator(),
		0,
		parent
	);

}

void converted_model_cook::on_sources_loaded( resources::queries_result& data, resources::query_result_for_cook* parent )
{
	R_ASSERT(data.is_successful());
	memory::writer destination						( resources::unmanaged_allocator());
	resources::pinned_ptr_const<u8> vertices_ptr	( data[0].get_managed_resource() );
	resources::pinned_ptr_const<u8> indices_ptr		( data[1].get_managed_resource() );
	
	configs::binary_config_ptr config = static_cast_resource_ptr<configs::binary_config_ptr>(data[2].get_unmanaged_resource());

	memory::reader vertices_reader	( vertices_ptr.c_ptr(), vertices_ptr.size() );
	memory::reader indices_reader	( indices_ptr.c_ptr(), indices_ptr.size() );
		
	u16 model_type					= u16(config->get_root()["type"]);
	convert_surface_to_platform		( model_type, vertices_reader, indices_reader, destination, true );


	fs_new::virtual_path_string		dest_path;
	dest_path.assign_replace		( parent->get_requested_path(), "resources/models", "resources.converted/models");

	fs_new::native_path_string				physical_path;
	bool const converted_path			=	resources::convert_virtual_to_physical_path(& physical_path, dest_path, resources::sources_mount);
	R_ASSERT								(converted_path);

	resources::managed_resource_ptr created_resource	= resources::allocate_managed_resource(destination.size(), resources::sound_rms_class);
	if ( !created_resource )
	{
		parent->set_out_of_memory	( resources::managed_memory, destination.size() );
		parent->finish_query		( result_out_of_memory );
		return;
	}

	resources::pinned_ptr_mutable<u8> p					= pin_for_write<u8>(created_resource);
	memory::copy										( p.c_ptr(), p.size(), destination.pointer(), destination.size() );
	
	parent->set_managed_resource	( created_resource );

	parent->save_generated_resource	( resources::save_generated_data::create(created_resource, physical_path) );

	parent->finish_query			( result_success );
}
#endif //#ifndef MASTER_GOLD

void converted_model_cook::on_converted_loaded( resources::queries_result& data, resources::query_result_for_cook* parent )
{
	R_ASSERT						( data.is_successful() );
	parent->set_managed_resource	( data[0].get_managed_resource() );
	parent->finish_query			( result_success );
}

void converted_model_cook::delete_resource( resources::resource_base* /*resource*/ )
{
	UNREACHABLE_CODE();
}
} // namespace render
} // namespace xray
