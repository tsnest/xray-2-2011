////////////////////////////////////////////////////////////////////////////
//	Created		: 21.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/mixing_n_ary_tree.h>
#include "mixing_n_ary_tree_destroyer.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include <xray/animation/base_interpolator.h>
#include "mixing_n_ary_tree_dumper.h"
#include "bone_matrices_computer.h"
#include "mixing_n_ary_tree_event_iterator.h"
#include "mixing_n_ary_tree_animation_time_calculator.h"
#include "mixing_n_ary_tree_weight_calculator.h"
#include "mixing_n_ary_tree_time_scale_calculator.h"
#include "mixing_n_ary_tree_time_scale_start_time_modifier.h"
#include <xray/animation/animation_callback.h>
#include "i_editor_mixer_inline.h"
#include "mixing_animation_state.h"
#include <xray/animation/animation_player.h>
#include <xray/animation/skeleton.h>

using xray::animation::mixing::n_ary_tree;
using xray::animation::skeleton;
using xray::math::float4x4;
using xray::animation::mixing::animation_state;
using xray::animation::base_interpolator;
using xray::animation::mixing::n_ary_tree_intrusive_base;
using xray::animation::skeleton_animation_ptr;

#ifndef MASTER_GOLD
void n_ary_tree::fill_animation_states	( xray::vectora<xray::animation::editor_animation_state>& result )
{
	if ( !m_root ) {
		result.clear			( );
		return;
	}

	u32 const animations_count	= m_animations_count;
	result.reserve				( animations_count );

	n_ary_tree_animation_node* i = m_root;
	mixing::animation_state const* data_begin = m_animation_states;
	mixing::animation_state const* data_end	= data_begin + animations_count;
	for(; data_begin!=data_end; ++data_begin, i=i->m_next_animation)
	{
		xray::animation::editor_animation_state new_state;
		R_ASSERT(i->user_data!=xray::memory::uninitialized_value<u32>());
		new_state.user_data = i->user_data;
		new_state.identifier = data_begin->event_iterator.animation().identifier();
		new_state.weight = data_begin->weight;
		new_state.length = data_begin->event_iterator.animation().animation_intervals()[data_begin->animation_interval_id].length()*1000.0f;
		n_ary_tree_animation_node* const sync_anim = i->driving_animation();
		if(!sync_anim) 
		{
			new_state.is_leading = true;
			n_ary_tree_time_scale_calculator time_scale_calculator(
				m_tree_actual_time_in_ms,
				data_begin->animation_interval_time,
				m_tree_actual_time_in_ms,
				n_ary_tree_time_scale_calculator::forbid_transitions_destroying
			);
			n_ary_tree_base_node** const operands = i->operands(sizeof(n_ary_tree_animation_node));
			(*operands)->accept(time_scale_calculator);
			new_state.time_scale = time_scale_calculator.time_scale();
			new_state.offset = 0.f;//!(i->is_on_zero_cycle()) ? 0.0f : i->animation_time_offset()*1000.0f;
		}
		else 
		{
			new_state.is_leading = false;
			n_ary_tree_time_scale_calculator time_scale_calculator(
				m_tree_actual_time_in_ms, 
				data_begin->animation_interval_time, 
				m_tree_actual_time_in_ms,
				n_ary_tree_time_scale_calculator::forbid_transitions_destroying
			);
			n_ary_tree_base_node** const operands = sync_anim->operands(sizeof(n_ary_tree_animation_node));
			(*operands)->accept		( time_scale_calculator );
			new_state.time_scale	= (i->animation_intervals()[i->animation_state().animation_interval_id].length() / sync_anim->animation_intervals()[sync_anim->animation_state().animation_interval_id].length()) * time_scale_calculator.time_scale();
			new_state.offset		= 0.f;//(!i->driving_animation()->is_on_zero_cycle()) ? 0.0f : i->animation_time_offset()*1000.0f;
		}

		new_state.position = data_begin->event_iterator.animation().animation_intervals()[data_begin->animation_interval_id].start_time()*1000.0f - new_state.offset;
		new_state.time_scaled_length = ((new_state.time_scale>0.0f) ? 
										new_state.length - data_begin->animation_interval_time*1000.0f :
										-data_begin->animation_interval_time*1000.0f) 
										/ new_state.time_scale;

		if(new_state.time_scaled_length<0.0f) // REMOVE THIS AFTER FIXING INTERVALS VIEWING!
			new_state.time_scaled_length = 0.0f;

		result.push_back(new_state);
	}
}
#endif // #ifndef MASTER_GOLD

n_ary_tree::n_ary_tree				(
		n_ary_tree_animation_node* const root,
		base_interpolator const** const interpolators,
		animation_state* const animation_states,
		animation_state** const animation_events,
		n_ary_tree_intrusive_base* const reference_counter,
		u32 const animations_count,
		u32 const interpolators_count,
		u32 const current_time_in_ms,
		float4x4 const& object_transform
	) :
	m_root							( root ),
	m_interpolators					( interpolators ),
	m_animation_states				( animation_states ),
	m_animation_events				( animation_events ),
	m_reference_counter				( reference_counter ),
	m_animations_count				( animations_count ),
	m_interpolators_count			( interpolators_count ),
	m_tree_actual_time_in_ms		( current_time_in_ms ),
	m_object_transform				( object_transform )
{
	initialize					( );
}

