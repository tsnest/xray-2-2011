////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_SOURCE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_SOURCE_H_INCLUDED

namespace xray {
namespace render {

class shader_constant_source {//: private boost::noncopyable {
public:
	inline			shader_constant_source		( pvoid pointer, u32 size );
	inline			shader_constant_source		( shader_constant_source const& other );
	inline	shader_constant_source&	operator =	( shader_constant_source const& other );
	inline	pvoid	pointer						( ) const;
	inline	u32		size						( ) const;

private:
	pvoid const		m_pointer;
	u32 const		m_size;
}; // class shader_constant_source

	inline	bool	operator ==				( shader_constant_source const& left, shader_constant_source const& right );

} // namespace render
} // namespace xray

#include <xray/render/core/shader_constant_source_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_SOURCE_H_INCLUDED