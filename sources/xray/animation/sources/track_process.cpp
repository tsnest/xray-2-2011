////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_world.h"

#include <xray/render/base/debug_renderer.h>
#include <xray/animation/anim_track_common.h>

#if (0)

#include <xray/maya_animation/world.h>
#include <xray/maya_animation/raw_bi_spline.h>
#include <xray/maya_animation/i_anim_track_discrete_channels.h>


using xray::animation::animation_world;
using xray::animation::poly_curve_order3_domain;
using xray::animation::poly_curve;
using xray::animation::bi_spline_data;
using namespace xray::math;



typedef float2 spline_point_type;
typedef xray::animation::vector< std::pair<float, spline_point_type> >	spline_type;



spline_point_type get_c_spline_value	( spline_type const& spline, float time )
{
	bool found							= false;
	spline_type::const_iterator const b	= spline.begin();
	spline_type::const_iterator i		= b;
	spline_type::const_iterator const e	= spline.end();
	for ( ; i != e; ++i ) {
		if ( (*i).first > time ) {
			found	= true;
			break;
		}
	}

	ASSERT								( found );
	ASSERT								( i - b >= 3 );
	u32 const index						= u32(i - b - 1);

	float dT[6]						= {
		spline[index - 1].first - spline[index - 2].first,	// delta Tj-1
		spline[index + 0].first - spline[index - 1].first,	// delta Tj
		spline[index + 1].first - spline[index + 0].first,	// delta Tj+1
		spline[index + 2].first - spline[index + 1].first,	// delta Tj+2
		spline[index + 3].first - spline[index + 2].first,	// delta Tj+3
	};
	u32 const j = 1;
	float const k0						= 1.f/( dT[j+1]* (dT[j+1] + dT[j]  )* (dT[j+1] + dT[j]   + dT[j-1]) );
	float const k10						= k0;//1.f/( dT[j+1]* (dT[j+1] + dT[j]  )* (dT[j+1] + dT[j]   + dT[j-1]) );
	float const k11						= 1.f/( dT[j+1]* (dT[j+1] + dT[j]  )* (dT[j+1] + dT[j]   + dT[j+2]) );
	float const k12						= 1.f/( dT[j+1]* (dT[j+1] + dT[j+2])* (dT[j+1] + dT[j]   + dT[j+2]) );
	float const k20						= k11;//1.f/( dT[j+1]* (dT[j+1] + dT[j]  )* (dT[j+1] + dT[j]   + dT[j+2]) );
	float const k21						= k12;//1.f/( dT[j+1]* (dT[j+1] + dT[j+2])* (dT[j+1] + dT[j]   + dT[j+2]) );
	float const k22						= 1.f/( dT[j+1]* (dT[j+1] + dT[j+2])* (dT[j+1] + dT[j+2] + dT[j+3]) );
	float const k3						= k22;//1.f/( dT[j+1]* (dT[j+1] + dT[j+2])* (dT[j+1] + dT[j+2] + dT[j+3]) );

	//using math::sqr;

	float4x4 factors_matrix;
	float4 (&factors)[4]				= factors_matrix.lines;
	factors[0]							= float4( k3, -k20-k21-k22, k10+k11+k12, -k0 );
	factors[1]							= float4(
											0,
											k20*(dT[j+1] - 2*dT[j]) + k21*(dT[j+1] + dT[j+2] - dT[j]) + k22*(dT[j+1] + dT[j+2] + dT[j+3]),
											k10*(dT[j-1] + dT[j] -2*dT[j+1]) + k11*(-2*dT[j+1] - dT[j+2] + dT[j]) + k12*(-2*(dT[j+1] + dT[j+2])),
											k0*3*dT[j+1]
										);
	factors[2]							= float4(
											0,
											k20*(2*dT[j]*dT[j+1] - sqr(dT[j])) + k21*(dT[j]*(dT[j+1] + dT[j+2])),
											k10*(sqr(dT[j+1]) - 2*dT[j+1]*(dT[j-1] + dT[j])) + k11*(-dT[j]*dT[j+1] + (dT[j+1] - dT[j])*(dT[j+1] + dT[j+2])) + k12*(sqr(dT[j+1] + dT[j+2])),
											-k0*3*sqr(dT[j+1])
										);
	factors[3]							= float4(  0, k20*(dT[j+1]*sqr(dT[j])), k10*(sqr(dT[j+1])*(dT[j-1] + dT[j])) + k11*(dT[j+1]*dT[j]*(dT[j+1] + dT[j+2])), k0*dT[j+1]*sqr(dT[j+1]) );
	float const (&elements)[4][4]		= factors_matrix.elements;

	spline_point_type point_factors[]	= {
		elements[0][0]*spline[index  ].second + elements[0][1]*spline[index-1].second + elements[0][2]*spline[index-2].second + elements[0][3]*spline[index-3].second,
		elements[1][0]*spline[index  ].second + elements[1][1]*spline[index-1].second + elements[1][2]*spline[index-2].second + elements[1][3]*spline[index-3].second,
		elements[2][0]*spline[index  ].second + elements[2][1]*spline[index-1].second + elements[2][2]*spline[index-2].second + elements[2][3]*spline[index-3].second,
		elements[3][0]*spline[index  ].second + elements[3][1]*spline[index-1].second + elements[3][2]*spline[index-2].second + elements[3][3]*spline[index-3].second,
	};
	
	float const dt						= time - spline[index].first;
	float const polynom[]				= { dt*sqr(dt), sqr(dt), dt, 1 };
	return								
		polynom[0]*point_factors[0] + 
		polynom[1]*point_factors[1] + 
		polynom[2]*point_factors[2] + 
		polynom[3]*point_factors[3];
}




