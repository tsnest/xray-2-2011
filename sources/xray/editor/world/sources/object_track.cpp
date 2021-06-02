////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_track.h"
#include "tool_misc.h"
#include "level_editor.h"
#include "editor_world.h"
#include "le_transform_control_helper.h"
#include "animation_curve_editor_panel.h"
#include "project.h"
#include "project_items.h"
#include <xray/editor/base/collision_object_types.h>
#include <xray/ui/world.h>

#pragma managed( push, off )
#	include <xray/animation/anim_track_common.h>
#	include <xray/animation/anim_track.h>
#	include <xray/render/facade/debug_renderer.h>
#	include <xray/collision/space_partitioning_tree.h>
#	include <xray/collision/geometry.h>
#	include <xray/collision/api.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

namespace xray{
namespace editor{

typedef xray::editor::wpf_controls::animation_curve_editor animation_curve_editor_control;
typedef xray::editor::wpf_controls::property_container container; 
typedef xray::editor::wpf_controls::float_curve float_curve;
typedef xray::editor::wpf_controls::float_curve_key float_curve_key;
typedef xray::editor::wpf_controls::curve_editor::visual_curve visual_curve;
typedef System::Windows::Point Point;
typedef System::Windows::Vector Vector;

using	xray::editor::wpf_controls::curve_editor::panel_curve_group;

inline float3 matrix_get_scale( math::float4x4 const& m);


////////////////		I N I T I A L I Z E			////////////////

object_track::object_track					( tool_misc^ tool, xray::render::scene_ptr const& scene ) :
	super				( tool ),
	m_scene				( NEW (render::scene_ptr) (scene) ),
	m_tool_misc			( tool )
{
	m_keys_collision	= NEW(vectora<collision_object_track_key*>)(g_allocator);
	m_track				= NEW(xray::animation::anm_track)(g_allocator, 10);
	m_track->initialize_empty( true, true, true );
	m_selected_key		= -1;
	m_selected_tangent	= -1;
	m_test_timer		= NEW( timing::timer );
	image_index			= xray::editor_base::camera_track;
	m_test_time_scale	= 1;
	m_animation_curves	= gcnew List<float_curve^>( 7 );
	m_binders			= gcnew List<curve_to_channel_binder^>( 7 );
	show_frames			= false;

	R_ASSERT( keys_count( ) == 0 );

	m_text					= m_tool_misc->get_level_editor( )->get_editor_world( ).get_ui_world( )->create_text( );
	m_text->set_font		( ui::fnt_arial );
	m_text->set_text_mode	( ui::tm_default );

	m_camera_model		= NEW(render::static_model_ptr)( );
}

object_track::~object_track					( )
{
	if ( m_is_camera_model_in_scene )
	{
		owner_tool( )->get_level_editor( )->get_editor_renderer( ).scene().remove_model( *m_scene, (*m_camera_model)->m_render_model );
		m_is_camera_model_in_scene = false;
	}

	if( m_selected_animation_track == this )
		owner_tool( )->get_level_editor( )->remove_onframe_delegate( gcnew execute_delegate( this, &object_track::tick) );

	DELETE	( m_track );
	DELETE	( m_keys_collision );
	DELETE  ( m_camera_model );
	DELETE	( m_test_timer );
	
	DELETE  ( m_scene );
	m_tool_misc->get_level_editor( )->get_editor_world( ).get_ui_world( )->destroy_window( m_text->w( ) );

	animation_curve_editor_control^ animation_curve_editor = owner_tool( )->get_level_editor( )->get_animation_curve_editor_panel( )->editor;
	if( animation_curve_editor->get_viewed_object( ) == this )
		animation_curve_editor->reset( );
}

void object_track::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( &debug::g_mt_allocator );
}

////////////////		P R O P E R T I E S			////////////////

void			object_track::selected_key::set				( int idx )
{
	math::clamp<int>( idx, -1, keys_count() - 1 );
	m_selected_key	= idx;
	owner_tool()->get_level_editor()->set_transform_object( gcnew track_object_key_transform_data(this, m_selected_key) );
	if( m_temp_container )
		m_temp_container->update_properties( );

	destroy_key_tangents_collision( );
	
	if( m_selected_key != -1 )
		init_key_tangents_collision( );
}

////////////////		P R I V A T E   M E T H O D S			////////////////

void			object_track::selected_to_camera			( )
{
	if( m_selected_key != -1 )
	{
		xray::editor_base::scene_view_panel^ view_window = owner_tool( )->get_level_editor( )->view_window( );

		math::float4x4 camera_transform		= view_window->get_inverted_view_matrix( );

		math::float4x4 m	= view_window->get_inverted_view_matrix( );
		m					= m * math::invert4x3( get_transform( ) );
		set_key_transform	( m_selected_key, m );
	}
}

void			object_track::camera_to_selected			( )
{
	if( m_selected_key != -1 )
	{
		math::float4x4		camera_transform = get_key_transform( m_selected_key ) * get_transform( );
		owner_tool( )->get_level_editor( )->view_window()->set_flying_camera_destination( math::invert4x3( camera_transform ) );
	}
}

void			object_track::animation_curves_key_addedd	( int key_index )
{
	XRAY_UNREFERENCED_PARAMETER( key_index );
	update_collisions( );
}

void			object_track::animation_curves_key_changed	( int key_index )
{
	XRAY_UNREFERENCED_PARAMETER( key_index );
	update_collisions( );
}

void			object_track::animation_curves_key_removed	( int key_index )
{
	XRAY_UNREFERENCED_PARAMETER( key_index );
	update_collisions( );
}

float			object_track::min_track_time				( )
{
	return 0.0f;
}

float			object_track::max_track_time				( )
{
	return m_track->max_time( );
}

void			object_track::editor_track_time_changed		( )
{
	animation_curve_editor_control^ animation_curve_editor = owner_tool( )->get_level_editor( )->get_animation_curve_editor_panel( )->editor;
	m_test_current_time = (Single)animation_curve_editor->animation_time;

	if( animation_curve_editor->is_camera_view )
	{
		float4x4 key_transform;
		m_track->evaluate				( key_transform, m_test_current_time, use_time );
		key_transform					= key_transform * get_transform( );

		owner_tool( )->get_level_editor( )->view_window( )->set_view_matrix( math::invert4x3 ( key_transform ) );
	}
}
void			object_track::curve_editor_selection_changed( int index )
{
	m_selected_tangent		= 0;
	selected_key			= index;
}
void			object_track::tick							( )
{
	if( m_selected_animation_track == this )
	{
		owner_tool( )->get_level_editor( )->add_onframe_delegate( gcnew execute_delegate( this, &object_track::tick), 1 );
		owner_tool( )->get_level_editor( )->get_animation_curve_editor_panel( )->editor->tick( m_test_timer->get_elapsed_sec( ) );
		m_test_timer->start		( );

		if( owner_tool( )->get_level_editor( )->camera_view_panel( )->Visible )
		{
			float4x4 key_transform;
			m_track->evaluate				( key_transform, m_test_current_time, use_time );
			key_transform					= key_transform * get_transform( );

			owner_tool( )->get_level_editor( )->camera_view_panel( )->set_view_matrix( math::invert4x3 ( key_transform ) );
		}

		if( *m_camera_model != NULL && m_is_camera_model_in_scene )
		{
			math::float4x4 key_transform;
			m_track->evaluate				( key_transform, m_test_current_time, use_time );
			key_transform					= key_transform * get_transform( );
			key_transform.c					= key_transform.c + key_transform.k * -0.2f;
			
			owner_tool( )->get_level_editor( )->get_editor_renderer( ).scene( ).update_model	( *m_scene, (*m_camera_model)->m_render_model, key_transform );
		}
	}
}
void			object_track::tick_animation_curve_editor	( Single elapsed_time )
{
	animation_curve_editor_control^ animation_curve_editor = owner_tool( )->get_level_editor( )->get_animation_curve_editor_panel( )->editor;
	animation_curve_editor->tick( elapsed_time );
}
u32				object_track::keys_count					( )
{
	return m_track->m_channels[0]->keyList.size();
}

math::float3	object_track::key_position					( u32 idx )
{
	return math::float3(
		m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].value,
		m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].value,
		m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].value
		);
}

