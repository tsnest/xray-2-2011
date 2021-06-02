////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skin_exporter.h"
#include "skin_visual_writer.h"
#include "export_defines.h"
#include <xray/resources_fs.h>
#include <xray/fs/device_utils.h>
#include <xray/vfs/pack_archive.h>
#include "shellapi.h"
#include "maya_engine.h"
#include "time.h"

namespace xray{
namespace maya{

const MString skinned_visual_exporter::Name	( "xray_skin_export" );

void* skinned_visual_exporter::creator()
{
	return CRT_NEW(skinned_visual_exporter)();
}

MSyntax skinned_visual_exporter::newSyntax()
{
    MSyntax			syntax;
	syntax.addFlag	( file_name_flag, file_name_flag_l, MSyntax::kString );
	syntax.addFlag	( skin_name_flag, skin_name_flag_l, MSyntax::kString );
	syntax.addFlag	( skeleton_name_flag, skeleton_name_flag_l, MSyntax::kString );

	syntax.addFlag	( tootle_opt_flag, tootle_opt_flag_l, MSyntax::kNoArg );
	syntax.addFlag	( tootle_opt_hq_flag, tootle_opt_hq_flag_l, MSyntax::kNoArg );
	syntax.addFlag	( no_out_to_db_flag, no_out_to_db_flag_l, MSyntax::kNoArg );
	
	syntax.enableEdit( false );

	return syntax;
}

MStatus skinned_visual_exporter::check_args( MArgDatabase& arg_data )
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

	MStatus result				= arg_data.getFlagArgument( file_name_flag_l, 0, m_folder_name );
	CHK_STAT_R					(result);
	m_folder_name				+= ".skinned_model";

	if( !arg_data.isFlagSet( skin_name_flag ) )
	{
		error_str				= "skin name flag (";
		error_str				+= skin_name_flag_l;
		error_str				+= ") not specified";

		display_warning	(error_str);
		return					MStatus::kFailure;
	}

	if( !arg_data.isFlagSet( skeleton_name_flag ) )
	{
		error_str				= "skeleton name flag (";
		error_str				+= skeleton_name_flag_l;
		error_str				+= ") not specified";

		display_warning	(error_str);
		return					MStatus::kFailure;
	}



