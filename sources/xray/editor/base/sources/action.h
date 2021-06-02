////////////////////////////////////////////////////////////////////////////
//	Created		: 22.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_ACTION_H_INCLUDED
#define EDITOR_ACTION_H_INCLUDED

namespace xray
{
	namespace editor_base
	{
		using namespace System;
		using namespace System::Collections::Generic;

		public enum class checked_state
		{
			uncheckable,
			checked,
			unchecked
		};

		ref class action_shortcut;

		public ref class action abstract
		{
		public:
			action		( );

		public:
			virtual		String^			name			( ) abstract;
			virtual		bool			enabled			( ) abstract;
			virtual		checked_state	checked_state	( ) abstract;
			virtual		bool			do_it			( ) abstract;
			virtual		bool			capture			( ) abstract;
			virtual		bool			execute			( ) abstract;
			virtual		void			release			( ) abstract;

			List<String^>^				get_action_keys_list				( );
			List<String^>^				get_action_keys_list_with_contexts	( );

		internal:
			String^						m_default_shortcut;
			List<action_shortcut^>^		m_shortcuts;
		}; // class action

	} // namespace editor_base
} // namespace xray


#endif // #ifndef EDITOR_ACTION_H_INCLUDED