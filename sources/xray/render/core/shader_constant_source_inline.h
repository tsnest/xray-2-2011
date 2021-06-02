////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_SOURCE_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_SOURCE_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline shader_constant_source::shader_constant_source	( pvoid const pointer, u32 const size ) :
	m_pointer	( pointer ),
	m_size		( size )
{
}

inline shader_constant_source::shader_constant_source	( shader_constant_source const& other ) :
	m_pointer	( other.pointer() ),
	m_size		( other.size() )
{
}

inline shader_constant_source& shader_constant_source::operator =			( shader_constant_source const& other )
{
	this->~shader_constant_source		( );
	new (this) shader_constant_source	( other );
	return		*this;
}

inline pvoid shader_constant_source::pointer			( ) const
{
	return		m_pointer;
}

inline u32 shader_constant_source::size					( ) const
{
	return		m_size;
}

inline bool operator ==									(
		shader_constant_source const& left,
		shader_constant_source const& right
	)
{
	return		(left.pointer() == right.pointer()) && (left.size() == right.size());
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_SOURCE_INLINE_H_INCLUDED