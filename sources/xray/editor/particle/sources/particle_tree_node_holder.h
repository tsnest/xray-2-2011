////////////////////////////////////////////////////////////////////////////
//	Created		: 06.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_TREE_NODE_HOLDER_H_INCLUDED
#define PARTICLE_TREE_NODE_HOLDER_H_INCLUDED

namespace xray {
	namespace particle_editor {

		ref class xray::editor::controls::tree_node;

ref class particle_tree_node_holder {

public:
	particle_tree_node_holder(xray::editor::controls::tree_node^ parent, particle_editor^ parent_editor);

private:
	xray::editor::controls::tree_node^	m_tree_node;
	particle_editor^		m_parent_editor;

public:
	void on_tree_node_config_loaded				(xray::resources::queries_result& data);
	void on_tree_node_new_lod_config_loaded		(xray::resources::queries_result& data);
	void on_tree_node_delete_lod_config_loaded	(xray::resources::queries_result& data);

}; // class particle_tree_node_holder


} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_TREE_NODE_HOLDER_H_INCLUDED

