////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Igor Lobanchikov
//	Copyright ( color) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/scene_render.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/effect_compiler.h>

#include <xray/render/engine/vertex_formats.h>
#include <xray/render/engine/effect_combine.h>
#include <xray/render/engine/environment.h>
#include <xray/render/engine/model_manager.h>
#include <xray/render/engine/visual.h>
#include <xray/render/engine/stage_gbuffer.h>
#include <xray/render/engine/stage_texture_mip_clamp_test.h>
#include <xray/render/engine/stage_pre_lighting.h>
#include <xray/render/engine/stage_sun.h>
#include <xray/render/engine/stage_sun_materials.h>
#include <xray/render/engine/stage_skybox.h>
#include <xray/render/engine/stage_combine.h>
#include <xray/render/engine/stage_lights.h>
#include <xray/render/engine/stage_screen_image.h>
#include <xray/render/engine/scene_context.h>


namespace xray {
namespace render_dx10 {

struct init_params
{
	u32			width;
	u32			height;
	DXGI_FORMAT	format;
	u32			count;
};

static init_params  s_init_params[] = 
{
	{ 64, 64,		DXGI_FORMAT_BC1_UNORM, 30},
	{ 64, 32,		DXGI_FORMAT_BC1_UNORM, 10},
	{ 64, 16,		DXGI_FORMAT_BC1_UNORM, 7},
	{ 64, 8,		DXGI_FORMAT_BC1_UNORM, 7},

	{ 128, 128,		DXGI_FORMAT_BC1_UNORM, 60},
	{ 128, 64,		DXGI_FORMAT_BC1_UNORM, 30},
	{ 128, 32,		DXGI_FORMAT_BC1_UNORM, 10},
	{ 128, 16,		DXGI_FORMAT_BC1_UNORM, 7},

	{ 256, 256,		DXGI_FORMAT_BC1_UNORM, 50},
	{ 256, 128,		DXGI_FORMAT_BC1_UNORM, 25},
	{ 256, 64,		DXGI_FORMAT_BC1_UNORM, 10},
	{ 256, 32,		DXGI_FORMAT_BC1_UNORM, 7},

	{ 512, 512,		DXGI_FORMAT_BC1_UNORM, 50},
	{ 512, 256,		DXGI_FORMAT_BC1_UNORM, 25},
	{ 512, 128,		DXGI_FORMAT_BC1_UNORM, 7},
	{ 512, 64,		DXGI_FORMAT_BC1_UNORM, 5},

	{ 1024, 1024,	DXGI_FORMAT_BC1_UNORM, 15},
	{ 1024, 512,	DXGI_FORMAT_BC1_UNORM, 8},
	{ 1024, 256,	DXGI_FORMAT_BC1_UNORM, 4},
	{ 1024, 128,	DXGI_FORMAT_BC1_UNORM, 2},

	{ 2048, 2048,	DXGI_FORMAT_BC1_UNORM, 10},
	{ 2048, 1024,	DXGI_FORMAT_BC1_UNORM, 4},
	{ 2048, 512,	DXGI_FORMAT_BC1_UNORM, 3},
	{ 2048, 256,	DXGI_FORMAT_BC1_UNORM, 2},

//////////////////// DXGI_FORMAT_BC3_UNORM ////////////////////////////////////

	{ 64, 64,		DXGI_FORMAT_BC3_UNORM, 30},
	{ 64, 32,		DXGI_FORMAT_BC3_UNORM, 10},
	{ 64, 16,		DXGI_FORMAT_BC3_UNORM, 7},
	{ 64, 8,		DXGI_FORMAT_BC3_UNORM, 7},

	{ 128, 128,		DXGI_FORMAT_BC3_UNORM, 60},
	{ 128, 64,		DXGI_FORMAT_BC3_UNORM, 30},
	{ 128, 32,		DXGI_FORMAT_BC3_UNORM, 10},
	{ 128, 16,		DXGI_FORMAT_BC3_UNORM, 7},

	{ 256, 256,		DXGI_FORMAT_BC3_UNORM, 50},
	{ 256, 128,		DXGI_FORMAT_BC3_UNORM, 25},
	{ 256, 64,		DXGI_FORMAT_BC3_UNORM, 10},
	{ 256, 32,		DXGI_FORMAT_BC3_UNORM, 7},

	{ 512, 512,		DXGI_FORMAT_BC3_UNORM, 50},
	{ 512, 256,		DXGI_FORMAT_BC3_UNORM, 25},
	{ 512, 128,		DXGI_FORMAT_BC3_UNORM, 7},
	{ 512, 64,		DXGI_FORMAT_BC3_UNORM, 5},

	{ 1024, 1024,	DXGI_FORMAT_BC3_UNORM, 15},
	{ 1024, 512,	DXGI_FORMAT_BC3_UNORM, 8},
	{ 1024, 256,	DXGI_FORMAT_BC3_UNORM, 4},
	{ 1024, 128,	DXGI_FORMAT_BC3_UNORM, 2},

	{ 2048, 2048,	DXGI_FORMAT_BC3_UNORM, 10},
	{ 2048, 1024,	DXGI_FORMAT_BC3_UNORM, 4},
	{ 2048, 512,	DXGI_FORMAT_BC3_UNORM, 3},
	{ 2048, 256,	DXGI_FORMAT_BC3_UNORM, 2},
};



scene_render::scene_render( res_buffer* quad_ib): stage( &m_context)
{

#pragma message (RENDER_TODO("Implement filter none for mip, also anisotropic with linear."))

	// Here some initialization code will go into more general initialization stage 
	sampler_state_descriptor sampler_sim;
	sampler_sim.set( D3D_FILTER_ANISOTROPIC );
	backend::ref().register_sampler( "s_base", resource_manager::ref().create_sampler_state( sampler_sim));

	sampler_sim.set( D3D_FILTER_ANISOTROPIC );
	backend::ref().register_sampler( "s_base_hud", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_ANISOTROPIC );
	backend::ref().register_sampler( "s_detail", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_POINT, D3D_TEXTURE_ADDRESS_CLAMP );
	backend::ref().register_sampler( "s_position", resource_manager::ref().create_sampler_state( sampler_sim) );
	backend::ref().register_sampler( "s_normal", resource_manager::ref().create_sampler_state( sampler_sim) );
	backend::ref().register_sampler( "s_diffuse", resource_manager::ref().create_sampler_state( sampler_sim) );
	backend::ref().register_sampler( "s_accumulator", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_WRAP);
	backend::ref().register_sampler( "s_material", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_CLAMP);
	backend::ref().register_sampler( "s_material1", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_POINT, D3D_TEXTURE_ADDRESS_WRAP);
	backend::ref().register_sampler( "smp_nofilter", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D_TEXTURE_ADDRESS_WRAP);
	backend::ref().register_sampler( "smp_rtlinear", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_WRAP);
	backend::ref().register_sampler( "smp_linear", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_ANISOTROPIC, D3D_TEXTURE_ADDRESS_WRAP);
	backend::ref().register_sampler( "smp_base", resource_manager::ref().create_sampler_state( sampler_sim) );

	register_effect_descriptors();

// Add also for 
// 			"t_base"
// 			"t_lmap"
// 			"t_hemi"
// 			"t_env",
// 			"s_position",  
// 			"s_normal",    
// 			"s_diffuse",   
// 			"s_accumulator"
// 			"s_material"
//			"s_smap"
	u32 w = device::ref().get_width();
	u32 h = device::ref().get_height();

	m_context.m_rt_position		= resource_manager::ref().create_render_target( r2_rt_p, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, res_rt::enum_usage_render_target);
	m_context.m_rt_normal		= resource_manager::ref().create_render_target( r2_rt_n, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, res_rt::enum_usage_render_target);
	m_context.m_rt_color		= resource_manager::ref().create_render_target( r2_rt_albedo, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, res_rt::enum_usage_render_target);//take into account mixed_depth!!!!!!!!
	m_context.m_rt_accumulator	= resource_manager::ref().create_render_target( r2_rt_accum, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, res_rt::enum_usage_render_target);

	// generic( LDR) RTs
	m_context.m_rt_generic_0	= resource_manager::ref().create_render_target( r2_rt_generic0, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, res_rt::enum_usage_render_target);
	m_context.m_rt_generic_1	= resource_manager::ref().create_render_target( r2_rt_generic1, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, res_rt::enum_usage_render_target);

	// Textures to render targets
	m_context.m_t_position		= resource_manager::ref().create_texture( r2_rt_p);
	m_context.m_t_normal		= resource_manager::ref().create_texture( r2_rt_n);
	m_context.m_t_color			= resource_manager::ref().create_texture( r2_rt_albedo);
	m_context.m_t_accumulator	= resource_manager::ref().create_texture( r2_rt_accum);
	m_context.m_t_generic_0		= resource_manager::ref().create_texture( r2_rt_generic0);


// --Porting to DX10_
// 	u32	smap_size = device::ref().options.smap_size;
// 	DXGI_FORMAT	null_fmt = device::ref().options.null_rt_format;
// 	//PC DX9 requires fake color surface for rendering hw smaps
// 	//if we can we use "null" surface format which actually does not allocate memory
// 	m_context.m_rt_color_null = resource_manager::ref().create_render_target( r2_rt_smap_surf, smap_size, smap_size, null_fmt);

//#pragma message( RENDER_TODO( "uncomment when you are going to implement shadow maps"))	
//	D3DFORMAT	smap_fmt = device::ref().options.hw_smap_format;
//m_context.m_smap = resource_manager::ref().create_render_target( r2_rt_smap_depth, smap_size, smap_size, smap_fmt);

	backend& be = backend::ref();

	//create_quad_ib();
	m_context.m_quad_ib = quad_ib;
	m_context.m_g_quad_uv  = resource_manager::ref().create_geometry( vertex_formats::F_TL, sizeof(vertex_formats::TL), be.vertex.buffer(), &*m_context.m_quad_ib);
	m_context.m_g_quad_2uv = resource_manager::ref().create_geometry( vertex_formats::F_TL2uv, sizeof(vertex_formats::TL2uv), be.vertex.buffer(), &*m_context.m_quad_ib);

	effect_light_mask		m_b_accum_mask;
	m_context.m_sh_accum_mask   = effect_manager::ref().create_effect( &m_b_accum_mask);//,   "r2\\accum_mask");

	//m_t_envmap_0 = resource_manager::ref().create_texture( "sky/sky_9_cube#small");
	//m_t_envmap_1 = resource_manager::ref().create_texture( "sky/sky_19_cube#small");

	//s_combine_volumetric.create			( "combine_volumetric");
	//s_combine_dbg_0.create				( "effects\\screen_set",		r2_RT_smap_surf		);	
	//s_combine_dbg_1.create				( "effects\\screen_set",		r2_RT_luminance_t8	);
	//s_combine_dbg_Accumulator.create	( "effects\\screen_set",		r2_RT_accum			);
	//g_combine_VP.create					( dwDecl,		RCache.Vertex.Buffer(), RCache.QuadIB);
	//g_combine.create					( FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);
	//g_combine_2UV.create				( FVF::F_TL2uv,	RCache.Vertex.Buffer(), RCache.QuadIB);

	//u32 fvf_aa_blur				= D3DFVF_XYZRHW|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2( 0)|D3DFVF_TEXCOORDSIZE2( 1)|D3DFVF_TEXCOORDSIZE2( 2)|D3DFVF_TEXCOORDSIZE2( 3);
	//g_aa_blur.create			( fvf_aa_blur,	RCache.Vertex.Buffer(), RCache.QuadIB);

	//u32 fvf_aa_AA				= D3DFVF_XYZRHW|D3DFVF_TEX7|D3DFVF_TEXCOORDSIZE2( 0)|D3DFVF_TEXCOORDSIZE2( 1)|D3DFVF_TEXCOORDSIZE2( 2)|D3DFVF_TEXCOORDSIZE2( 3)|D3DFVF_TEXCOORDSIZE2( 4)|D3DFVF_TEXCOORDSIZE4( 5)|D3DFVF_TEXCOORDSIZE4( 6);
	//g_aa_AA.create				( fvf_aa_AA,		RCache.Vertex.Buffer(), RCache.QuadIB);
	//t_envmap_0.create( r2_T_envs0);
	//t_envmap_1.create( r2_T_envs1);

	if ( identity( true)) 
	{

		create_material_texture();
	//	rt_smap_ZB					= NULL;

	//	if ( RImplementation.o.advancedpp)
	//		s_accum_direct_volumetric.create( "accum_volumetric_sun");

		m_gstage				= NEW( stage_gbuffer)		( &m_context);
		m_tex_mip_clamp			= NEW( stage_texture_mip_clamp_test)		( &m_context);
		m_pre_lighting_stage	= NEW( stage_pre_lighting)	( &m_context);
		m_sun_stage				= NEW( stage_sun)			( &m_context);
		m_sun_stage_materials	= NEW( stage_sun_materials)	( &m_context);
		m_skybox_stage			= NEW( stage_skybox)		( &m_context);
		m_combine_stage			= NEW( stage_combine)		( &m_context);
		m_lighting_stage		= NEW( stage_lights)		( &m_context);
		m_present_stage			= NEW( stage_screen_image)	( &m_context);

	}
	else {
		m_gstage				= 0;
		m_tex_mip_clamp			= 0;
		m_pre_lighting_stage	= 0;
		m_sun_stage				= 0;
		m_sun_stage_materials	= 0;
		m_skybox_stage			= 0;
		m_combine_stage			= 0;
		m_lighting_stage		= 0;
		m_present_stage			= 0;
	}

	// Initialize textyres
	configs::lua_config_ptr config = xray::configs::create_lua_config();
	xray::configs::lua_config_value  & cfg = config->get_root();

	for( int i = 0; i< sizeof(s_init_params)/sizeof(init_params); ++i)
	{
		cfg[i]["width"]		= s_init_params[i].width;
		cfg[i]["height"]	= s_init_params[i].height;
		cfg[i]["format"]	= s_init_params[i].format;
		cfg[i]["count"]		= s_init_params[i].count;
	}

	//resource_manager::ref().initialize_texture_storage( cfg);
}

scene_render::~scene_render()
{
// --Porting to DX10_
// 	//model_manager::destroy_instance( m_test_visual);
// 	safe_release( m_context.m_base_rt);
// 	safe_release( m_context.m_base_zb);

	DELETE( m_gstage);
	DELETE( m_tex_mip_clamp);
	DELETE( m_pre_lighting_stage);
	DELETE( m_sun_stage);
	DELETE( m_sun_stage_materials);
	DELETE( m_skybox_stage);
	DELETE( m_combine_stage);
	DELETE( m_lighting_stage);
	DELETE( m_present_stage);
}

void scene_render::create_material_texture()
{
// 	ID3DTexture3D*	t_material_surf = 0;
// 	R_CHK( D3DXCreateVolumeTexture( device::ref().device(), tex_material_ldotn, tex_material_ldoth, 
// 								  4, 1, 0, D3DFMT_A8L8,D3DPOOL_MANAGED, &t_material_surf));

	u16	tempData[tex_material_ldotn*tex_material_ldoth*tex_material_count];

	// Fill it ( addr: x=dot( L,N),y=dot( L,H))
	for ( u32 slice=0; slice<4; ++slice)
	{
		for ( u32 y=0; y<tex_material_ldoth; ++y)
		{
			for ( u32 x=0; x<tex_material_ldotn; ++x)
			{
				u16*	p = (u16*)&tempData + slice*tex_material_ldotn*tex_material_ldoth + y*tex_material_ldotn + x;
				//u16*	p = ( u16*) ( LPBYTE ( R.pBits) + slice*R.SlicePitch + y*R.RowPitch + x*2);

				float	fd,fs;
				float	ld = float( x)/float( tex_material_ldotn-1);
				float	ls = float( y)/float( tex_material_ldoth-1) /*+ math::epsilon_7*/;
				
				ls *= powf( ld,1/32.f);
				
				switch ( slice)
				{
				case 0:
					{ // looks like OrenNayar
						fd	= powf( ld, 0.75f);		// 0.75
						fs	= 0.5f*powf( ls, 16.0f);
					}
					break;
				case 1:
					{// looks like Blinn
						fd	= powf( ld,0.90f);		// 0.90
						fs	= powf( ls,24.f);
					}
					break;
				case 2:
					{ // looks like Phong
						fd	= ld;					// 1.0
						fs  = powf( ls*1.01f, 128.f);
					}
					break;
				case 3:
					{ // looks like Metal
						float	s0	=	abs	( 1-abs( 0.05f*sin( 33.f*ld)+ld-ls));
						float	s1	=	abs	( 1-abs( 0.05f*cos( 33.f*ld*ls)+ld-ls));
						float	s2	=	abs	( 1-abs( ld-ls));
						fd  = ld;				// 1.0
						fs  = powf( std::max( std::max( s0,s1),s2), 24.f);
						fs *= powf( ld,1/7.f);
					}
					break;
				default:
					fd	= fs = 0;
				}

				s32	_d = ( s32)floor( fd*255.5f);
				math::clamp( _d, 0, 255);
				s32	_s = ( s32)floor( fs*255.5f); 
				math::clamp( _s, 0, 255);

				if ( ( y==tex_material_ldoth-1) && ( x==tex_material_ldotn-1))
				{
					_d = 255; _s=255;
				}

				*p = u16( _s*256 + _d);
			}
		}
	}

	// Assume that there is no padding into the data buffer.
	D3D_SUBRESOURCE_DATA	data;
	data.pSysMem			= tempData;
	data.SysMemSlicePitch	= sizeof(tempData)/tex_material_count;
	data.SysMemPitch		= sizeof(tempData)/(tex_material_count*tex_material_ldoth);

	resource_manager::ref().create_texture3d( r2_material, tex_material_ldotn, tex_material_ldoth, tex_material_count, &data, DXGI_FORMAT_R8G8_UNORM, D3D_USAGE_IMMUTABLE, 1 );
}

void scene_render::create_quad_ib()
{
	const u32 quad_count = 4*1024;
	const u32 idx_count  = quad_count*2*3;
	
// 	if ( device::ref().get_caps().geometry.software)
// 	{
// 		ASSERT( 0);
// 		usage |= D3DUSAGE_SOFTWAREPROCESSING;
// 	}

	u16	indices[idx_count];

	int		vertex_id = 0;
	int		idx= 0;
	for ( int i=0; i<quad_count; ++i)
	{
		indices[idx++]=u16( vertex_id+0);
		indices[idx++]=u16( vertex_id+1);
		indices[idx++]=u16( vertex_id+2);

		indices[idx++]=u16( vertex_id+3);
		indices[idx++]=u16( vertex_id+2);
		indices[idx++]=u16( vertex_id+1);

		vertex_id+=4;
	}
	m_context.m_quad_ib = resource_manager::ref().create_buffer( idx_count*sizeof(u16), indices, enum_buffer_type_index, false);
	
// 	u16	* indices = (u16*) m_context.m_quad_ib->map( D3D_MAP_WRITE_DISCARD);
// 
// 
// 	m_context.m_quad_ib->unmap();
}



void scene_render::render( )
{
	//PIX_EVENT( scene_render);
	//////////////////////////////////////////////////////////////////////////
	// Setup frame
	m_context.m_light_marker_id = 1;

	model_manager::ref().update( m_context.get_view_pos(), m_context.get_wvp());
	backend::ref().reset();
	backend::ref().flush();

	// --Porting to DX10_
	//device::ref().device()->SetRenderState( D3DRS_ALPHAFUNC, D3D_COMPARISON_LESS_EQUAL );

	//////////////////////////////////////////////////////////////////////////
	// Rendering stages 

	// G-Stage. The render targets will be cleared.
	m_gstage->execute();

 	// Texture_mip clamp test
// 	m_tex_mip_clamp->execute();

	// Lighting
	m_pre_lighting_stage->execute();

 	// Sun lighting
	m_sun_stage->execute( stage_sun::cascade_near);
	//m_sun_stage_materials->execute( stage_sun_materials::cascade_near);
	
	// dynamic lights
	m_lighting_stage->execute();

	// Ambient, hemi, skybox, postprocesses ???
	m_skybox_stage->execute();

	// Combine 1
	m_combine_stage->execute();

	// Present the final image to base render target
	m_present_stage->execute( m_context.m_t_generic_0);

// --Porting to DX10_
// 	// This maybe need to be removed
// 	backend::ref().set_stencil( false);
// 	backend::ref().set_z( true);
// 	device::ref().device()->SetRenderState( D3DRS_ZFUNC, D3D_COMPARISON_LESS_EQUAL);
}

void scene_render::test( pcstr args)
{
	XRAY_UNREFERENCED_PARAMETER	( args);

	resources::request	resources[]	= {
		{ "resources/barrel_close_1.ogf",					xray::resources::raw_data_class },
//		{ "resources/stalker_dolg_1.ogf",			xray::resources::raw_data_class },
	};

	xray::resources::query_resources	( 
		resources,
		array_size( resources),
		boost::bind(  &scene_render::on_test_visual_loaded, this, _1),
		render::g_allocator
	);
}

class test_effect: public effect
{
public:
	void compile( effect_compiler& compiler, const effect_compilation_options& options)
	{
		XRAY_UNREFERENCED_PARAMETER	( options);

		shader_defines_list	defines;
		make_defines( defines);

		compiler.begin_technique( /*LEVEL*/);
		compiler.begin_pass( "test", "test", defines)
				.set_depth( true, true) //.set_depth( TRUE)
				.set_alpha_blend( false)
			.end_pass()
		.end_technique();
	}

private:
};

void scene_render::on_test_visual_loaded	( xray::resources::queries_result& data)
{
	ASSERT							( !data.empty());

	resources::pinned_ptr_const<u8>	ogf_data	( data[ 0 ].get_managed_resource());

	memory::chunk_reader		chunk_reader( ogf_data.c_ptr(), ogf_data.size(), memory::chunk_reader::chunk_type_sequential);
	
	u16 type					= model_manager::get_visual_type( chunk_reader);
	m_test_visual				= model_manager::create_instance( type);
	m_test_visual->load			( chunk_reader);
	
	test_effect	tb;
	m_test_visual->m_effect = effect_manager::ref().create_effect( &tb);
}



} // namespace render 
} // namespace xray 
