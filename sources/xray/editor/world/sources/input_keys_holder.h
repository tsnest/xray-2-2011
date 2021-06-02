////////////////////////////////////////////////////////////////////////////
//	Created		: 21.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_KEYS_HOLDER_H_INCLUDED
#define INPUT_KEYS_HOLDER_H_INCLUDED

namespace xray {
namespace editor {

public delegate void keys_combination_changed ( System::String^ status );

public ref class input_keys_holder 
{
public:
	void		register_key_event				( System::Windows::Forms::KeyEventArgs^  e, bool bpress );
	void		register_mouse_event			( System::Windows::Forms::MouseEventArgs^ e, bool bpress );
	void		reset							( );
	System::Collections::ArrayList^	get_held_keys			( );
	int			get_current_keys_string			( System::String^& combination );
	void		subscribe_on_changed			( keys_combination_changed^ d );
private:
	System::String^			combine_string		( System::Windows::Forms::Keys modifiers, System::Collections::ArrayList^ keys );
	void					combination_changed	( );
	System::Collections::ArrayList				m_held_keys;
	System::Windows::Forms::KeysConverter		m_key_converter;

	event keys_combination_changed^ m_changed_delegate;
}; // class input_keys_holder

} // namespace editor
} // namespace xray

#endif // #ifndef INPUT_KEYS_HOLDER_H_INCLUDED