n_ary_tree::n_ary_tree				(
		n_ary_tree_animation_node* const root,
		base_interpolator const** const interpolators,
		animation_state* const animation_states,
		animation_state** const animation_events,
		n_ary_tree_intrusive_base* const reference_counter,
		u32 const animations_count,
		u32 const interpolators_count,
		u32 const current_time_in_ms,
		n_ary_tree const& previous_tree
	) :
	m_root							( root ),
	m_interpolators					( interpolators ),
	m_animation_states				( animation_states ),
	m_animation_events				( animation_events ),
	m_reference_counter				( reference_counter ),
	m_animations_count				( animations_count ),
	m_interpolators_count			( interpolators_count ),
	m_tree_actual_time_in_ms		( current_time_in_ms ),
	m_object_transform				( previous_tree.m_object_transform )
{
	initialize					( );
}

struct event_iterator_predicate {
	inline	bool	operator ( )				( animation_state const* const left, animation_state const* const right ) const
	{
		return		left->event_iterator.is_less( right->event_iterator );
	}
}; // struct event_iterator_predicate

void n_ary_tree::initialize			( )
{
	std::sort						( m_animation_events, m_animation_events + m_animations_count, event_iterator_predicate() );
}

n_ary_tree::~n_ary_tree				( )
{
	destroy						( );
}

void n_ary_tree::destroy			( )
{
	if ( !m_root )
		return;

	if ( m_reference_counter->reference_count() > 1 ) {
		m_reference_counter		= 0;
		return;
	}

	ASSERT						( static_cast<pcvoid>(m_reference_counter.c_ptr()) <= m_interpolators );
	ASSERT						( static_cast<pcvoid>(m_interpolators) <= m_root );

	n_ary_tree_destroyer		tree_destroyer;
	for (n_ary_tree_animation_node* i = m_root; i; i=i->m_next_animation )
		i->accept				( tree_destroyer );

	for (base_interpolator const* const* i = m_interpolators, * const*	const e = i + m_interpolators_count; i != e; ++i )
		(*i)->~base_interpolator( );

	animation_state* i			= m_animation_states;
	animation_state* const e	= m_animation_states + m_animations_count;
	for ( ; i != e; ++i )
		(*i).~animation_state	( );
}

#ifndef MASTER_GOLD
void n_ary_tree::dump_animation_states	( u32 const current_time_in_ms ) const
{
	LOG_INFO						( "-------time: %5d------------", current_time_in_ms );

	n_ary_tree_animation_node* j	= m_root;
	animation_state const* i		= m_animation_states;
	animation_state const* const e	= i + m_animations_count;
	for ( ; i != e; ++i, j = j->m_next_animation ) {
		cubic_spline_skeleton_animation_pinned	pinnded_animation ( (*i).event_iterator.animation().animation_intervals()[(*i).animation_interval_id].animation() );
		LOG_DEBUG					(
			"%s : weight=%.3f, time=%.3f, animation_length=%.3f, interval_id=%d, interval_time=%.3f, interval_length=%.3f",
			(*i).event_iterator.animation().identifier(),
			(*i).weight,
			(*i).animation_time,
			pinnded_animation->length_in_frames()/default_fps,
			(*i).animation_interval_id,
			(*i).animation_interval_time,
			j->animation_intervals()[ (*i).animation_interval_id ].length()
		);
	}
}

void n_ary_tree::dump_tree				( u32 const current_time_in_ms ) const
{
	LOG_DEBUG						( "------------------------------" );

	mixing::n_ary_tree_dumper dumper( current_time_in_ms );
	n_ary_tree_animation_node* i = m_root;
	for ( ; i; i = i->m_next_animation )
		(*i).accept					( dumper );
}
#endif

float4x4 n_ary_tree::get_object_transform	( ) const
{
	return							m_animation_states ? bone_matrices_computer( 0, m_animation_states, m_animations_count ).get_object_transform( ) * m_object_transform : m_object_transform;
}

void n_ary_tree::set_object_transform		( n_ary_tree_animation_node& animation_node )
{
	animation_state& animation_state = animation_node.animation_state();
	animation_interval const* const animation_interval	= animation_node.animation_intervals() + animation_node.animation_state().animation_interval_id;
	current_frame_position frame_position;
	frame frame_transform;
	{
		cubic_spline_skeleton_animation_pinned pinned_animation	= cubic_spline_skeleton_animation_pinned( animation_interval->animation() );
		frame_transform						=
			pinned_animation->bone( u32(0) ).bone_frame (
				animation_state.animation_time * pinned_animation->get_frames_per_second( ),
				frame_position
			);
	}

	bone_matrices_computer_data& bone_matrices_computer	= animation_state.bone_matrices_computer;

	if ( animation_state.are_there_any_weight_transitions ) {
		bone_matrices_computer.previous_object_movement.translation		= frame_transform.translation;
		bone_matrices_computer.previous_object_movement.rotation		= math::quaternion( frame_transform.rotation );
		bone_matrices_computer.previous_object_movement.scale			= frame_transform.scale;

		bone_matrices_computer.accumulated_object_movement.translation	= float3( 0.f, 0.f, 0.f );
		bone_matrices_computer.accumulated_object_movement.rotation		= math::quaternion( float3(0.f, 0.f, 0.f) );
		bone_matrices_computer.accumulated_object_movement.scale		= float3( 1.f, 1.f, 1.f );
	}
	else {
		bone_matrices_computer.accumulated_object_movement.translation	= frame_transform.translation;
		bone_matrices_computer.accumulated_object_movement.rotation		= math::quaternion( frame_transform.rotation );
		bone_matrices_computer.accumulated_object_movement.scale		= frame_transform.scale;
	}
}

void n_ary_tree::set_object_transform		( float4x4 const& object_transform )
{
	for ( n_ary_tree_animation_node* current_animation = m_root; current_animation; current_animation = current_animation->m_next_animation )
		set_object_transform			( *current_animation );

	m_object_transform					= object_transform;
}