math::float3	object_track::key_angles					( u32 idx )
{
	return math::float3(
		m_track->m_channels[xray::animation::channel_rotation_x]->keyList[idx].value,
		m_track->m_channels[xray::animation::channel_rotation_y]->keyList[idx].value,
		m_track->m_channels[xray::animation::channel_rotation_z]->keyList[idx].value
		);
}

float			object_track::key_time						( u32 idx )
{
	return m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].time;
}

curve_key_type	object_track::key_type						( xray::animation::enum_channel_id channel, u32 idx )
{
	return (curve_key_type)m_track->m_channels[channel]->keyList[idx].type;
}

math::float2	object_track::channel_key_tangent			( xray::animation::enum_channel_id channel, u32 idx, s32 tangent_idx )
{
	if( tangent_idx == -1 )
	{
		return math::float2( m_track->m_channels[channel]->keyList[idx].inTanX, m_track->m_channels[channel]->keyList[idx].inTanY );
	}
	else
	{
		return math::float2( m_track->m_channels[channel]->keyList[idx].outTanX, m_track->m_channels[channel]->keyList[idx].outTanY );
	}
}
void			object_track::fill_animation_curve_editor	( )
{
	if( m_selected_animation_track == this )
		return;

	if( m_selected_animation_track != nullptr )
		m_selected_animation_track->clear_animation_curve_editor( );

	m_selected_animation_track = this;

	if( !m_is_camera_model_in_scene )
	{
		query_result_delegate* q	= NEW(query_result_delegate)( gcnew query_result_delegate::Delegate( this, &object_track::on_camera_model_ready ), g_allocator );
		resources::query_resource	( "editor/camera", resources::static_model_instance_class, boost::bind(&query_result_delegate::callback, q, _1), g_allocator );
	}

	animation_curve_editor_control^ animation_curve_editor = get_animation_curve_editor( );
	owner_tool( )->get_level_editor( )->get_animation_curve_editor_panel( )->Text = "curves: " + get_name( );
	
	int count = keys_count( );

	if( count == 0 )
	{
		insert_key		( 0, math::float3( 0, 0, 0 ), math::float3( 0, 0, 0 ) );
		xray::animation::EtCurve* time_channel		= m_track->get_channel( (xray::animation::enum_channel_id)xray::animation::anm_track::m_time_channel_id, false );
		time_channel->insert( 0, 0 );
		count = 1;
	}
	
	float_curve^ x_curve	= gcnew float_curve( );
	x_curve->color			= System::Windows::Media::Colors::Red;
	float_curve^ y_curve	= gcnew float_curve( );
	y_curve->color			= System::Windows::Media::Colors::Green;
	float_curve^ z_curve	= gcnew float_curve( );
	z_curve->color			= System::Windows::Media::Colors::Blue;

	float_curve^ rx_curve	= gcnew float_curve( );
	rx_curve->color			= System::Windows::Media::Colors::Red;
	float_curve^ ry_curve	= gcnew float_curve( );
	ry_curve->color			= System::Windows::Media::Colors::Green;
	float_curve^ rz_curve	= gcnew float_curve( );
	rz_curve->color			= System::Windows::Media::Colors::Blue;

	float_curve^ time_curve	= gcnew float_curve( );
	time_curve->color		= System::Windows::Media::Colors::Khaki;

	m_animation_curves->Clear( );
	m_animation_curves->Add	( x_curve );
	m_animation_curves->Add	( y_curve );
	m_animation_curves->Add	( z_curve );
	m_animation_curves->Add	( rx_curve );
	m_animation_curves->Add	( ry_curve );
	m_animation_curves->Add	( rz_curve );
	m_animation_curves->Add	( time_curve );

	xray::animation::EtCurve* time		= m_track->get_channel( (xray::animation::enum_channel_id)xray::animation::anm_track::m_time_channel_id, false );
	u32 time_count = time->keyList.size( );

	math::float2	in_tangent;
	math::float2	out_tangent;

	for( u32 i = 0; i < time_count; ++i )
	{
		in_tangent					= channel_key_tangent( (xray::animation::enum_channel_id)xray::animation::anm_track::m_time_channel_id, i, -1 );
		out_tangent					= channel_key_tangent( (xray::animation::enum_channel_id)xray::animation::anm_track::m_time_channel_id, i, 1 );

		Point pt					= Point( time->keyList[i].time, time->keyList[i].value );

		time_curve->keys->Add( gcnew float_curve_key( 
			pt,
			(curve_key_type)time->keyList[i].type ,
			pt + (Vector)Point( time->keyList[i].inTanX, time->keyList[i].inTanY ),
			pt + (Vector)Point( time->keyList[i].outTanX, time->keyList[i].outTanY )
		) );
	}

	for( int i = 0; i < count; ++i )
	{
		
		float			time		= key_time( i );
		math::float3	key_pos		= key_position( i );
		math::float3	key_ang		= key_angles( i );

		float			x_value		= key_pos.x;
		float			y_value		= key_pos.y;
		float			z_value		= key_pos.z;

		float			rx_value	= math::rad2deg( key_ang.x );
		float			ry_value	= math::rad2deg( key_ang.y );
		float			rz_value	= math::rad2deg( key_ang.z );

		in_tangent					= channel_key_tangent( xray::animation::channel_translation_x, i, -1 );
		out_tangent					= channel_key_tangent( xray::animation::channel_translation_x, i, 1 );

		x_curve->keys->Add			(	gcnew float_curve_key( Point( time, x_value ),
										key_type( xray::animation::channel_translation_x, i ),
										Point( time, x_value ) + (Vector)Point( in_tangent.x, in_tangent.y ),
										Point( time, x_value ) + (Vector)Point( out_tangent.x, out_tangent.y ) ) );

		in_tangent					= channel_key_tangent( xray::animation::channel_translation_y, i, -1 );
		out_tangent					= channel_key_tangent( xray::animation::channel_translation_y, i, 1 );

		y_curve->keys->Add			(	gcnew float_curve_key( Point( time, y_value ),
										key_type( xray::animation::channel_translation_y, i ),
										Point( time, y_value ) + (Vector)Point( in_tangent.x, in_tangent.y ),
										Point( time, y_value ) + (Vector)Point( out_tangent.x, out_tangent.y ) ) );

		in_tangent					= channel_key_tangent( xray::animation::channel_translation_z, i, -1 );
		out_tangent					= channel_key_tangent( xray::animation::channel_translation_z, i, 1 );

		z_curve->keys->Add			(	gcnew float_curve_key( Point( time, z_value ),
										key_type( xray::animation::channel_translation_z, i ),
										Point( time, z_value ) + (Vector)Point( in_tangent.x, in_tangent.y ),
										Point( time, z_value ) + (Vector)Point( out_tangent.x, out_tangent.y ) ) );
		
		
		in_tangent					= channel_key_tangent( xray::animation::channel_rotation_x, i, -1 );
		out_tangent					= channel_key_tangent( xray::animation::channel_rotation_x, i, 1 );

		rx_curve->keys->Add			(	gcnew float_curve_key( Point( time, rx_value ),
										key_type( xray::animation::channel_rotation_x, i ),
										Point( time, rx_value ) + (Vector)Point( in_tangent.x, in_tangent.y ),
										Point( time, rx_value ) + (Vector)Point( out_tangent.x, out_tangent.y ) ) );

		in_tangent					= channel_key_tangent( xray::animation::channel_rotation_y, i, -1 );
		out_tangent					= channel_key_tangent( xray::animation::channel_rotation_y, i, 1 );

		ry_curve->keys->Add			(	gcnew float_curve_key( Point( time, ry_value ),
										key_type( xray::animation::channel_rotation_y, i ),
										Point( time, ry_value ) + (Vector)Point( in_tangent.x, in_tangent.y ),
										Point( time, ry_value ) + (Vector)Point( out_tangent.x, out_tangent.y ) ) );

		in_tangent					= channel_key_tangent( xray::animation::channel_rotation_z, i, -1 );
		out_tangent					= channel_key_tangent( xray::animation::channel_rotation_z, i, 1 );

		rz_curve->keys->Add			(	gcnew float_curve_key( Point( time, rz_value ), 
										key_type( xray::animation::channel_rotation_z, i ),
										Point( time, rz_value ) + (Vector)Point( in_tangent.x, in_tangent.y ),
										Point( time, rz_value ) + (Vector)Point( out_tangent.x, out_tangent.y ) ) );

	}

	panel_curve_group^ object_group	= animation_curve_editor->add_group( get_name( ) );
	panel_curve_group^ position_group = object_group->add_group( "Position" );
	panel_curve_group^ rotation_group = object_group->add_group( "Rotation" );

	position_group->add_curve( "position x", "X", x_curve );
	position_group->add_curve( "position y", "Y", y_curve );
	position_group->add_curve( "position z", "Z", z_curve );

	rotation_group->add_curve( "rotation x", "X", rx_curve );
	rotation_group->add_curve( "rotation y", "Y", ry_curve );
	rotation_group->add_curve( "rotation z", "Z", rz_curve );

	curves_link^ link = object_group->link_group_curves( );

	m_binders->Clear( );
	m_binders->Add( gcnew curve_to_channel_binder( this, link ) );
	m_binders->Add( gcnew curve_to_channel_binder( this, object_group->add_curve( "time", "Time", time_curve ), 9 ) );

	animation_curve_editor->get_camera_transform			= gcnew animation_curve_editor_control::transform( this, &object_track::get_camera_transform );
	animation_curve_editor->time_changed					+= gcnew Action( this, &object_track::editor_track_time_changed );
	animation_curve_editor->selected_to_camera_clicked		+= gcnew Action( this, &object_track::selected_to_camera );
	animation_curve_editor->camera_to_selected_clicked		+= gcnew Action( this, &object_track::camera_to_selected );
	animation_curve_editor->selection_changed				+= gcnew Action<Int32>( this, &object_track::curve_editor_selection_changed );
	
	animation_curve_editor->focus_on_curves( );
}
void			object_track::clear_animation_curve_editor	( )
{
	animation_curve_editor_control^ animation_curve_editor = owner_tool( )->get_level_editor( )->get_animation_curve_editor_panel( )->editor;
	animation_curve_editor->get_camera_transform = nullptr;
	animation_curve_editor->reset( );
	animation_curve_editor->time_changed -= gcnew Action( this, &object_track::editor_track_time_changed );
	m_binders->Clear( );
	m_animation_curves->Clear( );

	if( m_is_camera_model_in_scene )
	{
		owner_tool( )->get_level_editor( )->get_editor_renderer( ).scene().remove_model( *m_scene, (*m_camera_model)->m_render_model );
		m_is_camera_model_in_scene	= false;
		*m_camera_model				= NULL;
	}
}
void			object_track::init_collision				( )
{
	ASSERT( !m_collision->initialized( ) );
	float3 extents				( 0.3f,0.3f,0.3f );

	collision::geometry_instance* geom	= &*collision::new_box_geometry_instance( &debug::g_mt_allocator, create_scale( extents ) );
	m_collision->create_from_geometry( true, this, geom, xray::editor_base::collision_object_type_dynamic | xray::editor_base::collision_object_type_touch );
	m_collision->insert			( m_transform );
}

