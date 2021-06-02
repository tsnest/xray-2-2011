////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_ENGINE_H_INCLUDED
#define INPUT_ENGINE_H_INCLUDED

#include "input_action.h"

namespace xray {

namespace configs{
	class lua_config_value;
} // configs

namespace editor {

class editor_world;

ref class input_action_base;

public ref class input_engine
{
public:
			input_engine( editor_world& world );
	virtual	~input_engine( );
			!input_engine( );

			// Registers an input action without assigning to it any key;
			void register_action	( xray::editor::input_action^  action );

			// Registers an input action with keys combination
			void register_action	( xray::editor::input_action^  action, System::String^ keys );

			// Unregisters an input action with the specified name. All shortcuts will be removed.
			bool unregister_action	( System::String^ name, bool destroy );

			// Executes input on (each frame).
			void execute			( );

			// Pauses input handling by input actions. 
			void stop				( );

			// Resumes input handling by input actions. 
			void resume				( );

			// Creates a combination string based on current held keys, and returns led keys count. 
			// Modifier keys are not included into keys count.
//			int	get_current_keys_string	( System::String^& keys);


			System::Collections::Generic::List<System::String^>^ get_action_keys_list	( System::String^ name );

			// 
			bool add_action_shortcut	( System::String^ name, System::String^ combination );
			bool remove_action_shortcut	( System::String^ name, System::String^ combination );
			void remove_all_shortcuts	( );
			
			bool get_actions_by_shortcut(	System::String^ combination, 
											System::Collections::Generic::List<System::String^>^& actions, 
											System::Collections::Generic::List<System::String^>^& shortcuts );

			bool get_actions_by_shortcut(	System::String^ combination, 
											System::Collections::Generic::List<System::String^>^& actions );

			// 
			System::Collections::ArrayList^ get_public_actions	( );



			void load					( /*xray::configs::lua_config_value const& cfg, bool reset_all */);
			void save					( /*xray::configs::lua_config_value const& cfg*/ );
private:
			//
			void excract_shortcut_pair	( System::String^ combination, System::String^& first_keys, System::String^& second_keys );

			// Arranges keys in a single string in a correct order.
			System::String^					arrange_keys		( System::String^ keys_string );

			// Tries to convert a single string into a Keys enum.
	inline	System::Windows::Forms::Keys	try_convert			( System::String^ str );

			// Combines keys and modifiers into a arranged string.
	inline	System::String^					combine_string		( System::Windows::Forms::Keys modifiers, System::Collections::ArrayList^ keys );

			// Extracts modifiers keys (Alt, Control, Shift) and combines into a Windows::Forms::Keys enum.
//	inline	void							extract_modifyer_keys	( System::Collections::ArrayList^ keys, System::Windows::Forms::Keys %modifiers );


private:

	ref class action_item
	{
	public:
		action_item( 	xray::editor::input_action^	action,	
						System::String^	first_keys,
						System::String^	second_keys )
		{
			m_action = action;
			m_first_keys = first_keys;
			m_second_keys = second_keys;
		}

		inline System::String^ joined_keys( )
		{
			if( m_second_keys == nullptr)
				return m_first_keys;
			else
				return m_first_keys + ", " + m_second_keys; 
		}
	
		System::String^	m_first_keys;
		System::String^	m_second_keys;
		xray::editor::input_action^	m_action;
	};

private:

	editor_world&							m_world;
	System::Collections::Hashtable			m_keys_to_items;
	System::Collections::Hashtable			m_names_to_items;
	System::Windows::Forms::KeysConverter^  m_key_converter;
	System::String^							m_active_combination;
	bool									m_waiting_for_second;
	bool									m_first_keys_released;
	bool									m_paused;
	int										m_keys_count;
	System::Collections::Generic::List<action_item^>^	m_active_list;
	int										m_active_action_index;
	
}; // class input_engine


} // namespace editor
} // namespace xray



#endif // #ifndef INPUT_ENGINE_H_INCLUDED