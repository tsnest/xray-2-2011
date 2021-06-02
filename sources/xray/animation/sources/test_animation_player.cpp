////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test_animation_player.h"
#include "skeleton_animation.h"
#include "bone_mixer.h"
#include "test_mixing_animation_group.h"
#include "mixer.h"

using xray::animation::test_animation_player;
using xray::animation::test_mixing_animation_group;

using xray::configs::lua_config_ptr;
using xray::configs::lua_config_value;
using xray::configs::lua_config_iterator;

test_animation_player::test_animation_player		( ) :
	m_first_tick			( true ),
	m_skeleton				( 0 ),
	m_bone_mixer			( 0 ),
	m_mixer					( 0 ),
	m_loaded				( false )
{
	resources::request	resources[]	= {
		{ "resources/animations/test_animation_player.lua",	xray::resources::config_lua_class },
		{ "resources/animations/skeletons/human",		xray::resources::skeleton_class },
	};

	xray::resources::query_resources(
		resources,
		array_size(resources),
		boost::bind( &test_animation_player::on_test_config_loaded, this, _1 ),
		g_allocator
	);

	m_timer.start			( );
}

test_animation_player::~test_animation_player		( )
{
	animation_groups_backup_type::reverse_iterator i		= m_animation_groups_backup.rbegin( );
	animation_groups_backup_type::reverse_iterator const e	= m_animation_groups_backup.rend( );
	for ( ; i != e; ++i )
		DELETE				( *i );
	
	DELETE					( m_mixer );
	DELETE					( m_bone_mixer );
}

void test_animation_player::on_test_config_loaded	( xray::resources::queries_result& resource )
{
	R_ASSERT				( !resource.is_failed() );
	m_skeleton_resource		= resource[1].get_unmanaged_resource();
	m_skeleton				= static_cast_checked< skeleton* >( m_skeleton_resource.c_ptr() );
	
	m_bone_mixer			= NEW( bone_mixer ) ( *m_skeleton );

	m_config				=
		static_cast_resource_ptr< lua_config_ptr >( resource[0].get_unmanaged_resource() );

	lua_config_value const sequence	= (*m_config)["sequence"];

	u32 const sequence_size	= sequence.size();
	ASSERT_CMP_U			( sequence_size, >, 0 );
	m_animation_groups_backup.reserve	( sequence_size );

	lua_config_iterator i	= sequence.begin( );
	lua_config_iterator const e = sequence.end( );
	for ( ; i != e; ++i ) {
		m_animation_groups_backup.push_back( NEW( test_mixing_animation_group )( *i, m_skeleton ) );
	}
}

void test_animation_player::check_loaded			(  )
{
	if ( m_animation_groups_backup.empty() )
		return;
	
	animation_groups_backup_type::const_iterator i			= m_animation_groups_backup.begin();
	animation_groups_backup_type::const_iterator const e	= m_animation_groups_backup.end();
	for ( ; i != e; ++i ) {
		if ( !(*i)->loaded() )
			return;
	}

	m_loaded				= true;
}

void test_animation_player::tick			( )
{
	if ( !m_loaded ) {
		check_loaded		( );
		if ( m_loaded ) {
			test_mixer		( m_animation_groups_backup.front()->m_clips.front().first->animation(), *m_skeleton );
		}
		return;
	}

	float const time_scale_factor	= 0.1f;
	float const time_to_wait		= 5.f*time_scale_factor;
	u32 const current_time_in_ms = math::floor( float(m_timer.get_elapsed_ms())*time_scale_factor );
	float const current_time	= float(current_time_in_ms)/1000.f;
	if ( current_time < time_to_wait )
		return;
	
	if ( m_first_tick ) {
		m_first_tick			= false;

		m_animation_groups.assign( m_animation_groups_backup.begin() , m_animation_groups_backup.end() );
		m_animation_start_time	= current_time;

		DELETE					( m_mixer );
		m_mixer					= NEW( mixer )( *m_bone_mixer, *g_allocator );
		m_animation_groups.front()->set_target( *m_mixer, current_time_in_ms );
	}

	animation_group const& animation	= m_animation_groups.front( );
	float const animation_time	= current_time - m_animation_start_time;
	R_ASSERT_CMP				( animation_time, >=, 0.f );
	if ( animation->play_time() > animation_time ) {
		m_mixer->tick			( current_time_in_ms );
		return;
	}

	if ( m_animation_groups.size() == 1 ) {
		if ( animation_time < m_animation_groups.front()->max_time() )
			m_mixer->tick		( current_time_in_ms );
		else {
			m_animation_groups.pop_front	( );
			m_first_tick		= true;
			m_bone_mixer->reset_reference_pose( float4x4().identity() );
		}

		return;
	}

	m_animation_groups.pop_front	( );
	ASSERT						( !m_animation_groups.empty() );

	m_animation_start_time		= current_time;
	m_animation_groups.front()->set_target( *m_mixer, current_time_in_ms );

	//animation_lexeme		lexeme( buffer, m_animation_groups.front().first );

	//m_mixer->set_target			( lexeme, current_time_in_ms );
}

void test_animation_player::render	( xray::render::debug::renderer& renderer )
{
	if ( !m_mixer )
		return;

	m_mixer->render				( renderer, float4x4().identity(), 0 );
}