void			object_track::init_keys_collision			( )
{
	int count					= keys_count( );
	for(int i=0; i<count; ++i)
	{
		float3 p			= key_position( i );
		math::float4x4 m	= create_translation( p ) * get_transform( );

		collision::geometry_instance* geom	= &*collision::new_sphere_geometry_instance( g_allocator, m, 1 );
		collision_object_track_key* k = NEW(collision_object_track_key)(geom, this, i);
		m_keys_collision->push_back	( k );
		
		insert_collision_in_tree( k, m );
	}
}

void			object_track::init_key_tangents_collision	( )
{
	if( m_selected_key > 0 && m_input_tangent_collision == NULL )
	{
		float3 const tangent_pos = get_key_tangent_position( m_selected_key, -1 );
		float4x4 const m = create_translation( tangent_pos * get_transform( ) );

		collision::geometry_instance* geom	= &*collision::new_sphere_geometry_instance( g_allocator, m, 1 );
		m_input_tangent_collision			= NEW(collision_object_track_key_tangent)( geom, this, m_selected_key, -1 );
		
		insert_collision_in_tree( m_input_tangent_collision, m );
	}
	if( m_selected_key < (int)keys_count( ) - 1 && m_output_tangent_collision == NULL )
	{
		float4x4 m = create_translation( get_key_tangent_position( m_selected_key, 1 ) ) * get_transform( );
		collision::geometry_instance* geom	= &*collision::new_sphere_geometry_instance( g_allocator, m, 1 );
		m_output_tangent_collision	= NEW(collision_object_track_key_tangent)( geom, this, m_selected_key, 1 );
		insert_collision_in_tree( m_output_tangent_collision, m );
	}
}

