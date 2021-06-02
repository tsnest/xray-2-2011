////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "solid_visual_exporter.h"
#include "solid_visual_collector.h"
#include <xray/resources_fs.h>
#include <xray/fs/device_utils.h>
#include <xray/vfs/pack_archive.h>
#include <xray/math_randoms_generator.h>
#include "export_defines.h"
#include "maya_engine.h"
#include "time.h"
#include <direct.h>

namespace xray{
namespace maya{

const MString solid_visual_exporter::Name	( "xray_solid_visual_export" );



struct mesh_descr{
	pcstr visual;
	pcstr collision;
//	pcstr postfix;
};

mesh_descr input_meshes [] = {
	{"vis", "col" },
	{NULL, NULL},
};

MSyntax solid_visual_exporter::newSyntax()
{
    MSyntax			syntax;
	syntax.addFlag	( file_name_flag, file_name_flag_l, MSyntax::kString );
	syntax.addFlag	( root_name_flag, root_name_flag_l, MSyntax::kString );

	syntax.addFlag	( tootle_opt_flag, tootle_opt_flag_l, MSyntax::kNoArg );
	syntax.addFlag	( tootle_opt_hq_flag, tootle_opt_hq_flag_l, MSyntax::kNoArg );
	syntax.addFlag	( no_out_to_db_flag, no_out_to_db_flag_l, MSyntax::kNoArg );

	syntax.enableEdit( false );

	return syntax;
}

void* solid_visual_exporter::creator( )
{
	return CRT_NEW(solid_visual_exporter)();
}

MStatus solid_visual_exporter::check_args( MArgDatabase& arg_data )
{
	MString	error_str;
	if( !arg_data.isFlagSet( file_name_flag_l ) )
	{
		error_str				= "output file name flag (";
		error_str				+= file_name_flag_l;
		error_str				+= ") not specified";

		display_warning	(error_str);
		return					MStatus::kFailure;
	}

	MStatus result				= arg_data.getFlagArgument( file_name_flag_l, 0, m_file_name );
	CHK_STAT_R					(result);
	m_file_name					+= ".model";

	if( !arg_data.isFlagSet( root_name_flag ) )
	{
		error_str				= "root name flag (";
		error_str				+= root_name_flag_l;
		error_str				+= ") not specified";

		display_warning	(error_str);
		return					MStatus::kFailure;
	}
	return MStatus::kSuccess;
}

MStatus solid_visual_exporter::doIt( const MArgList& arglist)
{
	using namespace xray::fs_new;

	g_uniq_surface_counter		= 0;
	MStatus						result;

	MArgDatabase arg_data		(syntax(), arglist, &result);
	CHK_STAT_R					(result);
	
	result						= check_args( arg_data );
	CHK_STAT_R					(result);
	remove_file_or_directory	( m_file_name );

	MStringArray				file_list;

	MString						temp_path;
	if( !arg_data.isFlagSet( no_out_to_db_flag ) )
	{
		temp_path					= get_temp_unique_file_name( m_file_name );
		remove_file_or_directory	( temp_path );
	}
	else
	{
		temp_path					= m_file_name;
	}

	synchronous_device_interface const & device	=	xray::resources::get_synchronous_device();
	create_folder_r		( device, temp_path.asChar(), true );

	
	MDagPath					dag_path;
	MString						root_name;
	result						= arg_data.getFlagArgument( root_name_flag, 0, root_name );
	CHK_STAT_R					(result);

	MDagPath					root_dag_path;
	result						= get_path_by_name( root_name, root_dag_path, true );
	CHK_STAT_R					(result);
	MString last_name			= root_dag_path.partialPathName( &result );
	CHK_STAT_R					(result);

	mesh_descr& desc = input_meshes[0];

	MString	visual_name			= root_name + "|" + desc.visual;
						
	result						= get_path_by_name( visual_name, dag_path, false );// assert having LOD0 

	if( result.error() )
	{
		display_error	( "Geometry not found " + visual_name );
		display_error	( "Export " + m_file_name + " FAILED!\n\n" );
		return MStatus::kFailure;
	}

	MMatrix locator_matrix		= MMatrix::identity;
	if( root_dag_path.hasFn(MFn::kLocator) )
	{
		locator_matrix			= root_dag_path.inclusiveMatrix( &result );  
		CHK_STAT_R				( result );
	}

	u16 max_verts_count = 65500;

	for(;;)
	{
		solid_visual_collector collector	( arg_data, result, max_verts_count );
		CHK_STAT_R							( result );
		collector.set_locator_matrix		( locator_matrix );
		result								= collector.extract_render_static_geometry_from_path( dag_path  );
		
		if( result.error() )
		{
			display_error	( "Invalid geometry detected in " + visual_name );
			display_error	( "Export " + m_file_name + " FAILED!\n" );
			return MStatus::kFailure;
		}
		
		u32 render_surfaces_count			= collector.get_surfaces_count( );
		if(render_surfaces_count==0)
		{
			display_error	( "no render surfaces collected from " + visual_name );
			display_error	( "Export " + m_file_name + " FAILED!\n" );
			return			MStatus::kFailure;
		}

		MString	collision_name		= root_name + "|" + desc.collision;
		result						= get_path_by_name( collision_name, dag_path, false );

		if( !result.error() )
		{
			result					= collector.extract_collision_geometry_from_path( dag_path );
			CHK_STAT_R				(result);

			if(collector.get_collision_surface().vertices_count()==0)
			{
				display_error("no collision vertices collected from " + visual_name );
				display_error	( "Export " + m_file_name + " FAILED!\n" );
				return MStatus::kFailure;
			}
		}else
		{
			result				= collector.build_collision_geometry_by_render( );
			CHK_STAT_R			(result);
			if(collector.get_collision_surface().vertices_count()==0)
			{
				display_error	("no collision vertices collected from " + collision_name );
				display_error	( "Export " + m_file_name + " FAILED!\n" );

				return			MStatus::kFailure;
			}
		}

		MString locators_path		= root_name  + "|locators";
		result						= collector.extract_locators( locators_path, locator_matrix );
		CHK_STAT_R					( result );

		MString file_name			= temp_path;

		MStatus prepare_result = collector.prepare_surfaces		( );
		if(prepare_result==MStatus::kInsufficientMemory)
		{
			max_verts_count	-=500;
			continue;
		}

		collector.dump_statistic		( true );
	
		fs_new::synchronous_device_interface& fs_device	=	xray::resources::get_synchronous_device();

		MString fn					= file_name;
		{
			// write geometry and material
			fn							+= "/render";

			//write render model properties
			configs::lua_config_ptr	config	= configs::create_lua_config();
			result						= collector.write_properties( config->get_root());
			CHK_STAT_R					( result );

			create_folder_r		( fs_device, fn.asChar(), true );
			
			MString fnp					= fn;
			fnp							+= "/export_properties";
			config->save_as				( fnp.asChar(), configs::target_default );
			file_list.append			( fnp );
		}

		for( u32 idx = 0; idx < render_surfaces_count; ++idx )
		{
			surface const* s			= collector.get_surface( idx );
			if(s->empty())
				continue;

			MString fns					= fn;
			fns							+= "/";
			fns							+= s->save_name();
			MString fnsv;

			//write properties
			configs::lua_config_ptr	config	= configs::create_lua_config();
			result						= s->save_properties( config->get_root() );
			CHK_STAT_R					( result );
			
			create_folder_r		( fs_device, fns.asChar(), true );

			fnsv						= fns;
			fnsv						+= "/export_properties";
			config->save_as				( fnsv.asChar(), configs::target_default );
			file_list.append			( fnsv );

			//write vertices
			memory::writer wv			( &g_allocator );
			result						= s->save_vertices( wv );
			CHK_STAT_R					(result);

			fnsv						= fns;
			fnsv						+= "/vertices";
			wv.save_to					( fnsv.asChar() );
			file_list.append			( fnsv );

			//write indices
			memory::writer wi			( &g_allocator );
			result						= s->save_indices( wi );
			CHK_STAT_R					(result);

			fnsv						= fns;
			fnsv						+= "/indices";
			wi.save_to					( fnsv.asChar() );
			file_list.append			( fnsv );

		}

		{
			//write collision
			memory::writer w( &g_allocator );
			result						= collector.get_collision_surface().save_indices( w ) ? MStatus::kSuccess : MStatus::kFailure;
			CHK_STAT_R					(result);

			MString fn					= file_name;
			fn							+= "/collision/indices";

			fs_new::create_folder_r		( fs_device, fn.asChar(), false );

			w.save_to					( fn.asChar() );
			file_list.append			( fn );
			w.clear						( );

			result						= collector.get_collision_surface().save_vertices( w ) ? MStatus::kSuccess : MStatus::kFailure;
			CHK_STAT_R					(result);

			fn							= file_name;
			fn							+= "/collision/vertices";

			create_folder_r	( device, fn.asChar(), false );

			w.save_to					( fn.asChar() );
			file_list.append			( fn );
			w.clear					( );
		}

		// write high-quality collision geometry for editor
		if(!collector.is_collision_from_geometry())
			collector.build_collision_geometry_by_render();

		{
			memory::writer w( &g_allocator );
			result					= collector.get_collision_surface().save_indices( w ) ? MStatus::kSuccess : MStatus::kFailure;
			CHK_STAT_R				(result);

			MString fn				= file_name;
			fn						+= "/hq_collision/indices";

			fs_new::create_folder_r		( device, fn.asChar(), false );

			w.save_to				( fn.asChar() );
			file_list.append		( fn );
			w.clear					( );

			result					= collector.get_collision_surface().save_vertices( w ) ? MStatus::kSuccess : MStatus::kFailure;
			CHK_STAT_R				(result);

			fn						= file_name;
			fn						+= "/hq_collision/vertices";
			
			fs_new::create_folder_r		( device, fn.asChar(), false );

			w.save_to				( fn.asChar() );
			file_list.append		( fn );
			w.clear					( );

		}

		bool is_db_created = false;

		if( !arg_data.isFlagSet( no_out_to_db_flag ) )
		{

			MString db_file_name		= m_file_name;
			create_folder_r				( fs_device, db_file_name.asChar(), false );

			vfs::pack_archive_args	pack_args	(fs_device, NULL, (logging::log_flags_enum)0);
			pack_args.flags				= (vfs::save_flags_enum)
				(vfs::save_flag_forbid_folder_links | vfs::save_flag_forbid_empty_files);

			pack_args.sources			= native_path_string::convert(temp_path.asChar());
			pack_args.allocator			= & g_allocator;
			pack_args.target_db			= native_path_string::convert(db_file_name.asChar());
			pack_args.target_fat		= native_path_string::convert(db_file_name.asChar());
			pack_args.fat_align			= 512;
			pack_args.platform			= vfs::archive_platform_pc;

			bool const result			= vfs::pack_archive(pack_args);
			R_ASSERT					(result);
	

			is_db_created				= true;
		}

		if(is_db_created)
			remove_file_or_directory	( temp_path );
		 
		display_info				( "Export " + m_file_name + " successful!\n" );
		break;
	}

	return						MStatus::kSuccess;
}

} //namespace maya
} //namespace xray
