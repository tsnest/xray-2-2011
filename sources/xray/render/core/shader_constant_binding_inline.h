////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BINDING_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_BINDING_INLINE_H_INCLUDED

namespace xray {
namespace render {

template < typename T >
inline shader_constant_binding::shader_constant_binding				( shared_string const& name, T const* source ) : 
	m_name			( name ), 
	m_source		( (pvoid) source, constant_type_traits< T >::size ) ,
	m_type			( (enum_constant_type) constant_type_traits< T >::type ),
	m_class_id		( (enum_constant_class) constant_type_traits< T >::class_id )
{
	COMPILE_ASSERT	( constant_type_traits< T >::value, Only_types_described_above_are_supported_for_constant_binding );
	ASSERT			( source, "The source ptr cannot be NULL!" );
	ASSERT			( name.c_str(), "The name cannot be NULL!" );
}

inline shader_constant_binding::shader_constant_binding				( shader_constant_binding const& other ) :
	m_source		( other.source() ),
	m_name			( other.name() ),
	m_type			( other.type() ),
	m_class_id		( other.class_id() )
{
}

inline shader_constant_binding& shader_constant_binding::operator =	( shader_constant_binding const& other )
{
	this->~shader_constant_binding		( );
	new (this) shader_constant_binding	( other );
	return			*this;
}

inline shared_string const&	shader_constant_binding::name			( )	const
{
	return			m_name;
}

inline shader_constant_source const& shader_constant_binding::source( )	const
{
	return			m_source;
}

inline enum_constant_class shader_constant_binding::class_id		( )	const
{
	return			m_class_id;
}

inline enum_constant_type shader_constant_binding::type				( )	const
{
	return			m_type;
}

inline bool operator ==												( shader_constant_binding const& left, shader_constant_binding const& right )
{
	return			left.name() == right.name();
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BINDING_INLINE_H_INCLUDED