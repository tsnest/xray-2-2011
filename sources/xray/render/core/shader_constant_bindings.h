////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BINDINGS_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_BINDINGS_H_INCLUDED

#include <xray/render/core/shader_constant_binding.h>

namespace xray {
namespace render {

class shader_constant_binding;

class shader_constant_bindings : private boost::noncopyable {
public:
	typedef render::vector< shader_constant_binding >	bindings_type;

public:
	inline	void					add		( shader_constant_binding const& binding );
	inline	void					clear	( );
	inline	bindings_type const&	bindings( ) const;

private:
	bindings_type	m_bindings;
};

} // namespace render
} // namespace xray

#include <xray/render/core/shader_constant_bindings_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BINDING_H_INCLUDED