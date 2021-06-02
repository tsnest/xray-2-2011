////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BUFFER_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_CONSTANT_BUFFER_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/render/core/res_common.h>
#include <xray/render/core/shader_constant_slot.h>
#include <xray/render/core/shader_constant_defines.h>
#include <xray/render/core/shader_constant.h>

namespace xray {
namespace render {

enum enum_shader_type;

//template <class T, u32 limit>
//class t_cache
//{
//public:
//	t_cache() { m_array.resize( limit); flush();}
//
//	T*		access	( u32 id) { return &m_array[id];}
//
//	void	flush	() { m_low = m_hi = 0;}
//	void	dirty	( u32 low, u32 hi) {if( low < m_low) m_low = low; if( hi > m_hi) m_hi=hi;}
//
//	T*		access_dirty()	{ return access( m_low);}
//	u32		dirty_size	()	{ return m_hi-m_low;}
//
//	u32		low	()	{ return m_low;}
//	u32		hi	()	{ return m_hi;}
//
//private:
//	XRAY_ALIGN( 16)	fixed_vector<T,limit>	m_array;
//	u32										m_low, m_hi;
//
//}; // t_cache

class shader_constant;
class shader_constant_slot;

class shader_constant_buffer : public resource_intrusive_base {
protected:
	friend class resource_manager;
						shader_constant_buffer	( name_string_type const& name, enum_shader_type dest, D3D_CBUFFER_TYPE type, u32 size );
						~shader_constant_buffer	( );

public:
			void					destroy_impl( ) const;

public:
	inline	void					set			( shader_constant const & c);
	inline	void					zero		( shader_constant const & c);

	template < typename T > 
	inline	void					set_typed	( shader_constant_slot const& slot, T const& value);

	template < typename T > 
	inline	void					set_typed	( shader_constant_slot const& slot, T const* value, u32 array_size );
			bool					similiar	( shader_constant_buffer const& other ) const;
			void					update		( );
	inline	ID3DConstantBuffer*	hardware_buffer	( ) const;

private:
	inline	void					set			( shader_constant_slot const & c, void * ptr, u32 size);
	inline	void					set			( shader_constant_slot const & c, void * ptr, u32 size, u32 array_size);

	inline	void					zero		( shader_constant_slot const & c, u32 size);

	inline	void					set_memory	( const u32 offset, char const * src_ptr, const u32 size);
	inline	void					zero_memory	( const u32 offset, const u32 size);

	inline	pvoid					access		( u32 offset );

public:
	inline	name_string_type const&	name			( )	const;
	inline	enum_shader_type &		dest			( );
	inline	D3D_CBUFFER_TYPE		type			( )	const;
	inline	u32						size			( )	const;
	inline bool						is_registered	( ) const	{ return m_is_registered; }
	inline void						mark_registered	( )			{ R_ASSERT( !m_is_registered); m_is_registered = true; }

	enum {
		line_size	= 1//4*sizeof(float),
	};//;;

private:
	//	typedef		t_cache<float4, 256>	float_cache;
	// Will be used later
	//typedef		t_cache<int4,16>	t_i;
	//typedef		t_cache<bool,16>	t_b;
	//	XRAY_ALIGN(16) float_cache			m_float_cache;
	// Will be used later
	//	XRAY_ALIGN(16)	t_i					c_i;
	//	XRAY_ALIGN(16)	t_b					c_b;

	name_string_type			m_name;
	D3D_CBUFFER_TYPE			m_type;
	enum_shader_type			m_dest;
	pvoid						m_buffer_data;
 	u32							m_buffer_size;	//	Cache buffer size for debug validation
	ID3DConstantBuffer*			m_hardware_buffer;
  	bool						m_changed;
	bool						m_is_registered;
}; // shader_constant_buffer

typedef intrusive_ptr<
	shader_constant_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> shader_constant_buffer_ptr;

typedef intrusive_ptr<
	shader_constant_buffer const,
	resource_intrusive_base,
	threading::single_threading_policy
> shader_constant_buffer_const_ptr;

} // namespace render
} // namespace xray

#include <xray/render/core/shader_constant_buffer_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_SHADER_CONSTANT_BUFFER_H_INCLUDED