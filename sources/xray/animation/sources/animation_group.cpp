////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_group.h"

#include <xray/resources_queries_result.h>
#include <xray/configs_lua_config.h>
#include <xray/resources_fs.h>

#include "skeleton_animation_data.h"
#include "skeleton_animation.h"
#include "animation_mix.h"
#include "animation_layer.h"
#include "fixed_bone.h"

namespace xray {
namespace animation {

void animation_group::on_anim_data_loaded( resources::queries_result& resource )
{
	u32 size = resource.size();
	for( u32 i = 0; i < size; ++i )
	{

		resources::unmanaged_resource_ptr	data =  resource[i].get_unmanaged_resource();
		add_animation( * static_cast_checked<skeleton_animation_data*>	( data.c_ptr() )  );
	}
	ASSERT( time() >= play_time() );
	if( m_animations.size() == m_size )
		b_loaded = true;

	if( b_loaded && m_callback_on_loaded )
		m_callback_on_loaded( );

}



pcstr gaits[] = { "stand", "walk", "walk_fast", "run", "take" };

struct find_pred
{
	find_pred( pcstr _s ): s( _s ){}
	bool	operator () ( pcstr _s )
	{
		return strings::compare( s, _s) == 0;
	}

	pcstr s;
};

u32 gait( pcstr name )
{
	const pcstr* begin =	&gaits[0];
	const pcstr* end =		&gaits[ array_size( gaits ) ];
	
	const find_pred f( name );

	const pcstr* res = std::find_if( begin, end, f );

	if( res == end )
		return u32( -1 );

	return res - begin;
}




//static const float mix_in_time_to_clips_overlapping_factor =  1.f/0.97f;

animation_group::animation_group( configs::lua_config_value const &cfg, const skeleton *skel, const fixed_bones &fixed_bones ):
m_time( math::infinity ),
m_mix_in_time( 0 ),
m_play_time( 0 ),
m_gait( u32(-1) ),
m_skeleton( skel ),
b_loaded( false ),
m_size( 0 )
{
	m_mix_in_time	= cfg["animation_group"][ "mix_time" ];
	m_play_time		= cfg["animation_group"][ "play_time" ];

	m_gait			= animation::gait ( cfg["animation_group"]["gait"] );

	m_time = m_play_time + m_mix_in_time;


	m_size	= cfg["animation_group"][ "animations" ].size();

	fixed_bones.read_steps( m_foot_steps, m_play_time, cfg["animation_group"]["foot_steps"] );


	for( u32 i = 0; i < m_size; ++i )
	{
		
		
		pcstr a = cfg[ "animation_group" ][ "animations" ][ i ];

		resources::request resources [] =
		{
			a,
			xray::resources::animation_data_class
		};


		xray::resources::query_resources(
				resources ,
				array_size(resources),
				boost::bind( &animation_group::on_anim_data_loaded, this, _1 ),
				xray::animation::g_allocator
			);
	
	}
	
}

animation_group::~animation_group( )
{
	u32 size = m_animation_data.size();
	for( u32 i = 0; i < size; ++i )
	{
		DELETE( m_animations[i]  );
		//DELETE( m_animation_data[i]  );//.?
	}
}

void animation_group::add_animation	( skeleton_animation_data &data  )
{
	data.set_skeleton( m_skeleton );
	m_animations.push_back( NEW( skeleton_animation )( data ) ) ;

	ASSERT( m_time != math::infinity );
	ASSERT( m_time > 0 );
}

void animation_group::create_animations	(  )
{
	u32 size = m_animation_data.size();

	ASSERT( m_animations.size() == 0 );

	for( u32 i = 0; i < size; ++i )
		add_animation( *m_animation_data[i] );
	b_loaded = true;
}


void animation_group::get_end_matrix( float4x4 &m, u32 bone, const buffer_vector< float > &weights )const
{
	const u32 count =  animation_count();
	pvoid bbuffer = ALLOCA( count * sizeof( animation_layer ) );
	animation_mix mix( *m_skeleton, bbuffer, count );

	for( u32 i = 0; i < count; ++i )
	{
		animation_layer layer( m_animations[i], weights[i], 0, m_mix_in_time );
		mix.add( layer );
	}
	
	mix.bone_matrix( bone, play_time(), m );
}

void	g_calculate_anim_fixed_vertex( float3 &pos, const fixed_bone &fb, const animation_mix& mix, float time_anim );


float3 g_fixed_bone_shift(  const fixed_bone &fb, const animation_mix& mix, float time_anim, 
								   float4x4 const &sum_displacement, 
								   float3 const& fixed_global_shift,
								   float3 const& fixed_vetex_global
								   );

void g_calculate_anim_fixed_vertex( float3 &pos, const fixed_bone &fb, const animation_mix& mix, float time_anim, 
								   float4x4 const &sum_displacement, float3 const& fixed_global_shift );






float3 fixed_foots_displacement( const fixed_bones &f_bones, const foot_steps &f_steps, const buffer_vector< float > &weights, const animation_mix &m, float time )
{
	///////////////////////////////////////////////////////////////////////////////////
	const u32 intervals_size = f_steps.intervals_size( );

	buffer_vector< float > intervals( ALLOCA( sizeof(float) * intervals_size  ), intervals_size, intervals_size ) ;

	f_steps.get_intervals( intervals, weights );
	
	buffer_vector< float >::iterator i, b = intervals.begin(), e = intervals.end();


	float3 shift( 0, 0, 0 ); 

//	float3 disp( 0, 0, 0 );

	float3 g_fixed_vertex( 0, 0, 0 ) ;
	float3 g_full_shift( 0, 0, 0 ) ;

	float4x4 sum_displacement = float4x4().identity();

	for( i = b ; i != e - 1 ; ++i )
	{
		const u32 step			=  u32( i - b );
		const u32 fixed_bone_idx	= f_steps.m_bones.bones[ step ];
		if(fixed_bone_idx == u32(-1))
			continue;
		const animation::fixed_bone& fb	= f_bones.bone( fixed_bone_idx );
		
		float time_anim = *i;
		//fix vertex 
		g_calculate_anim_fixed_vertex( g_fixed_vertex, fb, m, time_anim, sum_displacement, g_full_shift );

		time_anim = *( i + 1 );
		
		if( time_anim > time )
		{
			g_full_shift += g_fixed_bone_shift( fb, m, time, sum_displacement, g_full_shift, g_fixed_vertex );
			break;
		}

		float3 shift = g_fixed_bone_shift( fb, m, time_anim, sum_displacement, g_full_shift, g_fixed_vertex );

		g_full_shift += shift;  


#if 0
		//////////////////////////////

		float3 pos0;
		g_calculate_anim_fixed_vertex( pos0, fb, m, *i );

		float3 pos1;
		g_calculate_anim_fixed_vertex( pos1, fb, m, *( i + 1 ) );

		disp += ( pos1 - pos0 );
#endif
		////////////////////////////////
	}

	g_full_shift.y = 0;
/////////////////////////////////////////////////////////////////////////////////////
	return ( g_full_shift );
}

void animation_group::get_displacement( frame &f, float3 &foot_displacement, u32 bone, const fixed_bones &fixed_bones, const buffer_vector< float > &weights )const
{
	

	const u32 count =  animation_count();

	pvoid bbuffer = ALLOCA( count * sizeof( animation_layer ) );
	
	animation_mix m( *m_skeleton, bbuffer, count );

	for( u32 i = 0; i < count; ++i )
	{
		animation_layer layer( m_animations[i], weights[i], 0, m_mix_in_time );
		m.add( layer );
	}
	
	float4x4 m0, m1;
	m.bone_matrix( bone, 0, m0 );
	m.bone_matrix( bone, play_time(), m1 );
	
	float4x4 im0; im0.try_invert( m0 );

	f = matrix_to_frame( im0 * m1 );


	foot_displacement = fixed_foots_displacement( fixed_bones, m_foot_steps, weights, m, play_time() );

}

void animation_group::play( animation_vector_type &anims, const buffer_vector< float > &weights, float time )const
{
	anims.clear( );
	u32 size = m_animations.size();

	//ASSERT( size == m_animation_data.size() );
	ASSERT( size == weights.size() );

	for( u32 i = 0; i < size; ++i )
		anims.push_back( animation_layer( m_animations[i], weights[i], time, m_mix_in_time  ) ); //() * mix_in_time_to_clips_overlapping_factor
}



u32		animation_group::bone_index( pcstr bone_name )const
{
	ASSERT( m_skeleton );
	return skeleton_bone_index( *m_skeleton, bone_name );
}

u32		animation_group::fixed_bone	( float time, const buffer_vector< float > &weights )const
{
	return m_foot_steps.bone( time, weights );
}


} // namespace animation
} // namespace xray