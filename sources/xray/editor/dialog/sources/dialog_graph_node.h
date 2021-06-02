//-------------------------------------------------------------------------------------------
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
//-------------------------------------------------------------------------------------------
#ifndef DIALOG_GRAPH_NODE_H_INCLUDED
#define DIALOG_GRAPH_NODE_H_INCLUDED

#include "dialogs_manager.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace dialog_editor {
	class dialog;
	class dialog_node_base;
	ref class dialog_editor_impl;
	ref class dialog_node_hypergraph;
	ref class dialog_hypergraph;

	public ref class dialog_graph_node : public xray::editor::controls::hypergraph::node
	{
		typedef xray::editor::controls::hypergraph::node	super;
	public:
		enum class e_id_category
		{
			none		= (1<<0),
			dialogs		= (1<<1),
			items		= (1<<2),
			tasks		= (1<<3),
			ui			= (1<<4),
			locations	= (1<<5),
		};

									dialog_graph_node		(dialog* parent_dialog);
									dialog_graph_node		();
		u32							id						();
		bool						is_dialog				();
		virtual String^				caption					() override;
		virtual void				draw_caption			(System::Windows::Forms::PaintEventArgs^ e) override;
		virtual	void				draw_frame				(System::Windows::Forms::PaintEventArgs^ e) override;
		virtual void				assign_style			(xray::editor::controls::hypergraph::node_style^ style) override;
		virtual void				on_added				(xray::editor::controls::hypergraph::hypergraph_control^ parent) override;
		virtual void				on_removed				(xray::editor::controls::hypergraph::hypergraph_control^ parent) override;
				void				request_dialog			();

		property dialog_node_base*	object
		{
			void					set						(dialog_node_base* new_obj)	{*m_object = new_obj;};
			dialog_node_base*		get						()							{return m_object->c_ptr();};
		};

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(true), DisplayNameAttribute("is_root"), DescriptionAttribute("Indicates if node is root")]
		property bool				is_root
		{
			bool					get						();
			void					set						(bool val);
		};

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(""),	DisplayNameAttribute("text"), DescriptionAttribute("text value")]
		property String^			text
		{
			String^					get						();
		};

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(""),	DisplayNameAttribute("object_id"), DescriptionAttribute("id value"), ReadOnlyAttribute(true)]
		property u32				object_id
		{
			u32						get						()							{return id();};
		};

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(""),	DisplayNameAttribute("string_table"), DescriptionAttribute("string_table value")]
		property String^			string_table
		{
			void					set						(String^ new_str_id);
			String^					get						();
		};

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

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(1), DisplayNameAttribute("category"), DescriptionAttribute("Indicates string_table_id category")]
		property e_id_category		category
		{
			e_id_category			get						();
			void					set						(e_id_category val);
		};

	protected:
		virtual void				recalc					() override;
		void						on_double_click			(System::Object^ sender, System::EventArgs^ e);
		void						on_exp_coll_clicked		(System::Object^ sender, System::EventArgs^ e);

		~dialog_graph_node();

	private:
		void						initialize				();
		void						fill_hypergraph			();
		void						iterate_subnodes		(dialog_node_base* n);
		void						on_dialog_loaded		(xray::resources::queries_result& result);

	private:
		System::ComponentModel::Container^			components;
		bool										m_has_top;
		bool										m_has_bottom;
		dialog_node_base_ptr*						m_object;
		System::Windows::Forms::Button^				button_exp_coll;
		dialog*										m_parent_dialog;
		dialog_node_hypergraph^						m_hypergraph;
		u32											m_id;
	}; // ref class dialog_graph_node
}; //namespace dialog_editor
}; //namespace xray
#endif // #ifndef DIALOG_GRAPH_NODE_H_INCLUDED
