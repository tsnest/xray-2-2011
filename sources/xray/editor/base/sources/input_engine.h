////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_ENGINE_H_INCLUDED
#define INPUT_ENGINE_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;
using Microsoft::Win32::RegistryKey;
using System::Windows::Forms::Keys;
typedef System::Runtime::InteropServices::OutAttribute Out;

namespace xray
{
	namespace editor_base
	{
		ref class			input_keys_holder;
		interface class		i_context_manager;

		ref class			action;
		ref class			action_shortcut;
		ref class			action_single;
		ref class			action_continuous;
		ref class			actions_layer;

		public ref class input_engine
		{
		public:
						input_engine	( input_keys_holder^ key_holder, i_context_manager^ context_manager );
			virtual		~input_engine	( );

		public:
			property i_context_manager^				contexts_manager		{ i_context_manager^	get( ) { return m_context_manager; } }
			property input_keys_holder^				keys_holder				{ input_keys_holder^	get( ) { return m_input_key_holder; } }

		public:
			void						execute						( );
			void						execute_action				( String^ name );
			void						execute_action				( action^ action );
			void						stop						( );
			void						resume						( );
				
			void						register_action				( action^ action, String^ shorcut );
			void						unregister_action			( String^ name );
			bool						action_exists				( String^ name );
			action^						get_action					( String^ name );
			List<String^>^				get_actions_names_list		( );
			List<String^>^				get_action_keys_list				( String^ name );
			List<String^>^				get_action_keys_list_with_contexts	( String^ name );

			bool						add_action_shortcut			( String^ name, String^ combination );
			bool						remove_action_shortcut		( String^ name, String^ combination );
			void						reset_to_defaults			( );
			
			void						add_actions_layer			( actions_layer^ layer );
			void						remove_actions_layer		( actions_layer^ layer );
			void						activate_actions_layer		( actions_layer^ layer );
			void						deactivate_actions_layer	( actions_layer^ layer );

			//bool						get_actions_by_shortcut		( String^ combination, [Out] List<String^>^% actions, [Out] List<String^>^% shortcuts );
			//bool						get_actions_by_shortcut		( String^ combination, [Out] List<String^>^% actions );

			void						load						( RegistryKey^ base_product_key );
			void						save						( );

		private:
			bool						process_combination			( String^ combination );
			bool						is_correct_settings_value	( String^ value );

		private:
			RegistryKey^				m_registry_key;
			input_keys_holder^			m_input_key_holder;
			i_context_manager^			m_context_manager;

			actions_layer^				m_default_actions_layer;
			List<actions_layer^>^		m_action_layers;
			List<actions_layer^>^		m_active_action_layers;
			String^						m_active_shortcut;
			List<action_shortcut^>^		m_active_shortcuts;
			Int32						m_active_shortcut_index;

			bool						m_waiting_for_second_combination;
			bool						m_first_keys_released;
			bool						m_paused;
			
		}; // class input_engine

	} // namespace editor_base
} // namespace xray

#endif // #ifndef INPUT_ENGINE_H_INCLUDED