////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skin_visual_writer.h"
#include <xray/animation/skeleton.h>
#include "export_defines.h"
#include <xray/resources_fs.h>


namespace xray {
namespace maya {

skin_visual_collector::skin_visual_collector( MArgDatabase const& arglist, MStatus& status )
:m_arglist(arglist)
{
	status = MStatus::kSuccess;
}

skin_visual_collector::~skin_visual_collector()
{
	clear			( );
}

void skin_visual_collector::clear()
{
	bones_vec			m_bones;
	MString				m_skin_name;
	MObject				m_skin_cluster_object;

	render_skinned_surfaces::iterator it		= m_render_skinned_surfaces.begin();
	render_skinned_surfaces::iterator it_e		= m_render_skinned_surfaces.end();
	for( ;it!=it_e; ++it)
	{
		render_skinned_surface* s	= *it;
		DELETE						( s );
	}
	m_render_skinned_surfaces.clear		( );
	m_bones.clear						( );
	m_skin_cluster_object				= MObject::kNullObj;
	m_locators.clear					( );
}


bool remap_result;

MStatus skin_visual_collector::export_skin_visual( MString const& skin_name )
{
	MStatus					status;

	MDagPath dag_path;
	status					= get_path_by_name	( skin_name, dag_path, true );
	CHK_STAT_R				( status );
	MObject					skin_cluster_object;
	status					= findSkinCluster	( dag_path, m_skin_cluster_objects );
	CHK_STAT_R				( status );
	
	status					= extract_bone_map	( );
	CHK_STAT_R				( status );

	status					= goto_bind_pose	( );
	CHK_STAT_R				( status );

	status					= extract_all_meshes_from_path( dag_path, process_mesh_delegate( this, &skin_visual_collector::extract_render_skinned_geometry) );
	if( status.error() )
	{
		display_error	( "Invalid geometry detected in " + skin_name );
		display_error	( "Export FAILED!\n" );
		return			MStatus::kFailure;
	}
	CHK_STAT_R				( status );
	

	render_skinned_surfaces::iterator it		= m_render_skinned_surfaces.begin();
	render_skinned_surfaces::iterator it_e		= m_render_skinned_surfaces.end();
	
	for( ;it!=it_e; ++it)
	{
		render_skinned_surface* s	= *it;
		status = s->prepare			( m_arglist );
		CHK_STAT_R					( status );
	}

	MString		skeleton_name;
	status		= m_arglist.getFlagArgument( skeleton_name_flag, 0, skeleton_name );
	CHK_STAT_R					( status );

	fs_new::virtual_path_string		skeleton_name_virtual_path;
	fs_new::native_path_string		skeleton_name_physical_path	=	fs_new::native_path_string::convert(skeleton_name.asChar()).make_lowercase();
	resources::convert_physical_to_virtual_path	( 
		&skeleton_name_virtual_path, 
		skeleton_name_physical_path, 
		resources::exported_mount );

	m_skeleton_name								 = skeleton_name_virtual_path.c_str();
	remap_result = true;

	resources::dispatch_callbacks();
	resources::dispatch_callbacks();
	resources::dispatch_callbacks();

	resources::query_resource_and_wait(
		skeleton_name_virtual_path.c_str(),
		resources::skeleton_class,
		boost::bind( &skin_visual_collector::on_skeleton_loaded, this, _1 ),
		&maya::g_allocator
		);

	if( !remap_result )
		status = MStatus::kFailure;

	return					status;
}

void skin_visual_collector::on_skeleton_loaded( resources::queries_result& data )
{
	if(!data.is_successful())
	{
		display_warning("Error while skeleton loading");
		remap_result = false;
		return;
	}

	m_skeleton			= static_cast_checked<animation::skeleton*>( data[0].get_unmanaged_resource().c_ptr() );

	vector<u16>					bones_remap;
	u32 count					= m_bones.size();
	bones_remap.resize			( count );

	// indices[i] == new index for bone {i}
	for(u16 i=0; i<count; ++i)
	{
		pcstr bone_name		= m_bones[i].m_name.asChar();
		u16 bid				= (u16)m_skeleton->get_bone_index(bone_name);
		if( bid >= ( count + 1 ) )
		{
			display_warning("skeleton is incompatible with this skin!!!");
			remap_result = false;
			return;
		}
		bones_remap[i]		= bid;
		bones_remap[i]		-= 1; // [0]= always " auto root bone", unwanted for skinning procedure
	}

	MStatus stat		= remap_bones( &bones_remap[0] );
	remap_result		= !stat.error();
}

MStatus skin_visual_collector::goto_bind_pose( )
{
	MStatus result;
	bones_vec::const_iterator it		= m_bones.begin();
	bones_vec::const_iterator it_e		= m_bones.end();

	for( ;it!=it_e; ++it)
	{
		sk_bone const& bone		= *it;

		MFnIkJoint joint		( bone.m_path, &result );
		CHK_STAT_R				( result );

		MString cmd				= "dagPose -r -g -bp " + joint.fullPathName();
		MGlobal::executeCommand	(cmd, result );
	}

	return					result;
}

} //namespace maya
} //namespace xray
