////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_mix.h"
#include "skeleton_animation.h"

#include <xray/render/base/debug_renderer.h>

namespace xray {
namespace animation {
	
	static const u32 max_mixing_animations = 10;
	static const u32 max_mixing_animations_mem_size = max_mixing_animations * sizeof( mixing_animation_type );

	animation_mix::animation_mix  ( const skeleton& skel ):
	m_skeleton( skel ),
	m_blend_vector( MALLOC( max_mixing_animations_mem_size, "animation_mix" ), max_mixing_animations ),
	m_external_buffer(false)
	{
		m_blend_vector.reserve( max_mixing_animations );
	}

	animation_mix::animation_mix( const skeleton& skel, pvoid buffer, u32 max_count ):
	m_skeleton( skel ),
	m_blend_vector( buffer, max_count ),
	m_external_buffer(true)
	{
		m_blend_vector.reserve( max_count );
	}

	animation_mix::~animation_mix		( )
	{
		if( m_external_buffer )
			return;
		m_blend_vector.clear();
		mixing_animation_type	*buffer = m_blend_vector.begin();
		FREE( buffer );
	}

	void	animation_mix::bone_matrix( bone_index_type bone_idx, float time, float4x4 &matrix )const
	{
		xray::animation::bone const	&b = m_skeleton.get_bone( bone_idx );
		const bone_index_type bone_count = m_skeleton.bone_count( );

		xray::animation::bone const** stack = (xray::animation::bone const**) ALLOCA( bone_count * sizeof( xray::animation::bone* ) );
		stack[0] = &b;

		//bone -> root
		u32 stack_p = 0;
		for( ;  stack[stack_p]->parent()!=0 ; ++stack_p  )
		{
			ASSERT( bone_index_type( stack_p + 1 ) < bone_count  );
			stack[stack_p+1] = stack[stack_p]->parent();
		}

		//root -> bone
		float3 parren_scale = float3( 1.f, 1.f, 1.f );
		matrix.identity();
		for( ; stack_p!=u32(-1); --stack_p)
		{
			
			float3 scale ; float4x4 bone_matrix;


			get_bone_matrix(	bone_matrix, stack[stack_p]->index(), 
								time, parren_scale, scale );

			parren_scale = scale;
			matrix =  bone_matrix * matrix ;
		}

		//parren_scale = float3( 1.f, 1.f, 1.f );float3 scale ; 
		// m_bone_data[index[bone_idx]].bone_matrix( time, parren_scale, matrix, scale );
	}

	void	animation_mix::calculate_bones( float time, buffer_vector<float4x4> &matrices )const
	{
		
		ASSERT( !m_blend_vector.empty() );
		
		const bone_index_type	bone_count	= m_skeleton.bone_count( );
		matrices.resize( bone_count );

		bone const		**stack_bone			= (bone const**)	ALLOCA( bone_count * sizeof( bone* ) );
		bone_index_type *stack_current_child_idx= (bone_index_type*)ALLOCA( bone_count * sizeof( bone_index_type ) );
		float3			*stack_scale			= (float3*)			ALLOCA( bone_count * sizeof( float3 ) );

		std::fill( stack_current_child_idx, stack_current_child_idx + bone_count, bone_index_type( 0 )  );

		u32						stack_p		= 0;
		const bone&				root		= m_skeleton.get_bone( 0 );

		stack_bone[ stack_p ] = &root;
	
		float3		parent_scale	= float3( 1, 1, 1 );
		float4x4	parent_matrix	= float4x4().identity();

		for(;;)
		{
			bone const			*current_bone		= stack_bone[ stack_p ];
			bone const			*current_child_begin= current_bone->children_begin();
			bone const			*current_child_end	= current_bone->children_end();
			float4x4			&bone_matrix		= matrices[ current_bone->index() ];

/////////////////////calculate bone matrix///////////////////////////////////////////////////////////
			float3 &scale = stack_scale[ current_bone->index() ];
			get_bone_matrix(  bone_matrix, current_bone->index(),
							  time, parent_scale, scale );
			bone_matrix =  bone_matrix * parent_matrix;
////////////////////////////////////////////////////////////////////////////////////////////////////

			for( stack_current_child_idx[ stack_p ] = 0;  ; ++stack_current_child_idx[ stack_p ] )
			{
				bone const	*current_child = current_child_begin + stack_current_child_idx[ stack_p ];
				if( current_child != current_child_end )
				{
					++stack_p;
					parent_scale			= stack_scale[ current_bone->index() ];
					parent_matrix			= matrices[ current_bone->index() ];
					stack_bone[ stack_p ]	= current_child;
					break;///////->calculate
				}

				if( current_bone->parent() == 0 )
						return;
				--stack_p;
				current_bone		= stack_bone[ stack_p ];
				current_child_begin = current_bone->children_begin();
				current_child_end	= current_bone->children_end();
			}
		 }
	}

