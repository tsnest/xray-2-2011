////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_NODE_PLAYING_INSTANCE_H_INCLUDED
#define ANIMATION_NODE_PLAYING_INSTANCE_H_INCLUDED

#include "enum_defines.h"

using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace animation_editor {
	ref class time_scale_instance;
	ref class time_instance;
	ref class animation_viewer_graph_node_base;

	public ref class animation_node_playing_instance
	{
		typedef xray::editor::wpf_controls::property_container wpf_property_container;
	public:
									animation_node_playing_instance		(animation_viewer_graph_node_base^ parent);
									~animation_node_playing_instance	();		
		wpf_property_container^		container							() {return m_container;};
		void						save								(xray::configs::lua_config_value& cfg);
		void						load								(xray::configs::lua_config_value const& cfg);
		animation_viewer_graph_node_base^ parent						() {return m_parent;};

		[CategoryAttribute("Node properties"), DisplayNameAttribute("time_offset")]
		property wpf_property_container^ time_offset
		{
				wpf_property_container^	get								();
				void					set								(wpf_property_container^) {};
		};

		[CategoryAttribute("Node properties"), DefaultValueAttribute(mix_self(0)), DisplayNameAttribute("mixing_type")]
		property mix_self			mixing_type
		{
				mix_self			get									();
				void				set									(mix_self new_val);
		};

		[CategoryAttribute("Node properties"), DisplayNameAttribute("time_scale")]
		property wpf_property_container^ time_scale
		{
			wpf_property_container^	get									();
			void					set									(wpf_property_container^) {};
		};

	private:
		wpf_property_container^				m_container;
		time_instance^						m_offset;
		mix_self							m_mixing_type;
		time_scale_instance^				m_time_scale_inst;
		animation_viewer_graph_node_base^	m_parent;
	}; // class animation_node_playing_instance

} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_NODE_PLAYING_INSTANCE_H_INCLUDED