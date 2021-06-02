////////////////////////////////////////////////////////////////////////////
//	Created		: 19.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BONE_ANIMATION_MIXING_H_INCLUDED
#define BONE_ANIMATION_MIXING_H_INCLUDED

namespace xray {
namespace animation {

	inline	void mul( frame &f, float factor )
	{
			f.translation	*= factor;
			f.rotation		*= factor;
			f.scale			*= factor;
	}
	
	inline	void add( frame &f, const  frame &add )
	{
			f.translation	+= add.translation;
			f.rotation		+= add.rotation;
			f.scale			*= add.scale;
	}
	inline	void sub( frame &f, const  frame &sub )
	{
			f.translation	-= sub.translation;
			f.rotation		-= sub.rotation;
			f.scale			-= sub.scale;
	}
	inline void mad( frame &f, float factor, const  frame &add )
	{
			f.translation	+= float3( float( factor ) * float3 ( add.translation ) );
			f.rotation		+= float3( float( factor ) * float3 ( add.rotation ) );
			f.scale			/= float( factor ) * float3 ( add.scale );
	}



		inline frame mix_pair( const frame &f0, float factor0, const frame &f1, float factor1 )
	{
		frame ff0 = f0, ff1 = f1;
		to_close_xyz( ff1.rotation, f0.rotation );
		mul( ff0, factor0 );
		mul( ff1, factor1 );
		add( ff0, ff1 );
		return ff0;
	}

	inline frame mix_pair_s( const frame &f0, float factor0, const frame &f1, float factor1 )
	{
		frame ff0 = f0, ff1 = f1,  res = zero;
		
		to_close_xyz( ff0.rotation, f1.rotation );

		mul( ff0, factor0 );

		mul( ff1, factor1 );
		add( ff0, ff1 );

		res = ff0;

		ff1 = f1;
		ff0 = f0;
		to_close_xyz( ff1.rotation, f0.rotation );

		mul( ff0, factor0 );

		mul( ff1, factor1 );
		add( ff0, ff1 );

	////////////////////////////////////////
		mul(ff0, 0.5f );
		mul(res, 0.5f );

		add( res, ff0 );
	////////////////////////////////////////
		ff1 = f1;
		ff0 = f0;
		to_close_xyz( ff0.rotation, res.rotation );
		to_close_xyz( ff1.rotation, res.rotation );

		mul( ff0, factor0 );
		mul( ff1, factor1 );
		add( ff0, ff1 );
		res = ff0;

		return res;
	}


	inline	void	mixed_frame_representation_reduce( frame &f,  bone_index_type bone, float time, const animation_vector_type &blends )
	{
		
		const u32 number = blends.size();
		if( number == 0 )
			return;

		u32 start = 0;
		float factor = 0;
		for( ; start < number; ++start )
		{
			
			factor = blends[ start ].factor( );
			if( factor == 0.f )//math::is_zero()
				continue;
			blends[ start ].get_frame( bone, time, f );
			break;
		}

		ASSERT( factor != 0 );

		for( u32 i = start + 1; i < number; ++i )
		{
			frame bone_f;
			blends[i].get_frame( bone, time, bone_f );
			
			float frame_factor = blends[i].factor( );
			float new_factor = factor + frame_factor;

			f = mix_pair( bone_f, frame_factor/new_factor,  f,  factor/new_factor );

			factor = new_factor;
		}
		
		
	}

	inline	void	mixed_frame( frame &f,  bone_index_type bone, float time, const animation_vector_type &blends )
	{
		
		const u32 number = blends.size();
		if( number == 0 )
			return;
		f = zero;
		for( u32 i = 0; i < number; ++i )
		{
			frame bone_f;
			blends[i].get_frame( bone, time, bone_f );
			mul( bone_f, blends[i].factor( ) );
			add( f, bone_f );
		}

	}

	inline float4x4 linear_angles_mix( const animation_vector_type &blends,  bone_index_type bone, float time, const float3 &parent_scale_, float3 &scale_ )
	{
		float4x4 result;
		frame f;
		mixed_frame(  f, bone, time, blends );
		frame_matrix( f, result, parent_scale_, scale_ );
		return result;
	}
	
	inline float4x4 linear_angles_mix_representation_reduce( const animation_vector_type &blends,  bone_index_type bone, float time, const float3 &parent_scale_, float3 &scale_ )
	{
		float4x4 result;
		frame f;
		mixed_frame_representation_reduce(  f, bone, time, blends );
		
		frame_matrix( f, result, parent_scale_, scale_ );
		return result;
	}



} // namespace animation
} // namespace xray

#endif // #ifndef BONE_ANIMATION_MIXING_H_INCLUDED