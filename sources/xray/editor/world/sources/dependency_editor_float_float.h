////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEPENDENCY_EDITOR_FLOAT_FLOAT_H_INCLUDED
#define DEPENDENCY_EDITOR_FLOAT_FLOAT_H_INCLUDED

#include "dependency_editor.h"
#include "dependency_editor_type.h"
#include "float_curve.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray
{
	namespace editor
	{
		ref class	float_curve_event_args;

		namespace controls
		{
			ref class	float_curve_editor;

			/// <summary>
			/// Summary for dependency_editor_float_float
			/// </summary>
			public ref class dependency_editor_float_float : public dependency_editor
			{

			#pragma region | Initialize |

			public:
				dependency_editor_float_float(void):
				  dependency_editor(dependency_editor_type::flot_to_float)
				{
					InitializeComponent();
					//
					//TODO: Add the constructor code here
					//
					in_constructor();
				}

			protected:
				/// <summary>
				/// Clean up any resources being used.
				/// </summary>
				~dependency_editor_float_float()
				{
					if (components)
					{
						delete components;
					}
				}

			private:
				void in_constructor();

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

			#pragma region |  Fields  |

				controls::float_curve_editor^		m_curve_editor;

			#pragma endregion 

			#pragma region | Properties |

			protected:
				property list_of_float^	values
				{
					virtual	list_of_float^	get		() override;
					virtual	void			set		(list_of_float^ value) override; 
				}
			
			#pragma endregion

			#pragma region |  Methods  |

			private:
				void curve_changed(Object^ sender, float_curve_event_args^ e);

			#pragma endregion

			};//class dependency_editor_float_float
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef DEPENDENCY_EDITOR_FLOAT_FLOAT_H_INCLUDED