void			object_track::update_collisions				( )
{
	float3 p;
	float4x4 m;

	int key_count			= keys_count( );
	int collision_count		= m_keys_collision->size( );

	while( key_count < (int)m_keys_collision->size( ) )
	{
		collision_object_track_key* k	= (*m_keys_collision)[collision_count - 1];
		m_keys_collision->pop_back( );
		m_collision->m_collision_tree->erase( k );
	}

	for ( int i = 0; i < key_count; ++i )
	{
		p								= key_position( i );
		m								= create_translation( p ) * get_transform( );

		if( i >= collision_count )
		{
			collision::geometry_instance* geom = &*collision::new_sphere_geometry_instance( g_allocator, m, 1 );
			collision_object_track_key* k = NEW(collision_object_track_key)( geom, this, i );
			m_keys_collision->push_back	( k );
			insert_collision_in_tree( k, m );
		}
		else
		{
			collision_object_track_key* k	= (*m_keys_collision)[i];
			m_collision->m_collision_tree->move( k, m );
			k->set_key_index( i );
		}
	}

	if( m_input_tangent_collision != NULL )
	{
		p								= get_key_tangent_position( m_selected_key, -1 );
		m								= create_translation( p ) * get_transform( );
		m_collision->m_collision_tree->move( m_input_tangent_collision, m );
	}

	if( m_output_tangent_collision != NULL )
	{
		p								= get_key_tangent_position( m_selected_key, 1 );
		m								= create_translation( p ) * get_transform( );
		m_collision->m_collision_tree->move( m_output_tangent_collision, m );
	}
}
void object_track::destroy_keys_collision( )
{
	vectora<collision_object_track_key*>::iterator it	= m_keys_collision->begin();
	vectora<collision_object_track_key*>::iterator it_e = m_keys_collision->end();
	for ( ; it != it_e; ++it )
	{
		m_collision->m_collision_tree->erase(*it);
		(*it)->get_geometry_instance()->destroy( g_allocator );
		DELETE( *it );
	}
	
	m_keys_collision->clear();
}

