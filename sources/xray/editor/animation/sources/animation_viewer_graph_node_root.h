////////////////////////////////////////////////////////////////////////////
//	Created		: 10.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_VIEWER_GRAPH_NODE_ROOT_H_INCLUDED
#define ANIMATION_VIEWER_GRAPH_NODE_ROOT_H_INCLUDED

#include "animation_viewer_graph_node_base.h"

namespace xray {
namespace animation_editor {

	ref class animation_viewer_graph_node_base;
	ref class animation_viewer_hypergraph;

	public ref class animation_viewer_graph_node_root: public animation_viewer_graph_node_base
	{
		typedef animation_viewer_graph_node_base	super;
	public:
									animation_viewer_graph_node_root	(System::String^ text) {initialize(); m_text = text;};
		virtual						~animation_viewer_graph_node_root	();
		virtual String^				get_text				() override;
		virtual void				save					(xray::configs::lua_config_value cfg) override;
		virtual void				load					(xray::configs::lua_config_value const& cfg, animation_viewer_hypergraph^ h) override;
animation_viewer_graph_node_root^	transition				();
				void				generate_caption		();

		property String^			caption
		{
			String^					get						();
		};

	protected:
		virtual void				initialize				() override;
		virtual void				on_double_click			(System::Object^ sender, System::EventArgs^ e) override;

	private:
		System::String^				m_text;
	}; // class animation_viewer_graph_node_root
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_GRAPH_NODE_ROOT_H_INCLUDED