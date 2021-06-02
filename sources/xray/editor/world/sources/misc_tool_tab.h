////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MISC_TOOL_TAB_H_INCLUDED
#define MISC_TOOL_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "tool_tab.h"


namespace xray{
namespace editor{
	ref class tool_misc;
	ref class library_objects_tree;
	/// <summary>
	/// Summary for misc_tool_tab
	/// </summary>
	public ref class misc_tool_tab :	public System::Windows::Forms::UserControl,
										public tool_tab
	{
	public:
		misc_tool_tab( tool_misc^ tool ):m_tool(tool)
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
		~misc_tool_tab()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^  objects_tree_place;
	protected: 

	protected: 


	public: 


	public: 
	private: 
	protected: 
	private: System::ComponentModel::IContainer^  components;
	public: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


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
			this->objects_tree_place->Size = System::Drawing::Size(211, 369);
			this->objects_tree_place->TabIndex = 0;
			// 
			// misc_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->objects_tree_place);
			this->Name = L"misc_tool_tab";
			this->Size = System::Drawing::Size(211, 369);
			this->ResumeLayout(false);

		}
#pragma endregion
		private:
			tool_misc^			m_tool;
	public:
	void	fill_objects_list							( );
	void	post_constructor							( );
	library_objects_tree^	m_library_objects_tree;
	virtual System::Windows::Forms::UserControl^	frame ( ) {return this;}
	virtual void		on_activate						( );
	virtual void		on_deactivate					( );
};

} // namespace editor
} // namespace xray

#endif // #ifndef MISC_TOOL_TAB_H_INCLUDED