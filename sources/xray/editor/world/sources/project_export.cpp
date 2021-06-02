////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project.h"
#include "object_base.h"
#include "tool_base.h"
#include "level_editor.h"
#include "object_skeleton_visual.h"

#pragma managed( push, off )
#	include <xray/render/engine/model_format.h>
#include <xray/geometry_utils.h>
#pragma managed( pop )

namespace xray{
namespace editor{

void on_vertices_loaded( resources::queries_result& data, 
						xray::geometry_utils::geometry_collector* collector, 
						float4x4 const* m, bool is_skeleton )
{
	if(!data.is_successful())
		return;

	resources::pinned_ptr_const<u8> vertices_ptr	( data[0].get_managed_resource() );
	resources::pinned_ptr_const<u8> indices_ptr		( data[1].get_managed_resource() );

	memory::reader vertices_reader		( vertices_ptr.c_ptr(), vertices_ptr.size() );
	memory::reader indices_reader		( indices_ptr.c_ptr(), indices_ptr.size() );

	u32 vert_struct_size				= is_skeleton ? sizeof(render::vert_boned_4w) : sizeof(render::vert_static);
	u32 position_offset					= is_skeleton ? (sizeof(u16[4])) : 0;

	/*u32 icount =*/ indices_reader.r_u32();
	u32 vert_start_offset = sizeof(u32) + position_offset;

	while(!indices_reader.eof())
	{
		u16 idx0			= indices_reader.r_u16();
		u16 idx1			= indices_reader.r_u16();
		u16 idx2			= indices_reader.r_u16();
		
		vertices_reader.seek(vert_start_offset+vert_struct_size*idx0);
		float3 p0			= vertices_reader.r_float3() * (*m);
		p0.z				*=-1.0f;

		vertices_reader.seek(vert_start_offset+vert_struct_size*idx1);
		float3 p1			= vertices_reader.r_float3() * (*m);
		p1.z				*=-1.0f;

		vertices_reader.seek(vert_start_offset+vert_struct_size*idx2);
		float3 p2			= vertices_reader.r_float3() * (*m);
		p2.z				*=-1.0f;

		if( p0.is_similar(p1) || p0.is_similar(p2) || p1.is_similar(p2) )
			continue;

		collector->add_triangle( p0, p2, p1 );
	}
}

void on_model_folder_fs_iterator_ready( vfs::vfs_locked_iterator const & in_fs_it, 
									   xray::geometry_utils::geometry_collector* collector, pcstr model_path, float4x4 const* m, bool is_skeleton )
{
	if( in_fs_it.is_end() )
		return;
	
	vfs::vfs_iterator fs_it			= in_fs_it.children_begin();
	vectora<fs_new::virtual_path_string>	surface_names(g_allocator);
	while(!fs_it.is_end())
	{
		if( fs_it.is_folder() )
		{
			fs_new::virtual_path_string	sname	= model_path;
			sname.append			( "/" );
			sname.append			( fs_it.get_name() );
			sname.append			( "/" );
			sname.append			( "vertices" );
			surface_names.push_back	( sname );

			sname					= model_path;
			sname.append			( "/" );
			sname.append			( fs_it.get_name() );
			sname.append			( "/" );
			sname.append			( "indices" );
			surface_names.push_back	( sname );

		}
		++fs_it;
	}
	u32 count = surface_names.size();
	for(u32 i=0; i<count; i+=2)
	{
		resources::request r[]={
			{ surface_names[i].c_str(), resources::raw_data_class },
			{ surface_names[i+1].c_str(), resources::raw_data_class },
		};

		resources::query_resources_and_wait(
			r,
			2,
			boost::bind(&on_vertices_loaded, _1, collector, m, is_skeleton),
			g_allocator
			);
	}
}

void export_model_as_obj( project^ p, 
							xray::geometry_utils::geometry_collector& collector, 
							System::String^ name, 
							math::float4x4 const& m, 
							bool is_skeleton )
{
	XRAY_UNREFERENCED_PARAMETERS( p );
	fs_new::virtual_path_string	model_name;

	model_name.assignf("resources/models/%s.model/render", unmanaged_string(name).c_str() );
	
	resources::query_vfs_iterator_and_wait(
		model_name.c_str(), 
		boost::bind(&on_model_folder_fs_iterator_ready, _1, &collector, model_name.c_str(), &m, is_skeleton ),
		g_allocator,
		resources::recursive_true
		);
}

void project::export_as_obj( bool selection_only )
{
	System::Windows::Forms::SaveFileDialog		saveFileDialog;
	
	if( System::Windows::Forms::DialogResult::OK == saveFileDialog.ShowDialog() )
	{

		project_items_list^	selection = (selection_only) ? selection_list() : get_all_items( );

		xray::geometry_utils::geometry_collector collector(debug::g_mt_allocator);
		for each( project_item_base^ itm in selection)
		{
			object_base^ o = itm->get_object();
			if(o==nullptr)
				continue;

			if(o->owner_tool()->name()!="solid_visual")
				continue;

			bool is_skeleton = dynamic_cast<object_skeleton_visual^>(o)!=nullptr;

			export_model_as_obj			( this, collector, o->get_library_name(), o->get_transform(), is_skeleton );
		}

		System::String^ fn	= saveFileDialog.FileName->Replace("/", "\\");
		fn					+=".obj";

		collector.write_obj_file		(unmanaged_string(fn).c_str(), 100.0f);
		get_level_editor()->ShowMessageBox( System::String::Format("OBJ file export to {0} succeeded.", fn), 
											System::Windows::Forms::MessageBoxButtons::OK, 
											System::Windows::Forms::MessageBoxIcon::Information );

	}
}


}
}
