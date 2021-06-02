////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_node.h"
#include "create_collision_cmd.h"
#include "build_collision_cmd.h"
#include "body_node.h"
#include "body_node_manip.h"
#include "joint_node.h"
#include "anim_export_util.h"
#include "skeleton_export_util.h"
#include "create_body_cmd.h"
#include "create_joint_cmd.h"
#include "physics_exporter.h"
#include "solid_visual_exporter.h"
#include "skin_exporter.h"
#include "fake_drawing_locator.h"
#include "xray_path_commands.h"
//#include "xray_material.h"
#include <xray/core/core.h>
#include <xray/command_line_extensions.h>
#include <xray/core/simple_engine.h>
#include "maya_engine.h"
#include <xray/maya_animation/world.h>
#include <xray/maya_animation/api.h>
#include <xray/os_include.h>
#include <xray/animation/api.h>
#include <xray/resources_fs.h>
#include <xray/resources.h>
#include <xray/fs/simple_synchronous_interface.h>

xray::maya::allocator_type			xray::maya::g_allocator(xray::memory::thread_id_const_false);
xray::maya::maya_engine				core_engine;
xray::maya::maya_engine&			xray::maya::g_maya_engine = core_engine;

xray::uninitialized_reference< xray::fs_new::simple_synchronous_interface >	fs_devices;
static xray::resources::mount_ptr	s_resources_mount;

static bool						s_on_resources_mounted_called	= false;

static void on_resources_mounted		( xray::resources::mount_ptr * out_mount, xray::resources::mount_ptr result )
{
	CHECK_OR_EXIT						( result, "Cannot mount resources" );
	* out_mount							= result;
	s_on_resources_mounted_called		= true;
}

static void scene_loading	( void* )
{
	xray::maya::g_maya_engine.m_is_scene_loading = true;
}

static void scene_loaded	( void * )
{
	xray::maya::g_maya_engine.m_is_scene_loading = false;
}

