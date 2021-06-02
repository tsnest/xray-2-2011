////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GUI_INPUT_H_INCLUDED
#define GUI_INPUT_H_INCLUDED


namespace xray {
namespace editor {


public ref class gui_input_ abstract{
public:

	virtual System::Collections::ArrayList^			get_held_keys				( );
	virtual System::Windows::Forms::MouseButtons	get_mouse_buttons			( );
	virtual System::Drawing::Point					get_mouse_position			( );
	virtual System::Drawing::Size					get_view_size				( );

}; // class gui_input
	
}//namespace editor 
}//namespace xray 
	




#endif // #ifndef GUI_INPUT_H_INCLUDED