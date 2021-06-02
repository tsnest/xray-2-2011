////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_GRAPH_NODE_BASE_H_INCLUDED
#define DIALOG_GRAPH_NODE_BASE_H_INCLUDED

#include "dialog_node_base.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Drawing;

namespace xray {
namespace dialog_editor {
	class dialog_node_base;

	#define VERTICAL_TAB_HEIGHT 8

	public ref class dialog_graph_node_base abstract: public xray::editor::controls::hypergraph::node
	{
		typedef xray::editor::controls::hypergraph::node	super;
	public:
		virtual						~dialog_graph_node_base	() {};
		virtual bool				is_dialog				() = 0;
		virtual String^				get_text				() = 0;

		virtual void				assign_style			(xray::editor::controls::hypergraph::node_style^ style) override;
		virtual void				draw_caption			(System::Windows::Forms::PaintEventArgs^ e) override;
		virtual	void				draw_frame				(System::Windows::Forms::PaintEventArgs^ e) override;

		virtual void				on_added				(xray::editor::controls::hypergraph::hypergraph_control^ parent) override;
		virtual void				on_removed				(xray::editor::controls::hypergraph::hypergraph_control^ parent) override;

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(""),	DisplayNameAttribute("object_id"), DescriptionAttribute("id value"), ReadOnlyAttribute(true)]
		property u32				id
		{
			u32						get						()				{return object->id();};
			void					set						(u32 new_id)	{object->set_id(new_id);};
		};

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(""),	DisplayNameAttribute("caption"), DescriptionAttribute("caption value")]
		property String^			caption
		{
			String^					get						()	{return get_text();};
		};

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(true), DisplayNameAttribute("is_root"), DescriptionAttribute("Indicates if node is root")]
		property bool				is_root
		{
			virtual	bool			get						() = 0;
			virtual	void			set						(bool rt) = 0;
		}

		[CategoryAttribute("Phrase properties"), DisplayNameAttribute("Position"), DescriptionAttribute("Current node position"), ReadOnlyAttribute(true)]
		property System::Drawing::Point	pos
		{
			System::Drawing::Point	get						()			{return m_position;};
		};

		[CategoryAttribute("Phrase properties"), DisplayNameAttribute("Scale"), DescriptionAttribute("Current node scale"), ReadOnlyAttribute(true)]
		property float				scl
		{
			float					get						()			{return m_style->scale;};
		};

		property dialog_node_base*	object
		{
			virtual	dialog_node_base* get					() = 0;
			virtual	void			set						(dialog_node_base* n) = 0;
		}

	protected:
		virtual void				initialize				();
		virtual void				on_double_click			(System::Object^ sender, System::EventArgs^ e) = 0;

	protected:
		System::ComponentModel::Container^			components;
		bool										m_has_top;
		bool										m_has_bottom;
	}; // ref class dialog_graph_node_base
}; //namespace dialog_editor
}; //namespace xray
#endif // #ifndef DIALOG_GRAPH_NODE_BASE_H_INCLUDED