XRAY_DLL_EXPORT MStatus initializePlugin( MObject obj )
{
	xray::core::preinitialize	(
		& core_engine,
		xray::logging::no_log, 
		"-enable_crt_memory_allocator -no_memory_usage_stats -no_fs_watch -floating_point_control_disabled -fpe_disabled -log_to_stdout -max_resources_size=0 -debug_allocator=64",
		xray::command_line::contains_application_false,
		"maya",
		__DATE__
	);

	MStatus stat						= MStatus::kSuccess;

	LOG_INFO( "initialize xray maya plugin!" );

	xray::maya::g_allocator.do_register	( 50 * xray::Mb, "maya" );
	xray::memory::allocate_region		( );

	pstr lua_config_device_folder_to_save_to = 0;
	STR_JOINA							( lua_config_device_folder_to_save_to, core_engine.get_resources_path(), "/exported/" );
	xray::core::initialize				( lua_config_device_folder_to_save_to, "maya plugin", xray::core::perform_debug_initialization );

	XRAY_CONSTRUCT_REFERENCE			( fs_devices, xray::fs_new::simple_synchronous_interface ) 
											( xray::fs_new::watcher_enabled_true );

	xray::core::initialize_resources	( fs_devices->hdd_async, fs_devices->dvd_async, xray::resources::enable_fs_watcher_false );
	xray::resources::start_resources_threads	( );

	xray::logging::push_filter			( "", xray::logging::warning, &xray::maya::g_allocator );
	
	xray::logging::push_filter			( "maya", xray::logging::trace, &xray::maya::g_allocator );

	xray::animation::set_memory_allocator( xray::maya::g_allocator );

	core_engine.animation_world = xray::animation::create_world( core_engine, 0, 0, 0 );
	ASSERT( core_engine.animation_world );
	
	xray::maya_animation::set_memory_allocator( xray::maya::g_allocator );
	core_engine.maya_animation_world = xray::maya_animation::create_world( core_engine );

	ASSERT( core_engine.maya_animation_world );


	MFnPlugin plugin	( obj, "GSC Game World", "1.0", "Any");

	build_sphere		(stat);
	CHK_STAT_R			( stat );

	build_cylinder		(stat);
	CHK_STAT_R			( stat );

	stat = plugin.registerTransform(	collision_node::typeName,
										collision_node::typeId,
										&collision_node::creator, 
										&collision_node::initialize, 
										MPxTransformationMatrix::creator, 
										MPxTransformationMatrix::baseTransformationMatrixId);

	CHK_STAT_R			( stat );

	stat = plugin.registerTransform(	body_node::typeName, 
										body_node::typeId, 
										&body_node::creator, 
										&body_node::initialize, 
										MPxTransformationMatrix::creator, 
										MPxTransformationMatrix::baseTransformationMatrixId);
	CHK_STAT_R			( stat );

	stat = plugin.registerTransform(	joint_node::typeName,
										joint_node::typeId,
										&joint_node::creator, 
										&joint_node::initialize, 
										MPxTransformationMatrix::creator, 
										MPxTransformationMatrix::baseTransformationMatrixId);

	CHK_STAT_R			( stat );

	stat = plugin.registerNode(			fake_drawing_locator::typeName,
										fake_drawing_locator::typeId,
										&fake_drawing_locator::creator, 
										&fake_drawing_locator::initialize, 
										MPxNode::kLocatorNode );

	CHK_STAT_R			( stat );

	stat = plugin.registerNode(			body_node_manip::typeName,
										body_node_manip::typeId, 
										&body_node_manip::creator,
										&body_node_manip::initialize, 
										MPxNode::kManipContainer );

	CHK_STAT_R			( stat );

	stat = plugin.registerCommand( createBodyCmd::Name, createBodyCmd::creator, createBodyCmd::newSyntax );
	CHK_STAT_R			( stat );

	stat = plugin.registerCommand( createCollisionCmd::Name, createCollisionCmd::creator, createCollisionCmd::newSyntax );
	CHK_STAT_R			( stat );

	stat = plugin.registerCommand( createJointCmd::Name, createJointCmd::creator, createJointCmd::newSyntax );
	CHK_STAT_R			( stat );

	stat = plugin.registerCommand( build_collision_cmd::Name, build_collision_cmd::creator, build_collision_cmd::newSyntax );
	CHK_STAT_R			( stat );

	stat = plugin.registerFileTranslator("xray_ph_exporter", NULL, &physics_exporter::creator);
	CHK_STAT_R			( stat );

	stat = plugin.registerCommand(	xray::maya::solid_visual_exporter::Name, 
									xray::maya::solid_visual_exporter::creator, 
									xray::maya::solid_visual_exporter::newSyntax );
	CHK_STAT_R			( stat );

	stat = plugin.registerCommand(xray_path_cmd::Name, xray_path_cmd::creator, xray_path_cmd::newSyntax );
	CHK_STAT_R			( stat );

	stat = plugin.registerCommand(anim_export_cmd::Name, anim_export_cmd::creator, anim_export_cmd::newSyntax );
	CHK_STAT_R			( stat );

	stat = plugin.registerCommand(skeleton_export_cmd::Name, skeleton_export_cmd::creator, skeleton_export_cmd::newSyntax );
	CHK_STAT_R			( stat );


	//const MString UserClassify( "shader/surface" );

	//stat = plugin.registerNode( xray_material_node::Name, 
	//							xray_material_node::id, 
	//							xray_material_node::creator, 
	//							xray_material_node::initialize,
	//							MPxNode::kDependNode, 
	//							&UserClassify );
	//CHK_STAT_R		( stat );

	//const MString& swatchName =	MHWShaderSwatchGenerator::initialize();
	//const MString UserClassify_hw( "shader/surface/utility/:swatch/"+swatchName );

	//stat = plugin.registerNode( "hwPhongShader", hwPhongShader::id, 
	//		                      hwPhongShader::creator, hwPhongShader::initialize,
	//							  MPxNode::kHwShaderNode, &UserClassify_hw );
	//CHK_STAT_R		( stat );

	//plugin.registerDragAndDropBehavior("hwPhongShaderBehavior", 
	//								   hwPhongShaderBehavior::creator);

	//
	//MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	//command			+= UserClassify;
	//command			+= "\");}\n";

	//MGlobal::executeCommand( command );

	//command = ( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	//command			+= UserClassify_hw;
	//command			+= "\");}\n";

	//MGlobal::executeCommand( command );

	stat = plugin.registerCommand(	xray::maya::skinned_visual_exporter::Name, 
									xray::maya::skinned_visual_exporter::creator, 
									xray::maya::skinned_visual_exporter::newSyntax );
	CHK_STAT_R			( stat );
	
	
	MCallbackId id;
	id					= MSceneMessage::addCallback( MSceneMessage::kBeforeOpen, scene_loading, NULL, &stat );
	CHK_STAT_R			( stat );
	xray::maya::g_maya_engine.register_callback_id( id );
	id					= MSceneMessage::addCallback( MSceneMessage::kAfterOpen, scene_loaded, NULL, &stat );
	CHK_STAT_R			( stat );
	xray::maya::g_maya_engine.register_callback_id( id );
	//id					= MSceneMessage::addCallback( MSceneMessage::kAfterPluginLoad, plugin_loaded, NULL, &stat );
	//CHK_STAT_R			( stat );
	//xray::maya::g_maya_engine.register_callback_id( id );


	
	xray::resources::query_mount			( "resources", 
											boost::bind(& on_resources_mounted, & s_resources_mount, _1), 
											&xray::maya::g_allocator );

	xray::timing::timer	timer;
	timer.start			( );
	while ( !s_on_resources_mounted_called )
	{
		xray::resources::dispatch_callbacks();
		if ( xray::threading::g_debug_single_thread )
			xray::resources::tick				( );

		if ( timer.get_elapsed_sec() > 10 && !xray::debug::is_debugger_present() )
			break;
	}
	
	xray::resources::wait_and_dispatch_callbacks( true );


	if ( !s_on_resources_mounted_called || !s_resources_mount )
		return					MStatus::kFailure;

	LOG_INFO("Initialized XRAY maya plugin!");

	return stat;
}

