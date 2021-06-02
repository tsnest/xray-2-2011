////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_sun.h"
#include <xray/render/core/effect_manager.h>
#include "effect_light_mask.h"
#include "effect_light_direct.h"
#include <xray/render/core/backend.h>
#include "convex_volume.h"
#include "scene_view.h"
#include "renderer_context.h"
#include <xray/render/core/pix_event_wrapper.h>
#include "renderer_context_targets.h"
#include "light.h"
#include "scene.h"
#include "lights_db.h"
#include "shared_names.h"
#include "vertex_formats.h"
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/effect_options_descriptor.h>
#include "material.h"
#include <xray/render/core/options.h>

#include "statistics.h"


// #pragma warning( push )
// #pragma warning( disable : 4995 )
// #pragma message( XRAY_TODO("Remove D3D math usage") )
// #include <d3dx9math.h>
// #pragma warning( pop )
// 
// #pragma comment( lib, "d3dx9.lib" )

namespace xray {
namespace render {

namespace
{
	float		ps_r2_sun_tsm_projection	= 0.18f;			// 0.18f
	float		ps_r2_sun_tsm_bias			= -0.05f;			// 
	float		ps_r2_sun_near				= 300.f;				// 12.0f

	float		ps_r2_sun_near_border		= 0.75f;			// 1.0f
	float		ps_r2_sun_depth_far_scale	= 1.00000f;			// 1.00001f
	float		ps_r2_sun_depth_far_bias	= 0.00000f;			// -0.0000f
	float		ps_r2_sun_depth_near_scale	= 1.00001f;			// 1.00001f
	float		ps_r2_sun_depth_near_bias	= -0.00004f;		// -0.00005f
	float		ps_r2_sun_lumscale			= 1.0f;				// 1.0f
	float		ps_r2_sun_lumscale_hemi		= 1.0f;				// 1.0f
	float		ps_r2_sun_lumscale_amb		= 1.0f;

	const	float	tweak_COP_initial_offs			= 1200.f	;
	const	float	tweak_ortho_xform_initial_offs	= 1000.f	;	//. ?
	const	float	tweak_guaranteed_range			= 20.f		;	//. ?

	//////////////////////////////////////////////////////////////////////////
	// tables to calculate view-frustum bounds in world space
	// note: D3D uses [0..1] range for Z
	static float3		corners [8]			= {
		float3( -1, -1,  0), float3( -1, -1, +1), 
		float3( -1, +1, +1), float3( -1, +1,  0), 
		float3( +1, +1, +1), float3( +1, +1,  0), 
		float3( +1, -1, +1), float3( +1, -1,  0)
	};
	static int			facetable[6][4]		= {
		{ 0, 3, 5, 7 }, 		{ 1, 2, 3, 0 }, 
		{ 6, 7, 5, 4 }, 		{ 4, 2, 1, 6 }, 
		{ 3, 2, 4, 5 }, 		{ 1, 0, 7, 6 }, 
	};

