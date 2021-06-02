////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BINDING_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_BINDING_H_INCLUDED

#include <xray/render/core/shader_constant_source.h>
#include <xray/render/core/shader_constant_defines.h>

namespace xray {
namespace render {

class shader_constant_binding {//: private boost::noncopyable {
public:
	template < typename T >
	inline					shader_constant_binding		( shared_string const& name, T const* source );
	inline					shader_constant_binding		( shader_constant_binding const& other );
	inline	shader_constant_binding&		operator =	( shader_constant_binding const& other );
	inline	shared_string const&			name		( )	const;
	inline	shader_constant_source const&	source		( )	const;
	inline	enum_constant_class				class_id	( )	const;
	inline	enum_constant_type				type		( ) const;

private:
	shader_constant_source	m_source;	
	shared_string			m_name;
	enum_constant_type		m_type;
	enum_constant_class		m_class_id;
}; // class shader_constant_binding

inline	bool	operator ==								( shader_constant_binding const& left, shader_constant_binding const& right );

} // namespace render
} // namespace xray

#include <xray/render/core/shader_constant_binding_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BINDING_H_INCLUDED