XRAY_DLL_EXPORT MStatus uninitializePlugin( MObject obj)
{
	s_resources_mount	=	NULL;
	LOG_INFO("uninitializePlugin called");
	MStatus			stat;
	MFnPlugin		plugin( obj );

	stat = plugin.deregisterNode( collision_node::typeId );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterNode( body_node::typeId );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterNode( body_node_manip::typeId );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterNode( fake_drawing_locator::typeId );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterNode( joint_node::typeId );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterCommand( createBodyCmd::Name );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterCommand( createCollisionCmd::Name );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterCommand( createJointCmd::Name );
	CHK_STAT_R			( stat );
	
	stat = plugin.deregisterCommand( build_collision_cmd::Name );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterFileTranslator("xray_ph_exporter");
	CHK_STAT_R			( stat );

	stat = plugin.deregisterCommand( xray::maya::solid_visual_exporter::Name );
	CHK_STAT_R			( stat );
	

	stat = plugin.deregisterCommand( xray::maya::skinned_visual_exporter::Name );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterCommand( xray_path_cmd::Name );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterCommand( anim_export_cmd::Name );
	CHK_STAT_R			( stat );

	stat = plugin.deregisterCommand( skeleton_export_cmd::Name );
	CHK_STAT_R			( stat );

	//CHK_STAT_R ( plugin.deregisterNode( xray_material_node::id ) );
	//CHK_STAT_R ( plugin.deregisterNode( hwPhongShader::id ) );
	//plugin.deregisterDragAndDropBehavior("hwPhongShaderBehavior");

	//const MString UserClassify( "shader/surface" );
	//MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	//command			+= UserClassify;
	//command			+= "\");}\n";
	//MGlobal::executeCommand( command );

	xray::maya_animation::destroy_world	( core_engine.maya_animation_world );
	xray::animation::destroy_world		( core_engine.animation_world );

	for(int i=0; i<20; ++i)
	{
		xray::resources::dispatch_callbacks		( );
		xray::threading::yield					(50);
	}

	xray::resources::finish_resources_threads	( );
	xray::core::finalize_resources		( );
	xray::core::finalize				( );

	XRAY_DESTROY_REFERENCE				( fs_devices );

	return stat;
}