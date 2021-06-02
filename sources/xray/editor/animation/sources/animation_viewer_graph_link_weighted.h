////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_GRAPH_LINK_WEIGHTED_H_INCLUDED
#define ANIMATION_VIEWER_GRAPH_LINK_WEIGHTED_H_INCLUDED

#include "animation_viewer_graph_link.h"
#include "enum_defines.h"

using namespace System;
using namespace System::Collections;
using namespace System::ComponentModel;


namespace xray {
namespace animation { struct base_interpolator; }
namespace animation_editor {
	ref class animation_viewer_graph_link;
	ref class animation_node_interpolator;

	public ref class animation_viewer_graph_link_weighted : public animation_viewer_graph_link
	{
		typedef xray::animation_editor::animation_viewer_graph_link		super;
		typedef xray::editor::controls::hypergraph::connection_point	connection_point;
		typedef xray::editor::wpf_controls::property_container			wpf_property_container;

	public:
						animation_viewer_graph_link_weighted	(connection_point^ src, connection_point^ dst);
						~animation_viewer_graph_link_weighted	();

		virtual	void	load									(xray::configs::lua_config_value const& cfg) override;
		virtual	void	save									(xray::configs::lua_config_value cfg) override;
		virtual	void	draw									(System::Drawing::Graphics^ g, System::Drawing::Pen^ pen, xray::editor::controls::hypergraph::nodes_link_style link_style) override;

		[CategoryAttribute("Link properties"), DefaultValueAttribute(1.0f), DisplayNameAttribute("weight")]
		[xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute(0.0f, 1.0f, 0.01f, 3, update_on_edit_complete = true)]
		property float	weight;

		[CategoryAttribute("Link properties"), DisplayNameAttribute("interpolator")]
		property wpf_property_container^ interpolator
		{
			wpf_property_container^	get						();
			void					set						(wpf_property_container^) {};
		};

	private:
		animation_node_interpolator^	m_interpolator;
	}; // ref class animation_viewer_graph_link_weighted
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_GRAPH_LINK_WEIGHTED_H_INCLUDED