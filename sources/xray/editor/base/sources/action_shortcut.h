////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_SHORTCUT_H_INCLUDED
#define ACTION_SHORTCUT_H_INCLUDED

using namespace System;
using System::Windows::Forms::Keys;
using System::Collections::ArrayList;
using System::Windows::Forms::KeysConverter;

namespace xray
{
	namespace editor_base
	{
		ref class action;

		ref class action_shortcut
		{
		public:
			action_shortcut( action^ action, String^ combinations );

		public:
			String^			joined_keys					( );
			String^			joined_keys_with_context	( );

		private:
			void			excract_shortcut_pair		( String^ combination, String^% keys_first, String^% keys_second, String^% context );
			String^			arrange_keys				( String^ keys_string );
			Keys			try_convert					( String^ str );
			String^			combine_string				( Keys modifiers, ArrayList^ keys );
			
		public:
			initonly action^	m_action;
			initonly String^	m_first_keys;
			initonly String^	m_second_keys;
			initonly String^	m_context;
			initonly String^	m_combination;

		private:
			static KeysConverter	s_key_converter;

		}; // class action_shortcut

	} // namespace editor
} // namespace xray

#endif // #ifndef ACTION_SHORTCUT_H_INCLUDED