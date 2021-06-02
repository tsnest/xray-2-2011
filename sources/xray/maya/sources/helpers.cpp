////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "shellapi.h"
#include "maya_engine.h"
#include <xray/math_randoms_generator.h>
#include "NvMender2003/nvMeshMender.h"

using namespace xray;

MStatus get_path_by_name(MString const& name, MDagPath& result, bool bassert )
{
	MStatus				stat;
	MSelectionList		lst;
	stat = MGlobal::getSelectionListByName(name, lst);
	if(lst.length()==1)
	{
		//MObject			parent;
		stat			= lst.getDagPath(0, result);
		CHK_STAT_R		(stat);
		stat			= MStatus(MStatus::kSuccess);
	}else
	{
		if(bassert)
		{
			stat			= MStatus(MStatus::kNotFound);
			MString s		= "Invalid path " + name;
			stat.perror		(s);
			CHK_STAT_R		(stat);
		}else
			stat			= MStatus(MStatus::kNotFound);
	}

	return stat;
}

void size_maya_to_xray( xray::math::float3_pod& pt )
{
	MDistance dst_x	( pt.x );
	MDistance dst_y	( pt.y );
	MDistance dst_z	( pt.z );
	pt.set		( (float)dst_x.asMeters(), (float)dst_y.asMeters(), (float)dst_z.asMeters() );
}

void translation_maya_to_xray(xray::math::float3_pod& pt)
{
	size_maya_to_xray( pt );
	pt.z = -pt.z;
}

void rotation_maya_to_xray(xray::math::float3_pod& pt)
{
	pt.z = -pt.z;
}

int get_local_idx(int const value, MIntArray const& arr, MStatus& status)
{
	status = MStatus::kSuccess;

	for(u32 i=0; i<arr.length(); ++i)
		if(arr[i]==value)
			return i;

	status = MStatus::kFailure;
	return -1;
}

MObject findShader( const MObject& setNode, 
					MFnMesh& fnMesh, 
					MStatus& status ) 
{
	status					= MStatus::kSuccess;

	MFnDependencyNode fnNode( setNode );
	MPlug shaderPlug		= fnNode.findPlug( "surfaceShader" );
			
	if (!shaderPlug.isNull()) 
	{			
		MPlugArray connectedPlugs;

		shaderPlug.connectedTo		( connectedPlugs, true, false, &status );

		if( !status )
		{
			display_warning	("MPlug::connectedTo");
			return					MObject::kNullObj;
		}

		if(1 != connectedPlugs.length()) 
		{
			display_warning("Error getting shader for: " + fnMesh.partialPathName());
		}else 
		{
			status	= MStatus::kSuccess;
			return	connectedPlugs[0].node();
		}
	}
	
	return MObject::kNullObj;
}

MStatus extract_base_texture( MPlug& colorPlug, MString& texture_name )
{
	MStatus	result;
	MItDependencyGraph itDG(colorPlug, 
							MFn::kFileTexture,
							MItDependencyGraph::kUpstream, 
							MItDependencyGraph::kBreadthFirst,
							MItDependencyGraph::kNodeLevel, 
							&result);
	CHK_STAT_R			(result);

	//disable automatic pruning so that we can locate a specific plug 
	itDG.disablePruningOnFilter();

	if(itDG.isDone()) 
		return			(MStatus::kFailure);

	//retrieve the filename and pass it in to output texture information
	MObject textureNode		= itDG.thisNode( &result );
	CHK_STAT_R				( result );
	MPlug filename_plug		= MFnDependencyNode(textureNode).findPlug("fileTextureName", result);
	CHK_STAT_R				( result );
	MString					fileTextureName;
	filename_plug.getValue	( fileTextureName );
	CHK_STAT_R				( result );

	texture_name			= get_relative_path( fileTextureName, "textures", false );

	// cut file extension
	texture_name			= texture_name.toLowerCase();
	int ext					= texture_name.index('.');
	texture_name			= texture_name.substring( 0, ext-1 );

	return					MStatus::kSuccess;
}

MStatus	shader_props(	const MObject& shader_object, 
//						MString& material_name, 
						MString& texture_name,
						bool allow_empty
						)
{
	texture_name			= "";

	MStatus					result;
	MFnDependencyNode		shader_dep_node(shader_object);
	
//	material_name			= shader_dep_node.name();

	MPlug colorPlug			= shader_dep_node.findPlug("color", &result);
	if(result.error())
	{
		MString warning_str = "node ";
		warning_str			+= shader_dep_node.name();
		warning_str			+= " has no connected color plugin, or it is invalid (unknown plugin used)";
		display_warning		( warning_str );
	}
	CHK_STAT_R				(result);

	MString shader_name		= shader_dep_node.typeName( &result );
	CHK_STAT_R				(result);

	MStatus texture_result	= extract_base_texture( colorPlug, texture_name );

	if(texture_result.error() && !allow_empty)
	{
		display_warning	( "shader has no connected FileTexture" );
		return			( MStatus::kFailure );
	}

	return result;
}

