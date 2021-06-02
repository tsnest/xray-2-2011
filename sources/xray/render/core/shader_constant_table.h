////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_TABLE_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_TABLE_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/render/core/shader_constant.h>
#include <xray/render/core/shader_constant_buffer.h>

namespace xray {
namespace render {

class shader_constant_bindings;

class shader_constant_buffer;
typedef intrusive_ptr<
	shader_constant_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> shader_constant_buffer_ptr;


class shader_constant_table : public resource_intrusive_base {
public:
	typedef render::vector< shader_constant_buffer_ptr >	c_buffers;
	typedef render::vector< shader_constant >				c_table;

public:
	inline shader_constant_table() : m_is_registered( false ) {}
	inline shader_constant_table( shader_constant_table const& other ) :
		m_table( other.m_table ),
		m_const_buffers( other.m_const_buffers ),
		m_is_registered( false )
	{
	}

	inline void mark_registered() { R_ASSERT( !m_is_registered ); m_is_registered = true; }
	void destroy_impl() const;

	void clear();

	bool parse( ID3DShaderReflection* shader_reflection, enum_shader_type type);

	void apply_bindings( shader_constant_bindings const& bindings);
	
	shader_constant const * get( LPCSTR const name) const;			// slow search ???
	shader_constant * get( shared_string const& name) ;	// fast search ???

	bool equal( shader_constant_table const& other) const;
	bool empty() const {return 0 == m_table.size();}
	bool parse_constant_buffer( ID3DShaderReflectionConstantBuffer* pTable, u32 buffer_index);

	bool is_registered() const { return m_is_registered;}

private:
	shader_constant * get( LPCSTR const name);			// slow search ???
	
	// This need to be moved into an utility file/namespace
	inline void fatal( LPCSTR s);

//protected:
public:
	c_table					m_table;
	c_buffers				m_const_buffers;
	bool					m_is_registered;
}; // class shader_constant_table

typedef intrusive_ptr<
	shader_constant_table,
	resource_intrusive_base,
	threading::single_threading_policy
> shader_constant_table_ptr;

typedef intrusive_ptr<
	shader_constant_table const,
	resource_intrusive_base const,
	threading::single_threading_policy
> shader_constant_table_const_ptr;

} // namespace render
} // namespace xray

#include <xray/render/core/shader_constant_table_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_TABLE_H_INCLUDED