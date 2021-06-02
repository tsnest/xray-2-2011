////////////////////////////////////////////////////////////////////////////
//	Created		: 24.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "viewer_document.h"
#include "viewer_editor.h"
#include "animation_editor.h"
#include "animation_node_clip_instance.h"
#include "animation_node_clip.h"
#include "animation_node_interval.h"
#include "animation_node_playing_instance.h"
#include "time_scale_instance.h"
#include "animation_node_interpolator.h"
#include "time_instance.h"
#include "animation_model.h"
#include "animation_viewer_time_panel.h"
#include "animation_viewer_hypergraph.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_viewer_graph_node_operator.h"
#include "animation_viewer_graph_node_root.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_link_weighted.h"
#include "animation_viewer_graph_link_timed.h"
#include "transform_control_helper.h"

#include <xray/animation/api.h>
#include <xray/animation/i_editor_mixer.h>
#include <xray/animation/mixing_base_lexeme.h>
#include <xray/animation/mixing_weight_lexeme.h>
#include <xray/animation/mixing_addition_lexeme.h>
#include <xray/animation/mixing_multiplication_lexeme.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>
#include <xray/animation/mixing_animation_interval.h>

static const u32 expression_alloca_buffer_size = 64 * xray::Kb;

using xray::animation::mixing::expression;
using xray::animation::mixing::animation_lexeme_ptr;
using xray::animation::mixing::animation_lexeme;
using xray::animation::mixing::weight_lexeme;
using xray::animation::mixing::animation_lexeme_parameters;
using xray::animation::editor_animations_event;


