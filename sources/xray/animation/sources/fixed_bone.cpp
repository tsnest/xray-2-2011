////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "fixed_bone.h"
#include <xray/animation/skeleton.h>
#include <xray/render/facade/debug_renderer.h>

namespace xray {

namespace animation {

	fixed_bones::fixed_bones( const animation::skeleton &s, memory::base_allocator& allocator  ):
		m_skeleton( s ),
		m_bones( allocator )
	{
		m_bones.push_back( fixed_bone() );
		m_bones.back().m_anim_skeleton_idx = skeleton_bone_index( s, "LeftToe" );
		m_bones.push_back( fixed_bone() );
		m_bones.back().m_anim_skeleton_idx = skeleton_bone_index( s, "RightToe" );
	}

	void fixed_bone::read( configs::binary_config_value const& config, const animation::skeleton &s )
	{
		pcstr bone_name = config["name"];
		
		m_anim_skeleton_idx = skeleton_bone_index( s, bone_name );
		ASSERT( m_anim_skeleton_idx != u32(-1), "fixed_bone :%s not found", bone_name );
		//float3 p = config["vertex"];
		m_fixed_vertex = config["vertex"];
	}


	void	fixed_bones::read ( configs::binary_config_value const& config  )
	{
		const u32 sz = config.size();
		m_bones.resize( sz );
		vector<fixed_bone>::iterator i, b = m_bones.begin(), e = m_bones.end();
		for ( i = b ; i != e; ++i )
			(*i).read( config[ u32( i - b ) ], m_skeleton );

	}
	
	struct find_predicate
	{
		u32 idx;

		find_predicate( u32 bone_idx ): idx( bone_idx ){}

		bool operator () ( const fixed_bone &b )const
		{
			return b.m_anim_skeleton_idx == idx;
		}
	};

	u32	fixed_bones::fixed_bone_idx	( pcstr name )const
	{
		u32 skel_bone_idx = skeleton_bone_idx( name );
		
		find_predicate fp( skel_bone_idx );
		vector<fixed_bone>::const_iterator r = std::find_if ( m_bones.begin(), m_bones.end(), fp );

		if ( r == m_bones.end() )
			return u32(-1);

		return u32( r - m_bones.begin() );
	}

	u32 fixed_bones::skeleton_bone_idx( pcstr name )const
	{
		pcstr	free_interval = "none";
		if ( strings::compare( name, free_interval ) == 0 )
			return u32( -1 );
		
		return skeleton_bone_index( m_skeleton, name );
	}

	void	fixed_bones::read_steps ( foot_steps &steps, float play_time, configs::binary_config_value const& config )const
	{
		
		const u32 sz_bones		= config["bones"].size();
		
		if ( sz_bones  == 0 )
			return;

		for ( u32 i = 0; i < sz_bones; ++i )
			steps.m_bones.bones.push_back( fixed_bone_idx( config["bones"][ i ] ) );

		const u32 sz_anims = config["intervals"].size();

		steps.m_steps.resize( sz_anims );

		for ( u32 j = 0; j < sz_anims; ++j )
		{
			const u32 sz_intervals	= config["intervals"][j].size();
			R_ASSERT( sz_intervals >= 2 );
			R_ASSERT( sz_bones + 1 == sz_intervals );

			for ( u32 i = 0; i < sz_intervals; ++i )
			{
				steps.m_steps[j].intervals.push_back( float ( config["intervals"][j][i] ) );
			}
			R_ASSERT_CMP( steps.m_steps[j].intervals[ sz_intervals - 2 ], <=, play_time  );

			steps.m_steps[j].intervals[ sz_intervals - 1 ] = play_time ;

		}
	}
	
	float	sum_weights		( const buffer_vector< float > &weights );
	void foot_steps::get_intervals( buffer_vector< float > &intervals, float start_time,  const buffer_vector< float > &weights )const
	{
		XRAY_UNREFERENCED_PARAMETERS( start_time );
		const u32 intervals_size = intervals.size();
		const u32 sz = m_steps.size();
		ASSERT( weights.size() >= sz );
		ASSERT( math::is_similar( sum_weights( weights ) , 1.f )  );

		for ( u32 i = 0; i < intervals_size; ++i )
		{
			intervals[i] = 0;

			for ( u32 j = 0; j < sz; ++j )
			{
				R_ASSERT( m_steps[ j ].intervals.size() == intervals_size );
				
				intervals[i] += weights[ j ] * m_steps[ j ].intervals[ i ];
			}

		}
	}

	u32 foot_steps::bone( float time, float start_time, const buffer_vector< float > &weights )const
	{
		float dummi;
		return bone( time, start_time, weights, dummi );
	}

	u32		foot_steps::bone			( float time, float start_time, const buffer_vector< float > &weights, float& interval_start )const
	{
		const u32 sz = weights.size();

		R_ASSERT_U( sz == m_steps.size( ) );

		const u32 intervals_size = foot_steps::intervals_size( );

		buffer_vector< float > intervals( ALLOCA( sizeof(float) * intervals_size  ), intervals_size, intervals_size ) ;
		
		get_intervals( intervals, start_time, weights );
		
		buffer_vector< float >::iterator  ib = intervals.begin() , ie = intervals.end();

		const float play_time = *(ie-1);

		float t = time + start_time;
		if ( t >= play_time )
			t -= play_time;

		buffer_vector< float >::iterator res = std::lower_bound( ib, ie, t );
		
		//if ( res == ib || res == ie )
		//	return u32(-1);

		if ( res == ib  )
		{
			interval_start = *ib;
			return m_bones.bones[ 0 ];
		}
		if ( res == ie  )
		{
			interval_start = play_time;
			return m_bones.bones[ m_bones.bones.size() - 1 ];
		}
		const u32 step = u32( res - ib ) - 1;
		u32 fixed_b_idx = m_bones.bones[ step ];
		
		interval_start = *( res - 1 );

		return fixed_b_idx;

	}

	void	foot_steps::render		( render::debug::renderer	&renderer, const float4x4 &matrix, u32 color, const buffer_vector< float > &weights, const buffer_vector< float > &time_scales, const float render_time_scale )
	{
		XRAY_UNREFERENCED_PARAMETERS( &renderer, &matrix, color, &weights, &time_scales, render_time_scale );
		
		const u32 sz = m_steps.size();
		R_ASSERT( weights.size() == sz );
		R_ASSERT( sz == time_scales.size() );

		buffer_vector< float > intervals ( ALLOCA( sizeof( float ) * sz ), sz ); 

		//get_intervals( intervals, weights );
		
		//const u32 color0		= math::color_xrgb( 255, 0,   255 );
		//const u32 color1		= math::color_xrgb( 0,   255, 255 );
		//const u32 color_none	= math::color_xrgb( 255, 255, 255 );
		
		//const float3 disp( 0, 0.02f, 0 );

		//for ( u32 i = 0; i < sz ; ++i )
		//{
			
		//}

	}

} // namespace animation
} // namespace xray