void object_track::destroy_key_tangents_collision( )
{
	if ( m_output_tangent_collision != NULL )
	{
		m_collision->m_collision_tree->erase( m_output_tangent_collision );
		m_output_tangent_collision->get_geometry_instance()->destroy( g_allocator );
		DELETE( m_output_tangent_collision );
		m_output_tangent_collision	= NULL;
	}
	if ( m_input_tangent_collision != NULL )
	{
		m_collision->m_collision_tree->erase( m_input_tangent_collision );
		m_input_tangent_collision->get_geometry_instance()->destroy( g_allocator );
		DELETE( m_input_tangent_collision );
		m_input_tangent_collision	= NULL;
	}
}

void object_track::insert_collision_in_tree		( collision::object* collision, float4x4 const& transform )
{
	m_collision->m_collision_tree->insert( collision, transform );
}

void			object_track::on_camera_model_ready			( resources::queries_result& data )
{
	if ( data.is_successful( ) )
	{
		*m_camera_model	= static_cast_resource_ptr<render::static_model_ptr>( data[0].get_unmanaged_resource( ) );
		
		if( m_is_selected )
		{
			owner_tool( )->get_level_editor( )->get_editor_renderer( ).scene().add_model(
				*m_scene,
				( *m_camera_model )->m_render_model,
				math::float4x4( ).identity( )
			);
			m_is_camera_model_in_scene = true;
		}
	}
}
////////////////		P U B L I C   M E T H O D S			////////////////

animation_curve_editor_control^ object_track::get_animation_curve_editor( )
{
	return owner_tool( )->get_level_editor( )->get_animation_curve_editor_panel( )->editor;
}
void			object_track::load_contents					( )
{
	if( !m_collision->initialized( ) )
		init_collision( );

	if( m_keys_collision->size( ) == 0 )
		init_keys_collision( );
}

void			object_track::unload_contents				( bool )
{
	destroy_collision	( );
	destroy_keys_collision( );
}

void			object_track::load_props					( configs::lua_config_value const& t )
{
	super::load_props			( t );

	m_track->load		( t["path"] );
}

void			object_track::save							( configs::lua_config_value t )
{
	super::save				( t );
	t["game_object_type"]	= "track";
//	t["registry_name"]		= unmanaged_string( m_project_item->get_full_name()).c_str();

	m_track->save			( t["path"] );
}

void			object_track::set_transform					( float4x4 const& transform )
{
	super::set_transform( transform );
	if(!m_keys_collision->empty())
	{
		vectora<collision_object_track_key*>::iterator it	= m_keys_collision->begin();
		vectora<collision_object_track_key*>::iterator it_e = m_keys_collision->end();
		for(int i=0; it!=it_e; ++it,++i)
		{
			collision_object_track_key* k = *it;
			float3 p			= key_position( i );
			R_ASSERT( math::valid( p ) );
			math::float4x4 m	= create_translation(p) * get_transform();
			m_collision->m_collision_tree->move( k, m );
		}
	}
}

void			object_track::insert_key					( float time, math::float3 const& pos, math::float3 const& rot )
{
	m_track->insert					( time, pos, rot, float3( 1, 1, 1 ) );
	m_track->calc_smooth_tangents	( );
	
	destroy_keys_collision			( );
	init_keys_collision				( );
}

void			object_track::remove_key					( u32 idx )
{
	m_track->remove					( idx );
	m_track->calc_smooth_tangents	( );

	destroy_keys_collision			( );
	init_keys_collision				( );
}

math::float4x4	object_track::get_key_transform				( u32 idx )
{
	math::float4x4 result	= create_rotation( key_angles(idx) ) * create_translation( key_position(idx) );
	return					result;
}

void			object_track::set_key_transform				( u32 idx, math::float4x4 const& t )
{
	math::float3 pos = t.c.xyz();
	math::float3 rot = t.get_angles_xyz();

	m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].value		= pos.x;
	m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].value		= pos.y;
	m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].value		= pos.z;
	
	math::float4x4 mx = t;

	double xPitch;
	double zRoll;
	double yYaw;

	xPitch				= Math::Asin( -mx.e21 );
	double threshold	= math::epsilon_3;
	double test			= Math::Cos( xPitch );

	if( test > threshold )
	{
		zRoll		= Math::Atan2( mx.e01, mx.e11 );
		yYaw		= Math::Atan2( mx.e20, mx.e22 );
	}
	else
	{
		zRoll		= Math::Atan2( -mx.e10, mx.e00 );
		yYaw		= 0.0;
	}

	m_track->m_channels[xray::animation::channel_rotation_x]->keyList[idx].value		= (float)-xPitch;
	m_track->m_channels[xray::animation::channel_rotation_y]->keyList[idx].value		= (float)-yYaw;
	m_track->m_channels[xray::animation::channel_rotation_z]->keyList[idx].value		= (float)zRoll;

	m_binders[0]->update_key			( idx );
	m_binders[0]->raise_key_changed		( idx );
}

math::float3	object_track::get_key_tangent_position		( int idx, int tangent_idx )
{
	if( tangent_idx == -1 )
	{
		return math::float3(
			m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].value + m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].inTanY,
			m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].value + m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].inTanY,
			m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].value + m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].inTanY
		);
	}
	else
	{
		return math::float3(
			m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].value + m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].outTanY,
			m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].value + m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].outTanY,
			m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].value + m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].outTanY
		);
	}
}

static void		fix_key_type								( xray::animation::EtKey &key )
{
	if( key.type == (int)wpf_controls::float_curve_key_type::locked_auto ||
		key.type == (int)wpf_controls::float_curve_key_type::locked_manual )
		key.type = (int)wpf_controls::float_curve_key_type::locked_manual;
	else
		key.type = (int)wpf_controls::float_curve_key_type::breaked;
}
void			object_track::set_key_tangent_position		( int idx, int tangent_idx, math::float3 position )
{
	if( tangent_idx == -1 )
	{
		m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].inTanY = position.x - m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].value;
		m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].inTanY = position.y - m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].value;
		m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].inTanY = position.z - m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].value;
	}
	else
	{
		m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].outTanY = position.x - m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx].value;
		m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].outTanY = position.y - m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx].value;
		m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].outTanY = position.z - m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx].value;
	}

	fix_key_type( m_track->m_channels[xray::animation::channel_translation_x]->keyList[idx] );
	fix_key_type( m_track->m_channels[xray::animation::channel_translation_y]->keyList[idx] );
	fix_key_type( m_track->m_channels[xray::animation::channel_translation_z]->keyList[idx] );

	m_binders[0]->update_key			( idx );
	m_binders[0]->raise_key_changed		( idx );
	get_animation_curve_editor( )->update_chacked_key_type_buttons( );
}