MString object_path( MObject const& o )
{
	MDagPath	pth;
	MDagPath::getAPathTo(o, pth );
	MString result = pth.fullPathName();
	return result;
}

MStatus findSkinCluster(MDagPath& dagPath, MObjectArray& skin_cluster_object_array/*, MString& skin_name*/ )
{
	MStatus					result;
	result					= dagPath.extendToShape();
	MObject this_object		= dagPath.node();
	MItDag					itdag;
	itdag.reset		(this_object, MItDag::kDepthFirst, MFn::kMesh );

	while(!itdag.isDone())
	{
		MObject o			= itdag.currentItem();

		MItDependencyGraph	it( o, MFn::kSkinClusterFilter, MItDependencyGraph::kUpstream );

		while(!it.isDone())
		{
			LOG_INFO("found mesh with skincluster: %s", object_path(o).asChar());
			skin_cluster_object_array.append( it.currentItem() );
			//MDagPath				pth;
			//result				= MDagPath::getAPathTo(o, pth );
			//skin_name				= pth.fullPathName();
			//result				= MStatus::kSuccess;
			//return				result;
			it.next( );
		}

		itdag.next( );
	}

	result		= (skin_cluster_object_array.length()!=0) ? MStatus::kSuccess : MStatus::kFailure;
	return		result;
}

MStatus extract_all_meshes_from_path( MDagPath& fnDagPath, process_mesh_delegate callback )
{
	MStatus	result;
	MItDag dagIterator		( MItDag::kDepthFirst, MFn::kInvalid, &result);
	CHK_STAT_R				( result );
	dagIterator.reset		( fnDagPath.node(), MItDag::kDepthFirst, MFn::kInvalid );

	for ( ; !dagIterator.isDone() ; dagIterator.next() )
	{
		
		MDagPath		current;
		result			= dagIterator.getPath( current );
		CHK_STAT_R		( result );

		MFnDagNode dagNode				( current, &result );
		CHK_STAT_R						( result );

		if (dagNode.isIntermediateObject()) 
		{
			continue;
		}else
		if( current.hasFn(MFn::kNurbsSurface) )
		{
			//display_warning ("Warning: skipping Nurbs Surface.");
			continue;
		}else 
		if( current.hasFn(MFn::kMesh) && current.hasFn(MFn::kTransform) )
		{
			// We want only the shape, 
			// not the transform-extended-to-shape.
			continue;
		}else 
		if( current.hasFn(MFn::kMesh) )
		{
			result			= callback( current );
			CHK_STAT_R		(result);
		}else
		if( current.hasFn(MFn::kDagContainer))
		{
//			display_info("found DagContainer " + full_path_name );
		}else
		{
			//display_warning ("Warning: skipping " + full_path_name +" : " + dagNode.object().apiTypeStr());
		}

	}
	
	return result;
}

void fill_mender_input( float3vec const& in_vertices, 
					   u16vec const& in_indices,
					   float3vec const& in_normals,
					   float2vec const& in_uvs,
					   vector< MeshMender::Vertex >& theVerts, u16vec& theIndices )
{
	theVerts.clear();
	theIndices.clear();
	// fill inputs ( verts )
	const u32 vertices_number	=  in_vertices.size();
	theVerts.resize				( vertices_number );

	float3vec::const_iterator itv		= in_vertices.begin();
	float3vec::const_iterator itv_e	= in_vertices.end();

	float3vec::const_iterator itn		= in_normals.begin();
	float2vec::const_iterator ituv	= in_uvs.begin();

	vector< MeshMender::Vertex >::iterator out_verts_it = theVerts.begin();

	for(;itv!=itv_e; ++itv, ++itn, ++ituv, ++out_verts_it)
	{
		MeshMender::Vertex& out_vertex	= *out_verts_it;

		out_vertex.pos		= *itv;
		out_vertex.normal	= *itn;
		out_vertex.s	= (*ituv).x;
		out_vertex.t	= (*ituv).y;
	}

	theIndices			= in_indices;
}

//void fix_mender_output( vector<MeshMender::Vertex>& mender_in_out_verts )
//{
//	typedef map< MeshMender::Vertex*, vector<MeshMender::Vertex*> > vertices_to_process_map;
//	vertices_to_process_map vertices_to_process;
//	
//	for (vector<MeshMender::Vertex>::iterator it_i=mender_in_out_verts.begin(); it_i!=mender_in_out_verts.end(); ++it_i)
//	{
//		vector<MeshMender::Vertex*>& near_vertices = vertices_to_process[&*it_i];
//		
//		for (vector<MeshMender::Vertex>::iterator it_j=mender_in_out_verts.begin(); it_j!=mender_in_out_verts.end(); ++it_j)
//		{
//			if (it_i==it_j)
//				continue;
//			
//			if (math::is_similar(
//					float3(it_i->pos.x, it_i->pos.y, it_i->pos.z), 
//					float3(it_j->pos.x, it_j->pos.y, it_j->pos.z), 
//					math::epsilon_5
//				) && 
//				math::is_similar(
//					float3(it_i->normal.x, it_i->normal.y, it_i->normal.z), 
//					float3(it_j->normal.x, it_j->normal.y, it_j->normal.z), 
//					math::epsilon_3
//				)
//			)
//			{
//				near_vertices.push_back(&*it_j);
//			}
//		}
//	}
//	
//	for (vertices_to_process_map::iterator it=vertices_to_process.begin(); it!=vertices_to_process.end(); ++it)
//	{
//		MeshMender::Vertex* vertex					= it->first;
//		vector<MeshMender::Vertex*>& near_vertices	= it->second;
//		u32 const num_near_vertices					= near_vertices.size();
//		
//		if (!num_near_vertices)
//			continue;
//		
//		float const alpha = 1.0f / (num_near_vertices + 1.0f);
//		
//		for (vector<MeshMender::Vertex*>::iterator it_near=near_vertices.begin(); it_near!=near_vertices.end(); ++it_near)
//		{
//			vertex->binormal = vertex->binormal * (1.0f - alpha) + (*it_near)->binormal * alpha;
//			vertex->tangent  = vertex->tangent  * (1.0f - alpha) + (*it_near)->tangent  * alpha;
//		}
//	}
//}

