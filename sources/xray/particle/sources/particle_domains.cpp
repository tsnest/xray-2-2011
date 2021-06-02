////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_domains.h"
#include "help_functions.h"

namespace xray {
namespace particle {

xray::math::float4x4 particle_domain_complex::get_transform() const
{
	//return m_transform;
	return xray::math::create_scale(m_scale) * xray::math::create_matrix(xray::math::quaternion(m_rotate), m_translate);
}

xray::math::float3 particle_domain_complex::to_local_space( xray::math::float3 const& point) const
{
	float4x4 inv_transform;
	inv_transform.try_invert(get_transform());
	return inv_transform.transform_position(point);
	//return m_inv_transform.transform_position(point);
}

void particle_domain_complex::set_defaults()
{
	m_affect_velocity	= false;
	m_translate			= float3(0.0f,0.0f,0.0f);
	m_rotate			= float3(0.0f,0.0f,0.0f);
	m_scale				= float3(1.0f,1.0f,1.0f);
	m_transform			= float4x4().identity();
	m_inv_transform		= float4x4().identity();
	m_point_position	= float3(0.0f,0.0f,0.0f);
	m_line_width		= 1.0f;
	m_box_width			= 1.0f;
	m_box_height		= 1.0f;
	m_box_depth			= 1.0f;
	m_inner_radius		= 0.0f;
	m_outer_radius		= 1.0f;
	m_cylinder_height	= 1.0f;
	m_domain_type		= point_domain;
	m_triangle_b		= float3(0.0f,0.0f,0.0f);
	m_triangle_c		= float3(0.0f,0.0f,0.0f);
}

xray::math::float3 particle_domain_complex::generate() const
{
	switch (m_domain_type)
	{
		case point_domain:
			return get_transform().transform_position( m_point_position );
		case line_domain:
			{
				return get_transform().transform_position( 
					linear_interpolation(float3(-m_line_width/2.0f, 0, 0), float3(m_line_width/2.0f, 0, 0), random_float(0, 1.0f))
				);
			}
		case triangle_domain:
			{
				float3 weights = random_float3(float3(0.0f,0.0f,0.0f), float3(1.0f,1.0f,1.0f)).normalize();
				weights /= math::max(weights.x+weights.y+weights.z, math::epsilon_3);
				
				return get_transform().transform_position( 
					m_triangle_a*weights.x + m_triangle_b*weights.y + m_triangle_c*weights.z
					);
			}
		case box_domain:
				return get_transform().transform_position(
					float3(
						random_float(-0.5f*m_box_width,  0.5f*m_box_width), 
						random_float(-0.5f*m_box_height, 0.5f*m_box_height),
						random_float(-0.5f*m_box_depth,  0.5f*m_box_depth)
						)
					);
		case rectangle_domain:
				return get_transform().transform_position(
					float3(
						random_float(-0.5f*m_box_width,  0.5f*m_box_width), 
						0.0f,
						random_float(-0.5f*m_box_height, 0.5f*m_box_height)
						)
					);
		case sphere_domain:
			{
				float3 dir(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
				return get_transform().transform_position( dir.normalize() * (m_outer_radius - random_float(0.0f, m_outer_radius - m_inner_radius)) );
			}
		case cylinder_domain:
			{
				float2 dir2d = float2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
				dir2d = dir2d.normalize();
				float height = random_float(-0.5f*m_cylinder_height, 0.5f*m_cylinder_height);
				float3 pos;
				pos.x = dir2d.x * (m_outer_radius - random_float(0.0f, m_outer_radius - m_inner_radius));
				pos.z = dir2d.y * (m_outer_radius - random_float(0.0f, m_outer_radius - m_inner_radius));
				pos.y = height;
				return get_transform().transform_position( pos );
			}
		case disc_domain:
			{
				float2 direction = float2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
				direction = direction.normalize();
				
				float3 position(
					direction.x * (m_outer_radius - random_float(0.0f, m_outer_radius - m_inner_radius)),
					0.0f,
					direction.y * (m_outer_radius - random_float(0.0f, m_outer_radius - m_inner_radius))
				);
				
				return get_transform().transform_position( position );
			}
		case cone_domain:
			{
				float2 const xz_offset = float2(random_float(-1.0f, 1.0f) * m_cone_radius, random_float(-1.0f, 1.0f) * m_cone_radius).normalize();
				float3 position = linear_interpolation(float3(0.0f, 0.0f, 0.0f), float3(xz_offset.x, -1.0f, xz_offset.y), random_float(0.0f, 1.0f));
				
				return get_transform().transform_position( position );
			}
	};
	return get_transform().lines[3].xyz();
}

bool particle_domain_complex::inside( xray::math::float3 const& point) const
{
	switch (m_domain_type)
	{
		case particle_domain_complex::point_domain:
			{
				float3 local_space_pos = to_local_space(point);
				return local_space_pos.length() < 0.01f;
			}
		case line_domain:
			{
				float3 local_space_pos = to_local_space(point);
				
				return  local_space_pos.x>=-m_line_width/2.0f && 
					local_space_pos.x<=-m_line_width/2.0f &&
					math::is_similar(local_space_pos.y, 0.0f, math::epsilon_3) &&
					math::is_similar(local_space_pos.z, 0.0f, math::epsilon_3);
			}
 		case triangle_domain:
 			{
 				float3 local_space_pos = to_local_space(point);
				
				return math::is_similar(local_space_pos.y, 0.0f, 0.1f) &&
					local_space_pos.x>0.0f && local_space_pos.x<1.0f &&
					local_space_pos.z>0.0f && local_space_pos.z<1.0f &&
					local_space_pos.z < 1.0f - local_space_pos.x;
 			}
		case box_domain:
			{
				float3 local_space_pos = to_local_space(point);
				return local_space_pos.x >= -m_box_width*0.5f && local_space_pos.x <= m_box_width*0.5f
					&& 
					local_space_pos.y >= -m_box_height*0.5f && local_space_pos.y <= m_box_height*0.5f
					&&
					local_space_pos.z >= -m_box_depth*0.5f && local_space_pos.z <= m_box_depth*0.5f;
			}
		case rectangle_domain:
			{
				float3 local_space_pos = to_local_space(point);
				return local_space_pos.x >= -m_box_width*0.5f && local_space_pos.x <= m_box_width*0.5f
					&& 
					local_space_pos.z >= -m_box_height*0.5 && local_space_pos.z <= m_box_height*0.5;
			}
		case sphere_domain:
			{
				float3 local_space_pos = to_local_space(point);
				float dist = local_space_pos.length();
				return dist >= m_inner_radius && dist <= m_outer_radius;
			}
		case cylinder_domain:
			{
				float3 const local_space_pos	= to_local_space(point);
				float const  from_center		= float2(local_space_pos.x,local_space_pos.z).length();
				
				return from_center >= m_inner_radius 
					&& from_center <= m_outer_radius 
					&& local_space_pos.y >= -m_cylinder_height*0.5
					&& local_space_pos.y <=  m_cylinder_height*0.5;
			}
		case disc_domain:
			{
				float3 const local_space_pos = to_local_space(point);
				float const  from_center	 = float2(local_space_pos.x,local_space_pos.z).length();
				
				return from_center >= m_inner_radius 
					&& from_center <= m_outer_radius;
			}
		case cone_domain:
			{
				#pragma message( XRAY_TODO( "cone domain inside(point)!" ) )
				return false;
			}
	}
	return false;
}

static particle_domain_complex::enum_domain_type domain_type_from_string(pcstr name)
{
		 if (strstr(name, "Point"))		return particle_domain_complex::point_domain;
	else if (strstr(name, "Line"))		return particle_domain_complex::line_domain;
	else if (strstr(name, "Triangle"))	return particle_domain_complex::triangle_domain;
	else if (strstr(name, "Plane"))		return particle_domain_complex::plane_domain;
	else if (strstr(name, "Box"))		return particle_domain_complex::box_domain;
	else if (strstr(name, "Sphere"))	return particle_domain_complex::sphere_domain;
	else if (strstr(name, "Cylinder"))	return particle_domain_complex::cylinder_domain;
	else if (strstr(name, "Cone"))		return particle_domain_complex::cone_domain;
	else if (strstr(name, "Blob"))		return particle_domain_complex::blob_domain;
	else if (strstr(name, "Disc"))		return particle_domain_complex::disc_domain;
	else if (strstr(name, "Rectangle")) return particle_domain_complex::rectangle_domain;
	
	else return particle_domain_complex::point_domain;
}

void particle_domain_complex::load				   (configs::binary_config_value const& config)
{
	load_impl(config);
}

#ifndef	MASTER_GOLD
void particle_domain_complex::load				   (configs::lua_config_value const& config)
{
	load_impl(config);
}
#endif // MASTER_GOLD


template<class ConfigValueType> 
void particle_domain_complex::load_impl(ConfigValueType const& prop)
{
	m_translate			= read_config_value<float3>(prop, "Translate", m_translate);
	m_rotate			= read_config_value<float3>(prop, "Rotate", m_rotate);
	m_scale				= read_config_value<float3>(prop, "Scale", m_scale);
	m_affect_velocity	= read_config_value<bool>(prop, "AffectVelocity", m_affect_velocity);
	
	ConfigValueType const& domain_type_config = prop["DomainType"];
	
	if (domain_type_config.value_exists("selected_value"))
	{
		m_domain_type = (u8)domain_type_from_string((pcstr)domain_type_config["selected_value"]);
	}
	
	switch (m_domain_type)
	{
		case point_domain:
			break;
		case line_domain:
			{
				ConfigValueType const& line_config		 = domain_type_config["Line"];
				m_line_width  = read_config_value<float>(line_config, "Length", m_line_width);
				break;
			}
		case box_domain:
			{
				ConfigValueType const& box_config		 = domain_type_config["Box"];
				m_box_width  = read_config_value<float>(box_config, "Width", m_box_width);
				m_box_height = read_config_value<float>(box_config, "Height", m_box_height);
				m_box_depth  = read_config_value<float>(box_config, "Depth", m_box_depth);
				break;
			}
		case rectangle_domain:
			{
				ConfigValueType const& box_config		 = domain_type_config["Rectangle"];
				m_box_width  = read_config_value<float>(box_config, "Width", m_box_width);
				m_box_height = read_config_value<float>(box_config, "Height", m_box_height);
				break;
			}
		case sphere_domain:
			{
				ConfigValueType const& sphere_config		 = domain_type_config["Sphere"];
				m_inner_radius  = read_config_value<float>(sphere_config, "RadiusInner", m_inner_radius);
				m_outer_radius  = read_config_value<float>(sphere_config, "RadiusOuter", m_outer_radius);
				break;

			}
		case cylinder_domain:
			{
				ConfigValueType const& cylinder_config	 = domain_type_config["Cylinder"];
				m_inner_radius		= read_config_value<float>(cylinder_config, "RadiusInner", m_inner_radius);
				m_outer_radius		= read_config_value<float>(cylinder_config, "RadiusOuter", m_outer_radius);
				m_cylinder_height	= read_config_value<float>(cylinder_config, "Height", m_cylinder_height);
				break;
			}
		case disc_domain:
			{
				ConfigValueType const& cylinder_config	 = domain_type_config["Disc"];
				m_inner_radius		= read_config_value<float>(cylinder_config, "RadiusInner", m_inner_radius);
				m_outer_radius		= read_config_value<float>(cylinder_config, "RadiusOuter", m_outer_radius);
				break;
			}
		case cone_domain:
			{
				ConfigValueType const& cylinder_config	 = domain_type_config["Cone"];
				m_cone_radius		= read_config_value<float>(cylinder_config, "Radius", m_cone_radius);
				m_cone_height		= read_config_value<float>(cylinder_config, "Height", m_cone_height);
				break;
			}
		case triangle_domain:
			{
				ConfigValueType const& cylinder_config	 = domain_type_config["Triangle"];
				m_triangle_a		= read_config_value<float3>(cylinder_config, "PointA", m_triangle_a);
				m_triangle_b		= read_config_value<float3>(cylinder_config, "PointB", m_triangle_b);
				m_triangle_c		= read_config_value<float3>(cylinder_config, "PointC", m_triangle_c);
				break;
			}
	}
}

} // namespace particle
} // namespace xray
