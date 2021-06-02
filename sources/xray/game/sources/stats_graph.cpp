////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stats_graph.h"
#include <xray/render/facade/ui_renderer.h>

using stalker2::stats_graph;
using xray::render::world;

stats_graph::stats_graph				(
		float const time_interval,
		float const invalid_value,
		float const important_value0,
		float const important_value1,
		u32 const color
	) :
	m_current_value					( 0 ),
	m_values_pool					( 0 ),
#ifdef FIXED_FPS
	m_last_render_time				( 0.f ),
#endif // #ifdef FIXED_FPS
	m_time_interval					( time_interval ),
	m_invalid_value					( invalid_value	),
	m_important_value0				( important_value0 ),
	m_important_value1				( important_value1 ),
	m_count							( 0 ),
	m_color							( color )
{
#ifdef FIXED_FPS
	m_last_commands[0]				= 0;
#endif // #ifdef FIXED_FPS
}

stats_graph::~stats_graph				( )
{
	for ( u32 i=0; i < m_count; ++i ) {
		stats_value* value			= m_current_value;
		m_current_value				= m_current_value->next;
		DELETE						( value );
	}

	while ( m_values_pool ) {
		stats_value* value			= m_values_pool;
		m_values_pool				= m_values_pool->next;
		DELETE						( value );
	}
}

void stats_graph::adjust_time_interval	( )
{
	ASSERT							( m_current_value->time >= m_current_value->next->time );
	if ( m_current_value->time - m_current_value->next->time <= m_time_interval )
		return;
	
	while ( m_current_value->time - m_current_value->next->next->time >= m_time_interval ) {
		stats_value* old_value		= m_current_value->next;
		m_current_value->next		= old_value->next;
		old_value->next->previous	= m_current_value;
		--m_count;

		ASSERT						( m_count >= 2 );
		ASSERT						( m_current_value->time - m_current_value->next->time >= m_time_interval );

		if ( !m_values_pool ) {
			m_values_pool			= old_value;
			m_values_pool->next		= 0;
		}
		else {
			old_value->next			= m_values_pool;
			m_values_pool			= old_value;
		}
	}
}

void stats_graph::add_value				( float time, float value )
{
	if ( (m_count > 1) && (time - m_current_value->next->next->time >= m_time_interval) ) {
		m_current_value				= m_current_value->next;
		m_current_value->time		= time;
		m_current_value->value		= value;
		adjust_time_interval		( );
		return;
	}

	stats_value*					new_value;
	if ( !m_values_pool )
		new_value					= NEW( stats_value );
	else {
		new_value					= m_values_pool;
		m_values_pool				= m_values_pool->next;
	}

	new_value->time					= time;
	new_value->value				= value;
	++m_count;

	if ( !m_current_value ) {
		new_value->next				= new_value;
		new_value->previous			= new_value;
		m_current_value				= new_value;
		return;
	}

	new_value->next					= m_current_value->next;
	new_value->previous				= m_current_value;
	m_current_value->next			= new_value;
	new_value->next->previous		= new_value;
	m_current_value					= new_value;
}

void stats_graph::set_time_interval		( float new_time_interval )
{
	m_time_interval					= new_time_interval;
	adjust_time_interval			( );
}

float stats_graph::stats_time			( ) const
{
	R_ASSERT						( m_count );
	R_ASSERT						( m_current_value );
	R_ASSERT						( m_current_value->next );
	return							m_current_value->time - m_current_value->next->time;
}

float stats_graph::average_value		( ) const
{
	float const time				= stats_time();
	if ( math::is_zero(time) )
		return						( 0.f );

	return							m_count / time;
}