MStatus optimize_uv( float2vec& in_out_uvs, MArgDatabase const& arglist )
{
	XRAY_UNREFERENCED_PARAMETERS( arglist );
    // Optimize texture coordinates
    // 1. Calc bounds
    xray::math::float2 		Tdelta;
    xray::math::float2 	Tmin, Tmax;
	Tmin.set	(math::float_max, math::float_max);
    Tmax.set	(math::float_min, math::float_min);

	float2vec::iterator it		= in_out_uvs.begin();
	float2vec::iterator it_e	= in_out_uvs.end();

	for(; it!=it_e; ++it)
	{
	  xray::math::float2& uv = *it;
	  Tmin.min				(uv);
	  Tmax.max				(uv);
	}

    Tdelta.x 	= floorf((Tmax.x-Tmin.x)/2+Tmin.x);
    Tdelta.y 	= floorf((Tmax.y-Tmin.y)/2+Tmin.y);

	// 2. Recalc UV mapping
	it = in_out_uvs.begin();
	for(; it!=it_e; ++it)
	{
	  xray::math::float2& uv = *it;
	  uv		-= Tdelta;
	}

	return MStatus::kSuccess;
}

void remove_file_or_directory( MString const& dir_path )
{
	SHFILEOPSTRUCT					data;
	memory::fill8					( &data, sizeof(data), 0, sizeof(data) );
	data.wFunc						= FO_DELETE;

	u32 const string_length			= dir_path.length();
	u32 const buffer_size			= (string_length + 2)*sizeof(char);
	pstr const folder_name_buffer	= static_cast<pstr>( ALLOCA( buffer_size ) );
	pcstr const folder_name			= dir_path.asChar();
	memory::copy					( folder_name_buffer, buffer_size, folder_name, string_length );
	folder_name_buffer[string_length]		= 0;
	folder_name_buffer[string_length + 1]	= 0;
	data.pFrom						= folder_name_buffer;

	data.fFlags						= FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION;
	/*int const operation_result	= */SHFileOperation	( &data );
	//if(operation_result!=0)
	//	LOG_ERROR("SHFileOperation failed with code %x", operation_result);
}

MString get_relative_path( MString const& absolute_path, MString const& first_cut, bool attach_first_cut )
{
	if(absolute_path.length()==0)
		return absolute_path;

	int idx		= absolute_path.indexW( first_cut );
	
	if(idx==-1)
	{// T:\act\act_stalker01.tga format
		int idx1	= absolute_path.indexW( ":/" );
		if(idx1==-1)
			idx1	= absolute_path.indexW( ":\\" );

		if(idx1==-1)
		{// ghm...
//			display_warning("texture file path is Invalid or Relative " + absolute_path);
			return absolute_path;
		}else
			idx1	+= 2; // length of ":/"

		MString res = absolute_path.substring(idx1, absolute_path.length()-1);
		return res;
	}else
	{
		if(!attach_first_cut)
			idx += first_cut.length()+1;// remove last "/"
		
		MString res = absolute_path.substring(idx, absolute_path.length()-1);
		return res;
	}
}


MString get_temp_unique_file_name( MString const& base_file_name )
{
	MString						temp_path;
	temp_path					= xray::maya::g_maya_engine.get_resources_path();
	temp_path					= temp_path.substring( 0, temp_path.rindex('/') );
	temp_path					+= "user_data";

	fixed_string512 unique_name;

	xray::math::random32	uid_random	(GetTickCount());

	u16 const uid_size = 6;
	char file_uid[uid_size];
		for(u16 i=0; i<uid_size; ++i)
			file_uid[i] = 'a' + (char)uid_random.random(u32('z'-'a'));
		file_uid[uid_size] = 0;

	unique_name.assignf("%s/%s_%s", 
						temp_path.asChar(), 
						fs_new::file_name_from_path( fs_new::native_path_string::convert( base_file_name.asChar()) ),
						file_uid
						);
	temp_path = unique_name.c_str();
	return temp_path;
}
