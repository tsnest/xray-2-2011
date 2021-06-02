////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_DX11_H_INCLUDED
#define UTILS_DX11_H_INCLUDED

//////////////////////////////////////////////////////////////////////////
#define DISABLE_TODO_STAND  1
#if DISABLE_TODO_STAND == 1
#define RENDER_TODO(message_to_show)		"@todo: " message_to_show
#else
#define RENDER_TODO(message_to_show)		XRAY_TODO(message_to_show)
#endif


//////////////////////////////////////////////////////////////////////////
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
// 		else
// 			str[i] -= (str[i] >= 'A' && str[i] <= 'Z') ? ('A'-'a') : 0;

		++i;
	}
}

inline void to_lower(char* str, u32 length)
{
	size_t i   = 0;

	while (i < length)
	{
		str[i] -= (str[i] >= 'A' && str[i] <= 'Z') ? ('A'-'a') : 0;
		++i;
	}
}



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
		strcpy_s(new_str, len, value);
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
			m_property_list.insert(utils::mk_pair(id, type)).second;
		ASSERT(inserted);
	}



private:
	associative_vector<prop_id_enum, prop_type_enum, render::vector>	m_property_list;
	
	typedef associative_vector<u32, char*, render::vector>	str_avec;
	typedef associative_vector<u32, bool,  render::vector>	bool_avec;
	typedef associative_vector<u32, u32,   render::vector>	uint_avec;
	typedef associative_vector<u32, float, render::vector>	float_avec;

	str_avec	m_str_properties;
	bool_avec	m_bool_properties;
	uint_avec	m_uint_properties;
	float_avec	m_float_properties;
};

template<typename T1, typename T2> 
std::pair<T1, T2> mk_pair(const T1& p1, const T2& p2)
{
	return std::pair<T1, T2>(p1, p2);
}


//////////////////////////////////////////////////////////////////////////
template <typename list>
bool calc_lists_diff_range( list const& first, list const& second, u32 &min, u32 &max);

//////////////////////////////////////////////////////////////////////////
u32 get_format_4x4_pixel_size( DXGI_FORMAT format);

// Calculates 2D texture size in bytes.
u32 calc_texture_size	( u32 width, u32 height, DXGI_FORMAT format, u32 mips, u32 array_count = 1, u32 row_min_pitch = 0, u32 surf_min_pitch = 0);

// Calculates 2D surface size in bytes. The surface minimal row pitch may be considered.
inline u32 calc_surface_size ( u32 width, u32 height, DXGI_FORMAT format, u32 row_min_pitch = 0);

// Calculates 2D surface size and row pitch in bytes considering given minimum row pitch. 
u32 calc_surface_size ( u32 width, u32 height, DXGI_FORMAT format, u32 row_min_pitch, u32 & row_pirch);

// Calculates logarithm with base 2 only for powers of 2.
inline u32 log_2( u32 val);

// Calculate mip-map level count for the texture with specified size
inline u32 calc_mipmap_count( u32 width, u32 height)			{	return log_2( math::max( width, height) ) + 1; }

// Matrix functions
//////////////////////////////////////////////////////////////////////////

// Inverts the pure projection matrix.
math::float4x4 get_projection_invert( math::float4x4 const & projection);


} // namespace utils
} // namespace render 
} // namespace xray 

#include <xray/render/core/utils_inline.h>

#endif // #ifndef UTILS_DX11_H_INCLUDED