////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_composite_visual.h"
#include "model_editor.h"
#include <xray/render/engine/model_format.h>

namespace xray{
namespace model_editor
{

bool edit_object_composite_visual::save( )
{
	if(System::String::IsNullOrEmpty(full_name()))
	{
		System::String^ selected	= nullptr;

		bool result = editor_base::resource_chooser::choose(	"solid_visual", 
																"", 
																nullptr, 
																selected,
																false );

		if(result)
			m_full_name = selected;
		else
			return false;
	
	}

	configs::lua_config_ptr config_ptr	= configs::create_lua_config( );
	configs::lua_config_value root		= config_ptr->get_root()["items"];

	int i=0;
	for each( composite_visual_item^ item in m_contents )
	{
		configs::lua_config_value current	= root[i++];
		current["model_name"]				= unmanaged_string(item->model_name).c_str();
		current["position"]					= item->m_matrix->c.xyz();
		current["rotation"]					= item->m_matrix->get_angles_xyz();
		current["scale"]					= item->m_matrix->get_scale();
		current["pivot"]					= *item->m_pivot;

	}	
	(*m_model_config)->get_root().assign_lua_value	( config_ptr->get_root() );

	fs_new::virtual_path_string					file_name;

	file_name.assignf				( "resources/composite_models/%s.composite_model/composite_render", 
										unmanaged_string(m_full_name).c_str() );

	(*m_model_config)->save_as		( file_name.c_str(), configs::target_sources );

	// assemble collision here

	// assemble hq collision here
	u32 vertices_total_count = 0;
	memory::writer out_vertices		( &debug::g_mt_allocator );
	memory::writer out_indices		( &debug::g_mt_allocator );
	out_vertices.open_chunk			( render::model_chunk_collision_v );
	out_indices.open_chunk			( render::model_chunk_collision_i );

	for each( composite_visual_item^ item in m_contents )
	{
		u32 vcount = vertices_total_count;
		resources::pinned_ptr_const<u8> vertices_ptr(*item->m_hq_collision_vertices);
		resources::pinned_ptr_const<u8> indices_ptr(*item->m_hq_collision_indices);
		
		memory::chunk_reader vertices_chunk_reader(vertices_ptr.c_ptr(), vertices_ptr.size(), memory::chunk_reader::chunk_type_sequential);
		memory::chunk_reader indices_chunk_reader(indices_ptr.c_ptr(), indices_ptr.size(), memory::chunk_reader::chunk_type_sequential);
		
		memory::reader vertices_reader = vertices_chunk_reader.open_reader(render::model_chunk_collision_v);
		memory::reader indices_reader  = indices_chunk_reader.open_reader(render::model_chunk_collision_i);


		while(!vertices_reader.eof())
		{
			float3 v					= vertices_reader.r_float3();
			v = item->m_matrix->transform_position(v);
			out_vertices.write_float3	( v );
			++vertices_total_count;
		}

		while(!indices_reader.eof())
		{
			u32 index					= indices_reader.r_u32();
			out_indices.write_u32		( vcount + index );
		}
	}

	out_vertices.close_chunk		( );
	out_indices.close_chunk			( );

	//file_name.assignf				( "../../resources/sources/composite_models/%s.composite_model/hq_collision/vertices", 
	//									unmanaged_string(m_full_name).c_str() );
	//out_vertices.save_to			( file_name.c_str() );

	file_name.assignf				( "../../resources/sources/composite_models/%s.composite_model/collision/vertices", 
										unmanaged_string(m_full_name).c_str() );
	out_vertices.save_to			( file_name.c_str() );


	//file_name.assignf				( "../../resources/sources/composite_models/%s.composite_model/hq_collision/indices", 
	//									unmanaged_string(m_full_name).c_str() );
	//out_indices.save_to				( file_name.c_str() );

	file_name.assignf				( "../../resources/sources/composite_models/%s.composite_model/collision/indices", 
										unmanaged_string(m_full_name).c_str() );
	out_indices.save_to				( file_name.c_str() );

	return super::save				( );
}

void edit_object_composite_visual::revert( )
{
	load_model_from_config		( );
	super::revert				( );

}

void edit_object_composite_visual::load( System::String^ model_full_name )
{
	get_model_editor()->action_focus_to_selection();

	super::load					( model_full_name );
	refresh_surfaces_panel		( );
	if(System::String::IsNullOrEmpty(full_name()))
	{
		(*m_model_config)		= configs::create_lua_config( );
		return;
	}

	fs_new::virtual_path_string path;
	path.assignf				( "resources/composite_models/%s.composite_model/composite_render", 
									unmanaged_string(model_full_name).c_str() );

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &edit_object_composite_visual::on_model_config_loaded), g_allocator);
	
	resources::query_resource	(
		path.c_str(),
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
		);
}

void edit_object_composite_visual::on_model_config_loaded( resources::queries_result& data )
{
	if(data.is_successful())
		(*m_model_config) = static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());

	load_model_from_config( );
}

void edit_object_composite_visual::load_model_from_config( )
{
	configs::lua_config_value root		= (*m_model_config)->get_root()["items"];

	configs::lua_config_iterator it			= root.begin();
	configs::lua_config_iterator const it_e = root.end();
	
	for( ;it!=it_e; ++it)
	{
		configs::lua_config_value current	= *it;
		composite_visual_item^ item			= gcnew composite_visual_item(this);

		float3 position		= current["position"];
		float3 rotation		= current["rotation"];
		float3 scale		= current["scale"];

		*item->m_pivot		= current["pivot"];
		*item->m_matrix		= create_scale(scale) * create_rotation(rotation) * create_translation(position);
		item->model_name	= gcnew System::String( (pcstr)current["model_name"] );
		m_contents.Add		( item );
	}
	refresh_surfaces_panel	( );
}

}
}

