////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_WORLD_INLINE_H_INCLUDED
#define EDITOR_WORLD_INLINE_H_INCLUDED

#pragma managed( push, off )
#include <xray/editor/world/api.h>
#include <xray/editor/world/engine.h>

namespace xray {
namespace editor {

inline xray::editor::engine& editor_world::engine			( )
{
	return	( m_engine );
}

inline gcroot<window_ide^> editor_world::ide	( ) const
{
	return	( m_window_ide );
}

render::editor::renderer&		editor_world::get_renderer			( )
{
	return m_engine.get_renderer_world().editor_renderer( );
}

//float4x4 editor_world::get_projection_matrix	( )
//{
//	return m_projection;
//}
//
//void editor_world::set_projection_matrix	( math::float4x4 const& matrix )
//{
//	m_projection = matrix;
//}
//
//float4x4 editor_world::get_inverted_view_matrix		( )
//{
//	return m_inverted_view;
//}

#pragma managed( pop )

} // namespace editor
} // namespace xray




#endif // #ifndef EDITOR_WORLD_INLINE_H_INCLUDED