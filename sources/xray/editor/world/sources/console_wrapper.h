////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CONSOLE_WRAPPER_H_INCLUDED
#define CONSOLE_WRAPPER_H_INCLUDED


namespace xray {
namespace engine{class console;}
namespace editor {

ref class console_wrapper : public xray::editor_base::input_handler
{
public:
				console_wrapper		( );
	void		on_activate			( editor_base::input_keys_holder^ key_holder );
	void		on_deactivate		( editor_base::input_keys_holder^ key_holder );
	bool		get_active			( );
	void		tick				( );

	xray::engine::console*				m_console;
	editor_base::input_keys_holder^		m_active_key_holder;

	virtual bool	on_key_up			( System::Windows::Forms::KeyEventArgs^ k );
	virtual bool	on_key_down			( System::Windows::Forms::KeyEventArgs^ k );
}; // class console_wrapper

} // namespace editor
} // namespace xray

#endif // #ifndef CONSOLE_WRAPPER_H_INCLUDED