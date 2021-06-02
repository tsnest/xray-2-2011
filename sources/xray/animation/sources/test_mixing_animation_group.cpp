////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test_mixing_animation_group.h"
#include "fermi_dirac_interpolator.h"
#include "linear_interpolator.h"
#include "instant_interpolator.h"
#include "mixing_animation_lexeme.h"
#include "mixing_weight_lexeme.h"
#include "mixing_addition_lexeme.h"
#include "mixing_multiplication_lexeme.h"
#include "mixing_expression.h"
#include "skeleton_animation_data.h"
#include "mixer.h"
#include "skeleton_animation.h"
#include "mixing_animation_clip.h"

using xray::animation::test_mixing_animation_group;

using xray::configs::lua_config_ptr;
using xray::configs::lua_config_value;
using xray::configs::lua_config_iterator;

void	test_mixing_animation_group::add_animation( lua_config_value const &config, xray::resources::query_result_for_user const &anim, xray::animation::skeleton const*	skel )
{

		lua_config_value cfg = config["animation"];
		

		base_interpolator* interpolator = 0;
		pcstr const interpolator_type = cfg["interpolator_type"];
		if ( !strings::compare(interpolator_type,"fermi") ) {
			interpolator	= NEW(fermi_dirac_interpolator)(cfg["interpolation_time"], cfg["interpolation_epsilon"] );
		} else if ( !strings::compare(interpolator_type,"linear") ) {
			interpolator	= NEW(linear_interpolator)( cfg["interpolation_time"] );
		} else if ( !strings::compare(interpolator_type,"instant") ) {
			interpolator	= NEW(instant_interpolator);
		} else if ( !strings::compare(interpolator_type,"instant") ) {
			UNREACHABLE_CODE( );
		}

		skeleton_animation_data* const animation_data	= 
			static_cast_checked<skeleton_animation_data*>( anim.get_unmanaged_resource().c_ptr() );
		
		animation_data->set_skeleton( skel );
		skeleton_animation* const skeleton_animation = NEW( animation::skeleton_animation )( *animation_data ) ;
		m_p_animations.push_back( skeleton_animation );
			
		ASSERT				( interpolator );

		mixing::animation_clip* const animation =
			NEW (mixing::animation_clip) (
				cfg["animation"],
				interpolator,
				skeleton_animation,
				!strings::compare( "full", cfg["animation_type"]) ? mixing::animation_clip::full : mixing::animation_clip::additive
			);

		m_max_time = math::min( m_max_time, animation->time() );
		m_max_mix_time = math::max( m_max_mix_time, interpolator->transition_time() );
		m_clips.push_back	(std::make_pair(  animation, config["weight"] ) );

}

test_mixing_animation_group::test_mixing_animation_group( lua_config_value cfg, xray::animation::skeleton const*	skel    ): 
m_max_mix_time( 0 ),
m_max_time( math::infinity ),
m_config( cfg ),
m_skeleton( skel ),
m_loaded( false )
{
	
	
	u32 const size	= cfg["group"].size();

	resources::request* const resources = static_cast<resources::request*>( ALLOCA( size*sizeof(resources::request) ) );

	lua_config_iterator i	= cfg["group"].begin( );
	lua_config_iterator const e =  cfg["group"].end( );
	for (resources::request* j = resources; i != e; ++i, ++j ) {
		j->path				= (*i)["animation"]["animation"];
		j->id				= resources::animation_data_class;
	}

	xray::resources::query_resources(
		resources,
		size,
		boost::bind( &test_mixing_animation_group::on_animations_loaded, this, _1 ),
		g_allocator
	);
}

float	test_mixing_animation_group::play_time() const 
{ 
	ASSERT( m_max_time != math::infinity );
	return m_max_time - m_max_mix_time ;
}





test_mixing_animation_group::~test_mixing_animation_group( )
{
	
	{
		vector< animation_pair >::iterator  i = m_clips.begin()  , e = m_clips.end();

		for ( ; i != e; ++i ) 
			DELETE( (*i).first );
	}


	{
		vector< skeleton_animation* >::iterator  i = m_p_animations.begin()  , e = m_p_animations.end();

		for ( ; i != e; ++i )
			DELETE( (*i) );
	}

}


void	test_mixing_animation_group::on_animations_loaded	( xray::resources::queries_result& resource )
{
	u32 const size	=resource.size();
	for (u32 i=0; i < size; ++i ) {

		add_animation( m_config["group"][i], resource[i], m_skeleton );
	}
	m_loaded = true;
}




void	test_mixing_animation_group::set_target( xray::animation::mixer& mxr, const u32 time )const
{
	mutable_buffer				buffer( ALLOCA(mixer::stack_buffer_size), mixer::stack_buffer_size );
	mixing::animation_lexeme	lx_animation( buffer, m_clips.front().first );
	mixing::weight_lexeme		lx_weight( buffer, m_clips.front().second, &m_clips.front().first->interpolator() );

	mixing::expression expression =  lx_weight * lx_animation;

	const u32 count = m_clips.size();

	for( u32 i = 1; i < count; ++i )
	{
		mixing::animation_lexeme	animation( buffer, m_clips[i].first );
		mixing::weight_lexeme		weight( buffer, m_clips[i].second, & m_clips[i].first->interpolator() );
		expression					= animation*weight + expression;
	}

	mxr.set_target				( expression, time );
}