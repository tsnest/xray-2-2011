////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

namespace xray {
namespace render {
namespace utils {


inline void fix_path(char* str)
{
	size_t len = strlen(str);
	size_t i   = 0;

	while (i < len)
	{
		if (str[i] == '\\')
			str[i] = '/';

		++i;
	}
}

inline u32	color_argb	(u32 a, u32 r, u32 g, u32 b)	{	return ((a&0xff)<<24)|((r&0xff)<<16)|((g&0xff)<<8)|(b&0xff);	}
inline u32	color_rgba	(u32 r, u32 g, u32 b, u32 a)	{	return color_argb(a,r,g,b);		}

//u32	color_argb_f(f32 a, f32 r, f32 g, f32 b)	
//{
//	s32	 _r = clampr(iFloor(r*255.f),0,255);
//	s32	 _g = clampr(iFloor(g*255.f),0,255);
//	s32	 _b = clampr(iFloor(b*255.f),0,255);
//	s32	 _a = clampr(iFloor(a*255.f),0,255);
//	return color_argb(_a,_r,_g,_b);
//}

//u32	color_rgba_f(f32 r, f32 g, f32 b, f32 a) {return color_argb_f(a,r,g,b);}
inline u32	color_xrgb(u32 r, u32 g, u32 b) {return color_argb(0xff,r,g,b);}
inline u32	color_get_R(u32 rgba) {return (((rgba) >> 16) & 0xff);}
inline u32	color_get_G(u32 rgba) {return (((rgba) >> 8) & 0xff);}
inline u32	color_get_B(u32 rgba) {return ((rgba) & 0xff);}
inline u32 color_get_A(u32 rgba) {return ((rgba) >> 24);}
inline u32 subst_alpha(u32 rgba, u32 a) {return rgba&~color_rgba(0,0,0,0xff)|color_rgba(0,0,0,a);}
inline u32 bgr2rgb(u32 bgr) {return color_rgba(color_get_B(bgr),color_get_G(bgr),color_get_R(bgr),0);}
inline u32 rgb2bgr(u32 rgb) {return bgr2rgb(rgb);}

const float ps_r2_gloss_factor = 3.0f;
inline float	u_diffuse2s	(float x, float y, float z)	{float	v = (x+y+z)/3.f; return ps_r2_gloss_factor*((v<1) ? powf(v,2.f/3.f) : v);}
inline float	u_diffuse2s	(float3& c) {return u_diffuse2s(c.x,c.y,c.z);}

inline float3		wform	(float4x4 const& m, float3& v);

enum prop_type_enum
{
	type_string,
	type_bool,
	type_uint,
	type_float
};

template<typename prop_id_enum>
class property_container
{
public:
	property_container()
	{
		ASSERT(m_str_properties.size()==0);
		ASSERT(m_bool_properties.size()==0);
	}

	virtual ~property_container()
	{
		ASSERT(m_str_properties.size()==0);
		ASSERT(m_bool_properties.size()==0);

		str_avec::iterator	it  = m_str_properties.begin(),
							end = m_str_properties.end();

		for (; it != end; ++it)
		{
			FREE(it->second);
		}
	}

	void reset()
	{
		m_str_properties.clear();
		m_bool_properties.clear();
	}

	void set(prop_id_enum id, const char* value)
	{
		if (!has(id))
			add_property(id, type_string);

		ASSERT(m_property_list[id] == pt_string);
		u32 len = static_cast<u32>(strlen(value)+1);
		char* new_str = ALLOC(char, len);
		strings::copy(new_str, len, value);
		m_str_properties[id] = new_str;
	}
	
	void set(prop_id_enum id, bool value)
	{
		if (!has(id))
			add_property(id, type_bool);

		ASSERT(m_property_list[id] == pt_bool);
		m_bool_properties[id] = value;
	}

	void set(prop_id_enum id, u32 value)
	{
		if (!has(id))
			add_property(id, type_uint);

		ASSERT(m_property_list[id] == pt_bool);
		m_uint_properties[id] = value;
	}

	void set(prop_id_enum id, float value)
	{
		if (!has(id))
			add_property(id, type_float);

		ASSERT(m_property_list[id] == pt_bool);
		m_float_properties[id] = value;
	}

	template<typename T>
	T get(prop_id_enum id);

	template<>
	const char* get<const char*>(prop_id_enum id)
	{
		str_avec::iterator it = m_str_properties.find(id);

		if (it != m_str_properties.end())
		{
			return it->second;
		}

		return 0;
	}

	template<>
	bool get<bool>(prop_id_enum id)
	{
		ASSERT(has(id));
		bool_avec::iterator it = m_bool_properties.find(id);

		if (it != m_bool_properties.end())
		{
			return it->second;
		}

		return false;
	}

	template<>
	u32 get<u32>(prop_id_enum id)
	{
		uint_avec::iterator it = m_uint_properties.find(id);

		if (it != m_uint_properties.end())
		{
			return it->second;
		}

		return 0;
	}

	template<>
	float get<float>(prop_id_enum id)
	{
		float_avec::iterator it = m_float_properties.find(id);

		if (it != m_float_properties.end())
		{
			return it->second;
		}

		return 0;
	}

	bool has(prop_id_enum id)
	{
		return m_property_list.find(id) != m_property_list.end();
	}

private:
	void add_property(prop_id_enum id, prop_type_enum type)
	{
#ifdef DEBUG
		bool inserted = 
#endif // #ifdef DEBUG
			m_property_list.insert(mk_pair(id, type)).second;
		ASSERT(inserted);
	}



private:
	associative_vector<prop_id_enum, prop_type_enum, vector>	m_property_list;
	
	typedef associative_vector<u32, char*, vector>	str_avec;
	typedef associative_vector<u32, bool,  vector>	bool_avec;
	typedef associative_vector<u32, u32,   vector>	uint_avec;
	typedef associative_vector<u32, float, vector>	float_avec;

	str_avec	m_str_properties;
	bool_avec	m_bool_properties;
	uint_avec	m_uint_properties;
	float_avec	m_float_properties;
};

} // namespace utils
} // namespace render 
} // namespace xray 

#include "utils_inline.h"

#endif // #ifndef UTILS_H_INCLUDED