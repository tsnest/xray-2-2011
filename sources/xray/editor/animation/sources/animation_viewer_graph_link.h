////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_GRAPH_LINK_H_INCLUDED
#define ANIMATION_VIEWER_GRAPH_LINK_H_INCLUDED

using namespace System;
using namespace System::Collections;
using namespace System::ComponentModel;

namespace xray {
namespace animation_editor {

	public ref class animation_viewer_graph_link : public xray::editor::controls::hypergraph::link
	{
		typedef xray::editor::controls::hypergraph::link				super;
		typedef xray::editor::controls::hypergraph::connection_point	connection_point;
		typedef xray::editor::wpf_controls::property_container			wpf_property_container;

	public:
						animation_viewer_graph_link	(connection_point^ src, connection_point^ dst);
						~animation_viewer_graph_link();
wpf_property_container^	container					() {return m_container;};

		virtual	void	load						(xray::configs::lua_config_value const& cfg);
		virtual	void	save						(xray::configs::lua_config_value cfg);

	protected:
		wpf_property_container^			m_container;
	}; // ref class animation_viewer_graph_link
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_GRAPH_LINK_H_INCLUDED