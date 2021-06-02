////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_GRAPH_NODE_DIALOG_H_INCLUDED
#define DIALOG_GRAPH_NODE_DIALOG_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Drawing;

#include "dialogs_manager.h"
#include "dialog_graph_node_base.h"

namespace xray {
namespace dialog_editor {
	#define VERTICAL_TAB_HEIGHT 8

	class dialog_container;
	ref class dialog_node_hypergraph;

	public ref class dialog_graph_node_dialog : public dialog_graph_node_base
	{
		typedef dialog_graph_node_base	super;
	public:
							dialog_graph_node_dialog() {initialize();};
		virtual				~dialog_graph_node_dialog();

		virtual bool		is_dialog				() override {return true;};
		virtual String^		get_text				() override;
				void		set_expanded			(bool val);

		[CategoryAttribute("Phrase properties"), DefaultValueAttribute(true), DisplayNameAttribute("is_root"), DescriptionAttribute("Indicates if node is root")]
		property bool				is_root
		{
			virtual	bool	get						() override;
			virtual	void	set						(bool rt) override;
		}

		property dialog_node_base*	object
		{
			virtual	dialog_node_base* get			() override;
			virtual	void			set				(dialog_node_base* n) override;
		}

	protected:
		virtual void		initialize				() override;
		virtual void		recalc					() override;
		virtual void		on_double_click			(System::Object^ sender, System::EventArgs^ e) override;

	private:
				void		fill_hypergraph			();
				void		on_exp_coll_clicked		(System::Object^ sender, System::EventArgs^ e);

	private:
		dialog_container*							m_object;
		System::Windows::Forms::Button^				button_exp_coll;
		dialog_node_hypergraph^						m_hypergraph;
	}; // ref class dialog_graph_node_dialog
}; //namespace dialog_editor
}; //namespace xray
#endif // #ifndef DIALOG_GRAPH_NODE_DIALOG_H_INCLUDED
