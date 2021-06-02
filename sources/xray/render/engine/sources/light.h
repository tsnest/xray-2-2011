////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/collision/object.h>

namespace xray {
namespace render {

enum light_type;

class res_effect;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

class light :
	public resource_intrusive_base,
	private boost::noncopyable
{
public:
	enum shadow_distribution_side
	{
		cast_to_x_side = 0,
		cast_to_neg_x_side,
		cast_to_y_side,
		cast_to_neg_y_side,
		cast_to_z_side,
		cast_to_neg_z_side,
	};
public:
					light					( collision::space_partitioning_tree* tree );
					~light					( );
	
			void	destroy_impl			( ) const;
	
	inline	void	set_type				( render::light_type const type ) { flags.type = type; }
	inline	u32		get_type				( ) const { return flags.type; }
	
			void	enable					( bool const b );
	inline	bool	is_enabled				( ) { return flags.is_enabled; }
	inline	void	set_cast_shadows		( bool const b ) { flags.does_cast_shadows = b; }
			bool	is_cast_shadows			( ) const;
	
	inline  bool	is_cast_shadows_in		( shadow_distribution_side side ) const { return shadow_distribution_sides[side]; }
	
//	inline	void	set_volumetric			( bool const b ) { flags.is_volumetric = b; }
			
			void	set_position			( const float3& P);
			void	set_orientation			( const float3& D, const float3& R);
			void	set_range				( float R);
			void	set_color				( math::color const& color, float const intensity );
//			void	set_texture				( LPCSTR name);

	inline	void	set_hud_mode			( bool const b) { flags.is_hud_mode = b; }
	inline	bool	get_hud_mode			( ) { return flags.is_hud_mode; }

			bool	is_inside_light_volume	( float3 const& position ) const;
			//void	gi_generate				( );
			void	xform_calc				( );
			//void	vis_prepare				( );
			//void	vis_update				( );

			void	set_attenuation_power	( float value );
			void	set_scale				( float3 const& scale );

			//float	get_LOD					( );
			void	on_properties_changed	( );

private:
	struct light_flags {
		u32	type				:	4;
		u32	is_static			:	1;
		u32	is_enabled			:	1;
		u32	does_cast_shadows	:	1;
//		u32	is_volumetric		:	1;
		u32	is_hud_mode			:	1;
		u32	cast_shadow_in_x	:	1;
		u32	cast_shadow_in_neg_x:	1;
		u32	cast_shadow_in_y	:	1;
		u32	cast_shadow_in_neg_y:	1;
		u32	cast_shadow_in_z	:	1;
		u32	cast_shadow_in_neg_z:	1;
	};

public:
	float4x4		m_xform;
	float4x4		m_plane_spot_xform;

	float3			color;
	float			intensity;

	float3			position;
	float			spot_umbra_angle;

	float3			direction;
	float			spot_penumbra_angle;

	float3			right;
	float			spot_falloff;

	float3			scale;
	float			attenuation_power;

	float			range;

	float			shadow_transparency;
	
	u32				m_xform_frame;
	
	collision::space_partitioning_tree* const	m_collision_tree;
	collision::geometry_instance*				m_collision_geometry;
	collision::object*							m_collision_object;

	float			diffuse_influence_factor;
	float			specular_influence_factor;

	bool			is_shadower;
	
	bool			use_with_lpv;
	
	u32				sun_shadow_map_size;
	u32				num_sun_cascades;
	
	float			shadow_z_bias;
	
	u32				shadow_map_size;
	u32				shadow_map_size_index;

	u32				lighting_model;
	
	light_flags		flags;

	bool			shadow_distribution_sides[6];

public:
	math::aabb		m_aabb;
};

typedef intrusive_ptr<light, resource_intrusive_base, threading::single_threading_policy> ref_light;

} // namespace render 
} // namespace xray 

#endif // #ifndef LIGHT_H_INCLUDED
