////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node.h"
#include "particle_graph_document.h"
#include "particle_time_parameters_holder.h"

namespace xray {
	namespace particle_editor {

void particle_time_parameters_holder::set_start_time_impl(float value){
	(*m_changes_config)["Delay"] = value;

	m_owner->parent_document->particle_system_property_changed(m_owner, *m_changes_config);

	(*m_config)["Delay"] = value;

	m_owner->parent_editor->reset_transform();
}

float particle_time_parameters_holder::get_start_time_impl(){
	return (float)(*m_config)["Delay"];
}

void particle_time_parameters_holder::set_length_time_impl(float value){
	(*m_changes_config)["Duration"] = value;

	m_owner->parent_document->particle_system_property_changed(m_owner, *m_changes_config);

	(*m_config)["Duration"] = value;

	m_owner->parent_editor->reset_transform();
}

float particle_time_parameters_holder::get_length_time_impl(){
	return (float)(*m_config)["Duration"];
}

	} // namespace particle_editor
} // namespace xray