void n_ary_tree::compute_bones_matrices		( skeleton const& skeleton, float4x4* const begin, float4x4* const end ) const
{
	bone_matrices_computer( &skeleton, m_animation_states, m_animations_count ).compute_bones_matrices( begin, end );
}

void n_ary_tree::accumulate_object_movement					(
		n_ary_tree_animation_node& animation_node,
		float const animation_interval_time,
		u32 const time_in_ms
	)
{
	animation_state& animation_state = animation_node.animation_state();
	n_ary_tree_weight_calculator weight_calculator( time_in_ms );
	animation_node.accept					( weight_calculator );
	float const weight						= weight_calculator.weight( );
	animation_state.weight					= weight;
	animation_state.animation_interval_time	= animation_interval_time;
	update_animation_time					( animation_state );
	
	animation_interval const* animation_interval	= animation_node.animation_intervals() + animation_state.animation_interval_id;
	cubic_spline_skeleton_animation_pinned pinned_animation	= cubic_spline_skeleton_animation_pinned( animation_interval->animation() );

	current_frame_position frame_position;
	frame const& frame_transform			=
		pinned_animation->bone( u32(0) ).bone_frame (
			animation_state.animation_time * pinned_animation->get_frames_per_second( ),
			frame_position
		);

	math::quaternion const& frame_transform_rotation	= math::quaternion(frame_transform.rotation);

	object_movement& previous_object_movement		= animation_state.bone_matrices_computer.previous_object_movement;
	object_movement& accumulated_object_movement	= animation_state.bone_matrices_computer.accumulated_object_movement;

	accumulated_object_movement.translation	+= ( frame_transform.translation - previous_object_movement.translation );
	accumulated_object_movement.rotation	= frame_transform_rotation * math::conjugate(previous_object_movement.rotation) * accumulated_object_movement.rotation;
	accumulated_object_movement.scale		*= frame_transform.scale / previous_object_movement.scale;

	previous_object_movement.translation	= frame_transform.translation;
	previous_object_movement.rotation		= frame_transform_rotation;
	previous_object_movement.scale			= frame_transform.scale;
}

void n_ary_tree::update_synchronization_group_using_integration	(
		n_ary_tree_animation_node& animation_node,
		u32 const start_time_in_ms,
		u32 const target_time_in_ms
	)
{
	animation_state& animation_state		= animation_node.animation_state();
	n_ary_tree_base_node* time_scale_node	= *animation_node.operands( sizeof(n_ary_tree_animation_node) );

	float accumulated_animation_time		= animation_state.animation_interval_time;
	float const integration_interval_length	= integration_interval_length_in_ms/1000.f;

	animation_interval const* const animation_interval	= animation_node.animation_intervals() + animation_state.animation_interval_id;
	float const animation_interval_length	= animation_interval->length();
	u32 const full_intervals_count			= (target_time_in_ms - start_time_in_ms)/integration_interval_length_in_ms;
	for ( u32 i = 0; i <= full_intervals_count; ++i ) {
		n_ary_tree_time_scale_calculator time_scale_calculator(
			i < full_intervals_count ? (start_time_in_ms + (2*i+1)*(integration_interval_length_in_ms/2)) : target_time_in_ms,
			accumulated_animation_time,
			i < full_intervals_count ? (start_time_in_ms + (i ? 2*i-1 : 0)*(integration_interval_length_in_ms/2)) : (start_time_in_ms + full_intervals_count*integration_interval_length_in_ms),
			n_ary_tree_time_scale_calculator::permit_transitions_destroying,
			&animation_node
		);
		time_scale_node->accept				( time_scale_calculator );
		if ( time_scale_node != time_scale_calculator.result() ) {
			R_ASSERT						( time_scale_calculator.result() );
			time_scale_node					= time_scale_calculator.result();
			*animation_node.operands( sizeof(n_ary_tree_animation_node) )	= time_scale_node;
		}

		if ( i < full_intervals_count ) {
			accumulated_animation_time		+= time_scale_calculator.time_scale() * integration_interval_length;
			accumulated_animation_time		= math::min( math::max( accumulated_animation_time, 0.f ), animation_interval_length);
			R_ASSERT_CMP					( accumulated_animation_time, >=, 0.f );
			R_ASSERT_CMP					( accumulated_animation_time, <=, animation_interval_length );
		}
		else {
			float const tail				= float((target_time_in_ms - start_time_in_ms)%integration_interval_length_in_ms)/float(integration_interval_length_in_ms);
			accumulated_animation_time		+= time_scale_calculator.time_scale() * integration_interval_length * tail;
			accumulated_animation_time		= math::min( math::max( accumulated_animation_time, 0.f ), animation_interval_length);
			R_ASSERT_CMP					( accumulated_animation_time, >=, 0.f );
			R_ASSERT_CMP					( accumulated_animation_time, <=, animation_interval_length );
		}

		u32 const update_time				= i < full_intervals_count ? (start_time_in_ms + (i+1)*integration_interval_length_in_ms) : target_time_in_ms;
		if ( animation_state.are_there_any_weight_transitions )
			accumulate_object_movement		(
				animation_node,
				accumulated_animation_time,
				update_time
			);

		u32 const synchronization_group_id	= animation_node.synchronization_group_id();
		if ( synchronization_group_id == -1 )
			continue;

		for (
				n_ary_tree_animation_node* current_driven_animation = animation_node.m_next_animation ;
				current_driven_animation && (current_driven_animation->synchronization_group_id() == synchronization_group_id) ;
				current_driven_animation = current_driven_animation->m_next_animation
			)
		{
			if ( !current_driven_animation->animation_state().are_there_any_weight_transitions )
				continue;

			mixing::animation_interval const* animation_interval	= current_driven_animation->animation_intervals() + current_driven_animation->animation_state().animation_interval_id;
			float const drive_animation_time_scale	= animation_interval->length() / animation_interval_length;;
			accumulate_object_movement		(
				*current_driven_animation,
				accumulated_animation_time * drive_animation_time_scale,
				update_time
			);
		}
	}
}