void			object_track::get_camera_transform			( Vector3D% position, Vector3D% rotation, Vector3D% )
{
	xray::editor_base::scene_view_panel^ view_window = owner_tool( )->get_level_editor( )->view_window( );

	math::float4x4 inverterd_view	= view_window->get_inverted_view_matrix( );
	math::float4x4 transform		= get_transform( );
	math::float4x4 relative_camera	= inverterd_view * math::invert4x3( transform );
	
	math::float3 pos = relative_camera.c.xyz( );

	double xPitch;
	double zRoll;
	double yYaw;

	math::float4x4 mx = relative_camera;
	
	xPitch				= Math::Asin( -mx.e21 );
	double threshold	= math::epsilon_3;
	double test			= Math::Cos( xPitch );

	if( test > threshold )
	{
		zRoll		= Math::Atan2( mx.e01, mx.e11 );
		yYaw		= Math::Atan2( mx.e20, mx.e22 );
	}
	else
	{
		zRoll		= Math::Atan2( -mx.e10, mx.e00 );
		yYaw		= 0.0;
	}
	
	position.X = pos.x;
	position.Y = pos.y;
	position.Z = pos.z;

	rotation.X = -xPitch;//rot.x;
	rotation.Y = -yYaw;//rot.y;
	rotation.Z = zRoll;//rot.z;
}
void			object_track::render						( )
{
	super::render			( );

	get_debug_renderer( ).draw_origin		( *m_scene, get_transform( ), 2.0f );

	int count				= keys_count( );
	if( count != 0 )
	{
		math::float3	aabb_size			= math::float3( 0.5f, 0.5f, 0.5f );
		math::color		color				( 255, 255, 255 );
		math::color		color_selected		( 255, 0, 0 );
		math::color		color_frame			( 0, 128, 255 );

		for( int i = 0; i < count; ++i )
		{
			bool		is_current_key		= ( i == selected_key );
			float4x4	key_transform		= get_key_transform( i ) * get_transform( );
			float3		key_pos				= key_transform.c.xyz( );
			
			if( is_current_key )
			{
				if( m_selected_tangent == 0 )
					get_editor_renderer( ).draw_3D_screen_point( *m_scene, key_pos, c_key_size, color_selected, false );
				else
					get_editor_renderer( ).draw_3D_screen_point( *m_scene, key_pos, c_key_size, color, false );

				math::float3 dir	= key_transform.k.xyz( );
				math::float3 p2		= key_pos + dir * 10.0f;

				get_debug_renderer( ).draw_line( *m_scene, key_pos, p2, color_selected );

				//in tangent
				if( i > 0 )
				{
					float3	in_tangent_position = get_transform().transform_position( get_key_tangent_position( i, -1 ) );
					get_debug_renderer( ).draw_line( *m_scene, key_pos, in_tangent_position, color );
					if( m_selected_tangent == -1)
						get_editor_renderer( ).draw_3D_screen_point( *m_scene, in_tangent_position, c_tangent_size, color_selected , false );
					else
						get_editor_renderer( ).draw_3D_screen_point( *m_scene, in_tangent_position, c_tangent_size, color , false );
				}

				//out tangent
				if( i < count - 1 )
				{
					float3	out_tangent_position = get_transform().transform_position( get_key_tangent_position( i, 1 ) );
					get_debug_renderer( ).draw_line( *m_scene, key_pos, out_tangent_position, color );
					if( m_selected_tangent == 1 )
						get_editor_renderer( ).draw_3D_screen_point( *m_scene, out_tangent_position, c_tangent_size, color_selected , false );
					else
						get_editor_renderer( ).draw_3D_screen_point( *m_scene, out_tangent_position, c_tangent_size, color, false );
				}
			}
			else
				get_editor_renderer( ).draw_3D_screen_point( *m_scene, key_pos, c_key_size, color, false );
		}

		if( count > 1 )
		{
			float min_time = m_track->min_time( );
			float max_time = m_track->max_time( );
			
			math::float4x4 key_transform;

			render::debug_vertices_type		vertices( g_allocator );
			render::debug_indices_type		indices( g_allocator );

			render::vertex_colored		vertex;
			bool bsel					= m_project_item->get_selected( );
			vertex.color				= ( bsel ) ? math::color( 255, 255, 0 ) : math::color( 0, 200, 0 );

			m_track->evaluate			( key_transform, min_time, use_time );
			key_transform				= key_transform * get_transform( );

			vertex.position				= key_transform.c.xyz( );
			vertices.push_back			( vertex );

			u16 iidx = 0;

			for ( int i = 0; i < count - 1; ++i )
			{
				float left_key_time		= m_track->m_channels[0]->keyList[i].time;
				float one_tenth			= ( m_track->m_channels[0]->keyList[i + 1].time - left_key_time ) / 10;

				for( int j = 1; j < 11; j++ )
				{
					m_track->evaluate			( key_transform, left_key_time + one_tenth * j , use_time );
					key_transform				= key_transform * get_transform( ) ;
					vertex.position				= key_transform.c.xyz( );

					vertices.push_back			( vertex );
					++iidx;

					indices.push_back			( iidx - 1 );
					indices.push_back			( iidx );
				}
			}

			get_debug_renderer( ).draw_lines( *m_scene, vertices, indices );

			int i		= 0;
			if( show_frames && m_is_selected )
				for( float time = 0; time <= max_time; ++i, time = i/30 + ((float)(i%30))/30 )
				{
					float clamped_time			= math::clamp_r( time, 0.0f, max_time );
					m_track->evaluate			( key_transform, clamped_time, use_time );
					key_transform				= key_transform * get_transform( );
					
					get_debug_renderer( ).draw_cross( *m_scene, key_transform.c.xyz( ), aabb_size.x / 2, color_frame );
				}
			
			if( m_is_selected && show_frame_numbers )
			{

				xray::editor_base::scene_view_panel^ v          = owner_tool()->get_level_editor()->view_window();
				System::Drawing::Size   screen_size				= v->get_view_size();
				System::Drawing::Size   half_screen_size        = screen_size;
				half_screen_size.Width                          = half_screen_size.Width/2;
				half_screen_size.Height                         = half_screen_size.Height/2;
				
				math::float4x4 view;
				view.try_invert                         ( v->get_inverted_view_matrix() );
				math::float4x4 proj                     = v->get_projection_matrix();
				math::float4x4 view_projection			= math::mul4x4( view, proj );

				float last_on_track_position = 0;
				float current_on_track_position = 0;

				xray::ui::world* ui_world = m_tool_misc->get_level_editor( )->get_editor_world( ).get_ui_world( );

				int i = 0;
				for( float time = 0; time <= max_time; ++i, time = i / 30 + ( (float)( i%30 ) ) / 30 )
				{
					last_on_track_position = current_on_track_position;

					float clamped_time		= math::clamp_r( time, 0.0f, max_time );
					m_track->evaluate			( key_transform, clamped_time, use_time, current_on_track_position );

					math::float3 position		= ( key_transform * get_transform( ) ).c.xyz( ); 
					math::float3 pos			= view_projection.transform( position );

					if( pos.z < 0 || pos.z > 1 )
						continue;

					pos.x   *= half_screen_size.Width;
					pos.x   += half_screen_size.Width;
			        
					pos.y   *= -1;
					pos.y	*= half_screen_size.Height;
					pos.y	+= half_screen_size.Height;

					m_text->set_text			( unmanaged_string( i.ToString( ) ).c_str( ) );
					math::float2 size			= m_text->measure_string( );
					m_text->w( )->set_size		( size );
					m_text->w( )->set_position	( float2( pos.x - size.x / 2 , pos.y - 17 ) );
					m_text->w( )->set_visible	( true );
					m_text->set_color			( 0xff00ffff );
					m_text->w( )->tick			( );
					m_text->w( )->draw			( ui_world->get_renderer( ), ui_world->get_scene_view() );
				}
			}
			
		}// count>1
	}// count!=0
}

