////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_GRAPH_NODE_BASE_H_INCLUDED
#define ANIMATION_VIEWER_GRAPH_NODE_BASE_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Drawing;

namespace xray {
namespace animation_editor {

	#define VERTICAL_TAB_HEIGHT 8
	ref class animation_viewer_hypergraph;

	public ref class animation_viewer_graph_node_base: public xray::editor::controls::hypergraph::node
	{
		typedef xray::editor::controls::hypergraph::node	super;
	protected:
		typedef System::Collections::Generic::List<animation_viewer_graph_node_base^> nodes;

	public:
		virtual						~animation_viewer_graph_node_base	() {};
		virtual String^				get_text				() {return "ERROR";};
		virtual void				recalc					() override;

		virtual void				assign_style			(xray::editor::controls::hypergraph::node_style^ style) override;
		virtual void				draw_caption			(System::Windows::Forms::PaintEventArgs^ e) override;
		virtual	void				draw_frame				(System::Windows::Forms::PaintEventArgs^ e) override;

		virtual void				on_added				(xray::editor::controls::hypergraph::hypergraph_control^ parent) override;
		virtual void				on_removed				(xray::editor::controls::hypergraph::hypergraph_control^ parent) override;
		virtual void				save					(xray::configs::lua_config_value cfg);
		virtual void				load					(xray::configs::lua_config_value const& cfg, animation_viewer_hypergraph^ h);

		[CategoryAttribute("Node properties"), DisplayNameAttribute("id")]
		property u32				id
		{
			u32						get						()				{return m_id;};
			void					set						(u32 new_id)	{m_id = new_id;};
		};

		property animation_viewer_graph_node_base^			parent;
		property nodes^										childs;

animation_viewer_graph_node_base^	root					();

	protected:
		virtual void				initialize				();
		virtual void				on_double_click			(System::Object^ , System::EventArgs^ ) {};

	protected:
		System::ComponentModel::Container^			components;
		u32											m_id;
		bool										m_has_top;
		bool										m_has_bottom;
	}; // ref class dialog_graph_node_base
}; //namespace animation_editor
}; //namespace xray
#endif // #ifndef ANIMATION_VIEWER_GRAPH_NODE_BASE_H_INCLUDED