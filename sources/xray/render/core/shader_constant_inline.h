////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline shader_constant::shader_constant						( shader_constant_host const& host ) :
	m_host		( host ),
	m_source	( 0, 0 )
{
//	ASSERT		( host.type != rc_INVALID);
}

inline shader_constant::shader_constant						( shader_constant const& other ) :
	m_slot		( other.slot() ),
	m_source	( other.source() ),
	m_host		( other.host() )
{
}

inline shader_constant& shader_constant::operator =			( shader_constant const& other )
{
	this->~shader_constant( );
	new (this) shader_constant ( other );
	return		*this;
}

inline shader_constant_slot const& shader_constant::slot	( ) const
{
	return		m_slot;
}

inline shader_constant_source const& shader_constant::source( ) const
{
	return		m_source;
}

inline shader_constant_host const& shader_constant::host	( ) const
{
	return		m_host;
}

/// temporarily only!
inline shader_constant_host& shader_constant::get_host		( ) const
{
	return		const_cast<shader_constant_host&>( m_host );
}

inline void shader_constant::set_source						( shader_constant_source const& source )
{
	m_source.~shader_constant_source		( );
	new (&m_source) shader_constant_source	( source );
}

inline bool operator ==										( shader_constant const& left, shader_constant const& right )
{
	return
		( left.host() == right.host() ) &&
		( left.source() == right.source() ) &&
		( left.slot() == right.slot() );
}

inline bool operator !=										( shader_constant const& left, shader_constant const& right )
{
	return		!(left == right);
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_INLINE_H_INCLUDED