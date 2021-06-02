////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SCRIPTING_TOOL_TAB_H_INCLUDED
#define SCRIPTING_TOOL_TAB_H_INCLUDED

#include "tool_tab.h"

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	ref class tool_scripting;
	ref class library_objects_tree;
	/// <summary>
	/// Summary for scripting_tool_tab
	/// </summary>
	public ref class scripting_tool_tab : public System::Windows::Forms::UserControl,
		public tool_tab
	{
	public:
		scripting_tool_tab(tool_scripting^ tool)
		: m_tool(tool)
		{
			InitializeComponent		( );
			post_constructor		( );
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~scripting_tool_tab()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
	private: System::Windows::Forms::Panel^  objects_tree_place;
			 tool_scripting^						m_tool;




#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->objects_tree_place = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// objects_tree_place
			// 
			this->objects_tree_place->Dock = System::Windows::Forms::DockStyle::Fill;
			this->objects_tree_place->Location = System::Drawing::Point(0, 0);
			this->objects_tree_place->Name = L"objects_tree_place";
			this->objects_tree_place->Size = System::Drawing::Size(195, 319);
			this->objects_tree_place->TabIndex = 0;
			// 
			// scripting_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->objects_tree_place);
			this->Name = L"scripting_tool_tab";
			this->Size = System::Drawing::Size(195, 319);
			this->ResumeLayout(false);

		}
#pragma endregion

	void	post_constructor							( );
	library_objects_tree^	m_library_objects_tree;

	public:
		void							fill_objects_list( );
		virtual UserControl^			frame			( )	{return this;};
		virtual void					on_activate		( )	;
		virtual void					on_deactivate	( )	;


	};
}
}

#endif // #ifndef SCRIPTING_TOOL_TAB_H_INCLUDED