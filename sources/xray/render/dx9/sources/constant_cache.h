////////////////////////////////////////////////////////////////////////////
//	Created		: 23.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONSTANT_CACHE_H_INCLUDED
#define CONSTANT_CACHE_H_INCLUDED

namespace xray {
namespace render {

template <class T, u32 limit>
class t_cache
{
public:
	t_cache() {m_array.resize(limit); flush();}

	T*		access(u32 id) {return &m_array[id];}
	
	void	flush() {m_lo=m_hi=0;}
	void	dirty(u32 lo, u32 hi) {if (lo<m_lo) m_lo=lo; if (hi>m_hi) m_hi=hi;}

	T*		access_dirty() {return access(m_lo);}
	u32		dirty_size() {return m_hi-m_lo;}

	u32		lo() {return m_lo;}
	u32		hi() {return m_hi;}

private:
	XRAY_ALIGN(16)	fixed_vector<T,limit>	m_array;
	u32										m_lo, m_hi;
};

class constant_array
{
public:
	typedef		t_cache<math::float4, 256>	float_cache;
	//typedef		t_cache<Ivector4,16>	t_i;
	//typedef		t_cache<BOOL,16>		t_b;

public:
	float_cache& get_float_cache() {return m_float_cache;}

	bool is_dirty() {return m_float_cache.hi()>0;}

	void set(res_constant* c, constant_load& l, const float4x4& a)
	{
		XRAY_UNREFERENCED_PARAMETER	( c );

		ASSERT(rc_float == c->m_type);
		
		math::float4*	it = m_float_cache.access(l.m_index);
		switch(l.m_cls)
		{
		case rc_2x4:
			m_float_cache.dirty(l.m_index,l.m_index+2);
			it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
			it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
			break;
		case rc_3x4:
			m_float_cache.dirty(l.m_index,l.m_index+3);
			it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
			it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
			it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
			break;
		case rc_4x4:
			m_float_cache.dirty(l.m_index,l.m_index+4);
			it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
			it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
			it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
			it[3].set(a.e03, a.e13, a.e23, a.e33);// = a.c;
			break;
		default:
#ifdef DEBUG
			LOG_ERROR("invalid constant run-time-type for '%s'",*c->get_name());
#else
			NODEFAULT();
#endif
		}
	}

	void set(res_constant* c, constant_load& l, const math::float4& a)
	{
		XRAY_UNREFERENCED_PARAMETER	( c );

		ASSERT(rc_float == c->m_type);
		ASSERT(rc_1x4 == l.m_cls);
		
		*m_float_cache.access(l.m_index) = a;
		m_float_cache.dirty(l.m_index, l.m_index+1);
	}

	void seta(res_constant* c, constant_load& l, u32 e, const float4x4& a)
	{
		XRAY_UNREFERENCED_PARAMETER	( c );

		ASSERT(rc_float == c->m_type);
		
		u32		base;
		math::float4*	it;

		switch(l.m_cls)
		{
		case rc_2x4:
			base = l.m_index + 2*e;
			it   = m_float_cache.access(base);
			m_float_cache.dirty(base,base+2);
			it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
			it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
			break;
		case rc_3x4:
			base = l.m_index + 3*e;
			it   = m_float_cache.access(base);
			m_float_cache.dirty(base,base+3);
			it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
			it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
			it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
			break;
		case rc_4x4:
			base = l.m_index + 4*e;
			it   = m_float_cache.access(base);
			m_float_cache.dirty(base,base+4);
			it[0].set(a.e00, a.e10, a.e20, a.e30);// = a.i;
			it[1].set(a.e01, a.e11, a.e21, a.e31);// = a.j;
			it[2].set(a.e02, a.e12, a.e22, a.e32);// = a.k;
			it[3].set(a.e03, a.e13, a.e23, a.e33);// = a.c;
			break;
		default:
#ifdef DEBUG
			LOG_ERROR("invalid constant run-time-type for '%s'",*c->get_name());
#else
			NODEFAULT();
#endif
		}
	}

	void seta(res_constant* c, constant_load& l, u32 e, const math::float4& a)
	{
		XRAY_UNREFERENCED_PARAMETER	( c );

		ASSERT(rc_float	== c->m_type);
		ASSERT(rc_1x4 == l.m_cls);

		u32	base = l.m_index+e;
		*m_float_cache.access(base) = a;
		m_float_cache.dirty(base, base+1);
	}

public:
	XRAY_ALIGN(16) float_cache	m_float_cache;
	//	XRAY_ALIGN(16)	t_i					c_i;
	//	XRAY_ALIGN(16)	t_b					c_b;
};

class constant_cache
{
public:
	template <typename T>
	void set(res_constant* c, const T& arg)
	{
		if (c->m_destination&1) {m_a_pixel.set(c, c->m_ps, arg);}
		if (c->m_destination&2) {m_a_vertex.set(c, c->m_vs, arg);}
	}

	void set(res_constant* c, float x, float y, float z, float w)
	{
		set(c, math::float4(x, y, z, w));
	}

	template <typename T>
	void seta(res_constant* c, u32 index, const T& arg)
	{
		if (c->m_destination&1) {m_a_pixel.seta(c, c->m_ps, index, arg);}
		if (c->m_destination&2) {m_a_vertex.seta(c, c->m_vs, index, arg);}
	}

	void seta(res_constant* c, u32 index, float x, float y, float z, float w)
	{
		seta(c, index, math::float4(x, y, z, w));
	}

	void flush()
	{
		if (m_a_pixel.is_dirty() || m_a_vertex.is_dirty()) flush_cache();
	}

private:
	void flush_cache	();

private:
	XRAY_ALIGN(16) constant_array	m_a_pixel;
	XRAY_ALIGN(16) constant_array	m_a_vertex;
};

}// namespace render 
}// namespace xray 



#endif // #ifndef CONSTANT_CACHE_H_INCLUDED