void n_ary_tree::update_animation_time	( animation_state& animation_state )
{
	animation_interval const& animation_interval	= animation_state.event_iterator.animation().animation_intervals()[ animation_state.animation_interval_id ];
	float const animation_time						= animation_interval.start_time() + animation_state.animation_interval_time;
	float const animation_length					= cubic_spline_skeleton_animation_pinned( animation_interval.animation() )->length_in_frames() / default_fps;
	R_ASSERT_CMP									( animation_time, <, 2*animation_length );
	animation_state.animation_time					= math::min( math::max( animation_time - animation_state.animation_time_threshold, 0.f ), animation_length );
}

void n_ary_tree::update_animation_state	(
		n_ary_tree_animation_node& animation_node,
		u32 const start_time_in_ms,
		u32 const target_time_in_ms
	)
{
	animation_state& animation_state	= animation_node.animation_state();
	n_ary_tree_weight_calculator weight_calculator( target_time_in_ms );
	weight_calculator.visit		( animation_node );
	animation_state.weight		= weight_calculator.weight( );

	if ( !animation_node.driving_animation() ) {
		animation_state.animation_interval_time	=
			n_ary_tree_animation_time_calculator(
				animation_node,
				start_time_in_ms,
				animation_state.animation_interval_time,
				target_time_in_ms,
				false
			).animation_time( );
		update_animation_time	( animation_state );
		return;
	}

	mixing::animation_state const* const driving_animation_state = &animation_node.driving_animation()->animation_state();
	R_ASSERT_CMP				( driving_animation_state, <, &animation_state );

	animation_interval const* const driving_animation_interval	= animation_node.driving_animation()->animation_intervals() + driving_animation_state->animation_interval_id;
	animation_interval const* const animation_interval			= animation_node.animation_intervals() + animation_state.animation_interval_id;
	animation_state.animation_interval_time						= animation_interval->length() / driving_animation_interval->length() * (*driving_animation_state).animation_interval_time;
	update_animation_time		( animation_state );
}

void n_ary_tree::update_synchronization_group	(
		n_ary_tree_animation_node& animation_node,
		u32 const start_time_in_ms,
		u32 const target_time_in_ms
	)
{
	animation_state& animation_state					= animation_node.animation_state();
	R_ASSERT											( !animation_node.driving_animation() );
	u32 const synchronization_group_id					= animation_node.synchronization_group_id( );
	bool are_there_any_weight_transitions				= animation_state.are_there_any_weight_transitions;
	if ( !are_there_any_weight_transitions ) {
		if ( synchronization_group_id != -1 ) {
			for (
					n_ary_tree_animation_node* current_driven_animation = animation_node.m_next_animation ;
					current_driven_animation && (current_driven_animation->synchronization_group_id() == synchronization_group_id) ;
					current_driven_animation = current_driven_animation->m_next_animation
				)
			{
				if ( current_driven_animation->animation_state().event_iterator.are_there_any_weight_transitions() ) {
					are_there_any_weight_transitions		= true;
					break;
				}
			}
		}
	}

	if ( are_there_any_weight_transitions )
		update_synchronization_group_using_integration	( animation_node, start_time_in_ms, target_time_in_ms );
	else
		update_animation_state							( animation_node, start_time_in_ms, target_time_in_ms );

	if ( synchronization_group_id == -1 )
		return;

	for (
			n_ary_tree_animation_node* current_driven_animation = animation_node.m_next_animation ;
			current_driven_animation && (current_driven_animation->synchronization_group_id() == synchronization_group_id) ;
			current_driven_animation = current_driven_animation->m_next_animation
		)
	{
		if ( current_driven_animation->animation_state().are_there_any_weight_transitions )
			continue;
		
		update_animation_state	( *current_driven_animation, start_time_in_ms, target_time_in_ms );
	}
}

void n_ary_tree::update_animation_states( u32 const start_time_in_ms, u32 const target_time_in_ms )
{
	for ( n_ary_tree_animation_node* current_animation = m_root; current_animation; current_animation = current_animation->m_next_animation ) {
		if ( !current_animation->driving_animation() )
			update_synchronization_group	( *current_animation, start_time_in_ms, target_time_in_ms );
	}
}

bool n_ary_tree::need_new_transform		( u32 const target_time_in_ms ) const
{
	for ( n_ary_tree_animation_node* current_animation = m_root; current_animation; current_animation = current_animation->m_next_animation ) {
		if ( current_animation->animation_state().event_iterator->event_time_in_ms == target_time_in_ms )
			if ( ( current_animation->animation_state().event_iterator->event_type & time_event_need_new_object_transform ) != 0)
				return			true;
	}

	return						false;
}

void n_ary_tree::remove_animation	(
		n_ary_tree_animation_node*& i,
		n_ary_tree_animation_node* j
	)
{
	if ( j ) {
		j->m_next_animation		= i->m_next_animation;
	}
	else {
		m_root					= i->m_next_animation;
		R_ASSERT				( m_root );
	}

	n_ary_tree_destroyer destroyer;
	i->accept					( destroyer );

	if ( j )
		i						= j->m_next_animation;
	else
		i						= m_root;

	--m_animations_count;
}

