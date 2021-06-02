////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator.h"
#include <xray/configs_lua_config.h>
#include <xray/resources.h>

using xray::ai::navigation::graph_generator;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::navigation_triangle;
using namespace xray::configs;

void graph_generator::on_binary_config_resource ( xray::resources::queries_result& resources ) 
{
	if ( resources.is_successful() ) {
		m_input_triangles.indices.clear();
		m_input_triangles.vertices.clear();
		m_input_triangles.data.clear();

		if ( m_input_triangles.spatial_tree )
			m_input_triangles.destroy_spatial_tree ( );
		load_navigation_mesh( m_input_triangles, static_cast_resource_ptr<binary_config_ptr>( resources[0].get_unmanaged_resource() ) );
		m_input_triangles.build_spatial_tree	( );
	}
}

void graph_generator::save_navigation_mesh_console_command	( pcstr const args )
{
	string_path file_name;
	XRAY_SSCANF					( args, "%s", file_name );
	pstr full_path;
	STR_JOINA( full_path, "resources/navmesh/", file_name );
	save_navigation_mesh		( m_input_triangles, full_path );
}

void graph_generator::load_navigation_mesh_console_command	( pcstr const args )
{
	string_path file_name;
	XRAY_SSCANF					( args, "%s", file_name );
	pstr full_path;
	STR_JOINA( full_path, "resources/navmesh/", file_name );

	resources::query_resource	(
		full_path,
		resources::binary_config_class,
		boost::bind( &graph_generator::on_binary_config_resource, this, _1 ),
		g_allocator
	);
}

void graph_generator::save_geometry					( pcstr filename )
{
	save_navigation_mesh( m_input_triangles, filename );
}

void graph_generator::save_navigation_mesh ( triangles_mesh_type& navigation_mesh, pcstr file_name )
{
	if ( navigation_mesh.indices.empty() )
		return;

	lua_config_ptr config = create_lua_config	( file_name );
	lua_config_value& root = config->get_root	( );

	lua_config_value indices_table = root["indices"];

	for ( u32 i = 0; i < navigation_mesh.indices.size(); ++i )
		indices_table[i] = navigation_mesh.indices[i];

	lua_config_value vertices_table = root["vertices"];
	for ( u32 i = 0; i < navigation_mesh.vertices.size(); ++i )
		vertices_table[i] = navigation_mesh.vertices[i];

	lua_config_value triangle_data_table = root["triangle_data"];
	for ( u32 i = 0; i < navigation_mesh.data.size(); ++i ) {
		triangles_mesh_type::data_type::value_type& triangle = navigation_mesh.data[ i ];
		R_ASSERT( !triangle.is_marked );

		lua_config_value triangle_table = triangle_data_table[i];

		triangle_table["plane"] = triangle.plane.vector;

		lua_config_value obstructions_table = triangle_table["obstructions"];
		for ( u32 j = 0; j < 3; ++j ) {
			obstructions_table[j] = triangle.obstructions[ j ];
		}

		lua_config_value neighbours_table = triangle_table["neighbours"];
		for ( u32 j = 0; j < 3; ++j ) {
			neighbours_table[j] = triangle.neighbours[ j ];
		}
	}

	config->save								( xray::configs::target_sources );
}

void graph_generator::load_navigation_mesh ( triangles_mesh_type& navigation_mesh, binary_config_ptr config )
{
	binary_config_value root_table = config->get_root();
	if ( !root_table.value_exists("indices") 
		|| !root_table.value_exists("vertices")
		|| !root_table.value_exists("triangle_data") )
	{
		return;
	}

	binary_config_value indices_table = root_table["indices"];
	binary_config_value vertices_table = root_table["vertices"];
	binary_config_value triangle_data_table = root_table["triangle_data"];

	{
		binary_config_value::const_iterator i = vertices_table.begin();
		binary_config_value::const_iterator const e = vertices_table.end();
		for ( ; i != e; ++i )
			navigation_mesh.vertices.push_back( *i );
	}

	{
		binary_config_value::const_iterator i = indices_table.begin();
		binary_config_value::const_iterator const e = indices_table.end();
		for ( ; i != e; ++i )
			navigation_mesh.indices.push_back( *i );
	}

	{
		binary_config_value::const_iterator i = triangle_data_table.begin();
		binary_config_value::const_iterator const e = triangle_data_table.end();
		for ( ; i != e; ++i ) {
			navigation_triangle triangle;
			triangle.plane.vector = (*i)["plane"];

			binary_config_value neighbours_table = (*i)["neighbours"];
			for ( u32 j = 0; j < 3; ++j ) {
				triangle.neighbours[ j ] = neighbours_table[j];
			}

			binary_config_value obstructions_table = (*i)["obstructions"];
			for ( u32 j = 0; j < 3; ++j ) {
				triangle.obstructions[ j ] = obstructions_table[j];
			}
			navigation_mesh.data.push_back ( triangle );
		}
	}
}
