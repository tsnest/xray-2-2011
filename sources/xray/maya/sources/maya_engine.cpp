////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "maya_engine.h"
#include <xray/fs_path.h>
#include <xray/fs_utils.h>

namespace xray {
namespace maya{


maya_engine::maya_engine( )
:animation_world		( 0 ), 
maya_animation_world	( 0 ),
m_is_scene_loading		( true )
{
	assign_resource_path( );
}

pcstr	maya_engine::get_resources_path( )const			
{ 
	return m_resources_path; 
}

pcstr maya_engine::get_mounts_path( )const			
{ 
	return m_mounts_path; 
}

void maya_engine::create_physical_path	( string_path& result, pcstr resources_path, pcstr inside_resources_path ) const
{
	XRAY_UNREFERENCED_PARAMETER	( resources_path );
	strings::join				( result, m_resources_path, inside_resources_path );
}

void maya_engine::assign_resource_path( )
{
	MString					stk2_path;
	MStatus result			= MGlobal::executeCommand("getenv STK2_ROOT_PATH", stk2_path );

	stk2_path				= stk2_path.toLowerCase( );

	if(result.error() || stk2_path.length()==0)
	{
		display_warning("Variable STK2_ROOT_PATH was not defined in maya.env");
	}else
	{
		strings::join( m_resources_path, stk2_path.asChar(),	"/resources" );
		strings::join( m_mounts_path, stk2_path.asChar(),		"/mounts" );
	}
}

void maya_engine::register_callback_id( MCallbackId const& id )
{
	m_callback_ids.append( id );
}

void maya_engine::unregister_callback_id( MCallbackId const& id )
{
	for(u32 idx=0; idx<m_callback_ids.length(); ++idx)
		if(m_callback_ids[idx]==id)
		{
			m_callback_ids.remove(idx);
			break;
		}
	MMessage::removeCallback	( id );
}

void maya_engine::clear_all_callbacks( )
{
	for (unsigned int i=0; i<m_callback_ids.length(); i++ )
		MMessage::removeCallback( m_callback_ids[i] );
}

} // namespace maya
} // namespace xray