////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bone_matrices_computer.h"
#include <xray/animation/skeleton.h>
#include "mixing_animation_state.h"
#include "mixing_n_ary_tree_animation_node.h"

using xray::animation::skeleton_bone;
using xray::animation::skeleton;
using xray::animation::bone_matrices_computer;
using xray::math::float4x4;
using xray::math::float3;
using xray::math::quaternion;
using xray::animation::animation_types_enum;

namespace xray {
namespace animation {

struct bone_transform {
	inline bone_transform	( )
	{
	}

	inline bone_transform	(
			float3 const& translation,
			quaternion const& rotation,
			float3 const& scale,
			bool const visibility = true
		) :
		translation	( translation ),
		rotation	( rotation ),
		scale		( scale ),
		visibility	( visibility )
	{
	}

	inline explicit bone_transform	(
			frame const& frame,
			bool const visibility = true
		) :
		translation	( frame.translation ),
		rotation	( frame.rotation ),
		scale		( frame.scale ),
		visibility	( visibility )
	{
	}

	inline	void make_zero	( )
	{
		translation	= float3( 0.f, 0.f, 0.f ),
		rotation	= quaternion( float4(0.f, 0.f, 1.f, 0.f) ),
		scale		= float3( 0.f, 0.f, 0.f ),
		visibility	= true;
	}

	inline	void apply	( bone_transform const& transform )
	{
		translation	+= transform.translation;
		rotation	= rotation * transform.rotation;
		scale		*= transform.scale;
		visibility	&= transform.visibility;
	}

	float3			translation;
	quaternion		rotation;
	float3			scale;
	bool			visibility;
}; // struct bone_transform

} // namespace animation
} // namespace xray

using xray::animation::bone_transform;

bone_matrices_computer::bone_matrices_computer				(
		skeleton const* skeleton,
		xray::animation::mixing::animation_state* const animations,
		u32 const animations_count
	) :
	m_skeleton					( skeleton ),
	m_animations				( animations ),
	m_animations_count			( animations_count ),
	m_layers_count				( 0 )
{
	u32 max_layer_id			= 0;
	for (mixing::animation_state* i = m_animations, *e = i + m_animations_count; i != e; ++i ) {
		mixing::n_ary_tree_animation_node const& animation	= (*i).event_iterator.animation();
		max_layer_id										= math::max( max_layer_id, animation.additivity_priority() );
		mixing::animation_interval const* const interval	= animation.animation_intervals() + (*i).animation_interval_id;
		(*i).bone_matrices_computer.pinned_animation		= cubic_spline_skeleton_animation_pinned( interval->animation() );
	}

	m_layers_count				= max_layer_id + 1;
}

bone_matrices_computer::~bone_matrices_computer				( )
{
	for (mixing::animation_state* i = m_animations, *e = i + m_animations_count; i != e; ++i )
		(*i).bone_matrices_computer.pinned_animation	= resources::managed_resource_ptr(0);
}

typedef xray::buffer_vector< std::pair< xray::float3, float > >	tranforms_type;

static inline float3 mix_translations	( tranforms_type const& transforms )
{
	float3 result							= float3( 0.f, 0.f, 0.f );
	tranforms_type::const_iterator i		= transforms.begin( );
	tranforms_type::const_iterator const e	= transforms.end( );
	for ( ; i != e; ++i )
		result								+= (*i).first * (*i).second;

	return									result;
}

static inline xray::math::quaternion mix_rotations	( tranforms_type& transforms )
{
	if ( transforms.empty() )
		return								xray::math::quaternion( xray::math::float4( 0.f, 0.f, 0.f, 1.f ) );

	tranforms_type::iterator i				= transforms.begin( ) + 1;
	tranforms_type::iterator const e		= transforms.end( );
	for ( ; i != e; ++i )
		*i									= std::make_pair( to_close_xyz( (*i).first, (i-1)->first ), (*i).second );

	return									xray::math::quaternion( mix_translations( transforms ) );
}

