////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "model_collector.h"
#include "surface.h"

namespace xray{
namespace maya{

MStatus model_collector::find_surface( MDagPath const& fnMeshDagPath, 
										MString const& sg_name, 
										u32& surf_idx, 
										bool use_dag_path )
{
	u32 count = get_surfaces_count();
	
	for( surf_idx=0; surf_idx<count; ++surf_idx )
	{
		surface const* s	= get_surface(surf_idx);
		bool res = true;

		res = (!s->m_full_);

		if(!res)
			continue;


		if(use_dag_path)
			res = (s->m_mesh_dag_path==fnMeshDagPath);

		if(!res)
			continue;

		res = (s->m_sg_name == sg_name );

		if(res)
			return MStatus::kSuccess;
	}
	return MStatus::kFailure;
}

MStatus model_collector::write_locators( configs::lua_config_value& value )
{
	for(u32 locator_idx = 0; locator_idx<m_locators.size(); ++locator_idx)
	{
		configs::lua_config_value v = value[locator_idx];
		v["name"]		= m_locators[locator_idx].name.asChar();
		v["bone_name"]	= m_locators[locator_idx].bone_name.asChar();
		v["bone_idx"]	= m_locators[locator_idx].bone_idx;
		v["position"]	= m_locators[locator_idx].position;
		v["rotation"]	= m_locators[locator_idx].rotation;
	}

	return MStatus::kSuccess;
}


MStatus	model_collector::extract_locators( MString const& locators_root_path, MMatrix const& ext_locator_matrix )
{
	MStatus result;
	MDagPath				locators_root_dag_path;
	result					= get_path_by_name	( locators_root_path, locators_root_dag_path, false );
	
	if(result.error())// not found
		return MStatus::kSuccess;

	MGlobal::executeCommand("doEnableNodeItems true all");

	u32 child_count			= locators_root_dag_path.childCount();
	for(u32 i=0; i<child_count; ++i)
	{
		MObject 	child		= locators_root_dag_path.child( i, &result );
		pcstr type_name			= child.apiTypeStr();
		MFnDagNode	child_node	( child, &result );
		MDagPath				locator_path;
		result					= child_node.getPath( locator_path );

		LOG_INFO("%d is [%s] name is[%s]", i, type_name, locator_path.fullPathName().asChar() );
		locator_path.extendToShape();
		if( locator_path.hasFn(MFn::kLocator) )
		{
			LOG_INFO("%s is LOCATOR", locator_path.fullPathName().asChar() );

			// find constraints, attached to this locator
			result						= child_node.getPath( locator_path );
			u32 locator_childs_count	= locator_path.childCount(&result);

			locator_item	item;
			item.bone_idx	= -1;
			MMatrix			locator_matrix = ext_locator_matrix;

			for(u32 j=0; j<locator_childs_count; ++j)
			{
				MObject 	constraint		= locator_path.child( j, &result );
				MFnDagNode	constraint_node	( constraint, &result );
				MDagPath	constraint_path;
				result						= constraint_node.getPath( constraint_path );


				if(constraint_path.hasFn(MFn::kParentConstraint))
				{
					LOG_INFO("%s is CONSTRAINT", constraint_path.fullPathName().asChar() );
					MStringArray				joint_names;
					MString command_str			= "parentConstraint -q -tl ";
					command_str					+= constraint_path.fullPathName();
					MGlobal::executeCommand		( command_str, joint_names );
					u32 cnt						= joint_names.length();
					MString joint_name			= joint_names[0];
					LOG_INFO("target joint is %s", joint_name.asChar() );
					MDagPath					joint_dag_path;
					result						= get_path_by_name( joint_name, joint_dag_path, true );

					LOG_INFO("target joint PATH is %s", joint_dag_path.fullPathName().asChar() );
					locator_matrix				= joint_dag_path.inclusiveMatrix( &result );
					item.bone_name				= joint_name;
			
					break;
				}
			}

			MMatrix locator_matrix_inv = locator_matrix.inverse();

			MMatrix	offset;
			MMatrix l = locator_path.inclusiveMatrix();
			offset.setToProduct			( l, locator_matrix_inv );

//.			item.name					= locator_path.partialPathName();

			MFnDagNode n(locator_path);
			item.name					= n.name();
			MTransformationMatrix		m(offset);
			item.position				= float3(	(float)offset.matrix[3][0], 
													(float)offset.matrix[3][1], 
													(float)offset.matrix[3][2]);

			translation_maya_to_xray	( item.position );

			MTransformationMatrix::RotationOrder ro;
			double rot[3];
			result					= m.getRotation( rot, ro );

			item.rotation.x			= (float)rot[0];
			item.rotation.y			= (float)rot[1];
			item.rotation.z			= (float)rot[2];

			rotation_maya_to_xray	( item.rotation );
			m_locators.push_back	( item );
		}
	}

	MGlobal::executeCommand("doEnableNodeItems false all");

	return result;
}

struct geometry_collector_internal
{
	typedef vector< u32 >				geom_indices_type;
	typedef vector< math::float3 >		geom_vertices_type;

