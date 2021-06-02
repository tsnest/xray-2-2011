////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skin_visual_writer.h"

namespace xray{
namespace maya{

MStatus find_shape( MObjectArray const& skin_clusters, 
					MDagPath const& fnMeshDagPath, 
					MObject& o, 
					MFnSkinCluster& current_skin_cluster )
{
	MStatus						result;

	for(u32 i=0; i<skin_clusters.length(); ++i)
	{
		MFnSkinCluster skin_cluster	( skin_clusters[i], &result );
		CHK_STAT_R					( result );

		u32 shape_idx					= skin_cluster.indexForOutputShape( fnMeshDagPath.node() );
		if(shape_idx==u32(-1))
			continue;
		
		current_skin_cluster.setObject( skin_clusters[i] );
		o						= skin_cluster.inputShapeAtIndex( shape_idx, &result );
		return result;
	}
	return MStatus::kFailure;
}

MStatus skin_visual_collector::extract_render_skinned_geometry( MDagPath& fnMeshDagPath )
{
	MStatus						result;

	MMatrix	full_transform		= fnMeshDagPath.inclusiveMatrix();
	
	MFnMesh fnMesh				( fnMeshDagPath, &result );
	CHK_STAT_R					( result );

	MString						CurrentUVSetName;
	MSpace::Space current_space	= MSpace::kWorld;

	MIntArray					triangles;
	MIntArray					triangle_vertices;

	fnMesh.getCurrentUVSetName	( CurrentUVSetName );
	CHK_STAT_R					( result );

	result						= fnMesh.getTriangles( triangles, triangle_vertices );
	CHK_STAT_R					( result );

	const float* raw_points		= fnMesh.getRawPoints( &result );
	CHK_STAT_R					( result );

	fnMeshDagPath.extendToShape( );

	int instanceNum				= fnMeshDagPath.isInstanced() ? fnMeshDagPath.instanceNumber() : 0;

	MObjectArray				shaders_array;
	MIntArray					shaders_indices;
	fnMesh.getConnectedShaders	( instanceNum, shaders_array, shaders_indices );
	int scount					= shaders_array.length();
	
	if(scount==0)
		return MStatus::kSuccess;

	MFnSkinCluster current_skin_cluster;
	MObject		maya_input_object;
	result							= find_shape(	m_skin_cluster_objects, 
													fnMeshDagPath, 
													maya_input_object,
													current_skin_cluster );
	MString current = fnMeshDagPath.fullPathName();
	if( result.error() )
	{
		LOG_ERROR	("Cannot find skincluster, attached to mesh %s", fnMeshDagPath.fullPathName().asChar());
		return		result;
	}
	{
		u32 mesh_vertices_to_test		= fnMesh.numVertices();
		u32 pointCounter_to_test		= 0;
		MItGeometry geometryIt_test		( maya_input_object, &result );
		for (; !geometryIt_test.isDone(); geometryIt_test.next(), ++pointCounter_to_test);
		if(mesh_vertices_to_test!=pointCounter_to_test)
		{
			LOG_ERROR("Mesh skin cluster is old or invalid, mesh vCount=%d, skin vCount=%d",mesh_vertices_to_test,pointCounter_to_test);
			return MStatus::kFailure;
		}
	}


	u32 pointCounter				= 0;
	MItGeometry geometryIt			( maya_input_object, &result );
	CHK_STAT_R						( result );

	mesh_influences*	mi			= NEW(mesh_influences)();
	mi->m_shape_dag_path_			= fnMeshDagPath;
	mi->m_shape_string_path			= fnMeshDagPath.fullPathName();
	m_mesh_influences_vec.push_back	( mi );

	mi->m_all_influences.resize		( fnMesh.numVertices() );


	u16vec							error_vertices;
	for (; !geometryIt.isDone(); geometryIt.next(), ++pointCounter)
	{
		MObject mayaComponent		= geometryIt.component();
		MFloatArray					mayaWeightArray;
		u32							numInfluences; // is going to be discarded

		result = current_skin_cluster.getWeights(	fnMeshDagPath,
													mayaComponent,
													mayaWeightArray,
													numInfluences);

		CHK_STAT_R					( result );

		
		bone_influences& v_infl		= mi->m_all_influences[pointCounter];
		
		u16 cnt = 0;
		for (u16 j = 0; j < mayaWeightArray.length(); ++j)
		{
			// I know: we are working with floats, but Maya has a
			// function to eliminate weights, that are too small
			if (mayaWeightArray[j] != 0)
			{
				if(cnt < max_influence)
				{
					bone_influence& b_infl	= v_infl.influence_arr[cnt];
					b_infl.bone_index		= j;
					b_infl.weight			= mayaWeightArray[j];
					++cnt;
				}else
				{
					error_vertices.push_back( (u16)pointCounter );
				}
			}
		}
		v_infl.normalize				( cnt );
	}

	if(!error_vertices.empty())
	{
		display_warning("Incorrect vertices for shape "+ fnMeshDagPath.partialPathName());
		for(u32 ii =0; ii<error_vertices.size(); ++ii)
		{
			MString command = "select -add " + fnMeshDagPath.partialPathName() + ".vtx[" + error_vertices[ii] + "]";
			MGlobal::executeCommand( command );
			display_warning( command );
		}
	}

	MIntArray surfaces_indices_all(scount);
	for(int si=0; si<scount; ++si)
	{	
		MFnDependencyNode sh_dep_node	(shaders_array[si]);
		MString sg_name					= sh_dep_node.name();

		u32 surf_idx					= 0;
		MStatus found					= find_surface( fnMeshDagPath, sg_name, surf_idx, true );

		if(found==MStatus::kSuccess)
		{
			surfaces_indices_all[si]	= surf_idx;
		}else
		{
			render_skinned_surface*	s	= NEW( render_skinned_surface )();
			s->m_sg_name				= sg_name;
			s->m_mesh_dag_path			= fnMeshDagPath;
			s->m_mesh_full_name			= fnMeshDagPath.fullPathName();
			s->m_influences_			= mi;

			u32 surf_idx_nopath				= 0;
			MStatus found_no_path			= find_surface( fnMeshDagPath, sg_name, surf_idx_nopath, false );
			
			if(found_no_path==MStatus::kSuccess)
				s->increment_add_idx();

			m_render_skinned_surfaces.push_back( s );


			// parse material
			MObject shaderNode			= findShader( sh_dep_node.object(), fnMesh, result );
			CHK_STAT_R					(result);
			MString sh_name, tex_name;
			result						= shader_props( shaderNode, 
														s->m_texture_name, 
														true
														);
			CHK_STAT_R					(result);
			surfaces_indices_all[si]	= m_render_skinned_surfaces.size()-1;
		}
	}// for(int si=0; si<scount; ++si)

// fill object surfaces
	MObject				tmp_comp;
	MItMeshPolygon		it_mesh_poly( fnMeshDagPath, tmp_comp, &result );
	CHK_STAT_R			( result );

	u32vec error_faces;
	for ( ; !it_mesh_poly.isDone(); it_mesh_poly.next())
	{
		u32 face_idx					= it_mesh_poly.index( &result );
		CHK_STAT_R						( result );
		R_ASSERT						( face_idx<shaders_indices.length() );

		u32 material_idx				= shaders_indices[face_idx];
		
		material_idx					= surfaces_indices_all[material_idx];

		render_skinned_surface* current_surface	= m_render_skinned_surfaces[material_idx];
		int surface_tri_count			= 0;


		u32 point_struct_size		= 3 ; // raw_points pointer float[3]

		bool const b_has_valid_triangulation = it_mesh_poly.hasValidTriangulation();
		if(!b_has_valid_triangulation)
			continue;

		MIntArray				face_vertex_list;
		result					= it_mesh_poly.getVertices( face_vertex_list );
		CHK_STAT_R				(result);

		MFloatVectorArray		face_normals;
		result					= fnMesh.getFaceVertexNormals(	face_idx, 
																face_normals, 
																current_space );
		u32 tri_count			= triangles[face_idx];
		surface_tri_count		+= tri_count;
		u32 tri_idx				= 0;

			for(tri_idx=0; tri_idx<tri_count; ++tri_idx)
			{
				int vertexIndicies[3] = {-1};
				result				= fnMesh.getPolygonTriangleVertices(face_idx, tri_idx, vertexIndicies);
				CHK_STAT_R			(result);
				

				static int vert_remap_021[] = {0,2,1};
				static int vert_remap_012[] = {0,1,2};

				int const* vert_remap			= vert_remap_021;
				float normal_mul		= 1.0f;
				
				{
					float2 triangle_uv[3];
					for(int v_c=0; v_c<3; ++v_c)
					{
						int vc					= vert_remap[v_c];
						int vindex				= vertexIndicies[vc];

						int local_vertex_index	= get_local_idx( vindex, face_vertex_list, result );
						CHK_STAT_R				(result);


						int offset			= point_struct_size * vindex;
						MVector pos		(	*(raw_points + offset + 0), 
											*(raw_points + offset + 1), 
											*(raw_points + offset + 2) );

						MFloatVector	v = face_normals[local_vertex_index];
						float3 norm		( v.x*normal_mul, v.y*normal_mul, -v.z*normal_mul );

						if(norm.is_similar(float3(0,0,0)))
						{
							display_warning				( "face has vertex with ZERO normal" );
							MString command				= "select -add " + fnMeshDagPath.partialPathName() + ".f[" + face_idx + "]";
							display_warning				( command );
							MGlobal::executeCommand		( command );
							return						MStatus::kFailure;
						}

					norm.normalize	( );

						float			uv[2];
						result			= it_mesh_poly.getUV(	local_vertex_index, 
																uv,
																&CurrentUVSetName );

						if(result.error())
						{
							display_warning				( "face has vertex without UVset assigned" );
							MString command				= "select -add " + fnMeshDagPath.partialPathName() + ".f[" + face_idx + "]";
							display_warning				( command );
							MGlobal::executeCommand		( command );
							return						MStatus::kFailure;
						}

						triangle_uv[v_c] = float2(uv[0], 1.0f - uv[1]);

						MVector p_transformed			= pos * full_transform;
						float3	p ((float)p_transformed.x, (float)p_transformed.y, (float)p_transformed.z );
						p.x		+= (float)full_transform[3][0];
						p.y		+= (float)full_transform[3][1];
						p.z		+= (float)full_transform[3][2];
						translation_maya_to_xray			( p );
						current_surface->add_vertex			( p, norm, triangle_uv[v_c], (u16)vindex );

					}//for(int v_c=0; v_c<3; ++v_c)

					if(	triangle_uv[0].is_similar(triangle_uv[1]) || triangle_uv[0].is_similar(triangle_uv[2]) )
					{
						float2 offset(0.1f, 0.1f);
						triangle_uv[1] = triangle_uv[0] + offset;
						triangle_uv[2] = triangle_uv[1] + offset;
						error_faces.push_back(face_idx);
						error_faces.push_back(face_idx);
					}
				}//(int d=0; d<2; ++d)
			} // for(tri_idx=0; tri_idx<tri_count; ++tri_idx)
	} // for ( ; !it_mesh_poly.isDone(); it_mesh_poly.next())

	if(!error_faces.empty())
	{
		display_warning("------ Incorrect UV mapped faces for shape "+ fnMeshDagPath.partialPathName());
		if(!is_batch_mode())
			for(u32 ii =0; ii<error_faces.size(); ++ii)
			{
				MString command = "select -add " + fnMeshDagPath.partialPathName() + ".f[" + error_faces[ii] + "]";
				MGlobal::executeCommand( command );
				display_warning( command );
			}
		
		//return MStatus::kFailure;
	}
	return MStatus::kSuccess;
}

} //namespace maya
} //namespace xray
