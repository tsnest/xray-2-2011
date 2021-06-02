////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEPENDENCY_EDITOR_H_INCLUDED
#define DEPENDENCY_EDITOR_H_INCLUDED

#include "dependency_editor_type.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray
{
	namespace editor
	{
		namespace controls
		{
			/// <summary>
			/// Summary for dependency_editor
			/// </summary>
			public ref class dependency_editor abstract : public System::Windows::Forms::UserControl
			{
			
			#pragma region | Initialize |

			public:
				dependency_editor(dependency_editor_type set_editor_type):
				  m_editor_type(set_editor_type)
				{
					InitializeComponent();
					//
					//TODO: Add the constructor code here
					//
					values_changed = gcnew EventHandler(this, &dependency_editor::values_processor);
					m_loading = false;
				}

			protected:
				/// <summary>
				/// Clean up any resources being used.
				/// </summary>
				~dependency_editor()
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
		
			#pragma region Windows Form Designer generated code
				/// <summary>
				/// Required method for Designer support - do not modify
				/// the contents of this method with the code editor.
				/// </summary>
				void InitializeComponent(void)
				{
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				}
			#pragma endregion

			#pragma endregion

			#pragma region |   Fields   |

			private:
				dependency_editor_type		m_editor_type;
				list_of_float^				m_selected_values;

			public:
				Boolean						m_loading;

			#pragma endregion

			#pragma region | Properties |

			public:
				property dependency_editor_type		editor_type
				{
					dependency_editor_type	get()
					{
						return m_editor_type;
					}
				}
			
				property list_of_float^				selected_values
				{
					list_of_float^			get(){return m_selected_values;} 
					void					set(list_of_float^ values)
					{
						if(values->Count == 0)
							values->AddRange(this->values);
						else
							this->values = values;
						m_selected_values = values;
					}
				}

			protected:
				property list_of_float^				values
				{
					virtual list_of_float^	get() abstract; 
					virtual void			set(list_of_float^ value)abstract;
				}

				EventHandler^						values_changed;

			#pragma endregion 

			#pragma region |  Methods  |

			protected:
				void values_processor(Object^ sender, EventArgs^ e);

			#pragma endregion 

			};//class dependency_editor
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef DEPENDENCY_EDITOR_H_INCLUDED#pragma once