////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "solid_visual_collector.h"

namespace xray{
namespace maya{

MStatus solid_visual_collector::extract_render_static_geometry_from_path( MDagPath& fnDagPath )
{
	return extract_all_meshes_from_path( fnDagPath, process_mesh_delegate(this, &solid_visual_collector::extract_render_static_geometry) );
}

MStatus solid_visual_collector::extract_collision_geometry_from_path( MDagPath& fnDagPath )
{
	m_collision_from_geometry			= false;
	m_collision_surface.clear			( );
	return extract_all_meshes_from_path	( fnDagPath, process_mesh_delegate(this, &solid_visual_collector::extract_collision_geometry) );
}

MStatus solid_visual_collector::extract_collision_geometry( MDagPath& fnMeshDagPath )
{
	MMatrix	full_transform;
	full_transform.setToProduct	( fnMeshDagPath.inclusiveMatrix(), m_locator_matrix_inv );

	MFnMesh fnMesh				( fnMeshDagPath );

	MObjectArray				PolygonSets;
	MObjectArray				PolygonComponents;

	MStatus						result;

	MIntArray					triangles;
	MIntArray					triangle_vertices;

	result						= fnMesh.getTriangles( triangles, triangle_vertices );
	CHK_STAT_R					(result);

	const float* raw_points		= fnMesh.getRawPoints( &result );
	CHK_STAT_R					(result);
	
    //Make sure the set is a polygonal set.  If not, continue.
	MItMeshPolygon itMeshPolygon	(fnMeshDagPath, MObject::kNullObj, &result);
	CHK_STAT_R						(result);
	
	u32 point_struct_size		= 3 ; // raw_points pointer float[3]

	for (itMeshPolygon.reset(); !itMeshPolygon.isDone(); itMeshPolygon.next())
	{
		bool const b_has_valid_triangulation = itMeshPolygon.hasValidTriangulation();
		ASSERT_U				(b_has_valid_triangulation);

		u32 face_idx			= itMeshPolygon.index();

		MIntArray				face_vertex_list;
		result					= itMeshPolygon.getVertices( face_vertex_list );
		CHK_STAT_R				(result);

		u32 tri_count			= triangles[face_idx];
		u32 tri_idx				= 0;
		bool doublesided		= false;

		for(tri_idx=0; tri_idx<tri_count; ++tri_idx)
		{
			int vertexIndicies[3] = {-1};
			result				= fnMesh.getPolygonTriangleVertices(face_idx, tri_idx, vertexIndicies);
			CHK_STAT_R			(result);
			

			static int vert_remap_021[] = {0,2,1};
			static int vert_remap_012[] = {0,1,2};

			int const* vert_remap			= vert_remap_021;
			for(int d=0; d<2; ++d)
			{
				if(d==1)
				{
					if(!doublesided)
						break;
					vert_remap			= vert_remap_012;
				}

				for(int v_c=0; v_c<3; ++v_c)
				{
					int vc					= vert_remap[v_c];
					int vindex				= vertexIndicies[vc];

					int offset			= point_struct_size * vindex;
					MVector pos		(	*(raw_points + offset + 0), 
										*(raw_points + offset + 1), 
										*(raw_points + offset + 2) );
					
					MVector p_transformed		= pos * full_transform;
					float3	p ((float)p_transformed.x, (float)p_transformed.y, (float)p_transformed.z );
					p.x		+= (float)full_transform[3][0];
					p.y		+= (float)full_transform[3][1];
					p.z		+= (float)full_transform[3][2];
					translation_maya_to_xray			( p );

					m_collision_surface.add_vertex	( p );
				}
			}
		} // all triangles in polygon
	} // all polygons
		
	return MStatus::kSuccess;
}



MStatus solid_visual_collector::extract_render_static_geometry( MDagPath& fnMeshDagPath )
{
	MMatrix	full_transform;
	full_transform.setToProduct	( fnMeshDagPath.inclusiveMatrix(), m_locator_matrix_inv );

	MStatus						result;
	MFnMesh fnMesh				( fnMeshDagPath, &result );
	CHK_STAT_R					( result );

	MString						CurrentUVSetName;
	
	bool						status_failed = false;

	MSpace::Space current_space	= MSpace::kWorld;

	MIntArray					triangles;
	MIntArray					triangle_vertices;

	fnMesh.getCurrentUVSetName( CurrentUVSetName );
	CHK_STAT_R					(result);

	result						= fnMesh.getTriangles( triangles, triangle_vertices );
	CHK_STAT_R					(result);

	const float* raw_points		= fnMesh.getRawPoints( &result );
	CHK_STAT_R					(result);
	
	fnMeshDagPath.extendToShape( );

	int instanceNum				= fnMeshDagPath.isInstanced() ? fnMeshDagPath.instanceNumber() : 0;

	MObjectArray				shaders_array;
	MIntArray					shaders_indices;
	fnMesh.getConnectedShaders	( instanceNum, shaders_array, shaders_indices );
	int scount					= shaders_array.length();
	if(scount==0)
		return MStatus::kSuccess;

	MIntArray surfaces_indices_all(scount);

	for(int si=0; si<scount; ++si)
	{	
		MFnDependencyNode sh_dep_node(shaders_array[si]);
		MString sg_name				= sh_dep_node.name();

		u32 surf_idx				= 0;
		MStatus found				= find_surface( fnMeshDagPath, sg_name, surf_idx, false );
		
		if(found==MStatus::kSuccess)
		{
			surfaces_indices_all[si]	= surf_idx;
		}else
		{
			render_static_surface*	s	= NEW( render_static_surface )( m_max_verts_count );
			s->m_sg_name				= sg_name;
			s->m_mesh_dag_path			= fnMeshDagPath;
			m_render_static_surfaces.push_back( s );

	// parse material
			MObject shaderNode			= findShader( sh_dep_node.object(), fnMesh, result );
			CHK_STAT_R					(result);
			MString sh_name, tex_name;
			result						= shader_props( shaderNode, 
														s->m_texture_name, 
														true
														);
			CHK_STAT_R					(result);

			surfaces_indices_all[si]	= m_render_static_surfaces.size()-1;
		}
	}//for(int si=0; si<scount; ++si)

// fill object surfaces
	MObject				tmp_comp;
	MItMeshPolygon		it_mesh_poly( fnMeshDagPath, tmp_comp, &result );
	CHK_STAT_R			( result );

	u32vec error_faces;
	u32vec small_faces;
	for ( ; !it_mesh_poly.isDone(); it_mesh_poly.next())
	{
		u32 face_idx					= it_mesh_poly.index( &result );
		CHK_STAT_R						( result );
		R_ASSERT						( face_idx<shaders_indices.length() );

		int material_idx_sh				= shaders_indices[face_idx];
		if(material_idx_sh==-1)
		{
			display_warning				( "face has no material assigned" );
			MString command				= "select -add " + fnMeshDagPath.partialPathName() + ".f[" + face_idx + "]";
			display_warning				( command );
			MGlobal::executeCommand		( command );
			status_failed				= true;
			continue;
		}
		
		int material_idx				= surfaces_indices_all[material_idx_sh];

		render_static_surface* current_surface	= m_render_static_surfaces[material_idx];
		int surface_tri_count					= 0;


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
				if(current_surface->m_full_)
				{
					render_static_surface* s	= NEW( render_static_surface )( m_max_verts_count );
					s->m_sg_name				= current_surface->m_sg_name;
					s->m_mesh_dag_path			= current_surface->m_mesh_dag_path;
					m_render_static_surfaces.push_back		( s );
					surfaces_indices_all[material_idx_sh] = m_render_static_surfaces.size()-1;
					current_surface				= s;
				}

				int vertexIndicies[3] = {-1};
				result				= fnMesh.getPolygonTriangleVertices(face_idx, tri_idx, vertexIndicies);
				CHK_STAT_R			(result);
				

				static int vert_remap_021[] = {0,2,1};
				static int vert_remap_012[] = {0,1,2};

				int const* vert_remap			= vert_remap_021;

				float2 triangle_uv[3];
				float3 triangle_pos[3];
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
					float3 norm		( v.x, v.y, -v.z );
					if(norm.is_similar(float3(0,0,0)))
					{
						display_warning				( "------- face has vertex with ZERO normal" );
						MString command				= "select -r " + fnMeshDagPath.partialPathName() + ".f[" + face_idx + "]";
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
						display_warning				( "------- face has vertex without UVset assigned" );
						MString command				= "select -r " + fnMeshDagPath.partialPathName() + ".f[" + face_idx + "]";
						display_warning				( command );
						MGlobal::executeCommand		( command );
						return						MStatus::kFailure;
					}
					
					MVector p_transformed		= pos * full_transform;
					triangle_uv[v_c]			= float2(uv[0], 1.0f - uv[1]);
					
					float3	p ((float)p_transformed.x, (float)p_transformed.y, (float)p_transformed.z );
					p.x		+= (float)full_transform[3][0];
					p.y		+= (float)full_transform[3][1];
					p.z		+= (float)full_transform[3][2];
					translation_maya_to_xray			( p );
					
					triangle_pos[v_c] = p;

					current_surface->add_vertex		( p, norm, triangle_uv[v_c] );
				}//for(int v_c=0; v_c<3; ++v_c)

				if(	triangle_uv[0].is_similar(triangle_uv[1]) || triangle_uv[0].is_similar(triangle_uv[2]) )
				{
					float2 offset(0.1f, 0.1f);
					triangle_uv[1] = triangle_uv[0] + offset;
					triangle_uv[2] = triangle_uv[1] + offset;
					if(std::find(error_faces.begin(), error_faces.end(), face_idx)==error_faces.end() )
						error_faces.push_back(face_idx);
				}

				float l0 = (triangle_pos[0]-triangle_pos[1]).length();
				float l1 = (triangle_pos[1]-triangle_pos[2]).length();
				float l2 = (triangle_pos[2]-triangle_pos[0]).length();
				if( l0<0.003f || l1<0.003f || l2<0.003f )
				{
					if(std::find(small_faces.begin(), small_faces.end(), face_idx)==small_faces.end() )
						small_faces.push_back(face_idx);
				}
			} // for(tri_idx=0; tri_idx<tri_count; ++tri_idx)
	} // for ( ; !it_mesh_poly.isDone(); it_mesh_poly.next())