aabb			object_track::get_aabb						( )
{
	if( selected_key != -1 )
		return ( *m_keys_collision )[selected_key]->get_aabb( );
	
	return super::get_aabb( );
}
container^		object_track::get_property_container		( )
{
	wpf_controls::property_container^ cont = super::get_property_container( );
	m_temp_container		= cont;

	cont->properties->remove( "general/scale" );
	cont->properties->remove( "general/rotation" );

	return					cont;
}

void			object_track::on_selected					( bool bselected )
{ 
	m_is_selected = bselected;
	if( bselected )
	{
		fill_animation_curve_editor	( );
		owner_tool( )->get_level_editor( )->add_onframe_delegate( gcnew execute_delegate( this, &object_track::tick), 1 );
	}
	else
	{
		destroy_key_tangents_collision	( );
		
		selected_key				= -1;
		m_is_test_playing			= false;
		m_temp_container			= nullptr;
	}
}


///////////////             C U R V E   T O   C H A N N E L   B I N D E R               /////////////////

object_track::curve_to_channel_binder::curve_to_channel_binder				( object_track^ track_obj, curves_link^ link )
{
	m_track_obj			= track_obj;
	m_curves_link		= link;

	link->key_added		+= gcnew Action<Int32>( this, &object_track::curve_to_channel_binder::link_key_added );
	link->key_changed	+= gcnew Action<Int32>( this, &object_track::curve_to_channel_binder::link_key_changed );
	link->key_removed	+= gcnew Action<Int32>( this, &object_track::curve_to_channel_binder::link_key_removed );
}

