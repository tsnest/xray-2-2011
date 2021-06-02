////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixer.h"
#include "bone_mixer.h"
#include "mixing_expression.h"
#include "mixing_n_ary_tree_builder.h"
#include "mixing_n_ary_tree_comparer.h"
#include "mixing_n_ary_tree_animation_node.h"

using xray::animation::mixer;
using xray::animation::mixing::expression;
using xray::animation::mixing::n_ary_tree;

void mixer::render						( xray::render::debug::renderer	&renderer, const xray::math::float4x4 &matrix, float time )
{
	m_bone_mixer.render			( renderer, matrix, time );
}

void mixer::fill_bone_mixer_data		( u32 const current_time_in_ms )
{
	for ( mixing::n_ary_tree_animation_node* i = &*m_mixing_tree.root(); i; i = i->m_next_animation )
		m_bone_mixer.on_new_animation( (*i).animation(), (*i).bone_mixer_data(), current_time_in_ms );
}

void mixer::set_target_impl				( expression const& expression, u32 const current_time_in_ms )
{
	mixing::n_ary_tree_builder	builder( expression );
	u32 const buffer_size		= builder.needed_buffer_size( );

	bool const first_time		= !m_mixing_tree.root();
	pvoid const buffer_raw		=
		!first_time ?
		ALLOCA(buffer_size) :
		XRAY_MALLOC_IMPL(
			m_allocator,
			buffer_size,
			"mixing tree"
		);

	mutable_buffer buffer( buffer_raw, buffer_size );
	n_ary_tree const target_tree = builder.constructed_n_ary_tree( buffer, m_mixing_tree.root() ? 0 : &m_allocator, current_time_in_ms );
	R_ASSERT					( !buffer.size(), "buffer calculation failed: %d bytes left", buffer.size() );

	if ( first_time ) {
		bool const first_time	= !m_mixing_tree.root();
		make_inactual			( );
		m_mixing_tree			= target_tree;
		if ( first_time )
			fill_bone_mixer_data( current_time_in_ms );
		return;
	}

	mixing::n_ary_tree_comparer	comparer( m_mixing_tree, target_tree );
	if ( comparer.equal() )
		return;

	make_inactual				( );
	u32 const mixing_buffer_size	= comparer.needed_buffer_size( );
	ASSERT						( mixing_buffer_size );
	mutable_buffer mixing_buffer(
		XRAY_MALLOC_IMPL(
			m_allocator,
			mixing_buffer_size,
			"mixing tree"
		),
		mixing_buffer_size
	);

	m_mixing_tree				= comparer.computed_tree( mixing_buffer, &m_allocator, m_bone_mixer, current_time_in_ms );
	R_ASSERT					( !mixing_buffer.size(), "buffer calculation failed: %d bytes left", mixing_buffer.size() );
}

void mixer::set_target					( expression const& expression, u32 const current_time_in_ms )
{
	if ( m_mixing_tree.root() )
		tick					( current_time_in_ms );

	set_target_impl				( expression, current_time_in_ms );
	tick						( current_time_in_ms );
}

void mixer::tick						( u32 const current_time_in_ms )
{
	R_ASSERT					( m_mixing_tree.root() );

	if ( m_last_actual_time_ms == current_time_in_ms )
		return;

	m_last_actual_time_ms		= current_time_in_ms;
	m_mixing_tree.tick			( current_time_in_ms );
//	m_mixing_tree.dump			( );
	
	mixing::animation_state const* const data_begin	= m_mixing_tree.animation_states();
	m_bone_mixer.process		( data_begin, data_begin + m_mixing_tree.significant_animations_count() );
}