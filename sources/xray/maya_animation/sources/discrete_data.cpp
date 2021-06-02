////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/maya_animation/discrete_data.h>
#include "maya_animation_world.h"
#include "animation_data.h"
#include "anim_track_discrete_channels.h"
#include <xray/maya_animation/world.h>
#include <xray/animation/anim_track_common.h>
#include <xray/animation/sources/bi_spline_skeleton_animation.h>
#include <xray/animation/bi_spline_data.h>
#include <xray/shared_string.h>

using namespace xray::animation;

namespace xray {
namespace maya_animation {

class discrete_data_impl:
public  xray::maya_animation::discrete_data
{
public:
	typedef	t_animation_data< anim_track_discrete_channels > discrete_bone_data;

								discrete_data_impl	( );

	virtual	void				save				( xray::configs::lua_config_value &cfg ) const ;
	virtual	void				load				( xray::configs::lua_config_value const& cfg, bool bone_names_only = false ) ;
	virtual	void				check				( xray::configs::lua_config_value &discrete_data ) const;

	virtual void				add_frame			( u32 bone, frame const& frm, frame const& base_frm );
	virtual	void				calculate			( );

	void						fix_angles			(u32 bone );
	virtual void				get_frame			( u32 bone, u32 number, float &time, xray::animation::frame& frame )const;
	virtual u32					bone_count			( )const	;
	virtual u32					frame_count			( )const ;
	virtual void				set_bone_count		( u32 bc );
	virtual void				set_object_mover_id	( u32 id );
	virtual float				precision			( u32 , enum_channel_id channel)const{ return m_precision[channel]; }
	virtual float				time_scale			( )const{ return m_time_scale; }
	virtual	float				*channel			( u32 bone,  enum_channel_id ch );
	virtual u32					channel_size		( u32 bone,  enum_channel_id ch );
	
	virtual void				set_precision		( float const t, float const r, float const s );

	discrete_bone_data			&get_data			( ){ return data; }
	const	discrete_bone_data	&get_data			( ) const{ return data; }

private:
	void						object_mover_calculate( );
	void						set_root_difference	( );

private:
	u32														m_object_mover_id;


private:
	discrete_bone_data										data;
	
private:
	float													m_precision[channel_max];
	float													m_time_scale;

private:
	vector<float>											m_angle_normalize_base;

}; // class discrete_data_impl

discrete_data_impl::discrete_data_impl( )
:m_time_scale		( default_fps ), 
m_object_mover_id	( u32(-1) )
{
	set_precision	( 0.005f, 0.005f, 0.005f );
}

void discrete_data_impl::set_precision( float const t, float const r, float const s )
{
	m_precision[ channel_translation_x ] = t;
	m_precision[ channel_translation_y ] = t;
	m_precision[ channel_translation_z ] = t;

	m_precision[ channel_rotation_x ] = r;
	m_precision[ channel_rotation_y ] = r;
	m_precision[ channel_rotation_z ] = r;

	m_precision[ channel_scale_x ] = s;
	m_precision[ channel_scale_y ] = s;
	m_precision[ channel_scale_z ] = s;
}

u32	discrete_data_impl::bone_count( )const	
{ 
	return data.anim_bone_count();
}

float	*discrete_data_impl::channel( u32 bone,  enum_channel_id ch )
{
	return &( *( data.bone_animation( bone ).m_channels[ch].begin() ) );
}

u32	discrete_data_impl::channel_size( u32 bone,  enum_channel_id ch )
{
	return data.bone_animation( bone ).m_channels[ch].size();
}

void		discrete_data_impl::set_bone_count( u32 bc )
{
	data.set_anim_bone_count( bc );
	m_angle_normalize_base.resize( bc * 3, 0.f );
}

void	discrete_data_impl::set_object_mover_id	( u32 id )
{
	m_object_mover_id = id;
}

void	discrete_data_impl::calculate			( )
{
	set_root_difference	( );
}

void	discrete_data_impl::set_root_difference	( )
{
	data.bone_animation( 0 ).set_difference( );
}

void	discrete_data_impl::object_mover_calculate( )
{
	if( m_object_mover_id == u32(-1) )
		return;

	ASSERT( m_object_mover_id < data.anim_bone_count() );

	anim_track_discrete_channels &object_mover_bone_data	= data.bone_animation( m_object_mover_id );
	anim_track_discrete_channels &object_mover_data			= data.bone_animation( 0 );

	const u32 cnt = frame_count();

	float4x4 start_frame_invece;
	start_frame_invece.try_invert( animation::frame_matrix( object_mover_bone_data.get_frame( 0 ) ) );
	const float4x4 end_frame = ( animation::frame_matrix( object_mover_bone_data.get_frame( cnt ) ) );
	
	const float4x4 non_cyclic = end_frame * start_frame_invece;

	float4x4 non_cyclic_inverce;  non_cyclic_inverce.try_invert( non_cyclic ); 

//	std::fill( m_angle_normalize_base.begin(), m_angle_normalize_base.end(), 0.f );

	for( u32 i = 0; i < cnt; ++i )
	{
		const float4x4 object_mover_bone	=  frame_matrix( object_mover_bone_data.get_frame( i ) ) * non_cyclic_inverce;
		frame  f_object_mover_bone			= matrix_to_frame( object_mover_bone );

		const float4x4 object_mover			=  non_cyclic * frame_matrix( object_mover_data.get_frame( i ) );
		frame  f_object_mover				= matrix_to_frame( object_mover );
		if( i > 0 )
		{
			frame prev_object_mover_bone = object_mover_bone_data.get_frame( i - 1 );
			f_object_mover_bone.rotation = to_close_xyz( f_object_mover_bone.rotation, prev_object_mover_bone.rotation, math::pi_d2 );
			object_mover_bone_data.set_frame( i, f_object_mover_bone );

			frame f_prev_object_mover = object_mover_data.get_frame( i - 1 );
			f_object_mover.rotation = to_close_xyz( f_object_mover.rotation, f_prev_object_mover.rotation, math::pi_d2 );
			object_mover_data.set_frame( i, f_object_mover );

		}

	}
}

void	discrete_data_impl::save( xray::configs::lua_config_value &cfg ) const 
{
	for( u32 ch = channel_translation_x ; ch < channel_max; ++ch )
		cfg["precision"][ch] = m_precision[ch];

	cfg["time_scale"]  = m_time_scale;
	
	xray::configs::lua_config_value tmp_cfg = cfg["bones"];
	data.save( tmp_cfg );
	
}

void	discrete_data_impl::load( xray::configs::lua_config_value const& cfg, bool bone_names_only /*= false*/ ) 
{
	
	if( bone_names_only )
		return;
	for( u32 ch = channel_translation_x ; ch < channel_max; ++ch )
		m_precision[ch] = cfg["precision"][ch];

	m_time_scale = cfg["time_scale"];
	data.load( xray::configs::lua_config_value( cfg["bones"] ) );

}

void	discrete_data_impl::check( xray::configs::lua_config_value &discrete_data ) const
{
	data.check	( discrete_data["bones"]["animation_data"] );
}


void	discrete_data_impl::get_frame( u32 bone, u32 number, float &time, frame& frame )const
{
	for( int ch = channel_translation_x; ch < channel_max ; ++ch )
	{
		enum_channel_id current = enum_channel_id(ch);
		frame.channels[current] = data.bone_animation( bone ).m_channels[current][number];
	}

	time = float(number);
}





bool	do_fix_angle_normalize( float a0,  float &a1, float &saved_angle_base )
{

	a1 += saved_angle_base ;

	float dist = a1 - a0;
	float angular_dist = math::angle_normalize_signed( dist );		//math::angle_normalize_signed( dist );

	if( !math::is_similar( dist, angular_dist, math::epsilon_3 ) ) 
	{
		float new_a1 = a0 + angular_dist;

		saved_angle_base +=  ( new_a1 - a1 );

		a1 = new_a1;
		return true;
	}
	return false;
}



bool	normalize3_angles( float &x, float &y, float &z )// 
{
	
	float ax = math::abs( x );
//	float ay = math::abs( y );
	float az = math::abs( z );
	
	
	ASSERT( ax -  math::pi < math::epsilon_5 );
//	ASSERT( ay -  math::pi < math::epsilon_5 );
	ASSERT( az -  math::pi < math::epsilon_5 );
	
	if( ax < math::pi_d2 || az < math::pi_d2 )// ay < pi_d4
		return false;

	if( x > 0 )
		x-=math::pi;
	else
		x+=math::pi;

	if( z > 0 )
		z-=math::pi;
	else
		z+=math::pi;

	if( y > 0 )
		y-=math::pi;
	else
		y+=math::pi;

	//y = math::pi - y;
	
	//y = -y - math::pi; // <0

	ASSERT( math::abs( x ) -  math::pi_d2 < math::epsilon_5 );
	//ASSERT( math::abs( y ) -  math::pi < math::epsilon_5 );
	ASSERT( math::abs( z ) -  math::pi_d2 < math::epsilon_5 );

	return true;
}

void	fix_angle_normalize3( vector< float > &x, 
							  vector< float > &y, 
							  vector< float > &z
							 )
{
	ASSERT( x.size() == y.size() );
	ASSERT( x.size() == z.size() );

	const u32 size = x.size();
	float iy = - y[0];
	if( size == 1 && normalize3_angles( x[0], iy , z[0] ) )
		y[0] = iy;
	
	
	if( size < 2 )
		return;
	
	float const &ay0 = y[ size - 2 ];
	float 		&ay1 = y[ size - 1 ];
	
	float const &ax0 = x[ size - 2 ];
	float		&ax1 = x[ size - 1 ];

	float const	&az0 = z[ size - 2 ];
	float		&az1 = z[ size - 1 ];

	float rot_x = ax1 - ax0;
	float rot_y =  math::angle_normalize_signed(  -ay1 - ay0 );
	float rot_z = az1 - az0;

	
	if( !normalize3_angles( rot_x, rot_y, rot_z ) ) // rot_y do not use it indeed!
		return;

	ax1 = ax0 + rot_x;
	az1 = az0 + rot_z;
	ay1 = ay0 + rot_y;

}


void	fix_angle_normalize( vector< float > &v, float &saved_angle_base )
{
	const u32 size = v.size();
	if( size == 0 )
		return;

	if( size == 1 )
	{
		float an = math::angle_normalize_signed( v[0] );

		saved_angle_base = an - v[0]; //+=

		v[0] = an;
		
		return;
	}

	ASSERT( size > 1 );

	float &a0 = v[size - 2];
	float &a1 = v[size - 1];

	do_fix_angle_normalize( a0, a1, saved_angle_base );
}

void	discrete_data_impl::fix_angles( u32 bone )
{
	
	for( int ch = channel_rotation_x; ch <= channel_rotation_z ; ++ch )
	{
		enum_channel_id current = enum_channel_id(ch);
		fix_angle_normalize( data.bone_animation( bone ).m_channels[current], m_angle_normalize_base[bone + ch - channel_rotation_x ]  );
	}


	fix_angle_normalize3(
		data.bone_animation( bone ).m_channels[channel_rotation_x],
		data.bone_animation( bone ).m_channels[channel_rotation_y],
		data.bone_animation( bone ).m_channels[channel_rotation_z]
	);

}

inline	void sub2( frame &additive, const  frame &base )
{
	additive.translation	-= base.translation;
	additive.rotation		=
		//(
		//	math::transpose( math::create_rotation(base.rotation) ) *
		//	math::create_rotation(additive.rotation)
		//).get_angles_xyz();
		math::create_rotation(
			math::conjugate( math::quaternion( base.rotation ) ) *
			math::quaternion( additive.rotation )
		).get_angles_xyz();
	additive.scale			/= base.scale;
}

void	discrete_data_impl::add_frame( u32 bone, frame const& frm, frame const& base_frm ) 
{

	for( int ch = channel_translation_x; ch < channel_max ; ++ch )
	{
		enum_channel_id current = enum_channel_id(ch);
		data.bone_animation( bone ).m_channels[current].push_back( frm.channels[current] );
	}
	
	//fix_angles( bone );
	
	frame cur_frame;
	float time = 0; 
	get_frame( bone, data.bone_animation( bone ).frames_count() - 1, time, cur_frame );

	frame  bf = base_frm;
	bf.rotation = math::to_close_xyz( bf.rotation, cur_frame.rotation ); //, math::pi_d2

	sub2( cur_frame, bf );

	for( int ch = channel_translation_x; ch < channel_max ; ++ch )
	{
		enum_channel_id current = enum_channel_id(ch);
		 
		data.bone_animation( bone ).m_channels[current].back() = cur_frame.channels[current] ;
	}

	fix_angles( bone );
}

u32	discrete_data_impl::frame_count( )const 
{
	u32 frame_cnt = data.bone_animation( 0 ).m_channels[channel_translation_x].size();
	u32 bone_cnt = bone_count();
	for(u32 i = 0; i < bone_cnt; ++i )
		for(u32 j = channel_translation_x; j < channel_max; ++j )
		{
			R_ASSERT_CMP( data.bone_animation( i ).m_channels[j].size(), ==, frame_cnt );
		}
	return frame_cnt;
}

discrete_data*	maya_animation_world::create_discrete_data( )const
{
	return NEW(discrete_data_impl)();
}

void	maya_animation_world::destroy( discrete_data* &p )const
{
	DELETE( p );
}

void build_channel( bi_spline_data< float > &channel, float epsilon, const float *p, u32 size,  xray::maya_animation::raw_bi_spline_1d *raw_bi_spline )
{
	raw_bi_spline->build_approximation( p, size, epsilon );
	raw_bi_spline->get_data( channel );
}

void build_channel( u32 bone, u32 ch, float epsilon,
					discrete_data_impl::discrete_bone_data const& impl_data, 
					bi_spline_skeleton_animation &animation_data,
					xray::maya_animation::raw_bi_spline_1d *raw_bi_spline
					)
{
	xray::animation::bi_spline_data< float > &channel =
			animation_data.bone( bone).channel( (enum_channel_id)(ch) );
	const float * p = &(*impl_data.bone_animation( bone).m_channels[ch].begin());
	const u32 size = impl_data.bone_animation( bone).m_channels[ch].size();
	build_channel( channel, epsilon, p, size, raw_bi_spline );
}

static void set_fp_state( u32& previous_denormals, u32& previous_control_word )
{
#if XRAY_PLATFORM_WINDOWS
	previous_denormals					= _mm_getcsr();

	u32 const _MM_DENORMALS_ZERO_MASK	= 0x0040;
	u32 const _MM_DENORMALS_ZERO_ON		= 0x0040;
	_MM_SET_FLUSH_ZERO_MODE				(_MM_FLUSH_ZERO_ON);
	_mm_setcsr							( (previous_denormals & ~_MM_DENORMALS_ZERO_MASK) | _MM_DENORMALS_ZERO_ON );
#else
	XRAY_UNREFERENCED_PARAMETER			( previous_denormals );
#endif // #if XRAY_PLATFORM_WINDOWS

#ifdef _MSC_VER
	u32	control_word;
	errno_t error;

	error								= _controlfp_s( &control_word, 0, 0 );
	R_ASSERT							( !error );
	previous_control_word				= control_word;

	error								= _controlfp_s( &control_word, _PC_24, MCW_PC );
	R_ASSERT							( !error );

	error								= _controlfp_s( &control_word, _RC_CHOP, _MCW_RC );
	R_ASSERT							( !error );

	error								= _controlfp_s( &control_word, _DN_FLUSH, _MCW_DN );
	R_ASSERT							( !error );

	error								= _controlfp_s( &control_word, _IC_PROJECTIVE, _MCW_IC );
	R_ASSERT							( !error );
#else // #ifdef _MSC_VER
	XRAY_UNREFERENCED_PARAMETER			( previous_control_word );
#endif // #ifdef _MSC_VER
}

static void restore_fp_state			( u32 denormals, u32 control_word )
{
	XRAY_UNREFERENCED_PARAMETER			( denormals );

#ifdef _MSC_VER
	errno_t error;

	error								= _controlfp_s( &control_word, control_word, _MCW_PC );
	R_ASSERT							( !error );

	error								= _controlfp_s( &control_word, control_word, _MCW_RC );
	R_ASSERT							( !error );

	error								= _controlfp_s( &control_word, control_word, _MCW_IC );
	R_ASSERT							( !error );

	error								= _controlfp_s( &control_word, control_word, _MCW_EM );
	R_ASSERT							( !error );

	error								= _controlfp_s( &control_word, control_word, _MCW_DN );
	R_ASSERT							( !error );
#else // #ifdef _MSC_VER
	XRAY_UNREFERENCED_PARAMETER			( control_word );
#endif // #ifdef _MSC_VER

#if XRAY_PLATFORM_WINDOWS
	_mm_setcsr							( denormals );
#endif // #if XRAY_PLATFORM_WINDOWS
}

void	maya_animation_world::build_animation_data( const discrete_data &data, bi_spline_skeleton_animation &animation_data   )const
{
	u32 denormals, control_word;
	set_fp_state						( denormals, control_word );

	discrete_data_impl::discrete_bone_data const& impl_data 
			= static_cast_checked<const discrete_data_impl&> ( data ).get_data();
	
	xray::maya_animation::raw_bi_spline_1d *raw_bi_spline = create_1d_bi_spline	( );
	const u32 bone_count = data.bone_count();
	animation_data.set_num_bones( bone_count );

	for( u32 bone = 0; bone < bone_count; ++bone )
		for( int ch = channel_translation_x; ch < channel_max; ++ch )
			build_channel( bone, ch, data.precision( bone, enum_channel_id( ch ) ), impl_data, animation_data, raw_bi_spline );
		
	destroy( raw_bi_spline );

	restore_fp_state					( denormals, control_word );
}

};	// namespace maya_animation
};	// namespace xray 