object_track::curve_to_channel_binder::curve_to_channel_binder				( object_track^ track_obj, visual_curve^ curve, int channel_index )
{
	m_track_obj			= track_obj;
	m_curve				= curve;
	m_channel_index		= channel_index;

	curve->key_added	+= gcnew Action<visual_curve_key^>( this, &object_track::curve_to_channel_binder::curve_key_added );
	curve->key_changed	+= gcnew Action<Int32>( this, &object_track::curve_to_channel_binder::curve_key_changed );
	curve->key_removed	+= gcnew Action<visual_curve_key^>( this, &object_track::curve_to_channel_binder::curve_key_removed );
}
void			object_track::curve_to_channel_binder::curve_key_added		( visual_curve_key^ key )
{
	animation::EtCurve* channel = m_track_obj->m_track->m_channels[m_channel_index];
	channel->insert( (float)key->position.X, (float)key->position.Y );

	read_key( &channel->keyList[key->index], key->key, false );

	m_track_obj->update_collisions( );
}
void			object_track::curve_to_channel_binder::curve_key_changed	( int key_index )
{
	
	float_curve_key^ key = m_curve->keys[key_index]->key;
	animation::EtCurve* channel = m_track_obj->m_track->m_channels[m_channel_index];

	read_key( &channel->keyList[key_index], key, false );

	m_track_obj->update_collisions( );
}
void			object_track::curve_to_channel_binder::curve_key_removed	( visual_curve_key^ key )
{
	m_track_obj->m_track->m_channels[m_channel_index]->remove( key->index );
	m_track_obj->update_collisions( );
}
void			object_track::curve_to_channel_binder::link_key_added		( int key_index )
{
	int count = m_curves_link->curves->Count;
	for ( int idx = 0; idx < count; ++idx )
	{
		float_curve_key^ key = m_curves_link->curves[idx]->keys[key_index]->key;
		animation::EtCurve* channel = m_track_obj->m_track->m_channels[idx];
		
		int i						= key_index;

		bool need_convert = false;
		if( idx >= 3 )
			need_convert = true;

		channel->insert( (float)key->position.X, (float)key->position.Y );

		read_key( &channel->keyList[i], key, need_convert );
	}

	m_track_obj->update_collisions( );
}
void			object_track::curve_to_channel_binder::link_key_changed		( int key_index )
{
	int count = m_curves_link->curves->Count;
	for ( int idx = 0; idx < count; ++idx )
	{
		float_curve_key^ key = m_curves_link->curves[idx]->keys[key_index]->key;
		animation::EtCurve* channel = m_track_obj->m_track->m_channels[idx];

		int i						= key_index;

		bool need_convert = false;
		if( idx >= 3 )
			need_convert = true;

		read_key( &channel->keyList[i], key, need_convert );
	}

	m_track_obj->update_collisions( );
}
void			object_track::curve_to_channel_binder::link_key_removed		( int key_index )
{
	int count = m_curves_link->curves->Count;
	for ( int idx = 0; idx < count; ++idx )
		m_track_obj->m_track->m_channels[idx]->remove( key_index );

	m_track_obj->update_collisions( );
}
void			object_track::curve_to_channel_binder::read_key				( animation::EtKey* channel_key, float_curve_key^ key, bool need_convert )
{
	channel_key->time		= (Single)key->position.X;
	channel_key->value		= ( need_convert ) ? math::deg2rad( (Single)key->position.Y ) : (Single)key->position.Y;
	channel_key->inTanX		= (Single)( key->left_tangent.X - key->position.X );
	channel_key->inTanY		= ( need_convert ) ? math::deg2rad( (Single)( key->left_tangent.Y - key->position.Y ) ) : (Single)( key->left_tangent.Y - key->position.Y );
	channel_key->outTanX	= (Single)( key->right_tangent.X - key->position.X );
	channel_key->outTanY	= ( need_convert ) ? math::deg2rad( (Single)( key->right_tangent.Y - key->position.Y ) ) : (Single)( key->right_tangent.Y - key->position.Y );
	channel_key->type		= (u8)key->key_type;
}
void			object_track::curve_to_channel_binder::write_key			( animation::EtKey* channel_key, float_curve_key^ key, bool need_convert )
{
	key->position_x			= channel_key->time;
	key->position_y			= ( need_convert ) ? math::rad2deg( channel_key->value ) : channel_key->value;
	key->left_tangent		= Point(
		key->position_x + channel_key->inTanX,
		key->position_y + ( ( need_convert ) ? math::rad2deg( channel_key->inTanY ) : channel_key->inTanY )
	);
	key->right_tangent		= Point(
		key->position_x + channel_key->outTanX,
		key->position_y + ( ( need_convert ) ? math::rad2deg( channel_key->outTanY ) : channel_key->outTanY )
	);
	
	key->key_type = (curve_key_type)channel_key->type;
}
void			object_track::curve_to_channel_binder::update_key			( int key_index )
{
	if( m_curves_link != nullptr )
	{
		int count = m_curves_link->curves->Count;
		for ( int idx = 0; idx < count; ++idx )
		{
			float_curve_key^ key				= m_curves_link->curves[idx]->keys[key_index]->key;
			animation::EtCurve* channel			= m_track_obj->m_track->m_channels[idx];

			bool need_convert = false;
			if( idx >= 3 )
				need_convert = true;

			write_key( &channel->keyList[key_index], key, need_convert );

			m_curves_link->curves[idx]->keys[key_index]->update_visual( );
		}
	}
	else
	{
		float_curve_key^ key		= m_curve->keys[key_index]->key;
		animation::EtCurve* channel	= m_track_obj->m_track->m_channels[m_channel_index];

		write_key( &channel->keyList[key_index], key, false );
	}
}
void			object_track::curve_to_channel_binder::raise_key_changed	( Int32 key_index )
{
	m_curves_link->curves[0]->keys[key_index]->raise_key_changed( );
}
////////////////		C O L L I S I O N   O B J E C T   T R A C K   K E Y    			////////////////

collision_object_track_key::collision_object_track_key(	collision::geometry_instance* geometry, object_track^ o, int key_idx)
:super		( g_allocator, xray::editor_base::collision_object_type_touch, geometry ),
m_owner		( o ),
m_key_idx	( key_idx )
{}

bool			collision_object_track_key::touch			( ) const
{
	if( !m_owner->is_selected	 )
		m_owner->get_project( )->select_id( m_owner->id( ), xray::editor::enum_selection_method_set );

	m_owner->m_selected_tangent		= 0;
	m_owner->selected_key			= m_key_idx;
	m_owner->get_animation_curve_editor( )->selected_key_index = m_key_idx;
	return true;
}

void			collision_object_track_key::set_key_index	( int new_key_index )
{
	m_key_idx = new_key_index;
}
////////////////		C O L L I S I O N   O B J E C T   T R A C K   K E Y   T A N G E N T			////////////////

collision_object_track_key_tangent::collision_object_track_key_tangent(	collision::geometry_instance* geometry, object_track^ o, int key_idx, int tangent_idx )
:super			( g_allocator, xray::editor_base::collision_object_type_touch, geometry ),
m_owner			( o ),
m_key_idx		( key_idx ),
m_tangent_idx	( tangent_idx )
{}

bool			collision_object_track_key_tangent::touch	( ) const
{
	m_owner->m_selected_tangent		= m_tangent_idx;
	m_owner->owner_tool( )->get_level_editor( )->set_transform_object( gcnew track_object_key_tangent_transform_data( m_owner, m_owner->selected_key, m_tangent_idx ) );
	return true;
}


} // namespace editor
} // namespace xray
