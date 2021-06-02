////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skin_visual_writer.h"

namespace xray{
namespace maya{

MStatus skin_visual_collector::remap_bones( u16 const* indices )
{
		// indices[i] == new index for bone {i}
	bones_vec tmp_bones			= m_bones;
	u32 count					= tmp_bones.size();
	
	for( u16 i=0; i<count; ++i )
	{
		m_bones[indices[i]]		= tmp_bones[i];
	}

	for( u32 j=0; j<m_mesh_influences_vec.size(); ++j)
	{
		mesh_influences*	mi	= m_mesh_influences_vec[j];
		// indices[i] == new index for bone {i}
		u32 count = mi->m_all_influences.size();
		for(u32 i=0; i<count; ++i)
		{
			bone_influences& bi = mi->m_all_influences[i];
			for(u32 bi_idx=0; bi_idx<max_influence; ++bi_idx)
			{
				bone_influence& b	= bi.influence_arr[bi_idx];
				
				if(b.bone_index !=u16(-1))
					b.bone_index	= indices[b.bone_index];
			}
		}
	}

	return MStatus::kSuccess;
}

MStatus skin_visual_collector::extract_bone_map( )
{
	MStatus						result;
	m_bones.clear				( );

	for(u32 i=0; i<m_skin_cluster_objects.length(); ++i)
	{

		MFnSkinCluster	skin_cluster( m_skin_cluster_objects[i], &result );
		CHK_STAT_R					( result );

		MDagPathArray				infs_joints;
		u32 num_joints				= skin_cluster.influenceObjects( infs_joints, &result );
		CHK_STAT_R					( result );

		for( u32 idx =0; idx<num_joints; ++idx)
		{
			MDagPath	joint_path	= infs_joints[idx];
			MFnIkJoint	joint		( joint_path, &result );
			CHK_STAT_R				( result );
			bool found = false;
			for(u32 j=0; j<m_bones.size(); ++j)
				if(m_bones[j].m_name==joint.name())
				{
					found = true;
					break;
				}
			if(found)
				continue;

			sk_bone					bone;

			bone.m_path				= joint_path;
			bone.m_name				= joint.name();
			
			// set to bind pose
			MString cmd				= "dagPose -r -g -bp " + joint.fullPathName();
			MGlobal::executeCommand	(cmd, result );

			animation::frame		frm;
			get_joint_global_frame	( joint, frm );

			bone.m_bind_translation	= frm.translation;
			bone.m_bind_rotation	= frm.rotation;
			bone.m_bind_scale		= frm.scale;

			m_bones.push_back		( bone );
		}
	}

	return MStatus::kSuccess;
}

} //namespace maya
} //namespace xray
