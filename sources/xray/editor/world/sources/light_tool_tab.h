////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHT_TOOL_TAB_H_INCLUDED
#define LIGHT_TOOL_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "tool_tab.h"

namespace xray {
namespace editor {
ref class tool_light;
ref class library_objects_tree;
/// <summary>
	/// Summary for light_tool_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class light_tool_tab :	public System::Windows::Forms::UserControl, 
										public tool_tab
	{
	public:
		light_tool_tab(tool_light^ t):m_tool(t)
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
		~light_tool_tab()
		{
			if (components)
			{
				delete components;
			}
		}
	private: tool_light^	m_tool;

	protected: 


















	private: System::Windows::Forms::Panel^  objects_tree_place;


	private: 

	private: System::ComponentModel::IContainer^  components;
	public: 
	private: 

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
			this->objects_tree_place->Size = System::Drawing::Size(212, 390);
			this->objects_tree_place->TabIndex = 0;
			// 
			// light_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->objects_tree_place);
			this->Name = L"light_tool_tab";
			this->Size = System::Drawing::Size(212, 390);
			this->ResumeLayout(false);

		}
#pragma endregion
	void	post_constructor							( );
	library_objects_tree^	m_library_objects_tree;
	public:
			void		fill_objects_list				( );
	virtual System::Windows::Forms::UserControl^	frame ( ) {return this;}
	virtual void		on_activate						( );
	virtual void		on_deactivate					( );
};

} // namespace editor
} // namespace xray

#endif // #ifndef LIGHT_TOOL_TAB_H_INCLUDED