//static inline quaternion mix_rotations	( rotations_type const& rotations )
//{
//	using xray::math::quaternion;
//
//	float angle;
//	quaternion result						= rotations.front( ).first;
//	do {
//		quaternion result_conjugated		= conjugate( result );
//
//		quaternion sum						= quaternion( float3( 0.f, 0.f, 1.f ), 0.f );
//		rotations_type::const_iterator i	= rotations.begin( );
//		rotations_type::const_iterator const e	= rotations.end( );
//		for ( ; i != e; ++i )
//			sum.vector						+= logn(result_conjugated * (*i).first).vector * (*i).second;
//
//		result								= result * exp( sum );
//		result.vector.normalize				( );
//
//		quaternion const& delta				= result_conjugated * result;
//		float3 axis;
//		delta.get_axis_and_angle			( axis, angle );
//	} while ( angle >= xray::math::epsilon_3 );
//
//	return									result;
//}

static inline float3 mix_scales	( tranforms_type const& transforms )
{
	float3 result							= float3( 1.f, 1.f, 1.f );
	tranforms_type::const_iterator i		= transforms.begin( );
	tranforms_type::const_iterator const e	= transforms.end( );
	for ( ; i != e; ++i )
		result								*= pow( (*i).first, (*i).second );

	return									result;
}

inline xray::animation::frame identity_frame( )
{
	xray::animation::frame result;
	result.translation	= float3( 0.f, 0.f, 0.f );
	result.rotation		= float3( 0.f, 0.f, 0.f );
	result.scale		= float3( 1.f, 1.f, 1.f );
	return				result;
}

bone_transform bone_matrices_computer::computed_local_bone_transform(
		skeleton_bone const& bone,
		u32 const animation_layer_id
	) const
{
	tranforms_type translations( ALLOCA( m_animations_count * sizeof(tranforms_type::value_type) ), m_animations_count );
	tranforms_type rotations( ALLOCA( m_animations_count * sizeof(tranforms_type::value_type) ), m_animations_count );
	tranforms_type scales( ALLOCA( m_animations_count * sizeof(tranforms_type::value_type) ), m_animations_count );

	for (mixing::animation_state* i = m_animations, *e = i + m_animations_count; i != e; ++i ) {
		if ( (*i).event_iterator.animation().additivity_priority() != animation_layer_id )
			continue;

		current_frame_position frame_position;
		frame const& transform		= (*i).bone_matrices_computer.pinned_animation->has_bone( bone.id() ) ? (*i).bone_matrices_computer.pinned_animation->bone( bone.id() ).bone_frame( (*i).bone_matrices_computer.pinned_animation->get_frames_per_second()*(*i).animation_time, frame_position ) : identity_frame();
		translations.push_back		( std::make_pair( transform.translation, (*i).weight) );
		rotations.push_back			( std::make_pair( transform.rotation, (*i).weight) );
		scales.push_back			( std::make_pair( transform.scale, (*i).weight) );
	}

	return
		bone_transform(
			mix_translations( translations ),
			mix_rotations( rotations ),
			mix_scales( scales )
		);
}

float4x4 bone_matrices_computer::computed_local_bone_matrix	( skeleton_bone const& bone ) const
{
	typedef buffer_vector< bone_transform >	bone_transforms_type;
	bone_transforms_type bone_transforms( ALLOCA( m_layers_count*sizeof(bone_transform) ), m_layers_count );
	for ( u32 i = 0; i < m_layers_count; ++i )
		bone_transforms.push_back	( computed_local_bone_transform( bone, i ) );

	R_ASSERT						( !bone_transforms.empty() );
	bone_transform result			= bone_transforms.front();
	bone_transforms_type::const_iterator i			= bone_transforms.begin() + 1;
	bone_transforms_type::const_iterator const e	= bone_transforms.end();
	for ( ; i != e; ++i )
		result.apply				( *i );

	return
		math::create_scale( result.scale ) *
		math::create_rotation( result.rotation ) *
		math::create_translation( result.translation );
}