	return MStatus::kSuccess;
}

MStatus skinned_visual_exporter::doIt( const MArgList& arglist )
{
	g_uniq_surface_counter		= 0;

	MStatus						result;

	MArgDatabase arg_data		( syntax(), arglist, &result );
	CHK_STAT_R					( result );
	
	result						= check_args( arg_data );
	CHK_STAT_R					(result);

	bool nodb					= arg_data.isFlagSet( no_out_to_db_flag );
	//nodb = true;
	MString						skin_name;
	result						= arg_data.getFlagArgument( skin_name_flag, 0, skin_name );
	CHK_STAT_R					(result);

	skin_visual_collector collector( arg_data, result );
	CHK_STAT_R					(result);

//	clear						( );
	MString locators_path		= skin_name + "|locators";
	result						= collector.extract_locators( locators_path, MMatrix::identity );
	CHK_STAT_R					( result );

	MString skin_cluster_path	= skin_name;// + "|vis";
	result						= collector.export_skin_visual	( skin_cluster_path );
	CHK_STAT_R					(result);


	for(u32 i=0; i<collector.m_locators.size(); ++i)
	{
		locator_item& itm = collector.m_locators[i];
		if(itm.bone_name.length())
		{
			itm.bone_idx = collector.m_skeleton->get_bone_index(itm.bone_name.asChar()) - 1;
		}
	}

	MString temp_path;
	if( !nodb )
	{
		xray::fs_new::native_path_string	file_path;
		xray::fs_new::file_name_with_no_extension_from_path	(&file_path, xray::fs_new::native_path_string::convert(m_folder_name.asChar()).c_str() );

		MString fname = file_path.c_str();
		temp_path					= get_temp_unique_file_name( fname );

		xray::fs_new::synchronous_device_interface const & device	=	xray::resources::get_synchronous_device();
		xray::fs_new::create_folder_r		( device, temp_path.asChar(), true );
	}
	else
	{
		temp_path = m_folder_name;
	}

	MString file_name			= temp_path;
	{
		configs::lua_config_ptr	skeleton_cfg	= configs::create_lua_config();
		collector.write_skeleton_properties		( skeleton_cfg->get_root() );
	
		MString fnskeleton			= file_name;
		fnskeleton					+= "/skeleton";
		skeleton_cfg->save_as		( fnskeleton.asChar(), configs::target_default );
	}
	{
		// write geometry and material

		u32 const surfaces_count	= collector.get_surfaces_count();
		MString fn					= file_name;
		fn							+= "/render";

		//write render model properties
		configs::lua_config_ptr	config	= configs::create_lua_config();
		result						= collector.write_properties( config->get_root());
		CHK_STAT_R					( result );

		xray::fs_new::synchronous_device_interface const & device	=	xray::resources::get_synchronous_device();
		xray::fs_new::create_folder_r		( device, fn.asChar(), true );

		MString fnp				= fn;
		fnp							+= "/export_properties";
		config->save_as				( fnp.asChar(), configs::target_default );

		for( u32 idx = 0; idx < surfaces_count; ++idx )
		{
			surface const* s			= collector.get_surface( idx );
			MString fns					= fn;
			fns							+= "/";
			fns							+= s->save_name( );
			MString fnsv;

			//write properties
			configs::lua_config_ptr	config	= configs::create_lua_config();
			result						= s->save_properties( config->get_root() );
			CHK_STAT_R					( result );

			xray::fs_new::synchronous_device_interface const & device	=	xray::resources::get_synchronous_device();
			xray::fs_new::create_folder_r		( device, fns.asChar(), true );

			fnsv						= fns;
			fnsv						+= "/export_properties";
			config->save_as				( fnsv.asChar(), configs::target_default );

			//write vertices
			memory::writer wv			( &g_allocator );
			result						= s->save_vertices( wv );
			CHK_STAT_R					(result);

			fnsv						= fns;
			fnsv						+= "/vertices";
			wv.save_to					( fnsv.asChar() );

			//write indices
			memory::writer wi			( &g_allocator );
			result						= s->save_indices( wi );
			CHK_STAT_R					(result);

			fnsv						= fns;
			fnsv						+= "/indices";
			wi.save_to					( fnsv.asChar() );

		}
		{
			//write bones

			memory::writer				w2( &g_allocator );
			result						= collector.write_bones( w2 );
			CHK_STAT_R					(result);

			MString fnb					= fn;
			fnb							+= "/bind_pose";
			w2.save_to					( fnb.asChar() );
		}	
	}
	

	bool is_db_created = false;

	if( !nodb )
	{
		
		xray::fs_new::synchronous_device_interface & device	=	xray::resources::get_synchronous_device();
		
		MString db_file_name		= m_folder_name;
		xray::fs_new::create_folder_r	( device, db_file_name.asChar(), false );
		remove_file_or_directory	( m_folder_name );

		vfs::pack_archive_args	pack_args	(device, NULL, (logging::log_flags_enum)0);
		pack_args.flags				= (vfs::save_flags_enum)
				(vfs::save_flag_forbid_folder_links | vfs::save_flag_forbid_empty_files);
		pack_args.sources			= fs_new::native_path_string::convert(temp_path.asChar());
		pack_args.allocator			= & g_allocator;
		pack_args.target_db			= fs_new::native_path_string::convert(db_file_name.asChar());
		pack_args.target_fat		= fs_new::native_path_string::convert(db_file_name.asChar());
		pack_args.fat_align			= 512;
		pack_args.platform			= vfs::archive_platform_pc;

		bool const result			= vfs::pack_archive(pack_args);
		R_ASSERT					(result);

		is_db_created				= true;
	}

	if(is_db_created)
	{
		remove_file_or_directory	( temp_path );
	}

	display_info		("Export " + m_folder_name + " successful!");

	return						result;
}

} //namespace maya
} //namespace xray
