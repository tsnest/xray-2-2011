////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCES_HOLDER_H_INCLUDED
#define PARTICLE_DATA_SOURCES_HOLDER_H_INCLUDED

#include "particle_data_source_nodes.h"

using namespace System::Collections;


namespace xray {
namespace editor {

public ref class particle_data_sources_holder {
	typedef Generic::List<particle_data_source_cc_node^>				color_curve_data_sources;
	typedef Generic::List<particle_data_source_fc_node^>				float_curve_data_sources;

public:
	particle_data_sources_holder(particle_graph_document^ parent_document):
	m_parent_document(parent_document)
	{
		in_constructor();		
	}
	

private:
	~particle_data_sources_holder();

#pragma region | InnerTypes |

#pragma endregion

#pragma region | Initialize |
private:
	void									in_constructor();

#pragma endregion

#pragma region | Fields |
private:
	color_curve_data_sources^				m_color_curves;
	float_curve_data_sources^				m_float_curves;
	configs::lua_config_value*				m_data_sources_config;
	particle_graph_document^				m_parent_document;

#pragma endregion

#pragma region | Methods |
public:
	void					add_sources_to_panel		(particle_data_sources_panel^ panel);
	void					add_data_source				(particle_data_source_cc_node^ n);
	void					add_data_source				(particle_data_source_fc_node^ n);
	float_curve^			get_float_curve_by_name		(String^ name);
	color_curve^			get_color_curve_by_name		(String^ name);
	void					save						(configs::lua_config_value config);
	void					load						(configs::lua_config_value config);

	void					create_fc_data_source		(String^ name, configs::lua_config_value config);
	void					create_cc_data_source		(String^ name, configs::lua_config_value config);

#pragma endregion

#pragma region | Properties |
public:
	property color_curve_data_sources^		color_curves{
		color_curve_data_sources^			get(){return m_color_curves;}
	}
	property float_curve_data_sources^		float_curves{
		float_curve_data_sources^			get(){return m_float_curves;}
	}
	property configs::lua_config_value		float_curves_config{
		configs::lua_config_value			get(){return (*m_data_sources_config)["float_curves"];}
	}
	property configs::lua_config_value		color_curves_config{
		configs::lua_config_value			get(){return (*m_data_sources_config)["color_curves"];}
	}

#pragma endregion

#pragma region | Events |

#pragma endregion

}; // class particle_data_sources_holder

} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCES_HOLDER_H_INCLUDED