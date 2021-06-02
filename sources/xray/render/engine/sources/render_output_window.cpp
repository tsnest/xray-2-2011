////////////////////////////////////////////////////////////////////////////
//	Created		: 20.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_output_window.h"
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/device.h>
#include <xray/render/core/render_target.h>

namespace xray {
namespace render {

render_output_window::render_output_window( HWND window):
	m_targets		( get_window_client_size(window) ),
	m_window		( window),
	m_output		( resource_manager::ref().create_render_output( window, true) ),
	m_current_size	( get_window_client_size(window) )
{
}

void render_output_window::resize		( )
{
	math::uint2 const& new_size = get_window_client_size(m_window);
	if ( (new_size.x == 0) || (new_size.y == 0) )
		return;

	if ( (new_size.x == m_current_size.x) && (new_size.y == m_current_size.y) )
		return;

	m_current_size	= new_size;
	m_targets.resize( new_size );
	m_output->resize( );
}

math::uint2	render_output_window::get_window_client_size( HWND const window )
{
	RECT			rect;
	if ( GetClientRect( window, &rect) )
		return		math::uint2(rect.right-rect.left, rect.bottom-rect.top);

	return			math::uint2( 0, 0 );
}

} // namespace render
} // namespace xray