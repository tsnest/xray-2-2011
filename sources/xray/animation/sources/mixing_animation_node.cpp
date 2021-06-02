////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_animation_node.h"
#include "mixing_animation.h"
#include "base_interpolator.h"
#include "mixing_n_ary_tree_builder.h"

using xray::animation::mixing_animation_node;
using xray::animation::mixing_n_ary_tree_builder;
using xray::animation::mixing_base_node;

float mixing_animation_node::weight				( ) const
{
	return			1.f;
}

float mixing_animation_node::transition_weight	( float transition_time ) const
{
	return			m_animation->interpolator().interpolated_value( transition_time );
}

void mixing_animation_node::accept	( mixing_n_ary_tree_builder& visitor )
{
	visitor.visit	( *this );
}

base_interpolator
instant_interpolator

mixing_animation

mixing_binary_tree_base_node
mixing_binary_tree_animation_node
mixing_binary_tree_weight_node
mixing_binary_tree_binary_operation_node
mixing_binary_tree_addition_node
mixing_binary_tree_multiplication_node
mixing_binary_tree_subtraction_node

mixing_base_lexeme
mixing_animation_lexeme
mixing_weight_lexeme
mixing_binary_operation_lexeme
mixing_addition_lexeme
mixing_multiplication_lexeme
mixing_subtraction_lexeme

mixing_n_ary_tree_base_node
mixing_n_ary_tree_animation_node
mixing_n_ary_tree_weight_node
mixing_n_ary_tree_operation_node
mixing_n_ary_tree_addition_node
mixing_n_ary_tree_multiplication_node
mixing_n_ary_tree_subtraction_node
mixing_n_ary_tree_transition_node

mixing_mixer