////////////////////////////////////////////////////////////////////////////
//	Created		: 25.10.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "action_delegate.h"
#include "action_single.h"
#include "scene_view_panel.h"
#include <xray/editor/base/collision_object_types.h>

using namespace System;

namespace xray {
namespace editor_base{

action_delegate::action_delegate ( String^ name, execute_delegate_managed^ execute_del ):
action_single			( name ),
enabled_delegate_man	( nullptr ),
checked_delegate_man	( nullptr )
{
	ASSERT( execute_del != nullptr );
	execute_delegate_man = execute_del;
}

void action_delegate::set_enabled(enabled_delegate_managed^ d )
{
	enabled_delegate_man = d;
}

void action_delegate::set_checked(checked_delegate_managed^ d )
{
	checked_delegate_man	= d;
}

action_delegate::~action_delegate ()
{
}


mouse_action_touch_object::mouse_action_touch_object( System::String^ name, scene_view_panel^ v )
:super			( name ),
m_window_view	( v )
{}

bool mouse_action_touch_object::do_it( )
{
	if( !m_window_view->is_mouse_in_view() )
		return false;

	collision::object const* object		= NULL;

	if( m_window_view->ray_query( editor_base::collision_object_type_touch, &object, NULL ) )
	{
		return  object->touch( );
	}else
		return false;
}


} // namespace editor	
} // namespace xray
