////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BINDINGS_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_BINDINGS_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline void shader_constant_bindings::clear													( )
{
	m_bindings.clear	( );
}

inline shader_constant_bindings::bindings_type const& shader_constant_bindings::bindings	( ) const
{
	return				m_bindings;
}

inline void shader_constant_bindings::add													( shader_constant_binding const& binding_to_add )
{
	bindings_type::iterator const end			= m_bindings.end( );
	bindings_type::const_iterator const found	= std::find( m_bindings.begin(), end, binding_to_add );
	//R_ASSERT_U									( found == end, "the shader constant with the specified name already was added to bindings list." );
	
	if (found == end)
		m_bindings.push_back						( binding_to_add );
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BINDINGS_INLINE_H_INCLUDED