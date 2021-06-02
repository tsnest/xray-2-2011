////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/mixing_animation_lexeme.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>

using xray::animation::mixing::animation_lexeme;
using xray::animation::mixing::animation_lexeme_parameters;

animation_lexeme::animation_lexeme	( animation_lexeme_parameters const& parameters ) :
	binary_tree_animation_node	( parameters ),
	base_lexeme					( parameters.buffer() ),
	m_cloned_instance			( 0 )
{
}