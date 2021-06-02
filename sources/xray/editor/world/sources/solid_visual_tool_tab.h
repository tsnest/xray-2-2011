////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOLID_VISUAL_TOOL_TAB_H_INCLUDED
#define SOLID_VISUAL_TOOL_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;

#include "tool_tab.h"

namespace xray {
namespace editor {
ref class tool_base;
ref class library_objects_tree;
ref struct lib_item_base;

	/// <summary>
	/// Summary for solid_visual_tool_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class solid_visual_tool_tab :	public System::Windows::Forms::UserControl, 
												public tool_tab
	{
	public:
		solid_visual_tool_tab(tool_base^ tool):m_tool(tool)
		{
			InitializeComponent	( );
			post_constructor	( );
			//
			//TODO: Add the constructor code here
			//
		}
	private: System::Windows::Forms::Panel^  objects_tree_place;
	public: 

	public: 


	private: 




	private: 






	private: 

	public: 
		tool_base^		m_tool;
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~solid_visual_tool_tab()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::ComponentModel::IContainer^  components;
	protected: 

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
			this->objects_tree_place->Size = System::Drawing::Size(242, 385);
			this->objects_tree_place->TabIndex = 0;
			// 
			// solid_visual_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->objects_tree_place);
			this->Name = L"solid_visual_tool_tab";
			this->Size = System::Drawing::Size(242, 385);
			this->ResumeLayout(false);

		}
#pragma endregion
	protected:
	void	post_constructor							( );
	library_objects_tree^	m_library_objects_tree;
	public:
	void	fill_objects_list							( System::String^ initial );
	void	item_double_clicked							( lib_item_base^ item );
	virtual System::Windows::Forms::UserControl^	frame ( ) {return this;}
	virtual void		on_activate						( );
	virtual void		on_deactivate					( );
};

} // namespace editor
} // namespace xray

#endif // #ifndef SOLID_VISUAL_TOOL_TAB_H_INCLUDED