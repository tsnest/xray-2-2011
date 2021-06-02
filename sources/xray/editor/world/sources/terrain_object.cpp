////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_object.h"
#include "project.h"
#include "level_editor.h"
#include "editor_world.h"
#include "tool_terrain.h"

#pragma managed( push, off )
#	include <xray/editor/world/engine.h>
#	include <xray/render/engine/model_format.h>
#	include <xray/render/facade/terrain_base.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

#pragma unmanaged

// This is needed to use user_data_variant in managed code.
template<typename T> 
resources::user_data_variant*	create_variant( T t)
{
	resources::user_data_variant*	v = NEW(resources::user_data_variant);
	v->set							( t );

	return v;
}

void				delete_variant( resources::user_data_variant* v)
{
	DELETE(v);
}

#pragma managed


void vertices_db::get_height_bounds( float% min_height, float% max_height )
{
	R_ASSERT(!empty());
	min_height			= -0.01f;
	max_height			= 0.01f;

	for each (terrain_vertex% v in m_vertices)
	{
		float val = v.height;
		
		min_height = (min_height>val) ? val : min_height;
		max_height = (max_height<val) ? val : max_height;
	}
}

terrain_vertex vertices_db::get( u32 idx )
{
	R_ASSERT		(!empty());
	return m_vertices[idx];
}

void vertices_db::set(terrain_vertex v,  u32 idx )
{
	R_ASSERT		(!empty());
	m_vertices[idx] = v;
	m_modified		= true;
}

void vertices_db::clear( )
{
	R_ASSERT		(!empty());
	m_vertices.Clear();
	m_vertices.Capacity = 0;
}

bool vertices_db::empty( )
{
	return m_vertices.Count==0;
}

void vertices_db::generate( float size, float cell_size )
{
	R_ASSERT(empty());

 	m_dimension			= int(size / cell_size);
	// gen vertices
	float const def_y			= 0.0f;
	m_vertices.Capacity	= (m_dimension+1)*(m_dimension+1);

	// vertex index [0] is left-top corner
	for(int cy=0; cy<m_dimension+1; ++cy)
		for(int cx=0; cx<m_dimension+1; ++cx)
		{
			terrain_vertex		v;
			v.set_default		( );
			v.height			= def_y;
			m_vertices.Add		(v);
		}
	m_modified = true;
}

terrain_node::terrain_node(tool_base^ tool, terrain_core^ core)
:super( tool ),
m_core( core )
{
	m_cell_size			= 1.0f;
	image_index			= xray::editor_base::node_speedtree;
	m_model				= NEW (render::terrain_model_ptr)();
	m_min_height		= -0.01f;
	m_max_height		= 0.01f;
}


terrain_node::~terrain_node()
{
	DELETE				(m_model);
}

void terrain_node::load_vertices_sync( )
{
	if(m_vertices.empty())
	{
		load_contents( );

		while(m_vertices.empty())
		{
			resources::dispatch_callbacks	( );
			threading::yield				( 10 );
			LOG_ERROR("waiting for terrain node vertices");
		}
	}
}

void terrain_node::set_transform_internal(float4x4 const& transform)
{
	super::set_transform	(transform);
}

void terrain_node::set_visible(bool bvisible)
{
	ASSERT					(bvisible!=get_visible());

	super::set_visible		(bvisible);
}

void terrain_node::add_used_texture( System::String^ texture )
{
	ASSERT(!m_used_textures.Contains(texture));

	m_used_textures.Add(texture);
	fs_new::virtual_path_string	t(unmanaged_string(texture).c_str());
	get_editor_renderer().terrain_add_cell_texture( get_terrain_core()->scene(), (*m_model)->m_render_model, t, m_used_textures.IndexOf(texture));
}

void terrain_node::query_vertices( )
{
	R_ASSERT(m_vertices.empty());

	if(	m_vertices.m_ext_file )
	{
		R_ASSERT(m_vertices.m_actual_file_name->Length != 0);
		query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &terrain_node::on_vertices_ready), g_allocator);

		fs_new::native_path_string	actual_file_name	=	fs_new::native_path_string::convert(unmanaged_string(m_vertices.m_actual_file_name).c_str());
		actual_file_name.make_lowercase();
		fs_new::virtual_path_string resource_path;
		bool res =   xray::resources::convert_physical_to_virtual_path (&resource_path, actual_file_name, resources::sources_mount );

		R_ASSERT(res);
		resources::query_resource	(
			resource_path.c_str(),
			resources::raw_data_class,
			boost::bind(&query_result_delegate::callback, q, _1),
			g_allocator
			);
	}else
	{
		m_vertices.generate		( (float)size(), m_cell_size );
		query_render_model		( );
	}
}

