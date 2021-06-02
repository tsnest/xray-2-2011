////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

#include <xray/animation/anim_track.h>

MStatus get_path_by_name				(MString const& name, MDagPath& result, bool bassert );
MObject			get_node_by_name		( MString const& name, MStatus& stat );

void			translation_maya_to_xray	( xray::math::float3_pod& pt );
void			rotation_maya_to_xray		( xray::math::float3_pod& pt );
void			size_maya_to_xray			( xray::math::float3_pod& pt );

int				get_local_idx			( int const value, MIntArray const& arr, MStatus& result );
MStatus			lock_attribute			( const MFnDependencyNode& depNode, MStringArray attributes );
MStatus			findSkinCluster			( MDagPath& dagPath, MObjectArray& skin_cluster_object_array );

MObject			findShader				(	const MObject& setNode, 
											MFnMesh& fnMesh, 
											MStatus& result ) ;
MStatus			shader_props			(	const MObject& shaderNode, 
											MString& texture_name, 
											bool allow_empty
											) ;

void			get_joint_global_frame	( MFnIkJoint const &jnt, xray::animation::frame &frm );

typedef fastdelegate::FastDelegate< MStatus ( MDagPath& ) >		process_mesh_delegate;

MStatus			extract_all_meshes_from_path	( MDagPath& fnDagPath, process_mesh_delegate callback );
//void			fix_mender_output				( vector<MeshMender::Vertex>& mender_in_out_verts );

MStatus			optimize_uv						( float2vec& in_out_uvs, MArgDatabase const& arglist );
void			remove_file_or_directory		( MString const& path );

MString			get_relative_path				( MString const& absolute_path, MString const& first_cut, bool attach_first_cut );
MString			get_temp_unique_file_name		( MString const& base_file_name );

inline MStatus make_input_att(MFnAttribute& attr )
{
	CHK_STAT_R( attr.setKeyable(true) );
	CHK_STAT_R( attr.setStorable(true) );
	CHK_STAT_R( attr.setReadable(true) );
	return attr.setWritable(true);
}

inline MStatus make_oupput_att(MFnAttribute& attr )
{
	CHK_STAT_R( attr.setKeyable(false) );
	CHK_STAT_R( attr.setStorable(false) );
	CHK_STAT_R( attr.setReadable(true) );
	return attr.setWritable(false);
}

#endif // #ifndef HELPERS_H_INCLUDED