namespace xray {
namespace animation_editor {

////////////////			P U B L I C				////////////////

void viewer_document::set_target( xray::animation_editor::animation_viewer_graph_node_base^ n )
{
	if( n == nullptr )
	{
		m_prev_target_ids->Clear			( );
		m_cur_target_ids->Clear				( );
		m_assigned_ids->Clear				( );
		m_animation_items->Clear			( );
		m_editor->time_panel->clear_items	( );
	}

	if( m_active_model == nullptr || m_active_model->model( ) == NULL )
		return;

	if( m_target == n )
		return;

	m_prev_target_ids->Clear				( );
	m_cur_target_ids->Clear					( );
	m_assigned_ids->Clear					( );
	m_animation_items->Clear				( );
	m_editor->time_panel->clear_items		( );
	if( n == nullptr )
	{
		m_editor->time_panel->set_buttons_enable	( false );
		m_target = nullptr;
		stop( );
		return;
	}

	if( m_target == nullptr )
		m_editor->time_panel->set_buttons_enable	( true );

	if( !m_target_locked )
	{
		m_current_time	= 0.0f;
		paused			= true;
	}

	m_target			= n;
	recalc_mixer_events	( );
	set_target_impl		( );
}

void viewer_document::reset_target ( )
{
	animation_viewer_graph_node_base^ t = m_target;
	m_target = nullptr;
	set_target( t );
}


////////////////			 P R I V A T E				////////////////

animation_playback::animation_item^ viewer_document::item_by_user_data( u32 user_data )
{
	for each( animation_playback::animation_item^ item in m_animation_items )
	{
		if( item->user_data == user_data )
			return item;
	}

	return nullptr;
}

void viewer_document::create_animation_item( xray::animation::mixing::animation_lexeme_ptr lexeme )
{
	u32 user_data = lexeme->user_data;
	if( item_by_user_data( user_data ) != nullptr )
		return;

	animation_playback::animation_item^ item	= gcnew animation_playback::animation_item( );
	System::String^ name						= gcnew System::String( lexeme->identifier( ) );
	name = name->Remove							( 0, name->LastIndexOf("/") + 1 );

	item->name					= name + " \nuser_data[" + user_data.ToString( ) + "] sync_group[" + lexeme->synchronization_group_id( ) + "]";
	item->user_data				= user_data;
	item->sync_group			= lexeme->synchronization_group_id( );
	item->position				= 0.0f;
	item->offset				= 0.0f;
	item->length				= 0.0f;
	item->time_scaled_length	= 0.0f;
	item->time_scale			= 1.0f;
	item->weight				= 1.0f;
	m_animation_items->Add		( item );
}

void viewer_document::update_animation_items( xray::vectora<xray::animation::editor_animation_state> const& cur_anim_states )
{
	if( !m_editor->time_panel )
		return;

	for each( animation_playback::animation_item^ item in m_animation_items )
	{
		item->position				= current_time;
		item->offset				= 0.0f;
		item->length				= 0.0f;
		item->time_scaled_length	= 0.0f;
		item->is_leading			= false;
	}

	vectora<editor_animation_state>::const_iterator it_b = cur_anim_states.begin( );
	vectora<editor_animation_state>::const_iterator it_e = cur_anim_states.end( );
	for(; it_b != it_e; ++it_b )
	{
		animation_playback::animation_item^ item = item_by_user_data( it_b->user_data );
		R_ASSERT( item );
		item->position				= it_b->position;
		item->offset				= it_b->offset;
		item->length				= it_b->length;
		item->time_scaled_length	= it_b->time_scaled_length;
		item->time_scale			= it_b->time_scale;
		item->weight				= it_b->weight;
		item->is_leading			= it_b->is_leading;
	}
}

void viewer_document::recalc_mixer_events( )
{
	if( !can_create_expression_from_node( m_target ) )
		return set_target( nullptr );

	float prev_time			= 0.0f;
	float next_time			= 0.0f;
	void* buffer_memory		= ALLOCA( expression_alloca_buffer_size );
	mutable_buffer buffer	( buffer_memory, expression_alloca_buffer_size );
	expression expr			( create_expression_from_node( m_target, buffer, true ) );

	if( m_editor->use_detailed_weights_and_scales )
	{
		m_active_model->reset		( );
		m_active_model->set_target	( expression( expr ), 0 );
	}

	swap_target_ids( );
	animation_viewer_graph_node_root^ prev = dynamic_cast<animation_viewer_graph_node_root^>( m_target );
	if( prev )
	{
		animation_viewer_graph_node_root^ next = prev->transition( );
		while( next )
		{
			if( !can_create_expression_from_node( next ) )
				return set_target( nullptr );

			animation_viewer_graph_link^ link				= animation_viewer_hypergraph::find_link_by_ids( prev->get_parent( )->links_manager->visible_links( ), prev->id, next->id );
			animation_viewer_graph_link_timed^ link_timed	= safe_cast<animation_viewer_graph_link_timed^>( link );
			time_instance^ ti								= safe_cast<time_instance^>( link_timed->playing_time->owner );

			prev_time		= next_time;
			next_time		+= ti->time_s * 1000.0f;

			if( next_time >= max_time )
				break;

			if( m_editor->use_detailed_weights_and_scales )
			{
				for( float t = prev_time; t < next_time; t += m_editor->detalization_delta )
				{
					u32 time = (u32)System::Math::Round( t );
					vectora<editor_animation_state> cur_anim_states	( g_allocator );
					m_active_model->tick							( time );
					m_active_model->get_current_anim_states			( cur_anim_states, time );

					vectora<editor_animation_state>::const_iterator anim_states_it = cur_anim_states.begin( );
					for(; anim_states_it != cur_anim_states.end( ); ++anim_states_it )
					{
						animation_playback::animation_item^ item = item_by_user_data( anim_states_it->user_data );
						if( m_editor->show_detailed_weights )
							item->weights_by_time->Add	( time, anim_states_it->weight );

						if( m_editor->show_detailed_scales )
							item->scales_by_time->Add	( time, anim_states_it->time_scale );
					}
				}
			}

			u32 time = (u32)System::Math::Round( next_time );
			m_active_model->push_expression( expr, time );
			expr = create_expression_from_node( next, buffer, true );
			if( m_editor->use_detailed_weights_and_scales )
				m_active_model->set_target( expr, time );

			swap_target_ids( );
			prev = next;
			next = next->transition( );
		}
	}

	prev_time = next_time;
	if( m_editor->use_detailed_weights_and_scales )
	{
		for( float t = prev_time; t <= max_time; t += m_editor->detalization_delta )
		{
			u32 time = (u32)System::Math::Round( t );
			vectora<editor_animation_state> cur_anim_states( g_allocator );
			m_active_model->tick( time );
			m_active_model->get_current_anim_states( cur_anim_states, time );
			vectora<editor_animation_state>::const_iterator it_b = cur_anim_states.begin( );
			for(; it_b != cur_anim_states.end( ); ++it_b )
			{
				animation_playback::animation_item^ item = item_by_user_data( it_b->user_data );
				if( m_editor->show_detailed_weights )
					item->weights_by_time->Add( time, it_b->weight );

				if( m_editor->show_detailed_scales )
					item->scales_by_time->Add( time, it_b->time_scale );
			}
		}
	}

	m_active_model->push_expression( expr, (u32)System::Math::Round( max_time ) );
	vectora<editor_animations_event> events_happened( g_allocator );
	m_active_model->calculate_animations_events( events_happened );

	vectora<editor_animations_event>::iterator it_events = events_happened.begin( );
	for(; it_events != events_happened.end( ); ++it_events )
	{
		animation_playback::animation_item^ item = item_by_user_data( it_events->user_data );
		R_ASSERT( item != nullptr );
		animation_playback::animation_event^ ev = gcnew animation_playback::animation_event( item );
		ev->position = (float)it_events->time_event_happened;
		switch( it_events->type )
		{
			case editor_animations_event::new_animation:
			case editor_animations_event::start_animation:	
			{
				ev->text = ( it_events->additional_data + 1 ).ToString( );
				break;
			}
			case editor_animations_event::finish_animation:
			{
				ev->text = "F";
				break;
			}
			default: NODEFAULT( );
		}

		item->events->Add( ev );
	}

	animation_items_list^ lst = gcnew animation_items_list( m_animation_items );
	for each( animation_playback::animation_item^ itm in lst )
	{
		if( itm->events->Count == 0 )
			m_animation_items->Remove( itm );
	}

	m_editor->time_panel->set_items	( m_animation_items );
	m_prev_target_ids->Clear		( );
	m_cur_target_ids->Clear			( );
	m_lexemes->clear				( );
}

bool viewer_document::can_create_expression_from_node( xray::animation_editor::animation_viewer_graph_node_base^ n )
{
	if( n == nullptr )
		return false;

	animation_viewer_graph_node_animation^ anim = dynamic_cast<animation_viewer_graph_node_animation^>( n );
	if( anim )
	{
		wpf_property_container^ anim_clip_container = anim->animation_clip;
		if( !anim_clip_container )
		{
			//m_error_message = "Animation clip is not selected!";
			return false;
		}

		animation_node_clip_instance^ anim_clip = safe_cast<animation_node_clip_instance^>( anim_clip_container->owner );
		if( anim_clip->animation( ) == NULL )
		{
			//m_error_message = "Animation clip is loading! Please try again later.";
			return false;
		}

		
		if( anim->sync_node && anim != m_target )
		{
			u32 sync_group = anim->synchronization_group( );
			if( sync_group == u32( -1 ) )
			{
				m_error_message = "Animation ["+anim->get_text()+"] is synchronized with animation ["+anim->sync_node->get_text()+"] but has no synchronization group id set!";
				return false;
			}

			if( sync_group != anim->sync_node->synchronization_group( ) )
			{
				m_error_message = "Animation ["+anim->get_text()+"] is synchronized with animation ["+anim->sync_node->get_text()+"] but has a different synchronization group id!";
				return false;
			}
		}

		if( anim->parent != nullptr )
		{
			List<String^>^ lst = gcnew List<String^>( );
			for each( animation_viewer_graph_node_base^ child in anim->parent->childs )
			{
				animation_viewer_graph_node_animation^ child_anim = dynamic_cast<animation_viewer_graph_node_animation^>( child );
				if( !child_anim )
					continue;

				if( child_anim->sync_node == nullptr && child_anim->synchronization_group( ) != -1 )
					lst->Add( child_anim->get_text( ) );
			}

			if( lst->Count > 1 )
			{
				m_error_message = "Animations: \n";
				for each( String^ name in lst )
					m_error_message = m_error_message + "[" + name + "]\n";

				m_error_message = m_error_message + "are in the same synchronization_group but are not synchronized!";
				return false;
			}
		}

		return true;
	}

	if( n->childs->Count != 0 )
	{
		for each( animation_viewer_graph_node_base^ child in n->childs )
		{
			if( !can_create_expression_from_node( child ) )
				return false;
		}

		return true;
	}

	return false;
}

expression viewer_document::create_expression_from_node( xray::animation_editor::animation_viewer_graph_node_base^ n, xray::mutable_buffer& buffer, bool create_anim_items )
{
	animation_viewer_graph_node_animation^ anim = dynamic_cast<animation_viewer_graph_node_animation^>( n );
	if( anim )
	{
		if( anim->sync_node && anim != m_target )
		{
			animation_lexeme_ptr lexeme	= create_synk_animation_lexeme( anim, buffer, create_anim_items );
			return expression			( *lexeme );
		}
		else
		{
			animation_lexeme_ptr lexeme = create_animation_lexeme( anim, buffer, create_anim_items );
			return expression( *lexeme );
		}
	}

	if( dynamic_cast<animation_viewer_graph_node_root^>( n ) && n->childs->Count != 0 )
		return create_expression_from_node( n->childs[0], buffer, create_anim_items );

	R_ASSERT( dynamic_cast<animation_viewer_graph_node_operator^>( n ) );
	vector<expression> expr_vec;
	for each( animation_viewer_graph_node_base^ node in n->childs )
	{
		animation_viewer_graph_link^ lnk					= animation_viewer_hypergraph::find_link_by_ids( n->get_parent( )->links_manager->visible_links( ), node->id, n->id );
		animation_viewer_graph_link_weighted^ l				= dynamic_cast<animation_viewer_graph_link_weighted^>( lnk );
		animation_node_interpolator^ weight_interpolator	= dynamic_cast<animation_node_interpolator^>( l->interpolator->owner );

		weight_lexeme W			( buffer, l->weight, *weight_interpolator->interpolator( ) );
		expression weight_expr	( W );
		expression anim_expr	( create_expression_from_node( node, buffer, create_anim_items ) );
		expr_vec.push_back		( anim_expr * weight_expr );
	}

	vector<expression>::iterator it = expr_vec.begin( );
	expression add_expr				= expression( *it );
	++it;
	for(; it != expr_vec.end( ); ++it )
		add_expr += *it;

	return add_expr;
}

animation_lexeme_ptr viewer_document::create_synk_animation_lexeme( animation_viewer_graph_node_animation^ anim, mutable_buffer& buffer, bool create_anim_items )
{
	lexemes_collection_type::iterator it		= m_lexemes->find( anim->id );

	if( it != m_lexemes->end( ) )
		return it->second;

	animation_lexeme_ptr synk_lexeme	= create_animation_lexeme( anim->sync_node, buffer, create_anim_items );
	animation_lexeme lexeme				( animation_lexeme_parameters( buffer, unmanaged_string( anim->name() ).c_str(), anim->clip( ), *synk_lexeme ) );
	animation_lexeme* buffer_lexeme		= lexeme.cloned_in_buffer( );
	buffer_lexeme->user_data					= anim->id;

	m_lexemes->insert( std::pair<u32, animation_lexeme*>( anim->id, buffer_lexeme ) );

	if( create_anim_items )
		create_animation_item( buffer_lexeme );

	return buffer_lexeme;
}

animation_lexeme_ptr viewer_document::create_animation_lexeme( xray::animation_editor::animation_viewer_graph_node_animation^ node, xray::mutable_buffer& buffer, bool create_anim_items )
{
	u32 user_data = node->id;// get_user_data( node->get_text( ) );
	m_cur_target_ids->Add( user_data );

	lexemes_collection_type::iterator it = m_lexemes->find( user_data );
	if( it != m_lexemes->end( ) )
		return it->second;

	animation_node_playing_instance^ playing_inst	= dynamic_cast<animation_node_playing_instance^>( node->playing_instance->owner );
	time_scale_instance^ ts_inst					= dynamic_cast<time_scale_instance^>			( playing_inst->time_scale->owner );
	animation_node_interpolator^ ts_interpolator	= dynamic_cast<animation_node_interpolator^>	( ts_inst->interpolator->owner );
	time_instance^ ti								= dynamic_cast<time_instance^>					( playing_inst->time_offset->owner );

	R_ASSERT( playing_inst && ts_inst && ts_interpolator && ti );
	R_ASSERT( node->intervals->Count > 0 );

	buffer_vector<animation::mixing::animation_interval> intervals( ALLOCA( node->intervals->Count* sizeof(animation::mixing::animation_interval)), node->intervals->Count );

	for( int i = 0; i < node->intervals->Count; ++i ) {
		animation_node_interval^ next_interval		= node->intervals[i];
		intervals.push_back							(
			animation::mixing::animation_interval(
				next_interval->parent->animation( ),
				next_interval->offset,
				next_interval->length
			)
		);
	}

	animation_lexeme lexeme(
		animation_lexeme_parameters(
			buffer,
			unmanaged_string( node->name() ).c_str(),
			&*intervals.begin(),
			&*intervals.end()
		)
		.time_scale_interpolator( *ts_interpolator->interpolator() )
		.time_scale( ts_inst->time_scale_value() )
		.synchronization_group_id( node->synchronization_group() )
		.start_cycle_animation_interval_id( node->cycle_from_interval_id != u32( -1 ) ? node->cycle_from_interval_id : 0 )
	);

	m_active_model->subscribe_footsteps( lexeme );

	animation_lexeme* buffer_lexeme		= lexeme.cloned_in_buffer( );
	buffer_lexeme->user_data					= user_data;

	m_lexemes->insert( std::pair<u32, animation_lexeme*>( user_data, buffer_lexeme ) );
	
	if( create_anim_items )
		create_animation_item( buffer_lexeme );

	return buffer_lexeme;
}

void viewer_document::active_model_matrix_modified( cli::array<System::String^>^ )
{
	float4x4 m					= m_editor->get_animation_editor()->transform_helper->m_object->get_ancor_transform();
	m_active_model->transform	= m;
	m_models_changed			= true;
}

void viewer_document::set_active_model( System::String^ name )
{
	m_editor->get_animation_editor( )->transform_helper->m_changed = true;
	if(m_editor->get_animation_editor( )->transform_helper->m_object != nullptr )
	{
		delete m_editor->get_animation_editor( )->transform_helper->m_object;
		m_editor->get_animation_editor( )->transform_helper->m_object = nullptr;
	}

	if( name == nullptr )
	{
		m_active_model = nullptr;
		return;
	}

	R_ASSERT( m_models->ContainsKey( name ) );
	m_active_model = m_models[name];
	m_editor->get_animation_editor( )->transform_helper->m_object = gcnew editor_base::transform_value_object( &m_active_model->transform, gcnew editor_base::value_modified( this, &viewer_document::active_model_matrix_modified ), true );
}

u32 viewer_document::get_user_data( System::String^ anim_name )
{
	if( m_assigned_ids->ContainsKey( anim_name ) )
	{
		for( int index = 0; index < m_assigned_ids[anim_name]->Count; ++index )
		{
			if( !m_prev_target_ids->Contains( m_assigned_ids[anim_name][index] ) &&
				!m_cur_target_ids->Contains( m_assigned_ids[anim_name][index] ) )
				return m_assigned_ids[anim_name][index];
		}
	}

	u32 i = 0;
	for each( KeyValuePair<String^, List<u32>^> p in m_assigned_ids )
	{
		for( int index = 0; index < p.Value->Count; ++index )
		{
			if( i <= p.Value[index] )
				i = p.Value[index] + 1;
		}
	}

	if( m_assigned_ids->ContainsKey( anim_name ) )
		m_assigned_ids[anim_name]->Add( i );
	else
	{
		List<u32>^ lst = gcnew List<u32>( );
		lst->Add( i );
		m_assigned_ids->Add( anim_name, lst );
	}

	return i;
}

void viewer_document::set_target_impl( )
{
	if( !can_create_expression_from_node( m_target ) )
		return set_target( nullptr );

	m_active_model->reset		( );
	float next_time				= 0.0f;
	mutable_buffer buffer		( ALLOCA( expression_alloca_buffer_size ), expression_alloca_buffer_size );
	expression expr				( create_expression_from_node( m_target, buffer, false ) );
	m_active_model->set_target	( expr, 0 );
	swap_target_ids				( );
	animation_viewer_graph_node_root^ prev = dynamic_cast<animation_viewer_graph_node_root^>( m_target );
	if( prev )
	{
		animation_viewer_graph_node_root^ next = prev->transition( );
		while( next )
		{
			if( !can_create_expression_from_node( next ) )
				return set_target( nullptr );

			animation_viewer_graph_link^ l			= animation_viewer_hypergraph::find_link_by_ids( m_hypergraph->links_manager->visible_links( ), prev->id, next->id );
			animation_viewer_graph_link_timed^ lnk	= safe_cast<animation_viewer_graph_link_timed^>( l );
			time_instance^ time						= safe_cast<time_instance^>( lnk->playing_time->owner );
			next_time += time->time_s * 1000.0f;
			if( next_time >= current_time )
				break;

			expression next_expr			( create_expression_from_node( next, buffer, false ) );
			m_active_model->set_target		( next_expr, (u32)System::Math::Round( next_time ) );
			swap_target_ids					( );
			prev = next;
			next = next->transition( );
		}
	}

	m_prev_target_ids->Clear	( );
	m_cur_target_ids->Clear		( );
}

void viewer_document::swap_target_ids	( )
{
	m_prev_target_ids->Clear( );
	for( int index = 0; index < m_cur_target_ids->Count; ++index )
		m_prev_target_ids->Add( m_cur_target_ids[index] );

	m_cur_target_ids->Clear		( );
	m_lexemes->clear			( );
}

} // namespace animation_editor
} // namespace xray