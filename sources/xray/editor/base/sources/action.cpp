////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "action.h"
#include "action_shortcut.h"

namespace xray{
namespace editor_base{

action::action ( )
{
	m_shortcuts = gcnew List<action_shortcut^>( );
}

List<String^>^ action::get_action_keys_list ( )
{
	if( ( m_shortcuts == nullptr) || ( m_shortcuts->Count == 0 ) || ( m_shortcuts[0]->m_first_keys == nullptr ) )
		return gcnew List<String^>( );
	
	List<String^>^ result = gcnew List<String^>( m_shortcuts->Count );

	for( int i = 0; i< m_shortcuts->Count; ++i )
		result->Add( m_shortcuts[i]->joined_keys( ) );

	return result;
}

List<String^>^ action::get_action_keys_list_with_contexts ( )
{
	if( !m_shortcuts || ( m_shortcuts->Count == 0 ) || ( m_shortcuts[0]->m_first_keys == nullptr ) )
		return gcnew List<String^>( );

	List<String^>^ result = gcnew List<String^>( m_shortcuts->Count );

	for( s32 i = 0; i < m_shortcuts->Count; ++i )
		result->Add( m_shortcuts[i]->joined_keys_with_context( ) );

	return result;
}

} // namespace editor_base
} // namespace xray