	if(!small_faces.empty())
	{
		display_warning( "------- Face has triangle with edge length less than 3mm" );
		if(!is_batch_mode())
			for(u32 ii =0; ii<small_faces.size(); ++ii)
			{
				MString command = "select -add " + fnMeshDagPath.partialPathName() + ".f[" + small_faces[ii] + "]";
				MGlobal::executeCommand( command );
				display_warning( command );
			}
	}

	if(!error_faces.empty())
	{
		display_warning("------ Incorrect UV mapped faces for shape " + fnMeshDagPath.partialPathName());
		if(!is_batch_mode())
			for(u32 ii =0; ii<error_faces.size(); ++ii)
			{
				MString command = "select -add " + fnMeshDagPath.partialPathName() + ".f[" + error_faces[ii] + "]";
				MGlobal::executeCommand( command );
				display_warning( command );
			}
		
		//. TEMPreturn MStatus::kFailure;
	}

	return (status_failed) ? MStatus::kFailure : MStatus::kSuccess;
}

MStatus solid_visual_collector::build_collision_geometry_by_render( )
{
	m_collision_from_geometry			= true;
	
	m_collision_surface.clear			( );
	
	render_static_surfaces_cit it		= m_render_static_surfaces.begin();
	render_static_surfaces_cit it_e		= m_render_static_surfaces.end();
	for(; it!=it_e; ++it)
	{
		render_static_surface* s		= *it;
		u32 count						= s->m_indices.size();
		
		for(u32 i=0; i<count; ++i)
		{
			u16 idx			= s->m_indices[i];
			float3 const v	= s->m_vertices[ idx ];
			m_collision_surface.add_vertex( v );
		}
	}

	return MStatus::kSuccess;
}

} //namespace maya
} //namespace xray
