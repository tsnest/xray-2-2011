////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_CONSTANT_BUFFER_H_INCLUDED
#define RES_CONSTANT_BUFFER_H_INCLUDED

#include "res_common.h"
#include "constant.h"

namespace xray {
namespace render_dx10 {

template <class T, u32 limit>
class t_cache
{
public:
	t_cache() { m_array.resize( limit); flush();}

	T*		access	( u32 id) { return &m_array[id];}

	void	flush	() { m_low = m_hi = 0;}
	void	dirty	( u32 low, u32 hi) {if( low < m_low) m_low = low; if( hi > m_hi) m_hi=hi;}

	T*		access_dirty()	{ return access( m_low);}
	u32		dirty_size	()	{ return m_hi-m_low;}

	u32		low	()	{ return m_low;}
	u32		hi	()	{ return m_hi;}

private:
	XRAY_ALIGN( 16)	fixed_vector<T,limit>	m_array;
	u32										m_low, m_hi;

}; // t_cache

class res_constant_buffer: public res_flagged
{
protected:
	friend class resource_manager;
	res_constant_buffer		( name_string const & name, enum_shader_type dest, D3D_CBUFFER_TYPE type, u32 size);

public: // This should be private 
	~res_constant_buffer	();
	void _free();

public:
//	typedef		t_cache<float4, 256>	float_cache;
	// Will be used later
	//typedef		t_cache<int4,16>	t_i;
	//typedef		t_cache<bool,16>	t_b;

public:
//	float_cache& get_float_cache() { return m_float_cache;}

	// --Porting to DX10_
//	bool is_dirty() { return m_float_cache.hi()>0;}

	inline void set		( constant const& c);
	inline void set_a	( constant const& c, u32 l);

	inline void zero	( constant const& c);

	template<typename T> 
	inline void set		( constant_slot const& slot, T const& value);

//	inline void set		( constant const& c, constant_slot const& s, void* source_ptr, u32 source_size);
//	inline void set_a	( constant const& c, constant_slot const& s, u32 l, void* source_ptr, u32 source_size);
// 	inline void set	( constant const& c, constant_slot const& l, const float4x4& a);
// 	inline void set	( constant const& c, constant_slot const& l, const float4& a);
// 	inline void seta	( constant const& c, constant_slot const& l, u32 e, const float4x4& a);
// 	inline void seta	( constant const& c, constant_slot const& l, u32 e, const float4& a);

	bool similiar( res_constant_buffer & other);

	
	void update();
	ID3DConstantBuffer*			get_hw_buffer()	 { return m_hw_buffer; }

private:

	inline void set		( constant_slot const& c, void* ptr, u32 size);
	inline void zero	( constant_slot const& c, u32 size);
	inline void set_a	( constant_slot const& c, u32 l, void* ptr, u32 size);

	inline void set_memory( const u32 offset, char const * src_ptr, const u32 size);
	inline void zero_memory( const u32 offset, const u32 size);

	inline void* access( u32 offset);

	name_string	const&			name()	{ return m_name;}
	enum_shader_type&		dest()	{ return m_dest;}
	D3D_CBUFFER_TYPE			type()	{ return m_type;}
	u32							size()	{ return m_buffer_size;}

	static const u32 line_size	= 1;//4*sizeof(float);

public:

//	XRAY_ALIGN(16) float_cache			m_float_cache;
	// Will be used later
	//	XRAY_ALIGN(16)	t_i					c_i;
	//	XRAY_ALIGN(16)	t_b					c_b;

	name_string							m_name;
	D3D_CBUFFER_TYPE					m_type;
	enum_shader_type				m_dest;
	void*								m_buffer_data;
 	u32									m_buffer_size;	//	Cache buffer size for debug validation
	ID3DConstantBuffer*					m_hw_buffer;
  	bool								m_changed;

}; // res_constant_buffer

typedef intrusive_ptr<res_constant_buffer, res_base, threading::single_threading_policy>	ref_constant_buffer;

} // namespace render
} // namespace xray



#include "res_constant_buffer_inline.h"

#endif // #ifndef RES_CONSTANT_BUFFER_H_INCLUDED