void bone_matrices_computer::compute_skeleton_branch		( skeleton_bone const& bone, float4x4* const result, float4x4 const& parent ) const
{
	*result						= computed_local_bone_matrix( bone ) * parent;

	skeleton_bone const* i		= bone.children_begin( );
	skeleton_bone const* const e = bone.children_end( );
	for ( ; i != e; ++i )
		compute_skeleton_branch	( *i, result + (i - &bone), *result );
}

void bone_matrices_computer::compute_bones_matrices			( float4x4* const begin, float4x4* const end ) const
{
	R_ASSERT_CMP_U				( m_skeleton->get_non_root_bones_count(), <=, end - begin );

	skeleton_bone const* const roots_begin		= &m_skeleton->get_root();
	skeleton_bone const* const roots_end		= roots_begin->children_begin();
	for ( skeleton_bone const* root = roots_begin; root != roots_end; ++root ) {
		skeleton_bone const* children_begin		= (*root).children_begin( );
		skeleton_bone const* const children_end	= (*root).children_end( );
		for (skeleton_bone const* child = children_begin; child != children_end; ++child )
			compute_skeleton_branch				( *child, begin + (child - children_begin), float4x4().identity() );
	}
}

#ifndef MASTER_GOLD
float4x4 bone_matrices_computer::computed_bone_matrix		( skeleton_bone const& bone ) const
{
	float4x4 const& result		= computed_local_bone_matrix( bone );
	return						bone.parent() ? result * computed_bone_matrix( *bone.parent() ) : result;
}
#endif // #ifndef MASTER_GOLD

float4x4 bone_matrices_computer::get_object_transform		( ) const
{
	tranforms_type translations( ALLOCA( m_animations_count * sizeof(tranforms_type::value_type) ), m_animations_count );
	tranforms_type rotations( ALLOCA( m_animations_count * sizeof(tranforms_type::value_type) ), m_animations_count );
	tranforms_type scales( ALLOCA( m_animations_count * sizeof(tranforms_type::value_type) ), m_animations_count );

	for (mixing::animation_state* i = m_animations, *e = i + m_animations_count; i != e; ++i ) {
		if ( (*i).bone_matrices_computer.pinned_animation->animation_type() != animation_type_full )
			continue;

		if ( (*i).are_there_any_weight_transitions ) {
			scales.push_back			( std::make_pair( (*i).bone_matrices_computer.accumulated_object_movement.scale, (*i).weight) );
			rotations.push_back			( std::make_pair( math::create_rotation( (*i).bone_matrices_computer.accumulated_object_movement.rotation ).get_angles_xyz(), (*i).weight) );
			translations.push_back		( std::make_pair( (*i).bone_matrices_computer.accumulated_object_movement.translation, (*i).weight) );
		}
		else {
			current_frame_position frame_position;
			bone_transform const& transform	= bone_transform( (*i).bone_matrices_computer.pinned_animation->bone( u32(0) ).bone_frame( (*i).bone_matrices_computer.pinned_animation->get_frames_per_second()*(*i).animation_time, frame_position ) );
			// if animation has been started not from the beginning
			// then we store inverted transform for object_movement bone at the time animation started
			// and now we use it to compute transform for object_movement bone
			math::quaternion const& conjugate_rotation	= math::conjugate( (*i).bone_matrices_computer.accumulated_object_movement.rotation );
			scales.push_back			( std::make_pair( transform.scale / (*i).bone_matrices_computer.accumulated_object_movement.scale, (*i).weight) );
			rotations.push_back			( std::make_pair( math::create_rotation( conjugate_rotation * transform.rotation ).get_angles_xyz(), (*i).weight) );
			translations.push_back		(
				std::make_pair(
					(
						(*i).bone_matrices_computer.accumulated_object_movement.rotation *
						math::quaternion( float4(transform.translation - (*i).bone_matrices_computer.accumulated_object_movement.translation, 0.f) ) *
						conjugate_rotation
					).vector.xyz(),
					(*i).weight
				)
			);
		}
	}

	return
		math::create_scale( mix_scales( scales ) ) *
		math::create_rotation( mix_rotations( rotations ) ) *
		math::create_translation( mix_translations( translations ) );
}
