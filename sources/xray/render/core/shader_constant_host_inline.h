////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_HOST_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_HOST_INLINE_H_INCLUDED

namespace xray {
namespace render {

shader_constant_host::shader_constant_host							( shared_string const& name, enum_constant_type const type ) : 
	m_source			( 0, 0 ),
	m_name				( name ),
	m_type				( type )
{
	reset_update_markers( );
}

inline void shader_constant_host::reset_update_markers				( )
{
	memory::zero		( m_update_markers, sizeof(m_update_markers) );
}

inline shader_constant_source const& shader_constant_host::source	( ) const
{
	return				m_source;
}

inline shader_constant_slot& shader_constant_host::shader_slot( u32 const slot_id )
{
	R_ASSERT_CMP		( slot_id, <, 3 );
	return				m_shader_slots[ slot_id ];
}

inline shader_constant_slot const& shader_constant_host::shader_slot( u32 const slot_id ) const
{
	R_ASSERT_CMP		( slot_id, <, 3 );
	return				m_shader_slots[ slot_id ];
}

inline shared_string const& shader_constant_host::name				( ) const
{
	return				m_name;
}

inline enum_constant_type shader_constant_host::type				( ) const
{
	return				m_type;
}

inline bool operator ==												( shader_constant_host const& left, shared_string const& right )
{ 
	return				left.name() == right;
}

inline bool	operator ==												( shared_string const& left, shader_constant_host const& right )
{ 
	return				left == right.name();
}

inline bool	operator <												( shader_constant_host const& left, shared_string const& right )
{ 
	return				left.name() < right;
}

inline bool	operator <												( shared_string const& left, shader_constant_host const& right )
{ 
	return				left < right.name();
}

inline bool	operator ==												( shader_constant_host const& left, shader_constant_host const& right )
{ 
	return				left.name() == right.name();
}

inline bool	operator <												( shader_constant_host const& left, shader_constant_host const& right )
{ 
	return				left.name() < right.name();
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_HOST_INLINE_H_INCLUDED