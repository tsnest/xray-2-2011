////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

namespace xray {
namespace animation {

	
	

	inline float sum_weights( const animation_vector_type	&blend_vector)
	{
		animation_vector_type::const_iterator	i = blend_vector.begin(),
												e = blend_vector.end();
		float ret = 0;
		for( ; i != e; ++i )
			ret += ( *i ).factor();
		return ret;
	}
	
	inline void check_blends( const animation_vector_type	&blend_vector)
	{
		R_ASSERT_U( math::similar( sum_weights( blend_vector ), 1.f ) );
	} 

	inline	void animation_mix::add( const animation_layer &blend )
	{ 
	//	if( m_blend_vector.empty() )
			//m_mix_time = blend.max_time();
		 
		//m_mix_time = math::min( m_mix_time, blend.max_time() );

		m_blend_vector.push_back( blend ); 

		R_ASSERT(  sum_weights( m_blend_vector ) <= 1.f );

#ifdef	DEBUG
		m_dbg_times.push_back(dbg_times());
#endif
	}

	
	inline void set_root_start( const frame &f, animation_vector_type	&blend_vector )
	{
		const u32 blends_count = blend_vector.size();

		for( u32 i = 0; i < blends_count; ++i )
				blend_vector[i].set_root_start( f );
		
	}
	

	inline void add_root_start( const frame &f, vector< animation_layer >	&blend_vector )
	{
		const u32 blends_count = blend_vector.size();

		for( u32 i = 0; i < blends_count; ++i )
				blend_vector[i].add_root_start( f );
		
	}

	inline void	matrix_to_frame( const float4x4 &matrix, frame &f )
	{

		f.translation	= matrix.c.xyz();
		f.rotation		= matrix.get_angles_xyz( ); //float3(0,0,0);//
		f.scale			= float3(1,1,1);
	}

	inline	void animation_mix::set_root_start( const frame &f )
	{
		//float4x4 root_bone_matrix;

		
		animation::set_root_start( f, m_blend_vector );


	}


	inline	void animation_mix::get_bone_matrix( float4x4 &bone_matrix, bone_index_type bone, float time, const float3 &parent_scale_, float3 &scale_ )const
	{
		R_ASSERT( math::similar( sum_weights( m_blend_vector ), 1.f ) );

		animation::get_bone_matrix(  bone_matrix, m_blend_vector, bone,
							  time, parent_scale_, scale_ );
	}



		rotation_mixing_method mixing_rotation();

	inline void get_bone_matrix( float4x4 &m, const animation_vector_type &blends,  bone_index_type bone, float time, const float3 &parent_scale_, float3 &scale_ )
	{
		
		switch( mixing_rotation( ) )
		{
			
			case rotation_mixing_angles:
				m = linear_angles_mix( blends, bone,time, parent_scale_, scale_ );
			break;

			case rotation_mixing_angles_representation_reduce:
				m = linear_angles_mix_representation_reduce( blends, bone, time, parent_scale_, scale_ );
			break;

			case rotation_mixing_qslim:
				m = qslim_mix( blends, bone, time );
			break;

			case rotation_mixing_sasquatch:
				m = sasquatch_mix( blends, bone, time );
			break;

			case rotation_mixing_multilinear:
				m = multilinear_mix( blends, bone, time );
			break;
				
			default: NODEFAULT();

		}

	}




} // namespace animation
} // namespace xray