#ifndef MASTER_GOLD
static pcstr get_event_id				( u32 const event )
{
	using namespace xray::animation::mixing;

	switch ( event ) {
		case time_event_animation_lexeme_started				: return "animation lexeme started";
		case time_event_animation_lexeme_ended					: return "animation lexeme ended";
		case time_event_animation_interval_ended				: return "animation interval ended";
		case time_event_animation_ended_in_positive_direction	: return "animation ended (in positive direction)";
		case time_event_animation_ended_in_negative_direction	: return "animation ended (in negative direction)";
		case time_event_channel_callback_should_be_fired		: return "channel callback";
		case time_event_time_direction_changed					: return "time direction changed";
		case time_event_weight_transitions_started				: return "weight transitions started";
		case time_event_weight_transitions_ended				: return "weight transitions ended";
		default													: NODEFAULT( return "<unknown event id>" );
	}
}
#endif

#ifndef MASTER_GOLD
static void log_event					( xray::animation::mixing::animation_event const& event, n_ary_tree_animation_node& animation )
{
	LOG_DEBUG							( "\tevent types:" );
	if ( event.event_time_in_ms != u32(-1) ) {
		for (u32 event_type = event.event_type; event_type; event_type &= event_type - 1 )
			LOG_DEBUG					( "\t\t%s", get_event_id(event_type ^ (event_type & (event_type - 1)) ) );
	}
	else
		LOG_DEBUG						( "\t\t<no events>" );
	LOG_DEBUG							( "\tanimation               : %s", animation.identifier() );

	if ( event.event_time_in_ms == u32(-1) )
		return;

	LOG_DEBUG							( "\ttime                    : %d.%03ds", event.event_time_in_ms/1000, event.event_time_in_ms % 1000 );

	if ( event.animation_interval_id == u32(-1) )
		return;

	LOG_DEBUG							( "\tanimation interval id   : %d", event.animation_interval_id );
	LOG_DEBUG							( "\tanimation interval time : %.3fs", event.animation_interval_time );
}
#endif //#ifndef MASTER_GOLD