spline_type s_bi_spline;

void	animation_world::process_track()
{
	
	//allocator_type* l = g_allocator;
	if( !m_bi_spline.points_number() ==0 )
		return;

	if(!b_load)
	{
		m_amimation_track_data.load( "test_anim_fixed_frames.track" );
		b_load = true;
	}
	if(!m_maya_animation_world)
		create_maya_animation();
	
/*
	for(u32 j = 0;j<m_amimation_track_data.anim_bone_count();++j)
	{
		for(int i = channel_translation_x; i<=channel_translation_z ; ++i)
		{
		  vector_portable<float> &t = m_amimation_track_data.bone_animation( j ).m_channels[i];
		  test_track( t.m_p, t.size(), 0.1f );
		}
		for(int i = channel_rotation_x; i<=channel_rotation_z ; ++i)
		{
		  vector_portable<float> &t = m_amimation_track_data.bone_animation( j ).m_channels[i];
		  test_track( t.m_p, t.size(), 0.0001f );
		}
		
		//for(int i = channel_scale_x; i<=channel_scale_z ; ++i)
		//{
		//  vector_portable<float> &t = m_amimation_track_data.bone_animation( j ).m_channels[i];
		//  test_track( t.m_p, t.size(), 0.001f );
		//}

		
	}

*/
	//m_bone = 20;
	if(m_amimation_track_data.anim_bone_count()<=m_bone)
		return;

	u32 sz = m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_translation_x].size();
	vector_portable<float> v; v.resize( sz * 3 );
	
	for( u32 i = 0; i<sz; ++i )
	{
		//v[i] =	float3
		//(
	
		v[i*3]		=m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_x][i];

		v[i*3+1]	=m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_y][i];

		v[i*3+2]	=m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_z][i];
		//);

	}
	



	//s1961( ep, im, idim, ipar, epar, eeps, ilend, irend, iopen, afctol, itmax,
	//	ik, &m_curve, emxerr, &jstat);
	

/*
	for( int i = 0; i<m_curve->in + m_curve->ik; ++i )
	{
		std::pair<float, spline_point_type> v( float(m_curve->et[i]), float2( float(m_curve->ecoef[i*3+1]),float(m_curve->ecoef[i*3+2]) ));
		

		s_bi_spline.push_back( v );
	}

*/
	//bi_spline_data<float3> d;
	//sisl_curve_to_bi_spline_data<float3,3>( *m_curve, d );

	
	m_raw_bi_spline->build_approximation( v.m_p, sz, float3( 0.0005f, 0.0005f, 0.0005f  ) );
	m_raw_bi_spline->get_data( m_bi_spline );

	m_poly_curve.create( m_bi_spline, 1 );

}

static float disp = 0.002f;
static float scale = 100.f;//100.f;

static float spline_max_time = 0;

void	animation_world::render_curves( xray::render::debug::renderer& renderer ) const	
{
	
	//if(!m_curve)
			//return;

	

	u32 sz = m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_translation_x].size();
	float d =1. ; //; 0.5f;

	//if( spline_max_time >= m_poly_curve.max_param()-2*d)
		//spline_max_time = 0;

	//spline_max_time+=0.00001f;
	///for( float t = m_poly_curve.min_param(); t< spline_max_time; t+=d )
	for( u32 i = 0; i< sz-2; ++i)
	{
		float t = float( i );
		//v[i] =	float3
		//(
		
		float3	start(
		m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_x][i],
		m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_y][i],
		m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_z][i]
		),
		end(
		m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_x][i+1],
		m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_y][i+1],
		m_amimation_track_data.bone_animation( m_bone ).m_channels[channel_rotation_z][i+1]
		);
		float3 vdisp = float3(0,disp,0);
		

		start*=scale;
		end*=scale;
		renderer.draw_line( start, end, math::color_xrgb( 255, 0, 0 )  );

		
	






		float3	start1, end1;

		ASSERT( m_raw_bi_spline );
		m_raw_bi_spline->evaluate( t, start1 );
		m_raw_bi_spline->evaluate( t+d, end1 );
		
		start1 += disp; start1*=scale;
		end1+= disp; end1*=scale;

		renderer.draw_line(start1 ,
							end1,
							math::color_xrgb( 0, 255, 0 )  );

		
		//float3	start2 , end2;
		//m_poly_curve.evaluate(t, start2 );
		//m_poly_curve.evaluate(t+d, end2 );
		//
		//start2+=2*vdisp;end2+=2*vdisp;
		//start2*=scale;end2*=scale;

		//renderer.draw_line(start2 ,
		//					end2,
		//					math::color_xrgb( 0, 0, 255 )  );


		
		

		//float2 ss = get_c_spline_value( s_bi_spline, t );
		//float2 ee = get_c_spline_value( s_bi_spline, t+d );

		//float3	start2(0,ss.x,ss.y) , end2(0,ee.x,ee.y);
	
		float3	start2 , end2;

		m_poly_curve.evaluate(t, start2 );
		m_poly_curve.evaluate(t+d, end2 );
		
		start2+=2*vdisp;end2+=2*vdisp;
		start2*=scale;end2*=scale;
		renderer.draw_line( start2 ,
							end2,
							math::color_xrgb( 0, 0, 255 )  );
		//);
		
	}
	
}

#endif