	struct lookup_item
	{
		float		square_magnitude;
		u32			vertex_id;

		inline bool operator < (lookup_item const& other)	const	{ return square_magnitude<other.square_magnitude; }
		inline bool operator < (float const& sqm)			const	{ return square_magnitude<sqm; }
	};

	typedef vector<lookup_item>		lookup_table;
	lookup_table					m_lookup;

	int								find_vertex			( float3 const& pos ) const;
	void							add_vertex			( float3 const& pos );

	geom_indices_type				m_indices;
	geom_vertices_type				m_vertices;
};

geometry_collector::geometry_collector( )
:m_data		( NEW(geometry_collector_internal)() ),
m_bbox		( math::create_zero_aabb() )
{}
geometry_collector::~geometry_collector( )
{
	DELETE( m_data );
}

bool geometry_collector::save_vertices( memory::writer& F) const
{
	// collision
	F.open_chunk		( xray::render::model_chunk_collision_v );
	F.write				( &m_data->m_vertices[0], m_data->m_vertices.size()*sizeof(float3) );
    F.close_chunk		( );

	return true;
}

bool geometry_collector::save_indices( memory::writer& F) const
{
	F.open_chunk		( xray::render::model_chunk_collision_i );
	F.write				(&m_data->m_indices[0], m_data->m_indices.size()*sizeof(u32));
	F.close_chunk		( );

	return true;
}

u32 geometry_collector::vertices_count( ) const				
{ 
	return m_data->m_vertices.size(); 
}

u32 geometry_collector::indices_count( ) const				
{ 
	return m_data->m_indices.size(); 
}

void geometry_collector::clear( )
{
	m_data->m_vertices.clear	( );
	m_data->m_indices.clear		( );
	m_data->m_lookup.clear		( );
}

int geometry_collector_internal::find_vertex(	float3 const& position ) const
{
	float const sqm	= position.squared_length();

	lookup_table::const_iterator it		= std::lower_bound(m_lookup.begin(), m_lookup.end(), sqm);
	lookup_table::const_iterator it_e	= m_lookup.end();
	for( ; it!=it_e; ++it)
	{
		lookup_item const& rec	= *it;
		if(rec.square_magnitude > sqm+0.1f)
			break;

		float3 const& p		= m_vertices[rec.vertex_id];
		if(p.is_similar(position))
			return rec.vertex_id;
	}
	return -1;
}

void geometry_collector_internal::add_vertex(	float3 const& position )
{
	int idx = find_vertex( position );

	if(-1==idx)
	{
		idx						= m_vertices.size();
		m_vertices.push_back	( position );
		
		lookup_item				rec;
		rec.square_magnitude	= position.squared_length();
		rec.vertex_id			= idx;

		lookup_table::iterator it = std::lower_bound(m_lookup.begin(), m_lookup.end(), rec);
		m_lookup.insert			( it, rec );
	}

	m_indices.push_back		( idx );
}

void geometry_collector::add_vertex(	float3 const& position )
{
	m_data->add_vertex( position );
}


}// namespace maya
}// namespace xray