n_ary_tree::process_event_result_enum n_ary_tree::process_event	(
		n_ary_tree_animation_node*& current_animation_node,
		n_ary_tree_animation_node* const previous_animation_node,
		u32 const event_types
	)
{
	animation_state& animation_state	= current_animation_node->animation_state();
	u32 const event_type				= animation_state.event_iterator->event_type & event_types;
	bool object_transform_has_been_setup = false;
	if ( event_type & time_event_animation_lexeme_ended ) {
#ifndef MASTER_GOLD
		n_ary_tree_weight_calculator	weight_calculator( animation_state.event_iterator->event_time_in_ms );
		weight_calculator.visit			( *current_animation_node );
		float const weight				= weight_calculator.weight( );
		R_ASSERT						( weight_calculator.null_weight_found() );
		R_ASSERT_CMP					( weight, ==, 0.f );

		i_editor_mixer::call_user_callback (
			editor_animations_event::finish_animation,
			weight_calculator.weight_transition_ended_time_in_ms(),
			current_animation_node,
			0
		);

		LOG_DEBUG						( "processed animation event:" );
		log_event						( *current_animation_node->animation_state().event_iterator, *current_animation_node );
#endif // #ifndef MASTER_GOLD

		mixing::animation_state** const new_end	= std::remove( m_animation_events, m_animation_events + m_animations_count, &animation_state );
		R_ASSERT_U						( new_end == (m_animation_events + m_animations_count - 1 ) );

		remove_animation				( current_animation_node, previous_animation_node );
		return							process_event_result_animation_removed;
	}

	if ( event_type & time_event_animation_lexeme_started ) {
		u32 const event_time_in_ms		= animation_state.event_iterator->event_time_in_ms;
		n_ary_tree_time_scale_calculator time_scale_calculator(
			event_time_in_ms,
			animation_state.animation_interval_time,
			event_time_in_ms,
			n_ary_tree_time_scale_calculator::forbid_transitions_destroying
		);
		n_ary_tree_animation_node& driving_animation = *(current_animation_node->driving_animation() ? current_animation_node->driving_animation() : current_animation_node);
		R_ASSERT						( !driving_animation.driving_animation() );
		(*driving_animation.operands( sizeof(n_ary_tree_animation_node) ))->accept	( time_scale_calculator );
		float time_scale				= time_scale_calculator.time_scale();
		// check if we start at the time moment, where time scale is 0,
		// but it changes over time
		// (it is possible to have animation with time scale = 0 as a constant)
		if ( time_scale == 0.f ) {
			n_ary_tree_time_scale_calculator time_scale_calculator(
				event_time_in_ms + 1,
				animation_state.animation_interval_time,
				event_time_in_ms + 1,
				n_ary_tree_time_scale_calculator::forbid_transitions_destroying
			);
			(*driving_animation.operands( sizeof(n_ary_tree_animation_node) ))->accept	( time_scale_calculator );
			time_scale					= time_scale_calculator.time_scale();
		}

		if ( time_scale >= 0.f ) {
			animation_interval const& animation_interval	= current_animation_node->animation_intervals()[ animation_state.animation_interval_id ];
			float const animation_time						= animation_interval.start_time() + animation_state.animation_interval_time;
			float const animation_length					= cubic_spline_skeleton_animation_pinned( animation_interval.animation() )->length_in_frames() / default_fps;
			R_ASSERT_CMP									( animation_time, <, 2*animation_length );
			if ( animation_time == animation_length )
				animation_state.animation_time				= 0.f;
		}

		if ( !object_transform_has_been_setup ) {
			animation_state.are_there_any_weight_transitions	= false;
			set_object_transform			( *current_animation_node );
			object_transform_has_been_setup	= true;
		}
	}

	if ( event_type & time_event_animation_interval_ended ) {
		animation_state.animation_interval_id	= animation_state.event_iterator->animation_interval_id;
		animation_state.animation_interval_time	= animation_state.event_iterator->animation_interval_time;

		animation_interval const& interval		= current_animation_node->animation_intervals()[ animation_state.animation_interval_id ];
		float const animation_length			= cubic_spline_skeleton_animation_pinned(current_animation_node->animation_intervals()[ animation_state.animation_interval_id ].animation())->length_in_frames()/default_fps;
		if ( interval.start_time() + interval.length() > animation_length ) {
			if ( interval.start_time() + animation_state.animation_interval_time > animation_length )
				animation_state.animation_time_threshold	= animation_length;
			else
				animation_state.animation_time_threshold	= 0.f;
		}
		else
			animation_state.animation_time_threshold		= 0.f;

		update_animation_time					( animation_state );

		if ( !current_animation_node->driving_animation() ) {
#ifndef MASTER_GOLD
			LOG_DEBUG							( "ON_ANIMATION_INTERVAL_ENDED: [%s] setting time scale start time: %d, %.3f", current_animation_node->identifier(), animation_state.event_iterator->event_time_in_ms, animation_state.animation_interval_time );
#endif
			n_ary_tree_time_scale_start_time_modifier(
				*current_animation_node,
				animation_state.event_iterator->event_time_in_ms,
				animation_state.animation_interval_time
			);
		}
	}

	if ( event_type & time_event_animation_ended_in_positive_direction ) {
		animation_state.animation_time			= 0.f;
		if ( !(event_type & time_event_animation_interval_ended) )
			animation_state.animation_time_threshold	= cubic_spline_skeleton_animation_pinned(current_animation_node->animation_intervals()[ animation_state.animation_interval_id ].animation())->length_in_frames()/default_fps;
	}

	if ( event_type & time_event_animation_ended_in_negative_direction ) {
		animation_state.animation_time				= cubic_spline_skeleton_animation_pinned( current_animation_node->animation_intervals()[ animation_state.animation_interval_id ].animation() )->length_in_frames()/default_fps;
		animation_state.animation_time_threshold	= 0.f;
	}

	if ( event_type & time_event_channel_callback_should_be_fired ) { }
	
	if ( event_type & time_event_time_direction_changed ) {
		if ( !current_animation_node->driving_animation() ) {
#ifndef MASTER_GOLD
			LOG_DEBUG							( "ON_TIME_DIRECTION_CHANGED: [%s] setting time scale start time: %d, %.3f", current_animation_node->identifier(), animation_state.event_iterator->event_time_in_ms, animation_state.animation_interval_time );
#endif
			n_ary_tree_time_scale_start_time_modifier(
				*current_animation_node,
				animation_state.event_iterator->event_time_in_ms,
				animation_state.animation_interval_time
			);
		}
	}

	if ( event_type & time_event_weight_transitions_started ) {
		animation_interval const* const animation_interval	= current_animation_node->animation_intervals() + current_animation_node->animation_state().animation_interval_id;
		current_frame_position frame_position;
		frame frame_transform;
		{
			cubic_spline_skeleton_animation_pinned pinned_animation	= cubic_spline_skeleton_animation_pinned( animation_interval->animation() );
			frame_transform						=
				pinned_animation->bone( u32(0) ).bone_frame (
					animation_state.animation_time * pinned_animation->get_frames_per_second( ),
					frame_position
				);
		}

		bone_matrices_computer_data& bone_matrices_computer				= animation_state.bone_matrices_computer;
		bone_matrices_computer.accumulated_object_movement.translation	= frame_transform.translation - bone_matrices_computer.accumulated_object_movement.translation;
		bone_matrices_computer.accumulated_object_movement.rotation		= conjugate(bone_matrices_computer.accumulated_object_movement.rotation) * math::quaternion( frame_transform.rotation );
		bone_matrices_computer.accumulated_object_movement.scale		= frame_transform.scale / bone_matrices_computer.accumulated_object_movement.scale;
		animation_state.are_there_any_weight_transitions				= true;
	}

	if ( event_type & time_event_weight_transitions_ended ) {
		animation_interval const* const animation_interval	= current_animation_node->animation_intervals() + current_animation_node->animation_state().animation_interval_id;
		current_frame_position frame_position;
		frame frame_transform;
		{
			cubic_spline_skeleton_animation_pinned pinned_animation	= cubic_spline_skeleton_animation_pinned( animation_interval->animation() );
			frame_transform						=
				pinned_animation->bone( u32(0) ).bone_frame (
					animation_state.animation_time * pinned_animation->get_frames_per_second( ),
					frame_position
				);
		}

		bone_matrices_computer_data& bone_matrices_computer				= animation_state.bone_matrices_computer;
		bone_matrices_computer.accumulated_object_movement.translation	= frame_transform.translation;
		bone_matrices_computer.accumulated_object_movement.rotation		= math::quaternion( frame_transform.rotation );
		bone_matrices_computer.accumulated_object_movement.scale		= frame_transform.scale;
		animation_state.are_there_any_weight_transitions				= false;
	}

	return								process_event_result_nothing_to_do;
}