bool g_block_visual_loading = false;
void terrain_node::query_render_model( )
{
	if(g_block_visual_loading)
		return;

	R_ASSERT				(m_needed_quality_);
	R_ASSERT				((*m_model).c_ptr()==NULL);
	R_ASSERT				(!m_vertices.empty());

	pbyte pbuffer			= NULL;
	u32 pbuffer_size		= 0;
	memory::writer			writer( &debug::g_mt_allocator );
	writer.external_data	= true;
	
	writer.open_chunk		( render::model_chunk_terrain_data );
	writer.write_u32		( m_vertices.m_dimension+1 );
	
	// physical size
	writer.write_float		( (float)size() );

	// used textures
	writer.write_u32		( m_used_textures.Count );
	for each ( System::String^ t in m_used_textures )
	{
		writer.write_string	( unmanaged_string(t).c_str() );
		m_core->add_texture	( t );
	}
	// used textures

	// transform
	float4x4	tr			= get_transform();
	writer.write_float3		( tr.i.xyz() );
	writer.write_float3		( tr.j.xyz() );
	writer.write_float3		( tr.k.xyz() );
	writer.write_float3		( tr.c.xyz() );
	// transform

	{
		vectora<render::terrain_data>	vertices_buffer(g_allocator);
		math::rectangle<math::uint2>	rect;
		rect.min.set(0,0);
		rect.max.set( u32(size()+1), u32(size()+1) );

		m_vertices.export_vertices		( rect, vertices_buffer );
		u32 size						= vertices_buffer.size()*sizeof(vertices_buffer[0]);


		writer.write					( &vertices_buffer[0], size );
	}

	writer.close_chunk				( );
	pbuffer							= writer.pointer();
	pbuffer_size					= writer.size();
	
	query_result_delegate* q		= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &terrain_node::on_render_model_ready), g_allocator);

	fs_new::virtual_path_string		query_path;
	pcstr sguid						= get_project()->session_id();
	
	query_path.assignf				("terrain_%s/%d_%d.terr", sguid, m_tmp_key.x, m_tmp_key.z);

	render::terrain_cook_user_data user_data;
	user_data.scene					= get_terrain_core()->scene();
	user_data.current_project_resource_path	= NULL;
	resources::user_data_variant* v = create_variant( user_data);

	resources::query_create_resource(
						query_path.c_str(),
						memory::buffer(pbuffer, pbuffer_size),
						resources::terrain_model_class,
						boost::bind(&query_result_delegate::callback, q, _1 ),
						g_allocator,
						v );

	delete_variant		( v );
}

void terrain_node::on_render_model_ready( resources::queries_result& data )
{
	R_ASSERT					( data.is_successful());
	
	if( (*m_model).c_ptr()!=NULL )
		return;
	
	if( !m_needed_quality_ )
		return;


	const_buffer creation_data_from_user = data[0].creation_data_from_user();
	pbyte pdata							= (pbyte)creation_data_from_user.c_ptr();
	XRAY_DELETE_IMPL					(debug::g_mt_allocator, pdata);

	*m_model		= static_cast_resource_ptr<render::terrain_model_ptr>(data[0].get_unmanaged_resource());
	ASSERT			( *m_model );

	initialize_collision			( );
	get_editor_renderer().scene().terrain_add_cell	( get_terrain_core()->scene(), (*m_model)->m_render_model );
}

void terrain_node::load_contents( )
{
	R_ASSERT				(m_vertices.empty());
	R_ASSERT				(!m_needed_quality_);
	m_needed_quality_		= true;
	query_vertices			( );
}

void terrain_node::unload_contents( bool bdestroy )
{
	m_needed_quality_		= false;

	if(m_vertices.empty())
		return;// quick load-unload sequence

	if( !bdestroy && m_vertices.m_modified )
	{
		System::String^ intermediate_node_path = System::String::Format("{0}/terrain/{1}_{2}.terr",
													get_project()->project_resources_intermediate_path(),
													m_tmp_key.x, 
													m_tmp_key.z
													);
		intermediate_node_path = System::IO::Path::GetFullPath(intermediate_node_path)->Replace("\\", "/");

		save_vertices			( intermediate_node_path, true );
	}
	m_needed_quality_			= false;
	destroy_collision			( );
	destroy_ph_collision		( );

	if( (*m_model).c_ptr() )
	{
		get_editor_renderer().scene().terrain_remove_cell	( get_terrain_core()->scene(), (*m_model)->m_render_model );
		(*m_model)					= NULL;
	}
	m_vertices.clear			( );
	R_ASSERT					( m_vertices.empty() );
}

u32 terrain_node::size( )
{
	return m_core->node_size_;
}

aabb terrain_node::get_local_aabb( )
{
	return create_aabb_min_max( float3(0.0f, m_min_height, -64.0f), 
								float3(64.0f,m_max_height, 0.0f) );
}

} // namespace editor
} // namespace xray
