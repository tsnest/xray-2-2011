////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_PRECONDITION_ACTION_EDITOR_H_INCLUDED
#define DIALOG_PRECONDITION_ACTION_EDITOR_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace dialog_editor {
	ref class dialog_editor_impl;
	ref class dialog_link_precondition;
	ref class dialog_link_action;
	ref class dialog_graph_link;
	class dialog_expression;

	public ref class dialog_precondition_action_editor : public System::Windows::Forms::Form
	{
	public:
		dialog_precondition_action_editor(dialog_editor_impl^ ed):m_editor(ed)
		{
			InitializeComponent();
			in_constructor();
		}
		void			show					(dialog_graph_link^ lnk, bool act_vis, bool prec_vis);

	protected:
		~dialog_precondition_action_editor()
		{
			if (components)
				delete components;
		}
	private:
		void	in_constructor					();
		void	on_form_closing					(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);
		void	on_node_select					(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e);
		void	on_tree_view_key_down			(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e);
		void	on_tree_view_key_up				(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e);
		void	on_node_property_changed		(System::Object^ sender, System::Windows::Forms::PropertyValueChangedEventArgs^ e);

		void	on_new_and_clicked				(System::Object^ sender, System::EventArgs^ e);
		void	on_new_or_clicked				(System::Object^ sender, System::EventArgs^ e);
		void	on_new_not_clicked				(System::Object^ sender, System::EventArgs^ e);
		void	on_new_function_clicked			(System::Object^ sender, System::EventArgs^ e);
		void	on_new_action_clicked			(System::Object^ sender, System::EventArgs^ e);
		void	on_ok_clicked					(System::Object^ sender, System::EventArgs^ e);
		void	on_cancel_clicked				(System::Object^ sender, System::EventArgs^ e);
		void	on_delete_clicked				(System::Object^ sender, System::EventArgs^ e);

		TreeNodeCollection^	get_root_and_check_prereq(System::String^ nm);
		void	make_nodes_from_preconditions	(dialog_expression* n, TreeNodeCollection^ root);
		void	add_link_action_with_name		(dialog_link_action^ act, TreeNodeCollection^ root);
		void	add_link_precondition_with_name	(dialog_link_precondition^ pr, TreeNodeCollection^ root);
		void	select_node						(TreeNode^ n);
		void	show_properties					(TreeNode^ n);
		void	delete_node_with_childs			(TreeNode^ n);
		void	destroy_node					(TreeNode^ n);

	private:
		dialog_editor_impl^						m_editor;
		dialog_graph_link^						m_link;
		bool									m_changed;
		bool									m_act_vis;
		bool									m_prec_vis;
		System::ComponentModel::Container^		components;
		System::Windows::Forms::TreeView^		treeView1;
		System::Windows::Forms::TreeNode^		m_prec_node;
		System::Windows::Forms::TreeNode^		m_act_node;
		System::Windows::Forms::Button^			button1;
		System::Windows::Forms::Button^			button2;
		System::Windows::Forms::Button^			button3;
		System::Windows::Forms::Button^			button4;
		System::Windows::Forms::Button^			button5;
		System::Windows::Forms::Button^			button6;
		System::Windows::Forms::Button^			button7;
		System::Windows::Forms::Button^			button8;
		System::Windows::Forms::Label^			label1;
		System::Windows::Forms::Label^			label2;
		System::Windows::Forms::PropertyGrid^	propertyGrid1;
		AttributeCollection^					m_prec_props;
		AttributeCollection^					m_act_props;
		AttributeCollection^					m_type_props;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->button8 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->propertyGrid1 = (gcnew System::Windows::Forms::PropertyGrid());
			this->SuspendLayout();
			// 
			// treeView1
			// 
			this->treeView1->AllowDrop = true;
			this->treeView1->HideSelection = false;
			this->treeView1->Location = System::Drawing::Point(3, 45);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(235, 273);
			this->treeView1->TabIndex = 0;
			this->treeView1->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &dialog_precondition_action_editor::on_tree_view_key_up);
			this->treeView1->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &dialog_precondition_action_editor::on_node_select);
			this->treeView1->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &dialog_precondition_action_editor::on_tree_view_key_down);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(3, 3);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(70, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"New AND";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &dialog_precondition_action_editor::on_new_and_clicked);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(141, 3);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(63, 23);
			this->button2->TabIndex = 2;
			this->button2->Text = L"New NOT";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &dialog_precondition_action_editor::on_new_not_clicked);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(79, 3);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(56, 23);
			this->button3->TabIndex = 3;
			this->button3->Text = L"New OR";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &dialog_precondition_action_editor::on_new_or_clicked);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(210, 3);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(98, 23);
			this->button4->TabIndex = 4;
			this->button4->Text = L"New FUNCTION";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &dialog_precondition_action_editor::on_new_function_clicked);
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(314, 3);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(80, 23);
			this->button5->TabIndex = 5;
			this->button5->Text = L"New ACTION";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &dialog_precondition_action_editor::on_new_action_clicked);
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(84, 324);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(75, 23);
			this->button6->TabIndex = 6;
			this->button6->Text = L"OK";
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &dialog_precondition_action_editor::on_ok_clicked);
			// 
			// button7
			// 
			this->button7->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button7->Location = System::Drawing::Point(165, 324);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(75, 23);
			this->button7->TabIndex = 7;
			this->button7->Text = L"Cancel";
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &dialog_precondition_action_editor::on_cancel_clicked);
			// 
			// button8
			// 
			this->button8->Location = System::Drawing::Point(3, 323);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(75, 23);
			this->button8->TabIndex = 8;
			this->button8->Text = L"Delete";
			this->button8->UseVisualStyleBackColor = true;
			this->button8->Click += gcnew System::EventHandler(this, &dialog_precondition_action_editor::on_delete_clicked);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(0, 29);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(177, 13);
			this->label1->TabIndex = 9;
			this->label1->Text = L"Select precondition or action to edit:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(262, 29);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(57, 13);
			this->label2->TabIndex = 10;
			this->label2->Text = L"Properties:";
			// 
			// propertyGrid1
			// 
			this->propertyGrid1->BackColor = System::Drawing::SystemColors::ControlLight;
			this->propertyGrid1->Location = System::Drawing::Point(253, 45);
			this->propertyGrid1->Name = L"propertyGrid1";
			this->propertyGrid1->Size = System::Drawing::Size(253, 301);
			this->propertyGrid1->TabIndex = 11;
			this->propertyGrid1->PropertyValueChanged += gcnew System::Windows::Forms::PropertyValueChangedEventHandler(this, &dialog_precondition_action_editor::on_node_property_changed);
			// 
			// dialog_precondition_action_editor
			// 
			this->AcceptButton = this->button6;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->CancelButton = this->button7;
			this->ClientSize = System::Drawing::Size(509, 349);
			this->Controls->Add(this->propertyGrid1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button8);
			this->Controls->Add(this->button7);
			this->Controls->Add(this->button6);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->treeView1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"dialog_precondition_action_editor";
			this->ShowInTaskbar = false;
			this->Text = L"Preconditions and actions editor";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &dialog_precondition_action_editor::on_form_closing);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	}; // ref class dialog_precondition_action_editor

	ref class precondition_action_editor_sorter : public System::Collections::IComparer 
	{
	public:
		virtual int Compare	(Object^ x, Object^ y);
	}; // ref class precondition_action_editor_sorter

	public ref class dialog_link_ui_type_editor : public xray::editor::wpf_controls::property_editors::i_external_property_editor
	{
	public:
		virtual	void			run_editor	( xray::editor::wpf_controls::property_editors::property^ prop );
	}; // ref class dialog_link_ui_type_editor
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef DIALOG_PRECONDITION_ACTION_EDITOR_H_INCLUDED