void n_ary_tree::process_events			( u32 const target_time_in_ms, u32 const event_types )
{
	animation_state* current_animation_state		= m_animation_states;
	animation_state* current_alive_animation_state	= current_animation_state;
	n_ary_tree_animation_node* previous_animation	= 0;
	for ( n_ary_tree_animation_node* current_animation = m_root; current_animation; ++current_animation_state ) {
		R_ASSERT						( &current_animation->animation_state() == current_animation_state );
		R_ASSERT_CMP					( current_animation_state->event_iterator->event_time_in_ms, >=, target_time_in_ms );
		if ( current_animation_state->event_iterator->event_time_in_ms == target_time_in_ms ) {
			if ( current_animation_state->event_iterator->event_type & event_types )
				if ( process_event( current_animation, previous_animation, event_types ) == process_event_result_animation_removed )
					continue;
		}

		if ( current_animation_state != current_alive_animation_state ) {
			*current_alive_animation_state			= *current_animation_state;
			current_animation->set_animation_state	( *current_alive_animation_state );
		}

		++current_alive_animation_state;
		previous_animation				= current_animation;
		current_animation				= current_animation->m_next_animation;
	}

	if ( current_alive_animation_state != current_animation_state ) {
		for ( animation_state* i = m_animation_states, *e = i + m_animations_count, **j = m_animation_events; i != e; ++i, ++j )
			*j							= i;
		std::sort						( m_animation_events, m_animation_events + m_animations_count, event_iterator_predicate() );
	}

	for ( ; current_alive_animation_state != current_animation_state; ++current_alive_animation_state )
		current_alive_animation_state->~animation_state	( );
}

namespace xray {
namespace animation {

struct callback_generator_info : private boost::noncopyable {
	inline	callback_generator_info	(
			skeleton_animation_ptr const& animation,
			float const	animation_time,
			u32 const event_type
		) :
		animation						( animation ),
		next							( 0 ),
		animation_time					( animation_time ),
		event_type						( event_type )
	{
	}

	skeleton_animation_ptr const		animation;
	callback_generator_info const*		next;
	float const							animation_time;
	u32 const							event_type;
}; // struct callback_generator_info

} // namespace animation
} // namespace xray

void n_ary_tree::update_event_iterators	( u32 const target_time_in_ms )
{
	// for each event iterator, with event which has just happenned, increment
	// it and insert into a proper position to keep event iterators in a sorted order
	while ( (*m_animation_events)->event_iterator->event_time_in_ms == target_time_in_ms ) {
#ifndef MASTER_GOLD
		LOG_DEBUG						( "processed animation event:" );
		log_event						( *(*m_animation_events)->event_iterator, (*m_animation_events)->event_iterator.animation() );
#endif
		++((*m_animation_events)->event_iterator);
		animation_state** const found	= std::lower_bound( m_animation_events + 1, m_animation_events + m_animations_count, *m_animation_events, event_iterator_predicate() );
		animation_state* const backup	= *m_animation_events;
		std::copy						( m_animation_events + 1, found, m_animation_events );
		*(found - 1)					= backup;
	}

#ifndef MASTER_GOLD
	LOG_DEBUG							( "NEXT animation events:" );
	for ( animation_state const* const* i = m_animation_events, * const* const e = m_animation_events + m_animations_count; i != e; ++i ) {
		LOG_DEBUG						( "animation event:" );
		log_event						( *(*i)->event_iterator, (*i)->event_iterator.animation() );
	}
#endif
}

void n_ary_tree::dispatch_callbacks		(
		xray::animation::callback_generator_info const* const callback_generators_head,
		xray::animation::subscribed_channel*& channels_head,
		u32 const current_time_in_ms,
		bool& callbacks_are_actual
	)
{
	// our tree is updated up to target_time_in_ms
	// therefore we are ready to dispatch callbacks
	// in which user is able to
	// * subscribe new callbacks
	// * unsubscribe old callbacks
	// * set up a new target!
	for ( callback_generator_info const* generator = callback_generators_head; generator; generator = generator->next ) {
		if ( (generator->event_type & (time_event_animation_ended_in_positive_direction | time_event_animation_ended_in_negative_direction | time_event_animation_interval_ended | time_event_animation_lexeme_ended)) != 0 ) {
			for ( subscribed_channel const* subscribed_channel = channels_head; subscribed_channel; subscribed_channel = subscribed_channel->next ) {
				if ( subscribed_channel->channel_id[0] >= channel_id_max )
					continue;

				R_ASSERT_CMP				( subscribed_channel->channel_id[1], ==, 0 );

				bool found					= false;
				if ( !found && (generator->event_type & (time_event_animation_ended_in_positive_direction | time_event_animation_ended_in_negative_direction)) )
					found					= subscribed_channel->channel_id[0] == channel_id_on_animation_end;

				if ( !found && (generator->event_type & time_event_animation_interval_ended) )
					found					= subscribed_channel->channel_id[0] == channel_id_on_animation_interval_end;

				//if ( !found && (generator->event_type & time_event_animation_lexeme_ended) )
				//	found					= subscribed_channel->channel_id[0] == channel_id_on_animation_lexeme_end;

				if ( !found )
					continue;

				for ( animation_callback* callback = subscribed_channel->first_callback; callback; callback = callback->next ) {
					if ( !(*callback).enabled ) {
						R_ASSERT			( !callbacks_are_actual );
						continue;
					}

					(*callback).enabled		= (*callback).callback( (*generator).animation, "", current_time_in_ms, u8(-1) ) == callback_return_type_call_me_again;
					callbacks_are_actual	= callbacks_are_actual && (*callback).enabled;
				}
			}
		}

		if ( (generator->event_type & time_event_channel_callback_should_be_fired) == 0 )
			continue;

		cubic_spline_skeleton_animation_pinned pinned_animation( (*generator).animation );
		u32 const channels_count			= pinned_animation->event_channels().channels_count();
		if ( !channels_count )
			continue;

		for ( subscribed_channel const* subscribed_channel = channels_head; subscribed_channel; subscribed_channel = subscribed_channel->next ) {
			u32 const channel_id		= pinned_animation->event_channels().get_channel_id( subscribed_channel->channel_id );
			if ( channel_id == u32(-1) )
				continue;

			event_channel const& channel = pinned_animation->event_channels().channel( channel_id );
			float const* const found	= std::lower_bound( channel.knots(), channel.knots() + channel.knots_count(), generator->animation_time*default_fps );
			u32 const knot_id			= found ? u32( found - channel.knots() ) : 0;
			u8 const domain_data		= channel.domain( knot_id ).data;
			for ( animation_callback* callback = subscribed_channel->first_callback; callback; callback = callback->next ) {
				if ( !(*callback).enabled ) {
					R_ASSERT			( !callbacks_are_actual );
					continue;
				}

				(*callback).enabled		= (*callback).callback( (*generator).animation, subscribed_channel->channel_id, current_time_in_ms, domain_data) == callback_return_type_call_me_again;
				callbacks_are_actual	= callbacks_are_actual && (*callback).enabled;
			}
		}
	}
}

