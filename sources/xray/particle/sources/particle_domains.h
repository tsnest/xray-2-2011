////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DOMAINS_H_INCLUDED
#define PARTICLE_DOMAINS_H_INCLUDED

#include <boost/noncopyable.hpp>

namespace xray {
namespace particle {

class particle_domain_complex : public boost::noncopyable {
public:
	enum enum_domain_type
	{
		point_domain		= 0,
		line_domain,
		triangle_domain,
		plane_domain,
		box_domain,
		sphere_domain,
		cylinder_domain,
		cone_domain,
		blob_domain,
		disc_domain,
		rectangle_domain,
	};
	xray::math::float4x4		get_transform			() const;
	xray::math::float3			generate				() const;
	xray::math::float3			to_local_space			(xray::math::float3 const& point) const;
	bool						inside					(xray::math::float3 const& ) const;
	bool						get_affect_velocity		() const {return m_affect_velocity;}
	void						set_defaults();
	void						load					(configs::binary_config_value const& config);

#ifndef	MASTER_GOLD
	void						load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD

private:
	template<class ConfigValueType> 
	void						load_impl				(ConfigValueType const& prop);

public:
	xray::math::float4x4		m_transform;
	xray::math::float4x4		m_inv_transform;
	
	xray::math::float3_pod		m_translate;
	xray::math::float3_pod		m_rotate;
	xray::math::float3_pod		m_scale;
	
	// Point type.
	union
	{
		xray::math::float3_pod		m_point_position;
		xray::math::float3_pod		m_triangle_a;
	};
	xray::math::float3_pod		m_triangle_b;
	xray::math::float3_pod		m_triangle_c;
	
	// Line type.
	float						m_line_width;

	// Box type.
	float						m_box_width;
	float						m_box_height;
	float						m_box_depth;

	// Sphere/Cylinder/Cone type.
	float						m_inner_radius;
	union
	{
		float					m_outer_radius;
		float					m_cone_radius;
	};
	union
	{
		float					m_cylinder_height;
		float					m_cone_height;
	};		
	
	
	bool						m_affect_velocity;
	bool						m_draw_gizmo;
	
	u8							m_domain_type;
};

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_DOMAINS_H_INCLUDED