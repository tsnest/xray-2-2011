////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include "light.h"
#include <xray/render/facade/light_props.h>
#include "renderer.h"
#include <xray/render/core/res_effect.h>
#include <xray/collision/space_partitioning_tree.h>

namespace xray {
namespace render {

light::light				( collision::space_partitioning_tree* tree ) :
	m_collision_tree		( tree ),
	m_collision_object		( 0 ),
	sun_shadow_map_size		( 2048 ),
	num_sun_cascades		( 4 ),
	shadow_z_bias			( 0.001f ),
	shadow_map_size			( 1024 ),
	shadow_map_size_index	( 0 ),
	lighting_model			( 1 ),
	shadow_transparency		( 0.1f ),
	use_with_lpv			( false ),
	m_collision_geometry	( 0 ),
	is_shadower				( false ),
	m_aabb					( math::create_zero_aabb() )
{
	
	flags.type				= render::light_type_point;
	flags.is_static			= false;
	flags.is_enabled		= false;
	flags.does_cast_shadows	= false;
//	flags.is_volumetric		= false;
	flags.is_hud_mode		= false;
	position				= float3( 0.f , -1000.f, 0.f );
	direction.set			( 0.f, 0.f, 1.f );
	right.set				( 1.f, 0.f, 0.f );
	range					= 8.f;
	color.set				( 1.f, 1.f, 1.f );
	
	for (u32 side=0; side<6; side++)
		shadow_distribution_sides[0] = true;
}

light::~light				( )
{
	if ( m_collision_tree && m_collision_object )
		m_collision_tree->erase	( m_collision_object );

	collision::delete_object			( g_allocator, m_collision_object );
	collision::delete_geometry_instance	( g_allocator, m_collision_geometry);
}

bool light::is_cast_shadows			( ) const 
{
	if (!flags.does_cast_shadows)
	{
		return false;
	}
	else
	{
		if (get_type()==light_type_point)
		{
			return is_cast_shadows_in(cast_to_x_side) || is_cast_shadows_in(cast_to_neg_x_side) ||
				   is_cast_shadows_in(cast_to_y_side) || is_cast_shadows_in(cast_to_neg_y_side) ||
				   is_cast_shadows_in(cast_to_z_side) || is_cast_shadows_in(cast_to_neg_z_side);
		}
		else
		{
			return true;
		}
	}
}

bool light::is_inside_light_volume	( float3 const& in_position ) const
{
	switch ( get_type() ) 
	{
		case light_type_point:
		{
			return (position - in_position).length() <= range * 1.3f;
		}
		case light_type_spot:
		{
			float4x4 world_to_local = m_xform;
			world_to_local.try_invert(world_to_local);
			
			float3 local_position = world_to_local.transform_position(in_position);
			
			local_position = local_position;
			
			static float3 const vertices[] = {
				float3(0.f, 0.f, 0.f),
				float3(-1.f,-1.f,1.f),
				float3(-1.f, 1.f,1.f),
				float3( 1.f,-1.f,1.f),
				float3( 1.f, 1.f,1.f),
			};
			
			float3 v1 = math::normalize(vertices[1]);
			float3 v2 = math::normalize(vertices[2]);
			float3 v3 = math::normalize(vertices[3]);
			float3 v4 = math::normalize(vertices[4]);
			
			float3 n0 = math::cross_product(v1, v2).normalize();
			float3 n1 = math::cross_product(v2, v4).normalize();
			float3 n2 = math::cross_product(v4, v3).normalize();
			float3 n3 = math::cross_product(v3, v1).normalize();
			float3 n4 = float3(0.0f, 0.0f, 1.0f);
			
			float const d0 = math::dot_product(n0, local_position);
			float const d1 = math::dot_product(n1, local_position);
			float const d2 = math::dot_product(n2, local_position);
			float const d3 = math::dot_product(n3, local_position);
			float const d4 = math::dot_product(n4, local_position) - 1.0f;
			
			float const epsilon = 0.1f;
			
			bool const result0 = d0 <= epsilon;
			bool const result1 = d1 <= epsilon;
			bool const result2 = d2 <= epsilon;
			bool const result3 = d3 <= epsilon;
			bool const result4 = d4 <= epsilon;
			
			return result0 && result1 && result2 && result3 && result4;
		}
		case light_type_obb:
		{
			return false;
		}
		case light_type_capsule:
		{
			return false;
		}
		default:
			NODEFAULT();
	}
	return false;
}

void light::destroy_impl	( ) const
{
	light const* this_ptr	= this;
	DELETE					( this_ptr );
}

void light::enable			( bool const value )
{
	flags.is_enabled		= value;

#ifdef DEBUG
	if ( value ) {
		float3	zero(0,-1000,0);
		if (position.is_similar(zero))	{
			LOG_INFO("- Uninitialized light position.");
		}
	}
#endif // #ifdef DEBUG
}

void	light::set_position		(const float3& P)
{
	float	eps					=	math::epsilon_7;	//_max	(range*0.001f,EPS_L);
	if (position.is_similar(P,eps))return	;
	position = (P);
}

void	light::set_color		( math::color const& c, float const intensity ) 
{ 
	color.x			= c.r;
	color.y			= c.g;
	color.z			= c.b;
	color			/= 255.0f;
	this->intensity	= intensity;
}

void	light::set_range		(float R)			
{
//	float	eps					=	std::max	(range*0.1f, math::epsilon_7);
//	if (math::is_similar(range,R,eps))	return	;
	range						= R		;
}

void	light::set_orientation		( float3 const& direction, float3 const& right )
{ 
	this->direction				= normalize( direction );
	this->right					= normalize( right );
}

void light::on_properties_changed	( )
{
	m_xform_frame				= u32(-1);
	xform_calc					( );

	if ( m_collision_tree && m_collision_object )
		m_collision_tree->erase	( m_collision_object );

	collision::delete_object			( g_allocator, m_collision_object );
	collision::delete_geometry_instance	( g_allocator, m_collision_geometry);

	float4x4 transform;
	m_aabb.zero					( );
	m_aabb						+= 1.f;						

	switch ( flags.type ) {
		case light_type_parallel:
		case render::light_type_point : {
			m_collision_object	= 
				&*collision::new_collision_object	(
					g_allocator,
					1,
					m_collision_geometry = &*collision::new_sphere_geometry_instance( g_allocator, math::create_translation( position ), range ),
					this
				);
			transform			= math::create_scale( float3(range, range, range) ) * math::create_translation( position );
			break;
		}
		case render::light_type_spot : {
			transform			= m_xform;
			float const radius	= range*math::tan( spot_penumbra_angle/2.f );
			float3 const box_half_length	= float3(
				radius,
				radius,
				range/2.f
			);
			transform			=
				math::create_scale( box_half_length ) *
				math::create_rotation( m_xform.get_angles_xyz() ) *
				math::create_translation( position + direction * range/2.f );

			m_collision_object	= 
				&*collision::new_collision_object(
					g_allocator,
					1,
					m_collision_geometry = &*collision::new_box_geometry_instance( g_allocator, transform ),
					this
			);

			break;
		}
		case render::light_type_obb : {
			float3 const box_half_length	= scale + range;
			transform			=
				math::create_scale( box_half_length ) *
				math::create_rotation( m_xform.get_angles_xyz() ) *
				math::create_translation( position );

			m_collision_object	= 
				&*collision::new_collision_object(
					g_allocator,
					1,
					m_collision_geometry = &*collision::new_box_geometry_instance( g_allocator, transform ),
					this
				);

			break;
		}
		case render::light_type_capsule : {
			float const scale_x_plus_range	= scale.x + range;
			float3 const box_half_length	= float3(
				scale_x_plus_range,
				scale_x_plus_range,
				scale.z + range
			);
			transform			=
				math::create_scale( box_half_length ) *
				math::create_rotation( m_xform.get_angles_xyz() ) *
				math::create_translation( position );

			m_collision_object	= 
				&*collision::new_collision_object(
					g_allocator,
					1,
					m_collision_geometry = &*collision::new_box_geometry_instance( g_allocator, transform ),
					this
				);
			break;
		}
		case render::light_type_sphere : {
			m_collision_object	= 
				&*collision::new_collision_object(
					g_allocator,
					1,
					m_collision_geometry = &*collision::new_sphere_geometry_instance( g_allocator, math::create_translation( position ), range + scale.x ),
					this
				);
			transform			=
				math::create_scale( float3(range, range, range) ) *
				math::create_translation( position );
			break;
		}
		case render::light_type_plane_spot : {
			float const range_X_tan_penumbra_angle_div_2 = range*math::tan(spot_penumbra_angle/2.f);
			float3 const& box_half_length	= float3( range_X_tan_penumbra_angle_div_2 + scale.x, range/2.f, range_X_tan_penumbra_angle_div_2 + scale.z );
			transform			=
				math::create_scale( box_half_length ) *
				math::create_rotation( m_xform.get_angles_xyz() ) *
				math::create_translation( position - math::normalize(math::cross_product(direction, right)) * range / 2.f );
			m_collision_object	= 
				&*collision::new_collision_object(
					g_allocator,
					1,
					m_collision_geometry = &*collision::new_box_geometry_instance( g_allocator, transform ),
					this
				);

			break;
		}
		default : NODEFAULT( );
	}

	m_collision_tree->insert	( m_collision_object, transform );
	m_aabb.modify				( transform );
}

void	light::xform_calc			()
{
//	if	(renderer::ref().frame_id() == m_xform_frame)	return;
//
//	m_xform_frame = renderer::ref().frame_id();

	// build final rotation / translation
	float3						L_dir,L_up,L_right;

	// dir
	L_dir = direction;
	float const direction_length = L_dir.length();
	R_ASSERT					( math::is_similar( direction_length, 1.f) );

	// R&N
	if (right.squared_length()>math::epsilon_5)
	{
		// use specified 'up' and 'right', just enshure ortho-normalization
		L_right = right; L_right.normalize();
		L_up = math::cross_product(L_dir,L_right); L_up.normalize();
		L_right = math::cross_product(L_up,L_dir);	L_right.normalize();
	} else {
		// auto find 'up' and 'right' vectors
		L_up.set(0,1,0); if (abs(math::dot_product(L_up, L_dir))>.99f)	L_up.set(0,0,1);
		L_right=math::cross_product(L_up,L_dir); L_right.normalize();
		L_up=math::cross_product(L_dir,L_right); L_up.normalize();
	}

	// matrix
	float4x4 rotation_X_translation;
	rotation_X_translation.i.xyz() = L_right;	rotation_X_translation.e03 = 0.f;
	rotation_X_translation.j.xyz() = L_up;		rotation_X_translation.e13 = 0.f;
	rotation_X_translation.k.xyz() = L_dir;		rotation_X_translation.e23 = 0.f;
	rotation_X_translation.c.xyz() = position;	rotation_X_translation.e33 = 1.f;

	switch ( flags.type ) {
		case render::light_type_point : {
			float const range		= this->range*1.05f;
			float4x4 const& scale	= math::create_scale( float3( range, range, range ) );
			m_xform					= scale * rotation_X_translation;
			break;
		}
		case render::light_type_spot : {
			float const range_X_tan_penumbra_angle_div_2 = range*math::tan(spot_penumbra_angle/2.f);
			float4x4 const& scale	= math::create_scale( float3( range_X_tan_penumbra_angle_div_2, range_X_tan_penumbra_angle_div_2, range ) );
			m_xform					= scale * rotation_X_translation;
			break;
		}
		case render::light_type_obb : {
			float4x4 const& scale	= math::create_scale( this->scale + range );
			m_xform					= scale * rotation_X_translation;
			break;
		}
		case render::light_type_capsule : {
			float4x4 const& scale	= math::create_scale( float3( this->scale.x, this->scale.x, this->scale.z ) + range );
			m_xform					= scale * rotation_X_translation;
			break;
		}
		case render::light_type_sphere : {
			float const range		= (this->range + scale.x)*1.05f;
			float4x4 const& scale	= math::create_scale( float3( range, range, range ) );
			m_xform					= scale * rotation_X_translation;
			break;
			break;
		}
		case render::light_type_plane_spot : {
			float const range_X_tan_penumbra_angle_div_2 = range*math::tan(spot_penumbra_angle/2.f);
			m_plane_spot_xform		= create_scale( scale ) * rotation_X_translation;
			float4x4 const& scale	= math::create_scale( float3( range_X_tan_penumbra_angle_div_2 + this->scale.x, range/2.f, range_X_tan_penumbra_angle_div_2 + this->scale.z ) );

			float4x4 rotation;
			rotation.i.xyz()		= L_right;	rotation.e03 = 0.f;
			rotation.j.xyz()		= L_up;		rotation.e13 = 0.f;
			rotation.k.xyz()		= L_dir;	rotation.e23 = 0.f;
			rotation.c.set			( 0.f, 0.f, 0.f, 1.f );

			m_xform					= scale * rotation * create_translation( position - L_up*this->range/2.f );
			break;
		}
		default : {
			m_xform.identity	();
			break;
		}
	}
}

void light::set_attenuation_power			( float value )
{
	attenuation_power	= value;
}

void light::set_scale				( float3 const& scale )
{
	this->scale			= scale;
}

//void light::set_texture		( LPCSTR name )
//{
//	XRAY_UNREFERENCED_PARAMETERS( name );
//
////	if ((0==name) || (0==name[0]))
////	{
////		// default shaders
////		s_spot = 0;
////		s_point = 0;
////		s_volumetric = 0;
////		return;
////	}
////
////	string256				temp;
////	
////	strconcat(sizeof(temp),temp,"r2\\accum_spot_",name);
////	//strconcat(sizeof(temp),temp,"_nomsaa",name);
////	//s_spot.create			(RImplementation.Target->b_accum_spot,temp,name);
////
//////#if	RENDER!=R_R3
////	s_volumetric.create		("accum_volumetric", name);
////#else	//	RENDER!=R_R3
////	s_volumetric.create		("accum_volumetric_nomsaa", name);
////	if( RImplementation.o.dx10_msaa )
////	{
////		int bound = 1;
////
////		if( !RImplementation.o.dx10_msaa_opt )
////			bound = RImplementation.o.dx10_msaa_samples;
////
////		for( int i = 0; i < bound; ++i )
////		{
////			s_spot_msaa[i].create				(RImplementation.Target->b_accum_spot_msaa[i],strconcat(sizeof(temp),temp,"r2\\accum_spot_",name),name);
////			s_volumetric_msaa[i].create	(RImplementation.Target->b_accum_volumetric_msaa[i],strconcat(sizeof(temp),temp,"r2\\accum_volumetric_",name),name);
////		}
////	}
////#endif	//	RENDER!=R_R3
//}

} // namespace render 
} // namespace xray 
