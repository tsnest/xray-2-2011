////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_GRAPH_NODE_PHRASE_H_INCLUDED
#define DIALOG_GRAPH_NODE_PHRASE_H_INCLUDED

#include "dialog_graph_node_base.h"
using namespace System;
using namespace System::ComponentModel;

namespace xray {
namespace dialog_editor {
	class dialog_phrase;

	public ref class dialog_graph_node_phrase : public dialog_graph_node_base
	{
		typedef dialog_graph_node_base	super;

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

							dialog_graph_node_phrase() {initialize();};
		virtual				~dialog_graph_node_phrase();
		virtual	bool		is_dialog				() override {return false;};
		virtual String^		get_text				() override;

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(true), DisplayNameAttribute("is_root"), DescriptionAttribute("Indicates if node is root")]
		property bool				is_root
		{
			virtual	bool			get						() override;
			virtual	void			set						(bool rt) override;
		}

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(""),	DisplayNameAttribute("string_table"), DescriptionAttribute("string_table value")]
		property String^	string_table
		{
			void			set						(String^ new_str_id);
			String^			get						();
		};

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(1), DisplayNameAttribute("category"), DescriptionAttribute("Indicates string_table_id category")]
		property e_id_category		category
		{
			e_id_category	get						();
			void			set						(e_id_category val);
		};

		property dialog_node_base*	object
		{
			virtual	dialog_node_base* get					() override;
			virtual	void			set						(dialog_node_base* n) override;
		}

	protected:
		virtual void		initialize				() override;
		virtual void		recalc					() override;
		virtual void		on_double_click			(System::Object^ sender, System::EventArgs^ e) override;

	private:
		dialog_phrase*		m_object;
	}; // ref class dialog_graph_node_phrase
}; //namespace dialog_editor
}; //namespace xray
#endif // #ifndef DIALOG_GRAPH_NODE_PHRASE_H_INCLUDED