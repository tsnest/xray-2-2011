////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SHADER_CONFIGURATION_H_INCLUDED
#define SHADER_CONFIGURATION_H_INCLUDED

namespace xray {
namespace render {
namespace union_base {

struct shader_configuration 
{
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)

	union
	{
		struct 
		{
			bool 	use_diffuse_texture					:1;
			bool 	use_alpha_test						:1;
			bool 	use_parallax						:1;
			bool 	use_normal_texture					:1;
			bool 	use_detail_normal_texture			:1;
			bool 	use_detail_texture					:1;
			bool 	use_transluceny_texture				:1;
			bool 	use_specular_intensity_texture		:1;
			bool 	use_specular_power_texture			:1;
			bool	use_diffuse_power_texture			:1;
			bool 	use_subuv							:1;
			bool 	use_transparency_texture			:1;
			bool	use_tfresnel_at_0_degree			:1;
			bool	use_variation_mask					:1;
			bool	shadowed_light						:1;
			
			bool	use_sequence						:1;
			bool	use_reflection_mask					:1;
			
			u8		light_type							:4;
			
			bool	use_bokeh_dof						:1;
			bool	use_bokeh_image						:1;
			
			u8		vertex_input_type					:6;

			u8 		use_reflection						:2;		// 0 - off, 1 - write to emissive, 2 - write to diffuse channel
			u8		use_emissive						:2;		// 0 - off, 1 - solid emission by uniform shader_constant, 2 - emission map my texture.
			
			u8		wind_motion							:3;		// 0 - off, 1 - branches, 2 - leafmeshes, 3 - leafcards, 4 - billboards.
			
			bool	decal_material						:1;
			
			u8		decal_type							:2;		// 0 - accumulated decals, 1 - forward decal, 2 - advanced decal (render geometry in which the projected decal)
			
			bool 	use_anisotropic_direction_texture	:1;
			
			bool	is_anisotropic_material				:1;
			
			bool	use_ao_texture						:1;
			
			bool	use_organic_scattering_amount_mask		:1;
			bool	use_organic_scattering_depth_texture	:1;
			bool	use_organic_back_illumination_texture	:1;
			bool	use_organic_subdermal_texture			:1;
		};
		u64 configuration;
	};

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

}; // class shader_configuration

} // namespace union_base 

struct shader_configuration		: public union_base::shader_configuration
{
		shader_configuration	()	{ reset	();	}
		inline void	reset		()	
		{
			configuration			= 0;
			//use_diffuse_texture		= 1;
		}
}; // class shader_configuration


} // namespace render
} // namespace xray

#endif // #ifndef SHADER_CONFIGURATION_H_INCLUDED// --porting to DX10