	float3 calc_hp( float h, float p)//What does it mean????????????????????
	{
		float ch= cos( h), cp=cos( p), sh=sin( h), sp=sin( p);
		float x = -cp*sh;
		float y = sp;
		float z = cp*ch;
		return float3( x, y, z);	
	}
}

class	dumb_convex_volume
{
public:
	struct	_poly
	{
		vector<int>	points;
		float3		planeN;
		float			planeD;
		float			classify	( float3& p)	{	return planeN.dot_product( p)+planeD; 	}
	};
	struct	_edge
	{
		int				p0, p1;
		int				counter;
		_edge		( int _p0, int _p1, int m) : p0( _p0), p1( _p1), counter( m){ if ( p0>p1)	std::swap( p0, p1); 	}
		bool			equal		( _edge& E)												{ return p0==E.p0 && p1==E.p1;	}
	};
public:
	vector<float3>		points;
	vector<_poly>		polys;
	vector<_edge>		edges;
public:
	void				compute_planes	()
	{
		for ( int it=0; it<int( polys.size()); it++)
		{
			_poly&			P	=	polys[it];
			float3		t1, t2;
			t1 = points[P.points[0]] - points[P.points[1]];
			t2 = points[P.points[0]] - points[P.points[2]];
			P.planeN = math::cross_product( t1, t2);
			P.planeN.normalize();
			P.planeD = -P.planeN.dot_product( points[P.points[0]]);

			// ASSERT
			//if ( _debug)
			//{
			//	Fvector&		p0	= points[P.points[0]];
			//	Fvector&		p1	= points[P.points[1]];
			//	Fvector&		p2	= points[P.points[2]];
			//	Fvector&		p3	= points[P.points[3]];
			//	Fplane	p012;	p012.build( p0, p1, p2);
			//	Fplane	p123;	p123.build( p1, p2, p3);
			//	Fplane	p230;	p230.build( p2, p3, p0);
			//	Fplane	p301;	p301.build( p3, p0, p1);
			//	ASSERT	( p012.n.similar( p123.n) && p012.n.similar( p230.n) && p012.n.similar( p301.n));
			//}
		}
	}
	void compute_caster_model( vector<math::plane>& dest, float3 direction)
	{
		//CRenderTarget&	T	= *RImplementation.Target;

		// COG
		float3	cog( 0, 0, 0);
		for( int it=0; it<int( points.size()); it++)
			cog += points[it];

		cog /= float( points.size());

		// planes
		compute_planes	();
		for ( int it=0; it<int( polys.size()); it++)
		{
			_poly&	base				= polys	[it];
			if ( base.classify( cog)>0)	std::reverse( base.points.begin(), base.points.end());
		}

		// remove faceforward polys, build list of edges -> find open ones
		compute_planes	();
		for ( int it=0; it<int( polys.size()); it++)
		{
			_poly&	base		= polys	[it];
			ASSERT	( base.classify( cog)<0);								// debug

			int		marker		= ( base.planeN.dot_product( direction)<=0)?-1:1;

			// register edges
			vector<int>&	plist		= polys[it].points;
			for ( int p=0; p<int( plist.size()); p++)	{
				_edge	E		( plist[p], plist[ ( p+1)%plist.size() ], marker);
				bool	found	= false;
				for ( int e=0; e<int( edges.size()); e++)	
					if ( edges[e].equal( E))
					{
						edges[e].counter += marker;
						found=true;
						break;
					}

				if ( !found)
				{
					edges.push_back	( E);
					//if	( _debug)	T.dbg_addline( points[E.p0], points[E.p1], color_rgba( 255, 0, 0, 255));
				}
			}

			// remove if unused
			if ( marker<0)	{
				polys.erase	( polys.begin()+it);
				it--;
			}
		}

		// Extend model to infinity, the volume is not capped, so this is indeed up to infinity
		for ( int e=0; e<int( edges.size()); e++)
		{
			if	( edges[e].counter != 0)	continue;
			_edge&		E		= edges[e];
			//if		( _debug)	T.dbg_addline( points[E.p0], points[E.p1], color_rgba( 255, 255, 255, 255));
			//float3	point;
			points.push_back	( /*point.sub*/( points[E.p0]-direction));
			points.push_back	( /*point.sub*/( points[E.p1]-direction));
			polys.push_back		( _poly());
			_poly&		P		= polys.back();	
			int			pend	= int( points.size());
			P.points.push_back	( E.p0);
			P.points.push_back	( E.p1);
			P.points.push_back	( pend-1);	//p1 mod
			P.points.push_back	( pend-2);	//p0 mod
			//if		( _debug)	T.dbg_addline( points[E.p0], point.mad( points[E.p0], direction, -1000), color_rgba( 0, 255, 0, 255));
			//if		( _debug)	T.dbg_addline( points[E.p1], point.mad( points[E.p1], direction, -1000), color_rgba( 0, 255, 0, 255));
		}

		// Reorient planes ( try to write more inefficient code :)
		compute_planes	();
		for ( int it=0; it<int( polys.size()); it++)
		{
			_poly&	base				= polys	[it];
			if ( base.classify( cog)>0)	std::reverse( base.points.begin(), base.points.end());
		}

		// Export
		compute_planes	();
		for ( int it=0; it<int( polys.size()); it++)
		{
			_poly&			P	= polys[it];
			math::plane		pp( P.planeN, P.planeD);
			dest.push_back	( pp);
		}
	}
};


stage_sun::stage_sun( renderer_context* context ): stage( context)
{
	effect_manager::ref().create_effect<effect_light_mask>(&m_sh_accum_mask);
	effect_manager::ref().create_effect<effect_light_direct<0>>(&m_sh_accum_direct[0]);
	effect_manager::ref().create_effect<effect_light_direct<1>>(&m_sh_accum_direct[1]);
	effect_manager::ref().create_effect<effect_light_direct<2>>(&m_sh_accum_direct[2]);
	effect_manager::ref().create_effect<effect_light_direct<3>>(&m_sh_accum_direct[3]);
	
	m_c_light_color					= backend::ref().register_constant_host( "light_color", rc_float );
	m_c_light_direction				= backend::ref().register_constant_host( "light_direction", rc_float );
	m_c_light_intensity				= backend::ref().register_constant_host( "light_intensity", rc_float ); 
	m_c_shadow_transparency			= backend::ref().register_constant_host( "shadow_transparency",  rc_float );
	m_c_lighting_model				= backend::ref().register_constant_host( "lighting_model", rc_int );
	m_c_diffuse_influence_factor	= backend::ref().register_constant_host( "light_diffuse_influence_factor", rc_float );
	m_c_specular_influence_factor	= backend::ref().register_constant_host( "light_specular_influence_factor", rc_float );
	
	m_shadow[0]					= backend::ref().register_constant_host( "m_shadow0",  rc_float );
	m_shadow[1]					= backend::ref().register_constant_host( "m_shadow1", rc_float );
	m_shadow[2]					= backend::ref().register_constant_host( "m_shadow2", rc_float );
	m_shadow[3]					= backend::ref().register_constant_host( "m_shadow3", rc_float );
	
	
	m_c_environment_skylight_upper_color = backend::ref().register_constant_host( "environment_skylight_upper_color", rc_float );
	m_c_environment_skylight_lower_color = backend::ref().register_constant_host( "environment_skylight_lower_color", rc_float );
	
	m_enabled					= options::ref().m_enabled_sun_stage;
	
//	m_sunmask					= backend::ref().register_constant_host( "sunmask", rc_float );
}

bool stage_sun::is_effects_ready() const
{
	return	m_sh_accum_mask.c_ptr() != NULL &&
			m_sh_accum_direct[0].c_ptr() != NULL &&
			m_sh_accum_direct[1].c_ptr() != NULL &&
			m_sh_accum_direct[2].c_ptr() != NULL &&
			m_sh_accum_direct[3].c_ptr() != NULL;
}

stage_sun::~stage_sun()
{
}

#if 0
float4x4 stage_sun::calc_sun_matrix( const float4x4& frustum, const float3& dir)
{
	float4x4	inv_frustum;
	D3DXMatrixInverse( ( D3DXMATRIX*)&inv_frustum, 0, ( D3DXMATRIX*)&frustum);

	// Compute volume( s) - something like a frustum for infinite directional light
	// Also compute virtual light position and sector it is inside
	convex_volume			cull_volume;
	vector<math::plane>		cull_planes;
	float3					cull_COP;
	float4x4				cull_xform;

	// Lets begin from base frustum
	dumb_convex_volume	hull;

	hull.points.reserve		( 9);
	for	( int p=0; p<8; p++)
	{
		float3				xf	= utils::wform		( inv_frustum, corners[p]);
		hull.points.push_back	( xf);
	}

	for ( int plane=0; plane<6; plane++)
	{
		hull.polys.push_back( dumb_convex_volume::_poly());
		for ( int pt=0; pt<4; pt++)	
			hull.polys.back().points.push_back( facetable[plane][pt]);
	}

	hull.compute_caster_model( cull_planes, dir);

	// COP - 100 km away
	cull_COP = m_context->get_view_pos() - tweak_COP_initial_offs*dir;

	// Create frustum for query
	//cull_volume/*.clear()*/ = math::cuboid( &cull_planes[0], cull_planes.size());
	cull_volume.create_from_planes( &cull_planes[0], cull_planes.size());
	//for ( u32 p=0; p<cull_planes.size(); ++p)
	//	cull_volume.add( cull_planes[p]);

	// Create approximate ortho-xform
	// view: auto find 'up' and 'right' vectors
	float4x4	mdir_View, mdir_Project;
	float3		L_dir, L_up, L_right, L_pos;
	
	L_pos = dir;
	L_dir = normalize( dir);

	L_right.set( 1, 0, 0);
	if ( abs( L_right.dot_product( L_dir))>.99f)
		L_right.set( 0, 0, 1);

	L_up = normalize( cross_product( L_dir, L_right));
	L_right = normalize( cross_product( L_up, L_dir));

	mdir_View = create_camera_direction( L_pos, L_dir, L_up);

	//	Simple
	math::aabb	bb;
	bb.invalidate();
	for ( int it=0; it<8; it++)
	{
		float3	xf	= utils::wform		( mdir_View, hull.points[it]);
		bb.modify		( xf);
	}

	//bb.grow( math::epsilon_5);
	bb.min -= math::epsilon_5;
	bb.max += math::epsilon_5;
	
	D3DXMatrixOrthoOffCenterLH( ( D3DXMATRIX*)&mdir_Project, bb.min.x, bb.max.x, bb.min.y, bb.max.y, bb.min.z-tweak_ortho_xform_initial_offs, bb.max.z);

	return mdir_Project;
}
#endif // #if 0

void stage_sun::execute()
{
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	BEGIN_CPUGPU_TIMER(statistics::ref().lights_stat_group.accumulate_lighting_time);
	execute_cascade(0);
	END_CPUGPU_TIMER;

	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}

void stage_sun::execute_cascade( u32 cascade)
{
	PIX_EVENT( stage_sun);

	//backend::ref().set_render_targets	( &*m_context->m_rt_accumulator, 0, 0, 0);
	backend::ref().set_render_targets	( &*m_context->m_targets->m_rt_accumulator_diffuse, &*m_context->m_targets->m_rt_accumulator_specular, 0, 0);
	
	backend::ref().reset_depth_stencil_target();
	//backend::ref().set_depth_stencil_target( NULL);
	
	// *** assume accumulator setted up ***
	light* sun = m_context->scene()->lights().get_sun().c_ptr();//( light*)RImplementation.Lights.sun_adapted._get()	;
	
	if ( !sun)
	{
		return;
	}
	/*
	sun->set_color(math::color(
		m_context->m_post_process_parameters.environment_sun_color.x, 
		m_context->m_post_process_parameters.environment_sun_color.y, 
		m_context->m_post_process_parameters.environment_sun_color.z, 
		1.0f
		), 1.f );
	*/
	//sun->set_position(m_context->m_post_process_parameters.environment_sun_position);
	//float3 d = -m_context->m_post_process_parameters.environment_sun_position.normalize();
	//sun->set_orientation(d, xray::math::cross_product(d, float3(0,1,0)));
	
	// Common calc for quad-rendering
	u32		offset;

	D3D_VIEWPORT viewport;
	backend::ref().get_viewport( viewport);

	float	w = float( backend::ref().target_width());
	float	h = float( backend::ref().target_height());
	float	z;

	// Adjust tc so that center of texel and pixel were aligned
	float2	p0( viewport.TopLeftX/w, viewport.TopLeftY/h);
	float2	p1( (viewport.TopLeftX + viewport.Width)/w, ( viewport.TopLeftY + viewport.Height)/h);

	// Common constants ( light-related)
	//float3	sun_dir( m_context->m_mat_view.transform_direction( /*( float3( 0.47652841f, -0.80543172f, -0.35242066f))*/sun->direction));
	float3	sun_dir( m_context->get_v().transform_direction( /*( float3( 0.47652841f, -0.80543172f, -0.35242066f))*/sun->direction));

	float3	sun_clr = sun->color;

	float	sun_spec = utils::u_diffuse2s( sun_clr);

	sun_dir.normalize(); //Is this necessary????


	u32 mat_count = 1;
	for( u32 mat_id = 0; mat_id < mat_count; ++mat_id)
	{
		if( mat_id == 0 && (m_context->get_lighting_material_strtegy() == lms_stenciling || ( m_context->get_lighting_material_strtegy() == lms_material_branching_separate) ) )
		{
			if ( 0==cascade)
				phase_sun_mask( mat_id, sun_dir, sun_clr, sun_spec, sun->intensity, sun->lighting_model, sun->diffuse_influence_factor, sun->specular_influence_factor );
		}

		float3	center_pt;
		
		float sun_near = m_context->get_far();
		
		center_pt = m_context->get_view_pos() + sun_near * m_context->get_view_dir();//mad( Device.vCameraPosition, Device.vCameraDirection, ps_r2_sun_near);
		float3 tr = m_context->/*m_full_xform*/get_vp().transform( center_pt);
		//float3 t2 = m_context->m_mat_proj.transform( float3( 0, 0, ps_r2_sun_near));
		float3 t2 = m_context->get_p().transform( float3( 0, 0, sun_near));
		//ASSERT( math::is_similar( tr.z, t2.z));
		z = t2.z;
	//	z = center_pt.z;

		// clouds xform
		
		float4x4				m_clouds_shadow;
		m_clouds_shadow.identity();
		/*
		{
			static float	w_shift = 0;
			float4x4		m_xform;
			float3			direction	= sun->direction;
			float			w_dir		= 0;//g_pGamePersistent->Environment().CurrentEnv->wind_direction	;
			//float	w_speed				= g_pGamePersistent->Environment().CurrentEnv->wind_velocity	;
			float3			normal;

			normal = calc_hp( w_dir, 0);
			//w_shift	+=	0.003f*Device.fTimeDelta;

			float3	position( 0, 0, 0);

			m_xform = math::create_camera_direction( position, direction, normal)	;
			
			float3	localnormal;
			
			localnormal = m_xform.transform_direction( normal);
			localnormal.normalize();
			
			//m_clouds_shadow = mul4x4( m_context->m_mat_view_inv, m_xform);
			m_clouds_shadow = mul4x4( m_context->get_v_inverted(), m_xform);
			m_xform = math::create_scale( float3( 0.002f, 0.002f, 1.f));
			m_clouds_shadow = mul4x4( m_clouds_shadow, m_xform);
			m_xform = math::create_translation( localnormal*w_shift);
			m_clouds_shadow = mul4x4( m_clouds_shadow, m_xform);
		}
		*/

		// Make jitter texture
		float	scale_X = w/float( tex_jitter);
		//float	scale_Y	= h/float( tex_jitter);
		float	tc_offset = 0/float( tex_jitter);
		
		// Adjust tex coords
		float2	j0( tc_offset, tc_offset);
		float2	j1( scale_X+tc_offset, scale_X+tc_offset);

		float3 const* eye_rays = m_context->get_eye_rays();
		// Fill vertex buffer
		vertex_formats::Tquad* pv = backend::ref().vertex.lock<vertex_formats::Tquad>( 4, offset);
		pv->set( 0, h, 0, 1.0, eye_rays[1].x, eye_rays[1].y, eye_rays[1].z, p0.x, p1.y); pv++;
		pv->set( 0, 0, 0, 1.0, eye_rays[0].x, eye_rays[0].y, eye_rays[0].z, p0.x, p0.y); pv++;
		pv->set( w, h, 0, 1.0, eye_rays	[3].x, eye_rays[3].y, eye_rays[3].z, p1.x, p1.y); pv++;
		pv->set( w, 0, 0, 1.0, eye_rays[2].x, eye_rays[2].y, eye_rays[2].z, p1.x, p0.y); pv++;
		backend::ref().vertex.unlock();
		
		//m_context->m_g_quad_2uv->apply();
		m_context->m_g_quad_eye_ray->apply();

		if( m_context->get_lighting_material_strtegy() == lms_stenciling)
		{
 			if( mat_id == 0)
				m_sh_accum_direct[ sun->lighting_model ]->apply( cascade);
			else
				m_sh_accum_direct[ sun->lighting_model ]->apply( cascade+1);
		}
		else if( m_context->get_lighting_material_strtegy() == lms_without_masking)
		{
			if( mat_id == 0)
				m_sh_accum_direct[ sun->lighting_model ]->apply( cascade+2);
			else
				m_sh_accum_direct[ sun->lighting_model ]->apply( cascade+1+2);
		}
		else if( m_context->get_lighting_material_strtegy() == lms_material_branching_separate)
		{
			if( mat_id == 0)
				m_sh_accum_direct[ sun->lighting_model ]->apply( cascade+4);
			else
				m_sh_accum_direct[ sun->lighting_model ]->apply( cascade+1+4);
		}

		backend::ref().set_ps_constant( m_c_light_direction, 	float4(sun_dir, 0) );
 		backend::ref().set_ps_constant( m_c_light_color,		float4(sun_clr, sun_spec) );
		backend::ref().set_ps_constant( m_c_light_intensity,	sun->intensity );
		backend::ref().set_ps_constant( m_c_lighting_model,		sun->lighting_model );
		backend::ref().set_ps_constant( m_c_diffuse_influence_factor, sun->diffuse_influence_factor );
		backend::ref().set_ps_constant( m_c_specular_influence_factor, sun->specular_influence_factor );
		
		// TODO: remove skylight from sun pass, add the skylight_pass
		backend::ref().set_ps_constant( m_c_environment_skylight_upper_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_upper_color );
		backend::ref().set_ps_constant( m_c_environment_skylight_lower_color, m_context->get_scene_view()->post_process_parameters().environment_skylight_lower_color );
		
		for (u32 cascade_index=0;cascade_index<sun_cascade:: num_max_sun_shadow_cascades; cascade_index++)
			backend::ref().set_ps_constant( m_shadow[cascade_index], transpose( m_context->get_view2shadow(cascade_index) ));
		
		backend::ref().set_ps_constant( m_c_shadow_transparency,		sun->shadow_transparency );
		
		//backend::ref().set_ps_constant( m_sunmask, transpose( m_clouds_shadow ) );

		// setup stencil
		//u32 mat_marker = (2+mat_id)<<6;

		/*if( m_context->get_lighting_material_strtegy() == lms_stenciling )
		{
			backend::ref().set_stencil_ref( mat_marker|0x01);
		}
		else if( m_context->get_lighting_material_strtegy() == lms_without_masking)
		{
			backend::ref().set_stencil_ref( mat_marker);
		}
		else if( m_context->get_lighting_material_strtegy() == lms_material_branching_separate)
		{
			backend::ref().set_stencil_ref( mat_marker|0x01);
		}
*/
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, offset);
		
	}