void n_ary_tree::update_event_iterators_and_dispatch_callbacks	(
		u32 const target_time_in_ms,
		xray::animation::subscribed_channel*& channels_head,
		bool& callbacks_are_actual
	)
{
	// create dispatch callback information in stack
	callback_generator_info const* callback_generators_head	= 0;
	callback_generator_info * previous_generator_info		= 0;
	for ( animation_state const* i = m_animation_states, * const e = m_animation_states + m_animations_count; i != e; ++i ) {
		if ( i->event_iterator->event_time_in_ms != target_time_in_ms )
			continue;

		if ( (i->event_iterator->event_type & (time_event_channel_callback_should_be_fired | time_event_animation_ended_in_positive_direction | time_event_animation_ended_in_negative_direction | time_event_animation_interval_ended | time_event_animation_lexeme_ended)) == 0 )
			continue;

		callback_generator_info* const new_generator_info	= static_cast<callback_generator_info*>( ALLOCA( sizeof(callback_generator_info) ) );
		mixing::n_ary_tree_animation_node const& animation	= (*i).event_iterator.animation();
		mixing::animation_interval const* const interval	= animation.animation_intervals() + (*i).animation_interval_id;
		new ( new_generator_info ) callback_generator_info(
			interval->animation(),
			(*i).animation_time,
			i->event_iterator->event_type
		);

		if ( previous_generator_info )
			previous_generator_info->next	= new_generator_info;
		else
			callback_generators_head		= new_generator_info;

		previous_generator_info				= new_generator_info;
	}

	update_event_iterators				( target_time_in_ms );
	dispatch_callbacks					( callback_generators_head, channels_head, target_time_in_ms, callbacks_are_actual );
}

#ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
void n_ary_tree::normalize_weights		( )
{
	u32 max_layer_id					= 0;
	for ( mixing::animation_state* i = m_animation_states, *e = i + m_animations_count; i != e; ++i )
		max_layer_id					= math::max( max_layer_id, (*i).event_iterator.animation().additivity_priority() );

	for ( u32 j = 0; j < max_layer_id; ++j ) {
		float weights_accumulator		= 0.f;
		for ( animation_state* i = m_animation_states, *e = i + m_animations_count; i != e; ++i ) {
			if ( (*i).event_iterator.animation().additivity_priority() == j )
				weights_accumulator		+= (*i).weight;
		}

		for ( animation_state* i = m_animation_states, *e = i + m_animations_count; i != e; ++i ) {
			if ( (*i).event_iterator.animation().additivity_priority() == j )
				(*i).weight				/= weights_accumulator;
		}
	}
}
#endif // #ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS

void n_ary_tree::tick					(
		u32 const target_time_in_ms,
		xray::animation::subscribed_channel*& channels_head,
		bool& callbacks_are_actual
	)
{
	if ( !m_root )
		return;

#ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
	bool need_weights_normalization		= false;
#endif // #ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS

	R_ASSERT_CMP						( m_tree_actual_time_in_ms, <=, target_time_in_ms );
	for ( ; (*m_animation_events)->event_iterator->event_time_in_ms <= target_time_in_ms;  ) {
		u32 const event_time_in_ms		= (*m_animation_events)->event_iterator->event_time_in_ms;
		R_ASSERT_CMP					( m_tree_actual_time_in_ms, <=, event_time_in_ms );
		if ( m_tree_actual_time_in_ms < event_time_in_ms )
			update_animation_states		( m_tree_actual_time_in_ms, event_time_in_ms );

		if ( need_new_transform( event_time_in_ms ) ) {
			process_events				( event_time_in_ms, time_event_animation_lexeme_started);
			float4x4 const& new_object_transform	= get_object_transform( );
			process_events				( event_time_in_ms, time_event_all_events ^ (time_event_animation_lexeme_started) );
			set_object_transform		( new_object_transform );
		}
		else {
			process_events				( event_time_in_ms, time_event_all_events );
		}

		m_tree_actual_time_in_ms		= event_time_in_ms;

		update_event_iterators_and_dispatch_callbacks	( event_time_in_ms, channels_head, callbacks_are_actual );

#ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
		need_weights_normalization		= true;
#endif // #ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
	}

	if ( m_tree_actual_time_in_ms != target_time_in_ms ) {
		R_ASSERT_CMP					( m_tree_actual_time_in_ms, <, target_time_in_ms );
		update_animation_states			( m_tree_actual_time_in_ms, target_time_in_ms );
		R_ASSERT_U						( !need_new_transform(target_time_in_ms) );
		m_tree_actual_time_in_ms		= target_time_in_ms;
#ifndef MASTER_GOLD
		dump_animation_states			( target_time_in_ms );
#endif // #ifndef MASTER_GOLD
#ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
		need_weights_normalization		= true;
#endif // #ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
	}

#ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
	if ( need_weights_normalization )
		normalize_weights				( );
#endif // #ifdef XRAY_NORMALIZE_ANIMATIONS_WEIGHTS
}