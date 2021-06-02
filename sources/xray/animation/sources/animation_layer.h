////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_LAYER_H_INCLUDED
#define ANIMATION_LAYER_H_INCLUDED

#include <xray/animation/anim_track_common.h>
#include <xray/animation/type_definitions.h>

namespace xray {
namespace animation {

enum rotation_mixing_method
{
	rotation_mixing_angles = 0,
	rotation_mixing_angles_representation_reduce,
	rotation_mixing_qslim,
	rotation_mixing_sasquatch,
	rotation_mixing_multilinear,
	rotation_mixing_max,

};


	
class	skeleton_animation;
struct  frame;

namespace mixing {
	struct animation_state;
} // namespace mixing

class animation_layer{

public:
	inline						animation_layer			( const skeleton_animation* b, float factor, float start_time, float blending_time );
	inline						animation_layer			( const mixing::animation_state & data );

	inline	float				factor					( )const	{ return m_factor; }
	inline	float				mix_factor				( )const	{ return m_mix_factor; }

	inline	void				set_factor				( float factor );
	inline	void				set_time_scale			( float scale );
	inline	void				set_start_time			( float time );

	inline	void				get_frame				( bone_index_type bone, float time, frame &f )const;

	inline	void				set_root_start			( const frame &f );
	inline	void				add_root_start			( const frame &f );
	inline	void				get_bone_displacement	( frame &disp, u32 bone, const float2 &interval  )const;

	inline const frame&			root_start				()const	{ return m_root_start; }								
	inline	void				set_moving_root			( u32 root );
	inline	u32					moving_root				( )const { return m_moving_root; }

	inline	float				transition_factor		( float time )const;

private:
	inline	float				max_time							( )const;
	inline	float				min_time							( )const;
	inline	float				time_length							( )const { return max_time() - min_time(); }
	inline	float				animation_time						( float time )const;
	inline	void				get_bone_displacement_non_cyclic	( frame &disp, u32 bone, const float2 &interval )const;
//	inline	float				animation_time_cyclic				( float time )const;


	inline	float				time_left				( float time )const;
	inline	bool				in_time_interval		( float time )const;
	
public:


	const skeleton_animation*	animation				( )const { return m_blend; }


private:
	inline	float				cycle_animation_frame	( float time, frame &new_start_frame )const;
	inline	u32					cycle_animation_time	( float time, float &new_start_time )const;
	inline	void				get_frame_non_cyclic	( bone_index_type bone, float anim_time, const frame &start_frame, frame &f )const;

private:
	const skeleton_animation*			m_blend;

	float								m_factor;
	float								m_mix_factor;

	float								m_time_scale;
	float								m_start_time;
	float								m_blending_time;

private:
	u32									m_moving_root;
//	u32									m_cycles;
	frame								m_root_start;

}; // class animation_layer


typedef animation_layer mixing_animation_type;

//typedef		vector< animation_layer >		mixing_animation_vector_type;
typedef		buffer_vector< mixing_animation_type >		animation_vector_type;

}  // namespace animation
}  // namespace xray

#include "animation_layer_inline.h"


#endif // #ifndef ANIMATION_LAYER_H_INCLUDED