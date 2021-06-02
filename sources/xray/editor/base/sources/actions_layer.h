////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ACTIONS_LAYER_H_INCLUDED
#define ACTIONS_LAYER_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;
typedef System::Runtime::InteropServices::OutAttribute Out;

namespace xray
{
	namespace editor_base
	{
		ref class action;
		ref class action_shortcut;

		public ref class actions_layer
		{
		public:
			actions_layer( );
			actions_layer( u32 capacity );

		public:
			initonly	Dictionary<String^, action^>^		m_action_registry;

		public:
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

			bool						get_actions_by_shortcut		( String^ combination, [Out] List<String^>^% actions, [Out] List<String^>^% shortcuts );
			bool						get_actions_by_shortcut		( String^ combination, [Out] List<String^>^% actions );

		internal:
			List<action_shortcut^>^		find_action_shortcuts_by_first_keys	( String^ first_keys );

		}; // class actions_layer
	} // namespace editor_base
} // namespace xray

#endif // #ifndef ACTIONS_LAYER_H_INCLUDED