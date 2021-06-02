////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skeleton_export_util.h"
#include "anim_export_util.h"
#include "export_utils.h"

#include <xray/fs/device_utils.h>


const MString skeleton_export_cmd::Name	( "xray_skeleton_export" );

static pcstr extension = "skeleton";

MStatus		export_skeleton_node( MObject  &ik_anim_joint_object, xray::configs::lua_config_value &cfg, const  MSelectionList &all_claster_joints )
{
	MStatus				stat;
	
	MFnDagNode			ik_anim_joint( ik_anim_joint_object, &stat ); CHK_STAT_R( stat );

	pcstr name			= ik_anim_joint.name(&stat).asChar();
	CHK_STAT_R			( stat )

	
	u32 ch_cnt			= ik_anim_joint.childCount( &stat );
	CHK_STAT_R			( stat );

	xray::configs::lua_config_value my_cfg = cfg; 

	//MObject		my_obj 	= ik_anim_joint.object( &stat );
	MDagPath	my_path;
	CHK_STAT_R			( ik_anim_joint.getPath( my_path ) );

	//if( all_claster_joints.hasItem( my_path, my_obj,  &stat ) )
	//if( ik_anim_joint_object.hasFn( MFn::kJoint ) )
	if( ik_anim_joint_object.hasFn( MFn::kJoint ) && all_claster_joints.hasItem( my_path, ik_anim_joint_object,  &stat ) )
	{
		CHK_STAT_R		( stat );
		my_cfg			= cfg[ name ];
		my_cfg.create_table();
	}; //else
		//return			stat;

	
	for(u32 i = 0 ; i < ch_cnt ; ++i )
	{
		MObject	 ik_anim_joint_child = ik_anim_joint.child( i, &stat );
		CHK_STAT_R( stat );

		//if( !obj.hasFn( MFn::kJoint ) )
		//	continue;
		//MFnIkJoint ik_anim_joint_child( obj, &stat );CHK_STAT_R( stat );
		
		xray::configs::lua_config_value temp_cfg = my_cfg;
		CHK_STAT_R( export_skeleton_node( ik_anim_joint_child, temp_cfg, all_claster_joints ) );
	}
	return stat;

}

MStatus		export_skeleton( MSelectionList &all_claster_joints, xray::configs::lua_config_value &cfg  )
{
	
	MStatus stat;
	
	MObject			anim_joint_object_first;
	CHK_STAT_R( all_claster_joints.getDependNode		(0, anim_joint_object_first) );

	// get root

	MObject			anim_joint_object_root = retrive_root_joint( anim_joint_object_first );
	
	if( anim_joint_object_root.isNull() )
	{
		display_warning( "Root joint not found!" );
		return MStatus::kFailure;
	}

	ASSERT( anim_joint_object_root.hasFn( MFn::kJoint ) );
	MFnIkJoint ik_anim_joint_root( anim_joint_object_root, &stat );

	CHK_STAT_R( stat );
	//u32 pcnt = ik_anim_joint_root.parentCount( &stat );CHK_STAT( stat );

	//R_ASSERT_U( pcnt==0 ,"not a root");

	xray::configs::lua_config_value object_movenent_cfg = cfg; 
	object_movenent_cfg = cfg[ object_mover_bone_name ];
	object_movenent_cfg.create_table();

	//export_skeleton_node( ik_anim_joint_root, object_movenent_cfg, all_claster_joints );
	export_skeleton_node( anim_joint_object_root, object_movenent_cfg, all_claster_joints );
	return stat;

}

MStatus		export_skeleton( MString file_name )
{
	MStringArray					all_joints_str;
	MGlobal::executeCommand			("skinCluster -q -inf", all_joints_str);
	
	

	MSelectionList all_joints;
	int length						= all_joints_str.length();
	
	MString str = "Export Skeleton: found ";	str += length;	str += " animation joints";
	display_info					(str);
	
	for(int i=0; i<length; ++i)
		all_joints.add( all_joints_str[i] );

	CHK_STAT_R( check_claster_joints ( all_joints ) );

	xray::configs::lua_config_ptr		cfg		= xray::configs::create_lua_config();
	xray::configs::lua_config_value		v_cfg  = *cfg;

	if( export_skeleton( all_joints, v_cfg ) == MStatus::kSuccess )
	{
		cfg->save_as	( file_name.asChar(), xray::configs::target_sources );
		display_info	( "Export " + file_name + " successful!\n" );
	}else
	{
		display_info	( "Export " + file_name + " FAILED!\n" );
		return MStatus::kFailure;
	}

	return MStatus::kSuccess;
}



MSyntax	skeleton_export_cmd::newSyntax()
{
	MSyntax	syntax;
	
	//syntax.addArg( 
	syntax.addFlag( file_name_flag, file_name_flag_l,	MSyntax::kString );

	return syntax;
}


 MStatus	skeleton_export_cmd::doIt( const MArgList& args )
{
	using namespace xray::fs_new;
	MStatus				stat;

	MArgDatabase argData( syntax(), args );
	MString				file_name = file_name_from_args( argData, &stat );
	CHK_STAT_R			( stat );	
	
	synchronous_device_interface const & device	=	xray::resources::get_synchronous_device();
	create_folder_r		( device, file_name.asChar(), false );
	
	if( MString( extension_from_path( file_name.asChar() ) ) != extension )
		file_name += MString(".") + MString( extension );

	return export_skeleton( file_name );
}