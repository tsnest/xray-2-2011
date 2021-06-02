////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ANIM_TRACK_COMMON_H_INCLUDED
#define ANIM_TRACK_COMMON_H_INCLUDED

namespace xray {
namespace animation {

enum enum_channel_id
{
	channel_translation_x = 0,
	channel_translation_y,
	channel_translation_z,
	channel_rotation_x,
	channel_rotation_y,
	channel_rotation_z,
	channel_scale_x,
	channel_scale_y,
	channel_scale_z,
	channel_max,
	channel_unknown
};

struct	frame
{
#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #ifdef _MSC_VER
	union{
		struct{
			xray::math::float3_pod translation;
			xray::math::float3_pod rotation;
			xray::math::float3_pod scale;
		};
		float channels[channel_max];
	};
#ifdef _MSC_VER
#	pragma warning(pop)
#endif // #ifdef _MSC_VER
}; // struct frame

//\li matrix = [S] * [RO] * [R] * [JO] * [IS] * [T]



inline void frame_matrix( const frame &f, float4x4 &matrix_, const float3 &parent_scale_, float3 &scale_ )
{
	
	XRAY_UNREFERENCED_PARAMETER( parent_scale_);

	scale_ = f.scale;

	//float4x4 rotation_		=			create_rotation( f.rotation );

	//matrix =	scale*rotation*translation*parent_scale*translation ;

	//matrix_ = rotation_;
	//matrix_.c.xyz() = f.translation;

	//matrix_			= create_scale(f.scale) * create_rotation( f.rotation );
	//matrix_.c.xyz()	= f.translation;
	matrix_				= create_scale(f.scale) * create_rotation( f.rotation ) * create_translation( f.translation );
}

inline void frame_matrix( const frame &f, float4x4 &matrix )
{
	float3 scale;
	float3 parent_scale( 1, 1, 1 );
	animation::frame_matrix( f, matrix, parent_scale, scale );
}

inline float4x4 frame_matrix( const frame &f )
{
	float4x4 ret;
	frame_matrix( f, ret );
	return ret;
}

inline  frame matrix_to_frame( const float4x4 &m )
{
	frame f;
	f.rotation = m.get_angles_xyz( );
	f.translation = m.c.xyz();
	f.scale = m.get_scale();
	return f;
}

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
		f.scale			/= sub.scale;
}
inline void mad( frame &f, float factor, const  frame &add )
{
		f.translation	+= float3( float( factor ) * float3 ( add.translation ) );
		f.rotation		+= float3( float( factor ) * float3 ( add.rotation ) );
		f.scale			/= float( factor ) * float3 ( add.scale );
}


inline frame inverce( const frame &f )
{
	frame ret;
	ret.rotation = -f.rotation;
	ret.translation = -f.translation;
	ASSERT( f.scale.x!=0.f );
	ASSERT( f.scale.y!=0.f );
	ASSERT( f.scale.z!=0.f );
	ret.scale = float3( 1.f / f.scale.x, 1.f / f.scale.y , 1.f / f.scale.z );
	return ret;
}




static	const frame zero = { float3().set(0,0,0),float3().set(0,0,0),float3().set(1,1,1) };
static	const float	default_fps	= 30.f;

inline u32 convert_frames_to_ms( float frames )
{
	return math::floor( frames / default_fps  * 1000 ) ;
}


} // namespace  animation
} // namespace  xray

#endif // #ifndef ANIM_TRACK_COMMON_H_INCLUDED