	//float	animation_mix::max_time( )const
	//{
	//	float max_time =  math::float_max;
	//	const u32 cnt = m_blend_vector.size();
	//	for( u32 i = 0; i < cnt; ++i )
	//		max_time = math::min( max_time, m_blend_vector[i].max_time() );
	//
	//	return max_time;
	//	
	//}



	void	animation_mix::clear				( )
	{
		m_blend_vector.clear();

#ifdef	DEBUG
		m_dbg_times.clear();
#endif	
	}

	void	animation_mix::insert( const animation_vector_type &animations )
	{
		m_blend_vector.insert( m_blend_vector.end(), animations.begin(), animations.end() );
		R_ASSERT( sum_weights( m_blend_vector ) <= 1.f + math::epsilon_6 );
	}

	static void	render_skeleton(	xray::render::debug::renderer			&renderer,
									const float4x4							&matrix,
									animation_mix							&animation,
									float									time
								)
	{
		skeleton	const&skel = animation.get_skeleton();

		const bone_index_type count = skel.bone_count();

		buffer_vector<float4x4>	matrices ( ALLOCA( count * sizeof(float4x4) ), count );
		
		animation.calculate_bones( time, matrices );

		for( bone_index_type i = 0; i<count; ++i )
		{
			bone	const& b =	skel.get_bone( i );

			if( !b.parent() )
				continue;

			bone	const& p = *b.parent();
			float4x4 mb, mp;
			float3 d = float3(0,0,0);

			mb = matrices[ b.index() ] * matrix;
			mp = matrices[ p.index() ] * matrix;
			
			float3 p0(0,0,0), p1(0,0,0);
			
			p0 = p0*mb;
			p1 = p1*mp;

			
			renderer.draw_line( p0+d, p1+d, math::color_xrgb(255,255,0) );
		}
	}

#if 0//def	DEBUG
 	void	animation_mix::render_animation_time_line( render::debug::renderer	&r, const float3 &p, dbg_times &times, float anim_time )
	{

		float scale_time_render = 0.1f;
		float scale_scale_move = 0.1f;
		const u32 max_size = 1000;
		

		float3 pos = p ;
		times.push_back( std::pair<float3, float>( pos, anim_time ) );

		const u32 num_pos = times.size();
		for( u32 i = 0; i < num_pos-1; ++i )
		{
			const std::pair<float3, float>   &t0 = times[i];
			const std::pair<float3, float>   &t1 = times[i+1];

			const float3 p0 = scale_scale_move * ( float3( t0.first ) + float3( 0, t0.second*scale_time_render, 0 ) );
			const float3 p1 = scale_scale_move * ( float3( t1.first ) + float3( 0, t1.second*scale_time_render, 0 ) );
			r.draw_line( p0, p1, math::color_xrgb(255,0,0)  );

		}

		if( num_pos > max_size )
			times.pop_front();

		//const float max_time = max_time();


		//const float3 &p = matrix.c.xyz();
		
	}
#endif

	void	animation_mix::render( render::debug::renderer	&renderer, const float4x4 &matrix, float time )
	{
		
		
		if(m_blend_vector.empty())
			return;
		render_skeleton( renderer, matrix, *this, time );

#if 0

		const u32 count = math::min( m_dbg_times.size(), m_blend_vector.size() );
		
		float4x4 m = matrix;
		for( u32 i = 0; i < count; ++i )
		{

			m.c.xyz() += float3( 0, 0.5f, 0 );
			float4x4 bone;
			float3 scale(1,1,1);
			animation::get_bone_matrix(  bone, m_blend_vector, 0, time, scale, scale );
			render_animation_time_line( renderer, m.c.xyz() + bone.c.xyz(), m_dbg_times[i],  m_blend_vector[i].animation_time( time ) );

		}

#endif
		
	}

	void animation_mix::reset_start_time( )
	{
		const u32 cnt = m_blend_vector.size();
		for( u32 i = 0; i < cnt; ++i )
			m_blend_vector[i].set_start_time( 0 );

#ifdef	DEBUG
		m_dbg_times.clear();
#endif	

	}

void get_weights( buffer_vector< float > &weights, 
				  const animation_vector_type &blends )
{
	
	const u32 count = blends.size();

	ASSERT( count <= blends.capacity() );


	weights.resize( count );

	for( u32 i = 0; i < count; ++i )
	{
		
		weights[i] = blends[i].factor();
	}

}

void animation_mix::get_weights	( buffer_vector< float > &weights )const
{
	animation::get_weights( weights, m_blend_vector );
}


} // namespace animation
} // namespace xray