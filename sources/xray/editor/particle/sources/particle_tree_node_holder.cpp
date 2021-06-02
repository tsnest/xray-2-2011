////////////////////////////////////////////////////////////////////////////
//	Created		: 06.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_editor.h"
#include "particle_tree_node_holder.h"
#include "particle_system_config_wrapper.h"

namespace xray {
namespace particle_editor {

static void copy_table_struct	(
		configs::lua_config_value& root_config,
		configs::lua_config_value& from_config,
		configs::lua_config_value& to_config,
		String^ supertable_path
	)
{
	pcstr field_id = from_config.get_field_id();
	if (strings::equal(field_id, "children_order"))
		to_config.assign_lua_value(from_config.copy());
	else
		to_config.add_super_table			(
			root_config,
			unmanaged_string( supertable_path ).c_str( )
		);	

	using configs::lua_config_value;
	lua_config_value::iterator i		= from_config.begin( );
	lua_config_value::iterator const e	= from_config.end( );
	for (;i != e;++i){
		if ( i.key( ).get_type( ) != configs::t_string )
			continue;

		if ( ((*i).get_type() != configs::t_table_indexed) && ((*i).get_type() != configs::t_table_named) )
			continue;

		pcstr const key					= static_cast<pcstr>( i.key() );
		String^ new_supertable_path		= supertable_path + String::Format("[\"{0}\"]", gcnew String(key) );
		lua_config_value lod_current	= from_config[ key ];
		lua_config_value lod_next		= to_config[ key ];
		lod_next.create_table			( );
		copy_table_struct				(
			root_config,
			lod_current,
			lod_next,
			new_supertable_path
		);
	}

	//
	//to_config.save("d:/to_config");
	//root_config.save("d:/root");
}

particle_tree_node_holder::particle_tree_node_holder(xray::editor::controls::tree_node^ parent, particle_editor^ parent_editor)
{
	m_tree_node = parent;
	m_parent_editor = parent_editor;
}

void particle_tree_node_holder::on_tree_node_config_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());

	configs::lua_config_ptr config	= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	
	int i = 1;
	while (config->value_exists((pcstr)unmanaged_string("LOD "+i).c_str())){
		xray::editor::controls::tree_node^ node	= m_tree_node->add_node_file_part("LOD "+i);
		m_parent_editor->add_context_menu_to_tree_view_node(node);
		i++;
	}
}

void particle_tree_node_holder::on_tree_node_new_lod_config_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());
	
	configs::lua_config_ptr config	= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	


	int i = 0;
	while (config->value_exists((pcstr)unmanaged_string("LOD "+i).c_str())){
		i++;
	}
	
	xray::editor::controls::tree_node^ node	= m_tree_node->add_node_file_part("LOD "+i);
	node->EnsureVisible	();
	node->Text = "LOD "+i;
	using configs::lua_config_value;
// 	lua_config_value& root			= config->get_root( );
// 
// 	root[ unmanaged_string("LOD " + i).c_str() ].add_super_table			(
// 		config->get_root( ),
// 		unmanaged_string("LOD " + (i-1)).c_str() 
// 		);	

  	lua_config_value& root			= config->get_root( );
 	lua_config_value lod_current	= root[ unmanaged_string("LOD " + (i-1)).c_str() ];
 	lua_config_value lod_next		= root[ unmanaged_string("LOD " + i).c_str() ];
 	lod_next.create_table			( );
 	copy_table_struct				(
 		root,
 		lod_current,
 		lod_next,
 		String::Format("[\"LOD {0}\"]", (i-1))
 	);

	if (m_parent_editor->particle_configs->ContainsKey(node->Parent->FullPath))
		m_parent_editor->particle_configs[node->Parent->FullPath] = gcnew particle_system_config_wrapper(config);

	config->save(configs::target_sources);
	
	m_parent_editor->add_context_menu_to_tree_view_node(node);
}
void particle_tree_node_holder::on_tree_node_delete_lod_config_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());

	configs::lua_config_ptr config	= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	int i = 0;
	bool start_delete_nodes = false;
	while (config->value_exists((pcstr)unmanaged_string("LOD "+i).c_str())){		
		if(!start_delete_nodes && "LOD " + i == m_tree_node->Text)
			start_delete_nodes = true;
		if (start_delete_nodes)
			config->get_root().erase((pcstr)unmanaged_string("LOD " + i).c_str());
		i++;
	}
	
	config->save(configs::target_sources);	
}

} // namespace particle_editor
} // namespace xray