void stats_graph::render				( xray::render::ui::renderer& renderer, xray::render::scene_view_ptr const& scene_view, u32 const current_frame_id, u32 const left_margin, u32 const top_margin, u32 const width, u32 const height )
{
	XRAY_UNREFERENCED_PARAMETER		(current_frame_id);
	if ( !m_current_value )
		return;

#ifdef FIXED_FPS
	float const time_delta			= m_current_value->time - m_last_render_time;
	float const spf					= 1.f/FIXED_FPS;
	if ( time_delta < spf )
		return;

	if ( m_last_commands[0] ) {
		for ( u32 i=0; i<command_count; ++i) {
			m_last_commands[i]->remove_frame_id	= current_frame_id;
			m_last_commands[i]				= 0;
		}
	}

	m_last_render_time				= m_current_value->time;// - time_delta + math::floor( time_delta/spf )*spf;
#endif // #ifdef FIXED_FPS

	float min_value					= 0.f;
	float max_value					= m_important_value1;

	stats_value *i					= m_current_value->next;
	u32 count						= 0;
	do {
		min_value					= math::min( i->value, min_value );
		if ( i->value != m_invalid_value ) {
			max_value				= math::max( i->value, max_value );
			++count;
		}
		
		i							= i->next;
	} while ( i->previous != m_current_value );

	if ( count < 2 )
		return;

	float const value_range			= max_value - min_value;
	float const value_height		= float(height);

	for ( u32 i=0; i<4; ++i ) {
		float						value;
		u32							color;
		switch ( i ) {
			case 0 : {
				value				= m_important_value0;
				color				= math::color_xrgb( 255, 0, 0 );
				break;
			}
			case 1 : {
				value				= m_important_value1;
				color				= math::color_xrgb( 255, 255, 0 );
				break;
			}
			case 2 : {
				value				= 0.f;
				color				= math::color_xrgb( 0, 0, 0);
				break;
			}
			case 3 : {
				value				= average_value();
				color				= math::color_xrgb( 0, 255, 255 );
				break;
			}
			default :				{
				NODEFAULT();
#ifdef DEBUG
				value				= math::SNaN;
				color				= *(u32*)&value;
#endif // #ifdef DEBUG
			}
		}

//		xray::render::ui::command* const command = renderer.create_command( 2, 1, 0 );
#ifdef FIXED_FPS
		m_last_commands[i]			= command;
#endif // #ifdef FIXED_FPS
		float y						= float(top_margin + math::floor( (1.f - value/float(value_range))*value_height ));
		xray::render::ui::vertex const vertices[] = {
			xray::render::ui::vertex( float(left_margin),				y, 0.f, color, 0, 0 ),
			xray::render::ui::vertex( float(left_margin + width - 1),	y, 0.f, color, 0, 0 ),
		};
		renderer.draw_vertices		( scene_view, array_begin(vertices), array_end(vertices), 1, 0 );
//		command->push_point			( float(left_margin),				y, 0.f, color, 0, 0 );
//		command->push_point			( float(left_margin + width - 1),	y, 0.f, color, 0, 0 );
#ifdef FIXED_FPS
		command->remove_frame_id	= u32(-1);
#endif // #ifdef FIXED_FPS
//		renderer.push_command		( command );
	}

//	xray::render::ui::command* const command = renderer.create_command( count, 1, 0 );
#ifdef FIXED_FPS
	m_last_commands[command_count - 1]	= command;
#endif // #ifdef FIXED_FPS

	typedef buffer_vector< xray::render::ui::vertex >	vertices_type;
	vertices_type vertices( ALLOCA(count*sizeof(xray::render::ui::vertex)), count );
	float total_time				= math::max	( stats_time(), m_time_interval );
	i								= m_current_value->next;
	float const start_time			= i->time;
	do {
		if ( i->value != m_invalid_value ) {
			u32 const x				= left_margin + math::floor( (i->time - start_time)/total_time*float(width - 1) );
			u32 const y				= top_margin + math::floor( (1.f - i->value/float(value_range))*value_height );
			vertices.push_back		( xray::render::ui::vertex( float(x), float(y), 0.f, m_color, 0, 0 ) );
		}
		i							= i->next;
	} while ( i != m_current_value );

#ifdef FIXED_FPS
	command->remove_frame_id		= u32(-1);
#endif // #ifdef FIXED_FPS
	renderer.draw_vertices			( scene_view, &*vertices.begin(), &*vertices.end(), 1, 0 );
//	renderer.push_command				( command );
}

void stats_graph::stop_rendering	( )
{
#ifdef FIXED_FPS
	if ( !m_last_commands[0] )
		return;

	for ( u32 i=0; i<command_count; ++i ) {
		m_last_commands[i]->remove_frame_id	= 0;
		m_last_commands[i]					= 0;
	}
#endif // #ifdef FIXED_FPS
}