	m_context->set_w					( float4x4().identity() );
}

void stage_sun::phase_sun_mask( u32 material_id, float3 const & light_dir, float3 const & light_color, float light_spec, float intensity, int lighting_model, float diffuse_influence_factor, float specular_influence_factor )
{
	PIX_EVENT( render_sun_mask);
	XRAY_UNREFERENCED_PARAMETER( material_id);

	D3D_VIEWPORT viewport;
	backend::ref().get_viewport( viewport);

	float	w = float( backend::ref().target_width());
	float	h = float( backend::ref().target_height());

	u32		color = math::color_rgba( 255u, 255u, 255u, 255u);

	float2	p0( viewport.TopLeftX/w, viewport.TopLeftY/h);
	float2	p1( (viewport.TopLeftX + viewport.Width)/w, ( viewport.TopLeftY + viewport.Height)/h);

	// Fill vertex buffer
	u32		offset;

	//FIX ME: May be we can fill this vb's once????
	vertex_formats::TL* pv = ( vertex_formats::TL*)backend::ref().vertex.lock( 4, sizeof( vertex_formats::TL)/*m_context->m_g_quad_uv->get_stride()*/, offset);
	//pv->set( math::epsilon_5, 	h+math::epsilon_5, z, w, color, p0.x, p1.y);	pv++;
	//pv->set( math::epsilon_5, 	math::epsilon_5, 	 z, w, color, p0.x, p0.y);	pv++;
	//pv->set( w+math::epsilon_5, 	h+math::epsilon_5, z, w, color, p1.x, p1.y);	pv++;
	//pv->set( w+math::epsilon_5, 	math::epsilon_5, 	 z, w, color, p1.x, p0.y);	pv++;
	pv->set( 0, h, 0, 1.0, color, p0.x, p1.y);	pv++;
	pv->set( 0, 0, 0, 1.0, color, p0.x, p0.y);	pv++;
	pv->set( w, h, 0, 1.0, color, p1.x, p1.y);	pv++;
	pv->set( w, 0, 0, 1.0, color, p1.x, p0.y);	pv++;
	backend::ref().vertex.unlock();
	
	// Setup rendering states and objects
	m_context->m_g_quad_uv->apply();

	//u32 mat_marker = (2+material_id)<<6;
	if( m_context->get_lighting_material_strtegy() == lms_material_branching_separate)
	{
		m_sh_accum_mask->apply( effect_light_mask::tech_mask_direct_single_material/*SE_MASK_DIRECT*/);	
		//backend::ref().set_stencil_ref( m_context->m_light_marker_id|mat_marker);
	}
	else
		m_sh_accum_mask->apply( effect_light_mask::tech_mask_direct/*SE_MASK_DIRECT*/);	

 	backend::ref().set_ps_constant( m_c_light_direction, 	float4( light_dir.x, light_dir.y, light_dir.z, 0));
	backend::ref().set_ps_constant( m_c_light_color,		float4( light_color, light_spec));
	backend::ref().set_ps_constant( m_c_light_intensity,	intensity ); // TODO!
	backend::ref().set_ps_constant( m_c_lighting_model,		lighting_model );
	backend::ref().set_ps_constant( m_c_diffuse_influence_factor, diffuse_influence_factor );
	backend::ref().set_ps_constant( m_c_specular_influence_factor, specular_influence_factor );

	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, offset);
}


} // namespace render
} // namespace xray
